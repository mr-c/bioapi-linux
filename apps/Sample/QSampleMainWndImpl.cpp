#include "QSampleMainWndImpl.h"
#include <qcombobox.h>
#include <qlabel.h>
#include <qmessagebox.h>
#include <qlineedit.h>
#include <stdio.h>

BioAPI_UUID_PTR QSampleMainWndImpl::s_uuid = NULL;

QSampleMainWndImpl::QSampleMainWndImpl() :
  QSampleMainWnd(),
  m_moduleHandle(0),
  m_bspSchemaArray(NULL),
  m_currSchema(NULL)
{
  s_uuid = (BioAPI_UUID *)malloc(sizeof(BioAPI_UUID));
}

void QSampleMainWndImpl::slotVerifyButtonPressed()
{
  BioAPI_BOOL bResponse;
  BioAPI_BOOL bPrecedence = BioAPI_TRUE;

  BioAPI_BIR_HANDLE CapturedTemplate, ProcessedBir;
  BioAPI_INPUT_BIR birEnroll, birCapture, InputBirProcessed;
 
  BioAPI_RETURN bioReturn;
  BioAPI_BIR_HEADER birHeader;
  BioAPI_FAR MaxFAR, AchievedFAR;
  
  if (m_userID->text().length() == 0)
  {
    QMessageBox::warning(this, "Bad User ID", "Please specify a user ID", QMessageBox::Ok, QMessageBox::NoButton);
  }
  else
  {
    QString szUserName = m_userID->text();

    if(InputFromFile(szUserName, &birEnroll) != BioAPI_OK)
    {
      QMessageBox::warning(this, "Bad User ID", "User not enrolled", QMessageBox::Ok, QMessageBox::NoButton);
      return;
    }

    // See if the BSP supports BioAPI_VerifyMatch by checking
    // the operations mask
    if(m_currSchema->Operations & BioAPI_VERIFYMATCH)
    {
      if((bioReturn = BioAPI_Capture(m_moduleHandle,
				     BioAPI_PURPOSE_VERIFY,
				     &CapturedTemplate,
				     -1,
				     NULL)) != BioAPI_OK)
      {
	PrintErrorCode(bioReturn);
	free(birEnroll.InputBIR.BIR);
	return;
      }

      if((bioReturn = BioAPI_GetHeaderFromHandle(m_moduleHandle,
						 CapturedTemplate,
						 &birHeader)) != BioAPI_OK)
      {
	PrintErrorCode(bioReturn);
	free(birEnroll.InputBIR.BIR);
	return;
      }

      if(birHeader.Type == BioAPI_BIR_DATA_TYPE_INTERMEDIATE)
      {
	birCapture.Form = BioAPI_BIR_HANDLE_INPUT;
	birCapture.InputBIR.BIRinBSP = &CapturedTemplate;
	if((bioReturn = BioAPI_Process(m_moduleHandle,
				       &birCapture,
				       &ProcessedBir)) != BioAPI_OK)
	{
	  PrintErrorCode(bioReturn);
	  free(birEnroll.InputBIR.BIR);
	  return;
	}
	
	MaxFAR = 1;
	InputBirProcessed.Form = BioAPI_BIR_HANDLE_INPUT;
	InputBirProcessed.InputBIR.BIRinBSP = &ProcessedBir;
      }
      else
      {
	MaxFAR = 1;
	InputBirProcessed.Form = BioAPI_BIR_HANDLE_INPUT;
	InputBirProcessed.InputBIR.BIRinBSP = &CapturedTemplate;
      }
      
      bioReturn = BioAPI_VerifyMatch(m_moduleHandle,
				     &MaxFAR,
				     NULL,
				     &bPrecedence,
				     &InputBirProcessed,
				     &birEnroll,
				     NULL,
				     &bResponse,
				     &AchievedFAR,
				     NULL,
				     NULL);
    }
    else		// We simply call BioAPI_Verify
    {
      MaxFAR = 1;
      bioReturn = BioAPI_Verify(m_moduleHandle,
				&MaxFAR,
				NULL,
				&bPrecedence,
				&birEnroll,
				NULL,
				&bResponse,
				&AchievedFAR,
				NULL,
				NULL,
				-1,
				NULL);
    }
    free(birEnroll.InputBIR.BIR);
    
    if(bioReturn != BioAPI_OK)
    {
      PrintErrorCode(bioReturn);
      return;
    }

    QMessageBox::information(this, "BioAPI", (bResponse == TRUE) ? "Match" : "No Match", QMessageBox::Ok, QMessageBox::NoButton);

  }
}

void QSampleMainWndImpl::slotEnrollButtonPressed()
{
  QString szUserName = m_userID->text();

  BioAPI_BIR_HANDLE EnrolledTemplate;

  if (m_userID->text().length() == 0)
  {
    QMessageBox::warning(this, "Bad User ID", "Please specify a user ID", QMessageBox::Ok, QMessageBox::NoButton);
  }
  else
  {
    BioAPI_RETURN  bioReturn = BioAPI_Enroll(m_moduleHandle,
					     BioAPI_PURPOSE_ENROLL_FOR_VERIFICATION_ONLY,
					     NULL,
					     &EnrolledTemplate,
					     NULL,
					     -1,
					     NULL);
    
    if(bioReturn != BioAPI_OK)
    {
      PrintErrorCode(bioReturn);
      return;
    }
    
    OutputToFile(szUserName, EnrolledTemplate);
  }
}

void QSampleMainWndImpl::slotSelectedBSPChanged(int idx)
{
  BioAPI_VERSION Version;
  m_currSchema = m_bspSchemaArray + idx;
  BioAPI_CopyUuid(s_uuid, m_currSchema->ModuleId);

  BioAPI_RETURN bioReturn = BioAPI_ModuleLoad(s_uuid, 0, NULL/*(BioAPI_ModuleEventHandler)BiometricEventHandler*/, 0);

  if(BioAPI_OK != bioReturn)
  {
    PrintErrorCode(bioReturn);
    free(s_uuid);
    s_uuid = NULL;
    return;
  }

  Version.Major = BioAPI_MAJOR;
  Version.Minor = BioAPI_MINOR;
  
  bioReturn = BioAPI_ModuleAttach(s_uuid, &Version, &BioAPIMemoryFuncs,
				  0,
				  0,0,0,
				  NULL,
				  0,
				  NULL,
				  &m_moduleHandle);

  if(BioAPI_OK != bioReturn)
  {
    PrintErrorCode(bioReturn);
    BioAPI_ModuleUnload (s_uuid, NULL, 0);
    free(s_uuid);
    s_uuid = NULL;
  }

  char buf[16];
  sprintf(buf, "0x%08X", m_moduleHandle);
  m_handleLabel->setText(QString(buf));
}

int QSampleMainWndImpl::initializeBioAPI()
{
  BioAPI_RETURN bioReturn;
  BioAPI_VERSION bioVersion; //UNUSED: , Version;

  BioAPI_BSP_SCHEMA *currSchema;
  //UNUSED:  int index;
  unsigned int ElementsNeeded, NumElementsReturned, i;

  bioVersion.Major = BioAPI_MAJOR;
  bioVersion.Minor = BioAPI_MINOR;
  bioReturn = BioAPI_Init(&bioVersion, 0, NULL, 0, NULL);
  if(BioAPI_OK != bioReturn)
  {
    if(BioAPIERR_H_FRAMEWORK_INCOMPATIBLE_VERSION == bioReturn)
    {
      fprintf(stderr, "This application is not compatible with the installed version of the BioAPI");
    }
    else
    {
      PrintErrorCode(bioReturn);
    }
    return 0;
  }

  bioReturn = BioAPI_EnumModules(NULL,
				 0,
				 &ElementsNeeded,
				 &NumElementsReturned);

  if(bioReturn != BioAPI_OK)
  {
    PrintErrorCode(bioReturn);
    return 0;
  }

  m_arraySize = ElementsNeeded;
  m_bspSchemaArray = (BioAPI_BSP_SCHEMA *)malloc(ElementsNeeded * sizeof(BioAPI_BSP_SCHEMA));
  if (!m_bspSchemaArray)
  {
    fprintf(stderr, "Unable to allocate BSP list");
    return 0;
  }
  
  bioReturn = BioAPI_EnumModules(m_bspSchemaArray,
				 m_arraySize,
				 &ElementsNeeded,
				 &NumElementsReturned);

  currSchema = m_bspSchemaArray;

  for(i = 0; i < NumElementsReturned; ++i)
  {
    char buf[256];
    strncpy(buf, currSchema->Description, 256);
    m_bspCombo->insertItem(buf);
    ++currSchema;
  }

  if (NumElementsReturned > 0)
  {
    m_bspCombo->setCurrentItem(0);
    slotSelectedBSPChanged(0);
  }

  return 1;
}

void QSampleMainWndImpl::PrintErrorCode(BioAPI_RETURN bioReturn)
{
  char szMessage[255];

  sprintf(szMessage, "BioAPI Error Code: 0x%x\n", bioReturn);
	QMessageBox::warning(NULL, "BioAPI Error", szMessage, "OK");
  //fprintf(stdout, szMessage);
}

BioAPI_RETURN QSampleMainWndImpl::OutputToFile(QString lptszUserName, BioAPI_BIR_HANDLE BirHandle)
{
  FILE *hFile;

  unsigned int dwBytesToWrite, dwBytesWritten;

  BioAPI_BIR_PTR birData = NULL;
  BioAPI_RETURN bioReturn;
  
  /* Retrieve the BIR from the BSP */
  bioReturn = BioAPI_GetBIRFromHandle(m_moduleHandle,
				      BirHandle,
				      &birData);
  if(bioReturn != BioAPI_OK)
    return(bioReturn);

  /* Open the file */
  QString szFileName = lptszUserName + ".bir";
  hFile = fopen(szFileName, "wb+");
  if (hFile == NULL) return(0); /* this return value is ignored anyway */
		
  /* Write the header */
  dwBytesWritten = fwrite(&(birData->Header), sizeof (BioAPI_BIR_HEADER), 1, hFile);

  /* Write the biometric data */
  dwBytesToWrite = LocalEndian4 (birData->Header.Length) - sizeof (BioAPI_BIR_HEADER);
  dwBytesWritten = fwrite(birData->BiometricData, dwBytesToWrite, 1, hFile);

  /* Write the signature if present */
  if (birData->Signature)
  {
    dwBytesWritten = fwrite(&(birData->Signature->Length), 4, 1, hFile);
    dwBytesToWrite = LocalEndian4 (birData->Signature->Length);
    dwBytesWritten = fwrite(birData->Signature->Data, dwBytesToWrite, 1, hFile);
  }

  /* Close the file */
  fclose(hFile);

  /* Free all the memory allocated for the BIR */
  free(birData->BiometricData);
  if (birData->Signature)
  {
    free(birData->Signature->Data);
    free(birData->Signature);
  }
  free(birData);

  return(BioAPI_OK);
}

BioAPI_RETURN QSampleMainWndImpl::InputFromFile(QString lptszUserName, BioAPI_INPUT_BIR * InputBir)
{
  FILE *hFile;
  unsigned int dwBytesToRead, dwBytesRead;
  BioAPI_BIR *bir;

  QString szFileName =  lptszUserName + ".bir";
  hFile = fopen(szFileName, "rb");
  if(hFile == NULL)
    return BioAPIERR_BSP_RECORD_NOT_FOUND; // Return this BSP error if the file isn't present

  /* Read the header in, to determine the size */
  dwBytesToRead = sizeof(BioAPI_BIR_HEADER);
  bir = (BioAPI_BIR *)malloc(sizeof (BioAPI_BIR));
  if (!bir) return (!BioAPI_OK);

  dwBytesRead = fread(&(bir->Header), dwBytesToRead, 1, hFile);

  /* Read the biometric data */
  dwBytesToRead = LocalEndian4 (bir->Header.Length) - sizeof (BioAPI_BIR_HEADER);
  bir->BiometricData = (BioAPI_BIR_BIOMETRIC_DATA_PTR)malloc(dwBytesToRead);
  if (!bir->BiometricData)
    return (!BioAPI_OK);
  dwBytesRead = fread(bir->BiometricData, dwBytesToRead, 1, hFile);

  /* Read the signature if present */
  dwBytesToRead = 0;

  dwBytesRead = fread(&dwBytesToRead, 4, 1, hFile);
  if (&dwBytesRead != 0 && dwBytesToRead != 0)
  {
    bir->Signature = (BioAPI_DATA_PTR)malloc(sizeof (BioAPI_DATA));
    if (!bir->Signature)
      return (!BioAPI_OK);
    bir->Signature->Length = dwBytesToRead;
    dwBytesToRead = LocalEndian4 (dwBytesToRead);
    bir->Signature->Data = (uint8 *)malloc(dwBytesToRead);
    if (!bir->Signature->Data)
      return (!BioAPI_OK);
    dwBytesRead = fread(bir->Signature->Data, dwBytesToRead, 1, hFile);
  }

  /* Close the file */
  fclose(hFile);

  /* Set up the return values */
  InputBir->InputBIR.BIR = bir;
  InputBir->Form = BioAPI_FULLBIR_INPUT;

  return(BioAPI_OK);
}
