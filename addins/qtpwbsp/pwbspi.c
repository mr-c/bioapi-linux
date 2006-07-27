/*-----------------------------------------------------------------------
 * File: pwbspi.c
 *
 *-----------------------------------------------------------------------
 */

#include "precomp.h"
#include "bioapi_spi.h"

#if !defined (USE_NO_GUI)
int DoPasswordDialog(char *password, int maxPasswordLen);
#endif

/*-----------------*
 * "Magic Numbers" *
 *-----------------*/

/* Buffer for password to be captured to; declared here
 * to allow access by multiple functions */
#define MAX_PASSWORD_SIZE 32
char inputPassword[MAX_PASSWORD_SIZE + 1]; /* +1 for null terminator */
char confirmPassword[MAX_PASSWORD_SIZE + 1]; /* +1 for null terminator */

/* Some parameters for the password based encryption of payloads */
const uint32 PBEKeyLength		 = 64;	 /* in bits */
const uint32 AlgorithmIterations = 1000; /* This must be sufficiently large to make
										  * generating all the keys for a large password
										  * dictionary noticibly more painful, while
										  * still being small enough to remain uncostly
										  * for our purposes. */


/*----------------------------------------------------------------------------------*
 *	Helper Functions and Data structures											*
 *----------------------------------------------------------------------------------*/

#define SPI_RETURN( errorCode ) \
{									\
	Addin_SPIEnd(pAddinContext);	\
	return errorCode;				\
}

#ifdef WIN32

#define WIN32_LEAN_AND_MEAN		/* Trim headers to only most commonly used stuff */
#include <windows.h>			/* Include Win API for password dialog			 */
#include "resource.h"			/* Include dialog resource definitions			 */

/*-----------------------------------------*
 *	Callback for Dialog box event handling *
 *-----------------------------------------*/
BOOL CALLBACK CapturePassword(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	RECT rcDlg, rcDesktop;

	switch (message)
	{
		/*	This message is sent when the dialog gets created */
		case WM_INITDIALOG:

			GetWindowRect(GetDesktopWindow(), &rcDesktop);
			GetWindowRect(hwndDlg, &rcDlg);

			/* Center Dialog on the desktop */

			SetWindowPos(hwndDlg,
				HWND_TOP,
				rcDesktop.right/2  - ((rcDlg.right	 - rcDlg.left)	/ 2),
				rcDesktop.bottom/2 - ((rcDlg.bottom	 - rcDlg.top)	/ 2),
				0, 0,  /* ignores size arguments; no resize desired */
				SWP_NOSIZE);

			/* Start dialog with focus in the password edit box */
			SetFocus(GetDlgItem(hwndDlg, IDC_PWEDIT));
			break;

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case IDOK:
					/*	Dump the password into BSP memory */
					if (!GetDlgItemText(hwndDlg, IDC_PWEDIT, inputPassword,
							MAX_PASSWORD_SIZE + 1))
						/* if call fails, null out the password cache */
						*inputPassword=0;

				/* fall through after ok is pressed */
				case IDCANCEL:
					EndDialog(hwndDlg, wParam);
					return TRUE;
			}
	}
	return FALSE;
}

#endif // WIN32 defined for password dialog callback



/*--------------------------------------------------------------------------------*
 * Private Helper functions:  These allow the functionality of the SPI to be used *
 * internally without violating the integrity checks imposed by EISL, since the	  *
 * BSP is not that app to which the calls should be returning!					  *
 *--------------------------------------------------------------------------------*/

BioAPI_RETURN _biospi_FreeBIRHandle(
					ADDIN_ATTACH_DATA	*pAttachData,
					BioAPI_BIR_HANDLE  BIRHandle )
{
	BIR_LIST_NODE *pCurListNode	 = NULL,
				  *pPrevListNode = NULL,
				  *pTempListNode = NULL;

	if (NULL == (pCurListNode = pPrevListNode = pAttachData->BIRList))
		return (BioAPIERR_BSP_INTERNAL_ERROR);

	/* Burn through the linked list of BIRs until the correct
	 * header is found or nothing remains in the list */
	while (pCurListNode->BIRHandle != BIRHandle && pCurListNode->NextNode != NULL)
	{
		pPrevListNode = pCurListNode;
		pCurListNode  = (BIR_LIST_NODE*)pCurListNode->NextNode;
	}

	/* if the BIR was successfully found */
	if (pCurListNode->BIRHandle == BIRHandle)
	{
		pTempListNode = (BIR_LIST_NODE*)pCurListNode->NextNode;

		/* Free the dynamically allocated biometric data
		 * field of the stored BIR */
		BioAPI_free(pCurListNode->BIR.BiometricData, NULL);

		/* If the BIR to free is the first one in the list */
		if (pCurListNode == pPrevListNode)
		{
			BioAPI_free(pAttachData->BIRList, NULL);
			pAttachData->BIRList = pTempListNode;
		}
		else
		{
			BioAPI_free(pCurListNode, NULL);

			pPrevListNode->NextNode = pTempListNode;
		}
	}
	else
		return (BioAPIERR_BSP_INVALID_BIR_HANDLE);

	return BioAPI_OK;
}


BioAPI_RETURN _biospi_GetBIRFromHandle(
					const ADDIN_ATTACH_DATA *pAttachData,
					BioAPI_BIR_HANDLE	   BIRHandle,
					BioAPI_BIR_PTR		  *BIR )
{
	BIR_LIST_NODE	*pCurListNode	= NULL;

	if (NULL == (pCurListNode = pAttachData->BIRList))
		return (BioAPIERR_BSP_INVALID_BIR_HANDLE);

	/* Burn through the linked list of BIRs until the correct
	 * header is found or nothing remains in the list */
	while (pCurListNode->BIRHandle != BIRHandle &&
		   pCurListNode->NextNode  != NULL)
		pCurListNode = (BIR_LIST_NODE*)pCurListNode->NextNode;

	/* If BIR was successfully found, capture it in the pointer for return
	 * to SPI functions */
	if (pCurListNode->BIRHandle == BIRHandle)
	{
		*BIR = &pCurListNode->BIR;
		return BioAPI_OK;
	}
	else
		return (BioAPIERR_BSP_INVALID_BIR_HANDLE);
}

BioAPI_RETURN _biospi_Capture(
					ADDIN_ATTACH_DATA		*pAttachData,
					BioAPI_BIR_PURPOSE	   Purpose,
					BioAPI_BIR_HANDLE_PTR  CapturedBIR)
{
	BIR_LIST_NODE	*pOldList		   = NULL,
					*pHeadNode		   = NULL;
	uint32			 capturedBIRLength = 0;
	BIO_DATA_HEADER	 bioDataHeader;

#ifdef WIN32

	INT_PTR dialogRetVal = 0;
	HANDLE	dllHandle;

#endif

	/*---------------------------------------*
	 * Get the password from the user.		 *
	 * Start of platform specific code here! *
	 *---------------------------------------*/

/*------------------------------------------------------------------------
 * WIN32
 *-----------------------------------------------------------------------*/

	#ifdef WIN32
	/*---------------------------------------*
	 * Create Dialog and prompt for password *
	 *---------------------------------------*/

	/* Windows will find this library in the system directory
	 * where the .dll is installed */
	dllHandle = LoadLibrary(BSP_SELF_CHECK_SECTION);

	if (IDOK != (dialogRetVal = DialogBox(
						dllHandle,	MAKEINTRESOURCE(IDD_PWDIALOG),
						NULL,		(DLGPROC)CapturePassword)))
	{
		/* Operation failed; password wasn't captured */
		 return (BioAPIERR_BSP_UNABLE_TO_CAPTURE);
	}

	#endif	// WIN32 defined

/*------------------------------------------------------------------------*/

#if defined (LINUX) || defined (SOLARIS)

  DoPasswordDialog(inputPassword, MAX_PASSWORD_SIZE);

	// Uncomment for text version
	//memset(inputPassword, 0, MAX_PASSWORD_SIZE + 1);

	/* VERIFY */
	//if (BioAPI_PURPOSE_VERIFY == Purpose) {
		//printf("Please enter your password for verification:\n");
		//fgets(inputPassword, MAX_PASSWORD_SIZE+1, stdin);
  /* ENROLL */
	//} else if (BioAPI_PURPOSE_ENROLL_FOR_VERIFICATION_ONLY == Purpose) {
		//memset(confirmPassword, 0, MAX_PASSWORD_SIZE + 1);

		//printf("Please enter your password for enrollment:\n");
		//fgets(inputPassword, MAX_PASSWORD_SIZE+1, stdin);
		//printf("Please re-enter your password for confirmation:\n");
		//fgets(confirmPassword, MAX_PASSWORD_SIZE+1, stdin);

		//if (strlen(inputPassword) != strlen(confirmPassword) ||
		//	memcmp(inputPassword, confirmPassword, strlen(inputPassword))) {
	
			/* Unlock the mutex for the next capture call */
			//TODO:RDS Figure out what to do about these
			//port_UnlockMutex(BSPMutex);

		//	return (BioAPIERR_BSP_UNABLE_TO_CAPTURE);
		//}

	//} else {

			/* Unlock the mutex for the next capture call */
			//port_UnlockMutex(BSPMutex); 
			//return (BioAPIERR_BSP_PURPOSE_NOT_SUPPORTED);
	//} 

#endif /* LINUX or SOLARIS defined */


	/*---------------------------------------*
	 * End of platform specific code. Assume *
	 * now that the password is stored in	 *
	 * the global buffer inputPassword.		 *
	 *---------------------------------------*/

	/* Keep track of BIR in addin until function FreeBIRHandle(...)
	 * releases it */

	pOldList = pAttachData->BIRList;
	pAttachData->BIRList = pHeadNode =
		(BIR_LIST_NODE*) BioAPI_malloc(sizeof(BIR_LIST_NODE), NULL);
	pHeadNode->NextNode		= (void*)pOldList;

	/*------------------*
	 * Build BIR Header *
	 *------------------*/
	pHeadNode->BIRHandle  = *CapturedBIR	 = pAttachData->HandleIndex++;
	pHeadNode->BIR.Header.HeaderVersion		 = BIR_HEADER_VERSION;
	pHeadNode->BIR.Header.Type = BioAPI_BIR_DATA_TYPE_RAW;
	pHeadNode->BIR.Header.Format.FormatOwner = LittleEndian2 (BIO_DATA_FORMAT_OWNER);
	pHeadNode->BIR.Header.Format.FormatID	 = LittleEndian2 (BIO_DATA_FORMAT_ID);
	pHeadNode->BIR.Header.Quality			 = 100; /* by definition */
	pHeadNode->BIR.Header.FactorsMask		 = LittleEndian4 (BioAPI_FACTOR_PASSWORD);

	/* The Purpose will be used in the HRS Process function
	 * to make sure that BIRs which are captured for verification
	 * are not hashed; otherwise, they're useless. */
	pHeadNode->BIR.Header.Purpose		 = Purpose;

	bioDataHeader.PWLength		   = capturedBIRLength = strlen(inputPassword);
	bioDataHeader.PayloadLength	   = 0;
	pHeadNode->BIR.Header.Length   = LittleEndian4 (sizeof(BioAPI_BIR_HEADER) +
									 sizeof(BIO_DATA_HEADER) + capturedBIRLength);

	/* Finish BIR */
	pHeadNode->BIR.Signature	 = NULL;
	pHeadNode->BIR.BiometricData =
		(BioAPI_BIR_BIOMETRIC_DATA*)
			BioAPI_malloc(capturedBIRLength + sizeof(BIO_DATA_HEADER), NULL);
	port_memcpy((void*)pHeadNode->BIR.BiometricData, (void*)&bioDataHeader,
				sizeof(BIO_DATA_HEADER));
	port_memcpy((void*)(pHeadNode->BIR.BiometricData + sizeof(BIO_DATA_HEADER)),
				inputPassword, capturedBIRLength);

	/* Wipe password out of buffer */
	port_memset((void*)inputPassword, 0, MAX_PASSWORD_SIZE);

	return BioAPI_OK;
}

BioAPI_RETURN _biospi_CreateTemplate(
			ADDIN_ATTACH_DATA			*pAttachData,
			const void		 *pLoadData,
			const BioAPI_BIR		  *pBIRtoWrap,
			BioAPI_BIR_HANDLE_PTR	   NewTemplate,
			const BioAPI_DATA			  *Payload )
{
	BIR_LIST_NODE  *pOldList					 = NULL,
				   *pHeadNode					 = NULL;
	uint8		   *pNewBioData					 = NULL;
	uint32			newBioDataLength			 = 0;
	BIO_DATA_HEADER bioDataHeader;

	/*---------------------------------------------------------*
	 * Only BIRs with the purpose of enrollment should be used *
	 *---------------------------------------------------------*/

	if (BioAPI_PURPOSE_ENROLL_FOR_VERIFICATION_ONLY
			!= pBIRtoWrap->Header.Purpose)
		return (BioAPIERR_BSP_INVALID_BIR);

	/*----------------------------------------------------------------------*
	 * Only BIRs created using the proper biometric data format can be used *
	 *----------------------------------------------------------------------*/

	if (BIO_DATA_FORMAT_ID	  != LocalEndian2 (pBIRtoWrap->Header.Format.FormatID) ||
		BIO_DATA_FORMAT_OWNER != LocalEndian2 (pBIRtoWrap->Header.Format.FormatOwner))
		return (BioAPIERR_BSP_UNSUPPORTED_FORMAT);

	/*------------------------------------------------*
	 * Only raw and processed BIRs should be accepted *
	 *------------------------------------------------*/

	/*---------------------------------------------------*
	 * Rewrap with payload, but don't encrypt - we no	 *
	 * longer have plaintext password for key derivation *
	 *---------------------------------------------------*/

	/* Retrieve the opaque header information from the BIR */
	port_memcpy((void*)&bioDataHeader, (void*)pBIRtoWrap->BiometricData,
				sizeof(BIO_DATA_HEADER));

	/* New biometric data will include the old data plus any payload */
	if (Payload)
		bioDataHeader.PayloadLength = Payload->Length;
	else
		bioDataHeader.PayloadLength = 0;
	newBioDataLength			= bioDataHeader.PWLength +
								  bioDataHeader.PayloadLength +
								  sizeof(BIO_DATA_HEADER);
	pNewBioData					= BioAPI_malloc(newBioDataLength, NULL);
	if (!pNewBioData)
		return (BioAPIERR_BSP_MEMORY_ERROR);

	/*---------------------------------------------------*
	 * Move the payload into the addin's memory space,	 *
	 * so that it can be included in a new template BIR	 *
	 *---------------------------------------------------*/

	/* In this biometric format, the header describing the length
	 * of the password and payload, the cryptographic salts, and the
	 * cryptographic initialization data vector is included in the
	 * opaque biometric data first, followed by the password, then
	 * the payload.	 We copy the headers and PW data over first */
	port_memcpy((void*)pNewBioData, (void*)&bioDataHeader,
				sizeof(BIO_DATA_HEADER));

	port_memcpy((void*)(pNewBioData + sizeof(BIO_DATA_HEADER)),
				(void*)(pBIRtoWrap->BiometricData + sizeof(BIO_DATA_HEADER)),
				bioDataHeader.PWLength);

	/* Then we copy over the payload */
	if (Payload)
	{
		port_memcpy((void*)(pNewBioData + sizeof(BIO_DATA_HEADER) + bioDataHeader.PWLength),
						(void*)Payload->Data, Payload->Length);
	} // if

	/*----------------------------------------------------*
	 * Build template with new biometric data and payload *
	 *----------------------------------------------------*/

	/* The assumption, at this point, is that for either case of BIR
	 * input, raw or processed, that pNewBioData contains the opaque
	 * data field to be stored, that newBioDataLength is the length
	 * of that field. */

	/* Keep track of BIR in addin until function FreeBIRHandle(...) releases it */

	pOldList = pAttachData->BIRList;
	pAttachData->BIRList = pHeadNode =
		(BIR_LIST_NODE*) BioAPI_malloc(sizeof(BIR_LIST_NODE), NULL);
	pHeadNode->NextNode		= (void*)pOldList;

	/* Build BIR Header */
	pHeadNode->BIRHandle  = *NewTemplate	 = pAttachData->HandleIndex++;
	pHeadNode->BIR.Header.HeaderVersion		 = BIR_HEADER_VERSION;
	pHeadNode->BIR.Header.Type = BioAPI_BIR_DATA_TYPE_PROCESSED;
	pHeadNode->BIR.Header.Format.FormatOwner = LittleEndian2 (BIO_DATA_FORMAT_OWNER);
	pHeadNode->BIR.Header.Format.FormatID	 = LittleEndian2 (BIO_DATA_FORMAT_ID);
	pHeadNode->BIR.Header.Quality			 = 100; /* by definition */
	pHeadNode->BIR.Header.FactorsMask		 = LittleEndian4 (BioAPI_FACTOR_PASSWORD);
	pHeadNode->BIR.Header.Purpose		 = BioAPI_PURPOSE_VERIFY;

	/* bio data length includes the format header, the password, and the payload,
	 * while the BIR header length includes all this and the BioAPI BIR header's length */
	pHeadNode->BIR.Header.Length = LittleEndian4 (newBioDataLength + sizeof(BioAPI_BIR_HEADER));

	/* Finish the BIR */
	pHeadNode->BIR.Signature	 = NULL;
	pHeadNode->BIR.BiometricData = (BioAPI_BIR_BIOMETRIC_DATA*)pNewBioData;

	return BioAPI_OK;
}

BioAPI_RETURN _biospi_Process(
			ADDIN_ATTACH_DATA			*pAttachData,
			const void		 *pLoadData,
			const BioAPI_BIR		  *pBIRtoProcess,
			BioAPI_BIR_HANDLE_PTR	   ProcessedBIR )
{
	BIR_LIST_NODE  *pOldList			 = NULL,
				   *pHeadNode			 = NULL;
	BIO_DATA_HEADER bioDataHeader;
	uint32			newBioDataLength;
	uint8		   *pNewBioData;

	/*--------------------------------*
	 * Check for the proper BIR flags *
	 *--------------------------------*/

	/*----------------------------------------------------------------------------*
	 * Note about signed BIRs: if this BSP could somehow support signed BIRs, the *
	 * signature would be verified at this point.  Note that this means that flag *
	 * can be discarded below to account for the check which could occur at this  *
	 * point in the future, if signatures were implemented here.				  *
	 *----------------------------------------------------------------------------*/

	/* Only raw BIRs should be processed. Signed raw data is OK,
	 * so strip that flag off before the check.	 This is to allow
	 * for the possibility of signed BIRs in the future; see above. */
	if (pBIRtoProcess->Header.Type & ~BioAPI_BIR_DATA_TYPE_SIGNED
			!= BioAPI_BIR_DATA_TYPE_RAW)
		return (BioAPIERR_BSP_INVALID_BIR);

	/* Only enrollment purpose is supported.  Since the Verify
	 * function requires that the BIR captured for verification comes
	 * to it with the password in plaintext, even the
	 * CSSM_HRS_PURPOSE_VERIFY is not supported in this function.
	 * When a BIR is "processed" within the context of this BSP,
	 * it can then be used to "enroll" a BIR for a payload which is
	 * stored in plaintext, instead of encrypted. */
	if (BioAPI_PURPOSE_ENROLL_FOR_VERIFICATION_ONLY !=
			pBIRtoProcess->Header.Purpose)
		return (BioAPIERR_BSP_PURPOSE_NOT_SUPPORTED);

	/* Only BIRs utilizing the proper biometric data format can be used */
	if (BIO_DATA_FORMAT_ID	  != LocalEndian2 (pBIRtoProcess->Header.Format.FormatID) ||
		BIO_DATA_FORMAT_OWNER != LocalEndian2 (pBIRtoProcess->Header.Format.FormatOwner))
		return (BioAPIERR_BSP_UNSUPPORTED_FORMAT);

	/*----------------------*
	 * Create processed BIR *
	 *----------------------*/

	/* copy the biometric header */
	port_memcpy((void*)&bioDataHeader, (void*)pBIRtoProcess->BiometricData,
				sizeof(BIO_DATA_HEADER));

	/* calc length of biometric data */
	newBioDataLength	  = bioDataHeader.PWLength +
						  bioDataHeader.PayloadLength +
						  sizeof(BIO_DATA_HEADER);

	/* allocate memory for the "biometric" data */
	pNewBioData			 = BioAPI_malloc(newBioDataLength, NULL);
	if (NULL == pNewBioData)
		return (BioAPIERR_BSP_MEMORY_ERROR);

	/* In this biometric format, the header describing the length
	 * of the password and payload, the cryptographic salts, and the
	 * cryptographic initialization data vector is included in the
	 * opaque biometric data first, followed by the password, then
	 * the payload.	 We copy the headers and PW data over first */
	port_memcpy((void*)pNewBioData, (void*)&bioDataHeader,
				sizeof(BIO_DATA_HEADER));

	port_memcpy((void*)(pNewBioData + sizeof(BIO_DATA_HEADER)),
				(void*)(pBIRtoProcess->BiometricData + sizeof(BIO_DATA_HEADER)),
				bioDataHeader.PWLength);

	/* Then we copy over the payload */
	port_memcpy((void*)(pNewBioData + sizeof(BIO_DATA_HEADER) + bioDataHeader.PWLength),
				(void*)(pBIRtoProcess->BiometricData + sizeof(BIO_DATA_HEADER) + bioDataHeader.PWLength),
				bioDataHeader.PayloadLength);


	/* Keep track of BIR in addin until function FreeBIRHandle(...)
	 * releases it */

	pOldList = pAttachData->BIRList;
	pAttachData->BIRList = pHeadNode =
		(BIR_LIST_NODE*) BioAPI_malloc(sizeof(BIR_LIST_NODE), NULL);
	if (NULL == pHeadNode)
	{
		BioAPI_free (pNewBioData, NULL);
		return (BioAPIERR_BSP_MEMORY_ERROR);
	}
	pHeadNode->NextNode		= (void*)pOldList;

	/* Build BIR Header */
	pHeadNode->BIRHandle  = *ProcessedBIR	 = pAttachData->HandleIndex++;
	pHeadNode->BIR.Header.HeaderVersion		 = BIR_HEADER_VERSION;
	pHeadNode->BIR.Header.Type = BioAPI_BIR_DATA_TYPE_PROCESSED;
	pHeadNode->BIR.Header.Format.FormatOwner = LittleEndian2 (BIO_DATA_FORMAT_OWNER);
	pHeadNode->BIR.Header.Format.FormatID	 = LittleEndian2 (BIO_DATA_FORMAT_ID);
	pHeadNode->BIR.Header.Quality			 = 100; /* by definition */
	pHeadNode->BIR.Header.FactorsMask		 = LittleEndian4 (BioAPI_FACTOR_PASSWORD);

	/* The input BIR's header has the length already converted to little-endian */
	pHeadNode->BIR.Header.Length   = pBIRtoProcess->Header.Length;

	/* This BIR is just another incarnation of the input BIR -
	 * it has the same purpose */
	pHeadNode->BIR.Header.Purpose		 = pBIRtoProcess->Header.Purpose;

	/* Finish BIR */
	pHeadNode->BIR.Signature	 = NULL;
	pHeadNode->BIR.BiometricData = (BioAPI_BIR_BIOMETRIC_DATA*)pNewBioData;

	return BioAPI_OK;
}


BioAPI_RETURN _biospi_VerifyMatch(
					const ADDIN_ATTACH_DATA		*pAttachData,
					const void		 *pLoadData,
					BioAPI_HANDLE				   ModuleHandle,
					const BioAPI_BIR		  *pCapturedBIR,
					const BioAPI_BIR		  *pTemplateBIR,
					BioAPI_BOOL *Result,
					BioAPI_DATA_PTR *Payload )
{
	BIO_DATA_HEADER		 bioDataHeaderCaptured, bioDataHeaderTemplate;


	/* Check for RAW data type - signed flag doesn't pertain to this check
	 * and can be stripped */
	if (BioAPI_BIR_DATA_TYPE_RAW !=
		(pCapturedBIR->Header.Type & ~BioAPI_BIR_DATA_TYPE_SIGNED))
		return (BioAPIERR_BSP_INVALID_BIR);

	/* Check that both BIRs are using the proper biometric data format */
	if (BIO_DATA_FORMAT_ID	  != LocalEndian2 (pCapturedBIR->Header.Format.FormatID) ||
		BIO_DATA_FORMAT_ID	  != LocalEndian2 (pTemplateBIR->Header.Format.FormatID) ||
		BIO_DATA_FORMAT_OWNER != LocalEndian2 (pCapturedBIR->Header.Format.FormatOwner) ||
		BIO_DATA_FORMAT_OWNER != LocalEndian2 (pTemplateBIR->Header.Format.FormatOwner))
		return (BioAPIERR_BSP_UNSUPPORTED_FORMAT);

	/* Check that both BIRs have the proper purpose flags */
	if (BioAPI_PURPOSE_VERIFY != pCapturedBIR->Header.Purpose ||
		BioAPI_PURPOSE_VERIFY != pTemplateBIR->Header.Purpose)
		return (BioAPIERR_BSP_INCONSISTENT_PURPOSE);

	/*-----------------------------------------------------*
	 * Compare the password plaintext					   *
	 *-----------------------------------------------------*/
	port_memcpy((void*)&bioDataHeaderTemplate, (void*)pTemplateBIR->BiometricData,
				sizeof(BIO_DATA_HEADER));
	port_memcpy((void*)&bioDataHeaderCaptured, (void*)pCapturedBIR->BiometricData,
				sizeof(BIO_DATA_HEADER));

	if( bioDataHeaderTemplate.PWLength != bioDataHeaderCaptured.PWLength ||
			port_memcmp((void*)(pCapturedBIR->BiometricData + sizeof(BIO_DATA_HEADER)),
					(void*)(pTemplateBIR->BiometricData + sizeof(BIO_DATA_HEADER)),
					bioDataHeaderTemplate.PWLength) )
	{
		/* Passwords were NOT the same; release no info or privileges */
		*Result	 = BioAPI_FALSE;
		if (Payload)
			*Payload = NULL;

		return (BioAPI_OK);
	}/* end password compare  if */

	/* passwords WERE the same; verify user! */
	*Result = BioAPI_TRUE;

	/*-------------------------------------------*
		 * Return payload to the calling application *
	 *-------------------------------------------*/
	if (NULL == Payload)
		return BioAPI_OK;

	*Payload = (BioAPI_DATA*)App_Malloc(ModuleHandle, sizeof(BioAPI_DATA));

	/* If there is no payload, we're done */
	if (bioDataHeaderTemplate.PayloadLength <= 0)
	{
		(*Payload)->Data   = NULL;
		(*Payload)->Length = 0;

		return (BioAPI_OK);
	}/* end payload check */

	if (!(BioAPI_BIR_DATA_TYPE_ENCRYPTED & pTemplateBIR->Header.Type))
	{
		(*Payload)->Data   = (uint8*)App_Malloc(ModuleHandle, bioDataHeaderTemplate.PayloadLength);
		(*Payload)->Length = bioDataHeaderTemplate.PayloadLength;
		port_memcpy((void*)(*Payload)->Data,
					(void*)(pTemplateBIR->BiometricData + sizeof(BIO_DATA_HEADER) +
					bioDataHeaderTemplate.PWLength),
					bioDataHeaderTemplate.PayloadLength);

		return (BioAPI_OK);
	}
	else
	{
		return(BioAPIERR_BSP_UNABLE_TO_WRAP_PAYLOAD);
	}/* end payload encryption check */

}

/*----------------------------------------------------------------------------------*
 *	HRS SPI Function Implementations												*
 *----------------------------------------------------------------------------------*/

BioAPI_RETURN BioAPI BioSPI_FreeBIRHandle(
					BioAPI_HANDLE		  ModuleHandle,
					BioAPI_BIR_HANDLE BIRHandle )
{
	BioAPI_RETURN		   ret			 = BioAPI_OK;
	void				*pAddinContext = NULL;
	ADDIN_ATTACH_DATA	*pAttachData   = NULL;

	/*-------------------------*
	 * Initialize MAF Contexts *
	 *-------------------------*/

	/* Note to keep in mind from here on:  after MAF has been initialized
	 * the function must release the context with ADDIN_SPIEnd(...).  The
	 * return macro for doing this is SPI_RETURN(<errorcode>).	This will
	 * be used to return in each function in each case after MAF is in use. */

	ADDIN_SPI_BEGIN(ret, ModuleHandle, pAddinContext);
	if (BioAPI_OK != ret) return ret;

	if (NULL == (pAttachData = Addin_GetAttachData()))
		SPI_RETURN(BioAPIERR_BSP_INTERNAL_ERROR);

	/*----------------------------------------------------------------*
	 * Call to the internal routine where the real functionality lies *
	 *----------------------------------------------------------------*/
	ret = _biospi_FreeBIRHandle(pAttachData, BIRHandle);
	if (BioAPI_OK != ret) SPI_RETURN(ret);

	SPI_RETURN(BioAPI_OK);
}

BioAPI_RETURN BioAPI BioSPI_GetBIRFromHandle(
					BioAPI_HANDLE		   ModuleHandle,
					BioAPI_BIR_HANDLE  BIRHandle,
					BioAPI_BIR_PTR	  *BIR )
{
	BioAPI_RETURN		 ret		   = BioAPI_OK;
	void			  *pAddinContext = NULL;
	BioAPI_BIR		*pFoundBIR	   = NULL;
	ADDIN_ATTACH_DATA *pAttachData	 = NULL;

	/*--------------------------*
	 * Check for bad parameters *
	 *--------------------------*/

	if (NULL == BIR)
		return BioAPIERR_BSP_INVALID_OUTPUT_POINTER;

	/*-------------------------*
	 * Initialize MAF Contexts *
	 *-------------------------*/

	ADDIN_SPI_BEGIN(ret, ModuleHandle, pAddinContext);
	if (BioAPI_OK != ret) return ret;

	if (NULL == (pAttachData = Addin_GetAttachData()))
		SPI_RETURN(BioAPIERR_BSP_INTERNAL_ERROR);

	/*----------------------------------------------------------------*
	 * Call to the internal routine where the real functionality lies *
	 *----------------------------------------------------------------*/
	 ret = _biospi_GetBIRFromHandle(pAttachData, BIRHandle, &pFoundBIR);
	 if (BioAPI_OK != ret) SPI_RETURN(ret);

	/* allocate memory in the Application's memory space
	 * for the BIR to be stored */
	*BIR = App_Malloc(ModuleHandle, sizeof(BioAPI_BIR));
	if (NULL == *BIR)
		return (BioAPIERR_BSP_MEMORY_ERROR);

	/* move BIR header to Application's memory space */
	port_memcpy((void*)&(*BIR)->Header, (void*)&(pFoundBIR->Header),
				sizeof(BioAPI_BIR_HEADER));

	/* allocate memory in the Application's memory space
	 * for the biometric data to be stored */
	(*BIR)->BiometricData = (BioAPI_BIR_BIOMETRIC_DATA*)
		App_Malloc(ModuleHandle, LocalEndian4 ((*BIR)->Header.Length) - sizeof(BioAPI_BIR_HEADER));
	if (NULL == (*BIR)->BiometricData)
		return (BioAPIERR_BSP_MEMORY_ERROR);

	/* move the BIR biometric data to the Application's memory space */
	port_memcpy((void*)(*BIR)->BiometricData,
				(void*)pFoundBIR->BiometricData,
				LocalEndian4 ((*BIR)->Header.Length) - sizeof(BioAPI_BIR_HEADER));

	/* This BSP cannot support signatures */
	(*BIR)->Signature = NULL;

	/*------------------------------------------------------------*
	 * Free the BIR now that it's been yielded to the application *
	 *------------------------------------------------------------*/
	ret = _biospi_FreeBIRHandle(pAttachData, BIRHandle);
	if (BioAPI_OK != ret) SPI_RETURN(ret);

	SPI_RETURN(BioAPI_OK);
}

BioAPI_RETURN BioAPI BioSPI_GetHeaderFromHandle(
					BioAPI_HANDLE			  ModuleHandle,
					BioAPI_BIR_HANDLE	  BIRHandle,
					BioAPI_BIR_HEADER_PTR Header )
{
	BioAPI_RETURN		 ret		   = BioAPI_OK;
	void			  *pAddinContext = NULL;
	BioAPI_BIR		*pFoundBIR	   = NULL;
	ADDIN_ATTACH_DATA *pAttachData	 = NULL;

	/*--------------------------*
	 * Check for bad parameters *
	 *--------------------------*/

	if (NULL == Header)
		return BioAPIERR_BSP_INVALID_OUTPUT_POINTER;

	/*-------------------------*
	 * Initialize MAF Contexts *
	 *-------------------------*/

	ADDIN_SPI_BEGIN(ret, ModuleHandle, pAddinContext);
	if (ret != BioAPI_OK) return ret;

	if ((pAttachData = Addin_GetAttachData()) == NULL)
		SPI_RETURN(BioAPIERR_BSP_INTERNAL_ERROR);

	/*-------------------------------------------------*
	 * Get the BIR from which the header will be taken *
	 *-------------------------------------------------*/
	ret = _biospi_GetBIRFromHandle(pAttachData, BIRHandle, &pFoundBIR);
	if (BioAPI_OK != ret) SPI_RETURN(ret);

	/* Copy the header into the application's memory space */
	port_memcpy((void*)Header, (void*)&(pFoundBIR->Header),
		sizeof(BioAPI_BIR_HEADER));

	SPI_RETURN(BioAPI_OK);
}

BioAPI_RETURN BioAPI BioSPI_EnableEvents(
					BioAPI_HANDLE				  ModuleHandle,
					BioAPI_MODULE_EVENT_MASK *Events )
{
	return BioAPIERR_BSP_FUNCTION_NOT_IMPLEMENTED;
}

BioAPI_RETURN BioAPI BioSPI_SetGUICallbacks(
					BioAPI_HANDLE ModuleHandle,
					BioAPI_GUI_STREAMING_CALLBACK GuiStreamingCallback,
					void *GuiStreamingCallbackCtx,
					BioAPI_GUI_STATE_CALLBACK GuiStateCallback,
					void *GuiStateCallbackCtx )
{
	return BioAPIERR_BSP_FUNCTION_NOT_IMPLEMENTED;
}


BioAPI_RETURN BioAPI BioSPI_SetStreamCallback(
					BioAPI_HANDLE				ModuleHandle,
					BioAPI_STREAM_CALLBACK	StreamCallback,
					void					 *StreamCallbackCtx )
{
	return BioAPIERR_BSP_FUNCTION_NOT_IMPLEMENTED;
}

BioAPI_RETURN BioAPI BioSPI_StreamInputOutput(
					BioAPI_HANDLE	ModuleHandle,
					BioAPI_DATA_PTR InMessage,
					BioAPI_DATA_PTR OutMessage )
{
	return BioAPIERR_BSP_FUNCTION_NOT_IMPLEMENTED;
}

BioAPI_RETURN BioAPI BioSPI_Capture(
					BioAPI_HANDLE			  ModuleHandle,
					BioAPI_BIR_PURPOSE	  Purpose,
					BioAPI_BIR_HANDLE_PTR CapturedBIR,
					sint32					Timeout,
					BioAPI_BIR_HANDLE_PTR AuditData )
{
	BioAPI_RETURN		 ret			   = BioAPI_OK;
	void			  *pAddinContext	 = NULL;
	ADDIN_ATTACH_DATA *pAttachData		 = NULL;

	/*------------------------------------------------------------------*
	 * Check for bad paramters and initialize output for the error case *
	 *------------------------------------------------------------------*/

	if (NULL == CapturedBIR)
		return BioAPIERR_BSP_INVALID_OUTPUT_POINTER;
	else
		*CapturedBIR = BioAPI_INVALID_BIR_HANDLE;

	if (NULL != AuditData)
		/* AuditData not supported */
		*AuditData = BioAPI_UNSUPPORTED_BIR_HANDLE;

	if (Purpose != BioAPI_PURPOSE_VERIFY &&
		Purpose != BioAPI_PURPOSE_ENROLL_FOR_VERIFICATION_ONLY)
		return BioAPIERR_BSP_PURPOSE_NOT_SUPPORTED;

	/*-------------------------*
	 * Initialize MAF Contexts *
	 *-------------------------*/

	ADDIN_SPI_BEGIN(ret, ModuleHandle, pAddinContext);
	if (ret != BioAPI_OK) return ret;

	if ((pAttachData = Addin_GetAttachData()) == NULL)
		SPI_RETURN(BioAPIERR_BSP_INTERNAL_ERROR);

	/*-----------------------------------------------------------------*
	 * Check to ensure there are still handles available for more BIRs *
	 *-----------------------------------------------------------------*/
	if (pAttachData->HandleIndex < 0)
		/* Overflow, we've run out of handles */
		SPI_RETURN(BioAPIERR_BSP_TOO_MANY_HANDLES);

	/*----------------------------------------------------------------*
	 * Call to the internal routine where the real functionality lies *
	 *----------------------------------------------------------------*/
	ret = _biospi_Capture(pAttachData, Purpose, CapturedBIR);
	if (BioAPI_OK != ret) SPI_RETURN(ret);

	SPI_RETURN(BioAPI_OK);
}

BioAPI_RETURN BioAPI BioSPI_CreateTemplate(
					BioAPI_HANDLE				ModuleHandle,
					const BioAPI_INPUT_BIR *CapturedBIR,
					const BioAPI_INPUT_BIR *StoredTemplate,
					BioAPI_BIR_HANDLE_PTR	NewTemplate,
					const BioAPI_DATA		   *Payload )
{
	BioAPI_RETURN			 ret		   = BioAPI_OK;
	void				  *pAddinContext = NULL;
	BioAPI_BIR			*pBIRtoWrap	   = NULL;
	ADDIN_ATTACH_DATA	  *pAttachData	 = NULL;
	const void *pLoadData	  = NULL;

	/*---------------------------------------------------------------*
	 * Check for bad paramters and initialize output for error cases *
	 *---------------------------------------------------------------*/
	if (NULL == CapturedBIR)
		return BioAPIERR_BSP_INVALID_INPUT_POINTER;

	if (NULL != StoredTemplate)
		return BioAPIERR_BSP_INVALID_BIR_HANDLE;

	if (NULL == NewTemplate)
		return BioAPIERR_BSP_INVALID_OUTPUT_POINTER;
	else
		*NewTemplate = BioAPI_INVALID_HANDLE;

	/*-------------------------*
	 * Initialize MAF Contexts *
	 *-------------------------*/

	ADDIN_SPI_BEGIN(ret, ModuleHandle, pAddinContext);
	if (BioAPI_OK != ret) return ret;

	if ((NULL == (pAttachData = Addin_GetAttachData())) )
		SPI_RETURN(BioAPIERR_BSP_INTERNAL_ERROR);

	/*-----------------------------------------------------------------*
	 * Check to ensure there are still handles available for more BIRs *
	 *-----------------------------------------------------------------*/
	if (pAttachData->HandleIndex < 0)
		/* Overflow, we've run out of handles */
		SPI_RETURN(BioAPIERR_BSP_TOO_MANY_HANDLES);

	/*-----------------------------------------*
	 * Find the BIR to be used in the template *
	 *-----------------------------------------*/

	if (BioAPI_FULLBIR_INPUT == CapturedBIR->Form)
		pBIRtoWrap = CapturedBIR->InputBIR.BIR;
	else if (BioAPI_BIR_HANDLE_INPUT == CapturedBIR->Form)
	{
		ret = _biospi_GetBIRFromHandle(
			pAttachData, *(CapturedBIR->InputBIR.BIRinBSP), &pBIRtoWrap);
		if (BioAPI_OK != ret) SPI_RETURN(ret);
	}
	else
		/* something went wrong - these are the only
		 * two forms supported by this BSP */
		SPI_RETURN(BioAPIERR_BSP_INVALID_BIR);

	/*----------------------------------------------------------------*
	 * Call to the internal routine where the real functionality lies *
	 *----------------------------------------------------------------*/
	ret = _biospi_CreateTemplate(  pAttachData, pLoadData, pBIRtoWrap,
								NewTemplate, Payload);
	if (BioAPI_OK != ret) SPI_RETURN(ret);

	SPI_RETURN(BioAPI_OK);
}

BioAPI_RETURN BioAPI BioSPI_Process(
					BioAPI_HANDLE				ModuleHandle,
					const BioAPI_INPUT_BIR *CapturedBIR,
					BioAPI_BIR_HANDLE_PTR	ProcessedBIR )
{
	BioAPI_RETURN			ret			   = BioAPI_OK;
	void				  *pAddinContext = NULL;
	BioAPI_BIR			*pBIRtoProcess = NULL;
	ADDIN_ATTACH_DATA	  *pAttachData	 = NULL;
	const void *pLoadData	  = NULL;


	/*-----------------------------------------------------------------------*
	 * Check for bad parameters and initialize output for the case of errors *
	 *-----------------------------------------------------------------------*/
	if (NULL == CapturedBIR)
		return BioAPIERR_BSP_INVALID_INPUT_POINTER;

	if (NULL == ProcessedBIR)
		return BioAPIERR_BSP_INVALID_OUTPUT_POINTER;
	else
		*ProcessedBIR = BioAPI_INVALID_HANDLE;

	/*-------------------------*
	 * Initialize MAF Contexts *
	 *-------------------------*/

	ADDIN_SPI_BEGIN(ret, ModuleHandle, pAddinContext);
	if (BioAPI_OK != ret) return ret;

	if ((NULL == (pAttachData = Addin_GetAttachData()))	 )
		SPI_RETURN(BioAPIERR_BSP_INTERNAL_ERROR);

	/*-----------------------------------------------------------------*
	 * Check to ensure there are still handles available for more BIRs *
	 *-----------------------------------------------------------------*/
	if (pAttachData->HandleIndex < 0)
		/* Overflow, we've run out of handles */
		SPI_RETURN(BioAPIERR_BSP_TOO_MANY_HANDLES);

	/*-------------------------*
	 * Find the BIR to process *
	 *-------------------------*/

	if (BioAPI_FULLBIR_INPUT == CapturedBIR->Form)
		pBIRtoProcess = CapturedBIR->InputBIR.BIR;
	else if (BioAPI_BIR_HANDLE_INPUT == CapturedBIR->Form)
	{
		ret = _biospi_GetBIRFromHandle(
			pAttachData, *(CapturedBIR->InputBIR.BIRinBSP), &pBIRtoProcess);
		if (BioAPI_OK != ret) SPI_RETURN(ret);
	}
	else
		/* something went wrong - these are the only two
		 * forms supported by this BSP */
		SPI_RETURN(BioAPIERR_BSP_INVALID_BIR);

	/*----------------------------------------------------------------*
	 * Call to the internal routine where the real functionality lies *
	 *----------------------------------------------------------------*/
	ret = _biospi_Process(pAttachData, pLoadData, pBIRtoProcess, ProcessedBIR);
	if (BioAPI_OK != ret) SPI_RETURN(ret);


	SPI_RETURN(BioAPI_OK);
}

BioAPI_RETURN BioAPI BioSPI_VerifyMatch(
					BioAPI_HANDLE				ModuleHandle,
					const BioAPI_FAR	   *MaxFARRequested,
					const BioAPI_FRR	   *MaxFRRRequested,
					const BioAPI_BOOL		   *FARPrecedence,
					const BioAPI_INPUT_BIR *ProcessedBIR,
					const BioAPI_INPUT_BIR *StoredTemplate,
					BioAPI_BIR_HANDLE	   *AdaptedBIR,
					BioAPI_BOOL				   *Result,
					BioAPI_FAR_PTR			FARAchieved,
					BioAPI_FRR_PTR			FRRAchieved,
					BioAPI_DATA_PTR			   *Payload )
{
	BioAPI_RETURN				ret			  = BioAPI_OK;
	void					 *pAddinContext = NULL;
	BioAPI_BIR			   *pCapturedBIR  = NULL;
	BioAPI_BIR			   *pTemplateBIR  = NULL;
	const ADDIN_ATTACH_DATA	 *pAttachData	= NULL;
	const void	  *pLoadData	 = NULL;

	/*-----------------------------------------------------------------------*
	 * Check for bad parameters and initialize output for the case of errors *
	 *-----------------------------------------------------------------------*/

	/* MaxFARRequested, MaxFRRRequested, and FARPrecedence are ignored
	 * anyway, so we won't quibble over those. See note below! */

	if (NULL == ProcessedBIR || NULL == StoredTemplate)
		return BioAPIERR_BSP_INVALID_INPUT_POINTER;

	if (NULL != AdaptedBIR)
		*AdaptedBIR = BioAPI_UNSUPPORTED_BIR_HANDLE;

	if (NULL == FARAchieved || NULL == Result)
		return BioAPIERR_BSP_INVALID_OUTPUT_POINTER;

	*Result	 = BioAPI_FALSE;
	if (Payload)
		*Payload = NULL;

	/*--------------------------------------------------------------------------*
	 * A note to begin this function, the false accept rate (FAR) for this BSP	*
	 * is by nature next to nothing.  The chances of someone being validated	*
	 * by entering an incorrect password whose hash collides with the correct	*
	 * one seems to me to have about the same odds as a person getting hit by	*
	 * lightning 3 times in each of 3 states where he's won the lotto.	We'll	*
	 * just consider this to be zero, and ignore the parameter.	 The false		*
	 * reject rate (FRR) is most certainly zero - if someone enters the correct *
	 * pasword, they WILL get access.  No question there, so this parameter can *
	 * also be ignored.	 Similarly, the returned FAR and FRR achieved will		*
	 * always be 0 and 0.														*
	 *--------------------------------------------------------------------------*/
	*FARAchieved = 0;
	if (FRRAchieved)
		*FRRAchieved = 0;

	/*-------------------------*
	 * Initialize MAF Contexts *
	 *-------------------------*/

	ADDIN_SPI_BEGIN(ret, ModuleHandle, pAddinContext);
	if (BioAPI_OK != ret) return ret;

	if ((NULL == (pAttachData = Addin_GetAttachData())) )
		SPI_RETURN(BioAPIERR_BSP_INTERNAL_ERROR);

	/*------------------------*
	 * Find the BIR to verify *
	 *------------------------*/

	if (BioAPI_FULLBIR_INPUT == ProcessedBIR->Form)
		pCapturedBIR = ProcessedBIR->InputBIR.BIR;
	else if (BioAPI_BIR_HANDLE_INPUT == ProcessedBIR->Form)
	{
		ret = _biospi_GetBIRFromHandle(
			pAttachData, *(ProcessedBIR->InputBIR.BIRinBSP), &pCapturedBIR);
		if (BioAPI_OK != ret) SPI_RETURN(ret);
	}
	else
		/* something went wrong - these are the only two
		 * forms supported by this BSP */
		SPI_RETURN(BioAPIERR_BSP_INVALID_BIR);

	/*-----------------------------------------*
	 * Find the template BIR to verify against *
	 *-----------------------------------------*/

	if (BioAPI_FULLBIR_INPUT == StoredTemplate->Form)
		pTemplateBIR = StoredTemplate->InputBIR.BIR;
	else if (BioAPI_BIR_HANDLE_INPUT == StoredTemplate->Form)
	{
		ret = _biospi_GetBIRFromHandle(
			pAttachData, *(StoredTemplate->InputBIR.BIRinBSP), &pTemplateBIR);
		if (BioAPI_OK != ret) SPI_RETURN(ret);
	}
	else
		/* something went wrong - these are the only two
		 * forms supported by this BSP */
		SPI_RETURN(BioAPIERR_BSP_INVALID_BIR);

	/*----------------------------------------------------------------*
	 * Call to the internal routine where the real functionality lies *
	 *----------------------------------------------------------------*/
	ret = _biospi_VerifyMatch( pAttachData,	   pLoadData,  ModuleHandle, pCapturedBIR,
							pTemplateBIR,	Result,		Payload);
	if (BioAPI_OK != ret) SPI_RETURN(ret);

	SPI_RETURN(BioAPI_OK);
}

BioAPI_RETURN BioAPI BioSPI_IdentifyMatch(
					BioAPI_HANDLE						  ModuleHandle,
					const BioAPI_FAR				 *MaxFARRequested,
					const BioAPI_FRR				 *MaxFRRRequested,
					const BioAPI_BOOL					 *FARPrecedence,
					const BioAPI_INPUT_BIR			 *ProcessedBIR,
					const BioAPI_IDENTIFY_POPULATION *Population,
					BioAPI_BOOL							  Binning,
					uint32								MaxNumberOfResults,
					uint32							   *NumberOfResults,
					BioAPI_CANDIDATE_ARRAY_PTR		 *Candidates,
					sint32								Timeout )
{
	return BioAPIERR_BSP_FUNCTION_NOT_IMPLEMENTED;
}

BioAPI_RETURN BioAPI BioSPI_Enroll(
					BioAPI_HANDLE				ModuleHandle,
					BioAPI_BIR_PURPOSE		Purpose,
					const BioAPI_INPUT_BIR *StoredTemplate,
					BioAPI_BIR_HANDLE_PTR	NewTemplate,
					const BioAPI_DATA		   *Payload,
					sint32					  Timeout,
					BioAPI_BIR_HANDLE_PTR	AuditData )
{
	BioAPI_RETURN			   ret				  = BioAPI_OK;
	void					*pAddinContext		= NULL;
	BioAPI_BIR_HANDLE	   CapturedBIR;
	BioAPI_BIR			  *pInputCapturedBIR;
	ADDIN_ATTACH_DATA		*pAttachData		= NULL;
	const void	 *pLoadData			 = NULL;

	/*-----------------------------------------------------------------------*
	 * Check for bad parameters and initialize output for the case of errors *
	 *-----------------------------------------------------------------------*/

	if (NULL != StoredTemplate)
		return BioAPIERR_BSP_INVALID_BIR_HANDLE;

	if (NULL == NewTemplate)
		return BioAPIERR_BSP_INVALID_OUTPUT_POINTER;
	else
		*NewTemplate = BioAPI_INVALID_HANDLE;

	if (NULL != AuditData)
		*AuditData = BioAPI_UNSUPPORTED_BIR_HANDLE;

	/*-------------------------*
	 * Initialize MAF Contexts *
	 *-------------------------*/

	ADDIN_SPI_BEGIN(ret, ModuleHandle, pAddinContext);
	if (BioAPI_OK != ret) return ret;


	if ((NULL == (pAttachData = Addin_GetAttachData())) )
		SPI_RETURN(BioAPIERR_BSP_INTERNAL_ERROR);

	/*---------------------------------------------*
	 * Capture the BIR that will be used to create *
	 * the new enrollment template				   *
	 *---------------------------------------------*/

	ret = _biospi_Capture(pAttachData, Purpose, &CapturedBIR);
	if (BioAPI_OK != ret) SPI_RETURN(ret);

	/*---------------------------------------------*
	 * Create the new enrollment template and	   *
	 * associate payload with the captured BIR	   *
	 *---------------------------------------------*/

	ret = _biospi_GetBIRFromHandle(pAttachData, CapturedBIR,
								&pInputCapturedBIR);
	if (BioAPI_OK != ret) SPI_RETURN(ret);

	ret = _biospi_CreateTemplate(  pAttachData,		   pLoadData,
								pInputCapturedBIR,	NewTemplate,
								Payload);
	if (BioAPI_OK != ret) SPI_RETURN(ret);

	/*---------------------------------------------------------*
	 * Release the BIR which was captured to perform the	   *
	 * enrollment											   *
	 *---------------------------------------------------------*/

	ret = _biospi_FreeBIRHandle(pAttachData, CapturedBIR);
	if (BioAPI_OK != ret) SPI_RETURN(ret);

	SPI_RETURN(BioAPI_OK);
}

BioAPI_RETURN BioAPI BioSPI_Verify(
					BioAPI_HANDLE				ModuleHandle,
					const BioAPI_FAR	   *MaxFARRequested,
					const BioAPI_FRR	   *MaxFRRRequested,
					const BioAPI_BOOL		   *FARPrecedence,
					const BioAPI_INPUT_BIR *StoredTemplate,
					BioAPI_BIR_HANDLE_PTR	AdaptedBIR,
					BioAPI_BOOL				   *Result,
					BioAPI_FAR_PTR			FARAchieved,
					BioAPI_FRR_PTR			FRRAchieved,
					BioAPI_DATA_PTR			   *Payload,
					sint32					  Timeout,
					BioAPI_BIR_HANDLE_PTR	AuditData )
{
	BioAPI_RETURN			   ret				  = BioAPI_OK;
	void					*pAddinContext		= NULL;
	BioAPI_BIR_HANDLE	   CapturedBIR;
	BioAPI_BIR			  *pInputCapturedBIR;
	BioAPI_BIR			  *pInputTemplateBIR;
	ADDIN_ATTACH_DATA		*pAttachData		= NULL;
	const void	 *pLoadData			 = NULL;

	/*-----------------------------------------------------------------------*
	 * Check for bad parameters and initialize output for the case of errors *
	 *-----------------------------------------------------------------------*/

	/* MaxFARRequested, MaxFRRRequested, and FARPrecedence are ignored
	 * anyway, so we won't quibble over those. See note below! */

	if (NULL == StoredTemplate)
		return BioAPIERR_BSP_INVALID_INPUT_POINTER;

	if (NULL != AdaptedBIR)
		*AdaptedBIR = BioAPI_UNSUPPORTED_BIR_HANDLE;

	if (NULL == FARAchieved || NULL == Result)
		return BioAPIERR_BSP_INVALID_OUTPUT_POINTER;

	if (NULL != AuditData)
		*AuditData = BioAPI_UNSUPPORTED_BIR_HANDLE;

	*Result	 = BioAPI_FALSE;
	if (Payload)
		*Payload = NULL;
	*FARAchieved = 0;
	if (FRRAchieved)
		*FRRAchieved = 0;

	/*-------------------------*
	 * Initialize MAF Contexts *
	 *-------------------------*/

	ADDIN_SPI_BEGIN(ret, ModuleHandle, pAddinContext);
	if (BioAPI_OK != ret) return ret;

	if (((ADDIN_ATTACH_DATA *)NULL == (pAttachData = Addin_GetAttachData())) )
		SPI_RETURN(BioAPIERR_BSP_INTERNAL_ERROR);

	/*---------------------------------------------------------*
	 * Capture the BIR that will be used as verification	   *
	 * against the input template BIR						   *
	 *---------------------------------------------------------*/

	ret = _biospi_Capture( pAttachData, BioAPI_PURPOSE_VERIFY, &CapturedBIR);
	if (BioAPI_OK != ret) SPI_RETURN(ret);

	/*---------------------------------------------------------*
	 * Verify that the two BIRs had the same plaintext		   *
	 * password, and if so, grab the payload from the template *
	 *---------------------------------------------------------*/

	ret = _biospi_GetBIRFromHandle(pAttachData, CapturedBIR, &pInputCapturedBIR);
	if (BioAPI_OK != ret) SPI_RETURN(ret);

	/*-----------------------------------------*
	 * Find the template BIR to verify against *
	 *-----------------------------------------*/

	if (BioAPI_FULLBIR_INPUT == StoredTemplate->Form)
		pInputTemplateBIR = StoredTemplate->InputBIR.BIR;
	else if (BioAPI_BIR_HANDLE_INPUT == StoredTemplate->Form)
	{
		ret = _biospi_GetBIRFromHandle(
			pAttachData, *(StoredTemplate->InputBIR.BIRinBSP), &pInputTemplateBIR);
		if (BioAPI_OK != ret) SPI_RETURN(ret);
	}
	else
		/* something went wrong - these are the only two
		 * forms supported by this BSP */
		SPI_RETURN(BioAPIERR_BSP_INVALID_BIR);

	ret = _biospi_VerifyMatch(pAttachData, pLoadData, ModuleHandle,
						   pInputCapturedBIR,
						   pInputTemplateBIR,
						   Result,
						   Payload);
	if (BioAPI_OK != ret) SPI_RETURN(ret);

	/*---------------------------------------------------------*
	 * Release the BIR which was captured to perform the	   *
	 * verification											   *
	 *---------------------------------------------------------*/

	ret = _biospi_FreeBIRHandle(pAttachData, CapturedBIR);
	if (BioAPI_OK != ret) SPI_RETURN(ret);

	SPI_RETURN(BioAPI_OK);
}

BioAPI_RETURN BioAPI BioSPI_Identify(
					BioAPI_HANDLE					ModuleHandle,
					const BioAPI_FAR		   *MaxFARRequested,
					const BioAPI_FRR		   *MaxFRRRequested,
					const BioAPI_BOOL			   *FARPrecedence,
					const BioAPI_IDENTIFY_POPULATION *Population,
					BioAPI_BOOL						Binning,
					uint32						  MaxNumberOfResults,
					uint32						 *NumberOfResults,
					BioAPI_CANDIDATE_ARRAY_PTR *Candidates,
					sint32						  Timeout,
					BioAPI_BIR_HANDLE_PTR		AuditData )
{
	return BioAPIERR_BSP_FUNCTION_NOT_IMPLEMENTED;
}

BioAPI_RETURN BioAPI BioSPI_Import(
					BioAPI_HANDLE						  ModuleHandle,
					const BioAPI_DATA					 *InputData,
					BioAPI_BIR_BIOMETRIC_DATA_FORMAT  InputFormat,
					BioAPI_BIR_PURPOSE				  Purpose,
					BioAPI_BIR_HANDLE_PTR			  ConstructedBIR )
{
	return BioAPIERR_BSP_FUNCTION_NOT_IMPLEMENTED;
}

BioAPI_RETURN BioAPI BioSPI_SetPowerMode(
					BioAPI_HANDLE		  ModuleHandle,
					BioAPI_POWER_MODE PowerMode )
{
	return BioAPIERR_BSP_FUNCTION_NOT_IMPLEMENTED;
}

BioAPI_RETURN BioAPI BioSPI_DbOpen(
					BioAPI_HANDLE			   ModuleHandle,
					const uint8				*DbName,
					BioAPI_DB_ACCESS_TYPE  AccessRequest,
					BioAPI_DB_HANDLE_PTR   DbHandle,
					BioAPI_DB_CURSOR_PTR   Cursor )
{
	return BioAPIERR_BSP_FUNCTION_NOT_IMPLEMENTED;
}

BioAPI_RETURN BioAPI BioSPI_DbClose(
					BioAPI_HANDLE		  ModuleHandle,
					BioAPI_DB_HANDLE  DbHandle )
{
	return BioAPIERR_BSP_FUNCTION_NOT_IMPLEMENTED;
}

BioAPI_RETURN BioAPI BioSPI_DbCreate(
					BioAPI_HANDLE				ModuleHandle,
					const uint8				 *DbName,
					BioAPI_DB_ACCESS_TYPE	AccessRequest,
					BioAPI_DB_HANDLE_PTR	DbHandle )
{
	return BioAPIERR_BSP_FUNCTION_NOT_IMPLEMENTED;
}

BioAPI_RETURN BioAPI BioSPI_DbDelete(
					BioAPI_HANDLE  ModuleHandle,
					const uint8 *DbName )
{
	return BioAPIERR_BSP_FUNCTION_NOT_IMPLEMENTED;
}

BioAPI_RETURN BioAPI BioSPI_DbSetCursor(
					BioAPI_HANDLE			  ModuleHandle,
					BioAPI_DB_HANDLE	  DbHandle,
					const BioAPI_UUID		 *KeyValue,
					BioAPI_DB_CURSOR_PTR  Cursor )
{
	return BioAPIERR_BSP_FUNCTION_NOT_IMPLEMENTED;
}

BioAPI_RETURN BioAPI BioSPI_DbFreeCursor(
					BioAPI_HANDLE			 ModuleHandle,
					BioAPI_DB_CURSOR_PTR Cursor )
{
	return BioAPIERR_BSP_FUNCTION_NOT_IMPLEMENTED;
}

BioAPI_RETURN BioAPI BioSPI_DbStoreBIR(
					BioAPI_HANDLE				ModuleHandle,
					const BioAPI_INPUT_BIR *BIRToStore,
					BioAPI_DB_HANDLE		DbHandle,
					BioAPI_UUID_PTR				Uuid )
{
	return BioAPIERR_BSP_FUNCTION_NOT_IMPLEMENTED;
}

BioAPI_RETURN BioAPI BioSPI_DbGetBIR(
					BioAPI_HANDLE			   ModuleHandle,
					BioAPI_DB_HANDLE	   DbHandle,
					const BioAPI_UUID		  *KeyValue,
					BioAPI_BIR_HANDLE_PTR  RetrievedBIR,
					BioAPI_DB_CURSOR_PTR   Cursor )
{
	return BioAPIERR_BSP_FUNCTION_NOT_IMPLEMENTED;
}

BioAPI_RETURN BioAPI BioSPI_DbGetNextBIR(
					BioAPI_HANDLE			  ModuleHandle,
					BioAPI_DB_CURSOR_PTR  Cursor,
					BioAPI_BIR_HANDLE_PTR RetievedBIR,
					BioAPI_UUID_PTR			  Uuid )
{
	return BioAPIERR_BSP_FUNCTION_NOT_IMPLEMENTED;
}

BioAPI_RETURN BioAPI BioSPI_DbQueryBIR(
					BioAPI_HANDLE				ModuleHandle,
					BioAPI_DB_HANDLE		DbHandle,
					const BioAPI_INPUT_BIR *BIRToQuery,
					BioAPI_UUID_PTR				Uuid )
{
	return BioAPIERR_BSP_FUNCTION_NOT_IMPLEMENTED;
}

BioAPI_RETURN BioAPI BioSPI_DbDeleteBIR(
					BioAPI_HANDLE		  ModuleHandle,
					BioAPI_DB_HANDLE  DbHandle,
					const BioAPI_UUID	 *KeyValue )
{
	return BioAPIERR_BSP_FUNCTION_NOT_IMPLEMENTED;
}


/* Define the attach function.
 */
BioAPI_RETURN SPI_ModuleAttach(
				   BioAPI_MODULE_FUNCS_PTR *FuncTbl )
{
	static BioAPI_MODULE_FUNCS FunctionList;
	static BioAPI_BSP_FUNCS Functions;
	if (!FuncTbl)
	{
		return BioAPIERR_BSP_INVALID_POINTER;
	}

	FunctionList.NumberOfServiceFuncs = sizeof(BioAPI_BSP_FUNCS) /
	sizeof(
	BioAPI_PROC_ADDR );
	FunctionList.ServiceFuncs = (BioAPI_PROC_ADDR*)&Functions;
	Functions.FreeBIRHandle = BioSPI_FreeBIRHandle;
	Functions.GetBIRFromHandle = BioSPI_GetBIRFromHandle;
	Functions.GetHeaderFromHandle = BioSPI_GetHeaderFromHandle;
	Functions.EnableEvents = BioSPI_EnableEvents;
	Functions.SetGUICallbacks = BioSPI_SetGUICallbacks;
	Functions.SetStreamCallback = BioSPI_SetStreamCallback;
	Functions.StreamInputOutput = BioSPI_StreamInputOutput;
	Functions.Capture = BioSPI_Capture;
	Functions.CreateTemplate = BioSPI_CreateTemplate;
	Functions.Process = BioSPI_Process;
	Functions.VerifyMatch = BioSPI_VerifyMatch;
	Functions.IdentifyMatch = BioSPI_IdentifyMatch;
	Functions.Enroll = BioSPI_Enroll;
	Functions.Verify = BioSPI_Verify;
	Functions.Identify = BioSPI_Identify;
	Functions.Import = BioSPI_Import;
	Functions.SetPowerMode = BioSPI_SetPowerMode;
	Functions.DbOpen = BioSPI_DbOpen;
	Functions.DbClose = BioSPI_DbClose;
	Functions.DbCreate = BioSPI_DbCreate;
	Functions.DbDelete = BioSPI_DbDelete;
	Functions.DbSetCursor = BioSPI_DbSetCursor;
	Functions.DbFreeCursor = BioSPI_DbFreeCursor;
	Functions.DbStoreBIR = BioSPI_DbStoreBIR;
	Functions.DbGetBIR = BioSPI_DbGetBIR;
	Functions.DbGetNextBIR = BioSPI_DbGetNextBIR;
	Functions.DbQueryBIR = BioSPI_DbQueryBIR;
	Functions.DbDeleteBIR = BioSPI_DbDeleteBIR;
	*FuncTbl = &FunctionList;
	return BioAPI_OK;
}
