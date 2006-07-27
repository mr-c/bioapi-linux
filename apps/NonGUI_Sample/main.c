/*
 * main.c
 *
 * Non-GUI Based Sample Application. This is a very simple sample application 
 * that demonstrates the use of the BioAPI without relying on any GUI code. 
 * Warning: It is dependent on hard coded items and is unforgiving in it flow
 * of execution. The program is serial in nature, run the program multiple
 * times to test different scenarios. 
 *
 */

#include <stdio.h>
#include <string.h>

#include "bioapi.h"
#include "bioapi_schema.h"
#include "port/bioapi_port.h"
#include "bioapi_uuid.h"
#include "bioapi_util.h"

/* 
 * Hard coded for PASSWORD BSP; Critical this is dependent on the order 
 * in which the BSP were installed 
 */
#define PASSWORD_BSP 1
#define MAX_USER_NAME 100

#define FALSE 0
#define TRUE  1

void PrintErrorCode(BioAPI_RETURN bioReturn);
void PrintModuleId(BioAPI_UUID moduleId);
void SetToPasswordBSP(int index);
int Enroll();
void Verify();
BioAPI_RETURN OutputToFile(char *userName, BioAPI_BIR_HANDLE BirHandle);
BioAPI_RETURN InputFromFile(char *userName, BioAPI_INPUT_BIR *InputBir);

char userID[MAX_USER_NAME];
BioAPI_BSP_SCHEMA *BspSchemaArray;
BioAPI_BSP_SCHEMA *CurrSchema;
BioAPI_BOOL g_is_loading, g_immediate_attach;
BioAPI_HANDLE gModuleHandle;

int main() {

		BioAPI_RETURN bioReturn;
		BioAPI_VERSION bioVersion;

		int index;
		uint32 ArraySize, ElementsNeeded, NumElementsReturned, i;

		printf("Starting Sample Application\n");

		bioVersion.Major = BioAPI_MAJOR;
		bioVersion.Minor = BioAPI_MINOR;
		printf("Major=%d Minor=%d\n", bioVersion.Major, bioVersion.Minor);
		bioReturn = BioAPI_Init(&bioVersion, 0, NULL, 0, NULL);

		if(BioAPI_OK != bioReturn) {
				if(BioAPIERR_H_FRAMEWORK_INCOMPATIBLE_VERSION == bioReturn) {
					printf("This application is not compatible with the installed version of the BioAPI\n");
				} else {
					PrintErrorCode(bioReturn);
				}
				return 0;

		}

		bioReturn = BioAPI_EnumModules(NULL, 0, &ElementsNeeded, 
			&NumElementsReturned);
    if(bioReturn != BioAPI_OK) {
			if (bioReturn == BioAPIERR_H_FRAMEWORK_FUNCTION_FAILED) {
				printf("No BSPs have been installed");
		} else {
			PrintErrorCode(bioReturn);
		}
		return 0;
																													      }
		ArraySize = ElementsNeeded;
		BspSchemaArray = (BioAPI_BSP_SCHEMA *)
			malloc(ElementsNeeded * sizeof(BioAPI_BSP_SCHEMA));
		if(BspSchemaArray == NULL) {
			printf("Unable to allocate BSP list\n");
			return 0;
		}
		bioReturn = BioAPI_EnumModules(BspSchemaArray, ArraySize, &ElementsNeeded, 
										&NumElementsReturned);
		if(bioReturn != BioAPI_OK) {
			free(BspSchemaArray);
			PrintErrorCode(bioReturn);
			return 0;
		}
		CurrSchema = BspSchemaArray;
		for(i = 0; i < NumElementsReturned; i++) {
		  /* Display some details of the installed BSPs */
			printf("BSP Index= %d\n", i);			
			printf("BSP Name: %s\n", CurrSchema->BSPName);
			printf("Description: %s\n", CurrSchema->Description);
			printf("Vendor: %s\n", CurrSchema->Vendor);
			PrintModuleId(CurrSchema->ModuleId);
			printf("Device ID: 0x%.8x\n", CurrSchema->DeviceId);
			CurrSchema++;
		} 
		SetToPasswordBSP(PASSWORD_BSP);
	  printf("Please enter your user id\n");
		fgets(userID, MAX_USER_NAME, stdin);
		// remove the newline
		userID[strlen(userID) - 1] = '\0';
		if (Enroll() == 1) {
			printf("Failed to Enroll--Good Bye\n"), exit(1);
		}
		Verify();
		free(BspSchemaArray);
		BioAPI_Terminate();

		printf("Ending Sample Application\n");

}


void PrintErrorCode(BioAPI_RETURN bioReturn) {
	printf("BioAPI Error Code: %ld (0x%lx)\n", bioReturn, bioReturn);
}

void PrintModuleId(BioAPI_UUID moduleId) {
	int i;
	printf("Module ID: {");
	for (i = 0; i < 16; i++) {
		printf("%.2x", moduleId[i]);
	}
	printf("}\n");
}

void Verify() {
  BioAPI_BOOL bResponse = FALSE;
  BioAPI_BOOL bPrecedence = BioAPI_TRUE;

  BioAPI_BIR_HANDLE CapturedTemplate, ProcessedBir;
  BioAPI_INPUT_BIR birEnroll, birCapture, InputBirProcessed;
 
  BioAPI_RETURN bioReturn;
  BioAPI_BIR_HEADER birHeader;
  BioAPI_FAR MaxFAR, AchievedFAR;
  char userName[MAX_USER_NAME];
  
  if (strlen(userID) == 0) {
			printf("Bad User ID, Please specify a user ID\n");
  } else {
    strcpy(userName, userID);

    if(InputFromFile(userName, &birEnroll) != BioAPI_OK) {
      printf("Bad User ID, User not enrolled");
      return;
    }

    // See if the BSP supports BioAPI_VerifyMatch by checking
    // the operations mask
    if(CurrSchema->Operations & BioAPI_VERIFYMATCH) {
      if((bioReturn = BioAPI_Capture(gModuleHandle, BioAPI_PURPOSE_VERIFY, 
				&CapturedTemplate, -1, NULL)) != BioAPI_OK) {
				PrintErrorCode(bioReturn);
				free(birEnroll.InputBIR.BIR);
				return;
      }

      if((bioReturn = BioAPI_GetHeaderFromHandle(gModuleHandle,
						 CapturedTemplate, &birHeader)) != BioAPI_OK) {
				PrintErrorCode(bioReturn);
				free(birEnroll.InputBIR.BIR);
				return;
      }

      if(birHeader.Type == BioAPI_BIR_DATA_TYPE_INTERMEDIATE) {
				birCapture.Form = BioAPI_BIR_HANDLE_INPUT;
				birCapture.InputBIR.BIRinBSP = &CapturedTemplate;
				if((bioReturn = BioAPI_Process(gModuleHandle, &birCapture,
				       &ProcessedBir)) != BioAPI_OK) {
	  			PrintErrorCode(bioReturn);
	  			free(birEnroll.InputBIR.BIR);
	  			return;
				}
	
				MaxFAR = 1;
				InputBirProcessed.Form = BioAPI_BIR_HANDLE_INPUT;
				InputBirProcessed.InputBIR.BIRinBSP = &ProcessedBir;
      	} else {
					MaxFAR = 1;
					InputBirProcessed.Form = BioAPI_BIR_HANDLE_INPUT;
					InputBirProcessed.InputBIR.BIRinBSP = &CapturedTemplate;
      }
      
      bioReturn = BioAPI_VerifyMatch(gModuleHandle, &MaxFAR, NULL,
				     &bPrecedence, &InputBirProcessed, &birEnroll, NULL,
				     &bResponse, &AchievedFAR, NULL, NULL);
    } else {		// We simply call BioAPI_Verify 
      MaxFAR = 1;
      bioReturn = BioAPI_Verify(gModuleHandle, &MaxFAR, NULL, &bPrecedence,
				&birEnroll, NULL, &bResponse, &AchievedFAR, NULL, NULL, -1, NULL);
    }
    free(birEnroll.InputBIR.BIR);
    
    if(bioReturn != BioAPI_OK) {
      PrintErrorCode(bioReturn);
      return;
    }

		if (bResponse == TRUE) {
			printf("MATCH!\n");
		} else {
			printf("NO MATCH!\n");
		}

  }
}

int Enroll() {
  char userName[MAX_USER_NAME];

  BioAPI_BIR_HANDLE EnrolledTemplate;
  strcpy(userName, userID);

  if (strlen(userID) == 0) {
    printf("Bad User ID, Please specify a user ID\n");
		return 1;
  } else {
    BioAPI_RETURN  bioReturn = BioAPI_Enroll(gModuleHandle,
					     BioAPI_PURPOSE_ENROLL_FOR_VERIFICATION_ONLY,
					     NULL, &EnrolledTemplate, NULL, -1, NULL);
    
    if(bioReturn != BioAPI_OK) {
      PrintErrorCode(bioReturn);
      return 1;
    }
    
    OutputToFile(userName, EnrolledTemplate);
  }
}

void SetToPasswordBSP(int idx) {

  BioAPI_VERSION Version;
  BioAPI_RETURN bioReturn;

  const BioAPI_UUID *uuid;
  uuid = (const BioAPI_UUID *) malloc(sizeof(BioAPI_UUID));

  CurrSchema = BspSchemaArray + idx;
  BioAPI_CopyUuid(uuid, CurrSchema->ModuleId);

  bioReturn = BioAPI_ModuleLoad(uuid, 0, NULL, 0);

  if(BioAPI_OK != bioReturn) {
    PrintErrorCode(bioReturn);
    free(uuid);
    uuid = NULL;
    return;
  }

  Version.Major = BioAPI_MAJOR;
  Version.Minor = BioAPI_MINOR;
  
  bioReturn = BioAPI_ModuleAttach(uuid, &Version, &BioAPIMemoryFuncs,
				  0, 0,0,0, NULL, 0, NULL, &gModuleHandle);

  if(BioAPI_OK != bioReturn) {
    PrintErrorCode(bioReturn);
    BioAPI_ModuleUnload (uuid, NULL, 0);
    free(uuid);
    uuid = NULL;
  }

}

BioAPI_RETURN OutputToFile(char *userName, BioAPI_BIR_HANDLE BirHandle) {
  FILE *hFile;

  unsigned int dwBytesToWrite, dwBytesWritten;
  char fileName[MAX_USER_NAME+4];

  BioAPI_BIR_PTR birData = NULL;
  BioAPI_RETURN bioReturn;
  
  /* Retrieve the BIR from the BSP */
  bioReturn = BioAPI_GetBIRFromHandle(gModuleHandle, BirHandle, &birData);
  if(bioReturn != BioAPI_OK) {
    return(bioReturn);
	}

  /* Open the file */
  strcpy(fileName, strcat(userName, ".bir"));
  hFile = fopen(fileName, "wb+");
  if (hFile == NULL) return(0); /* this return value is ignored anyway */
		
  /* Write the header */
  dwBytesWritten = fwrite(&(birData->Header), 
		sizeof (BioAPI_BIR_HEADER), 1, hFile);

  /* Write the biometric data */
  dwBytesToWrite = LocalEndian4 (birData->Header.Length) - 
		sizeof (BioAPI_BIR_HEADER);
  dwBytesWritten = fwrite(birData->BiometricData, dwBytesToWrite, 1, hFile);

  /* Write the signature if present */
  if (birData->Signature) {
    dwBytesWritten = fwrite(&(birData->Signature->Length), 4, 1, hFile);
    dwBytesToWrite = LocalEndian4 (birData->Signature->Length);
    dwBytesWritten = fwrite(birData->Signature->Data, dwBytesToWrite, 1, hFile);
  }

  /* Close the file */
  fclose(hFile);

  /* Free all the memory allocated for the BIR */
  free(birData->BiometricData);
  if (birData->Signature) {
    free(birData->Signature->Data);
    free(birData->Signature);
  }
  free(birData);

  return(BioAPI_OK);
}

BioAPI_RETURN InputFromFile(char *userName, BioAPI_INPUT_BIR *InputBir) {
  FILE *hFile;
  unsigned int dwBytesToRead, dwBytesRead;
  BioAPI_BIR *bir;

  char fileName[MAX_USER_NAME+4];
  strcpy(fileName, strcat(userName, ".bir"));
  hFile = fopen(fileName, "rb");
  if(hFile == NULL) {
	  // Return this BSP error if the file isn't present
    return BioAPIERR_BSP_RECORD_NOT_FOUND; 
	}

  /* Read the header in, to determine the size */
  dwBytesToRead = sizeof(BioAPI_BIR_HEADER);
  bir = (BioAPI_BIR *)malloc(sizeof (BioAPI_BIR));
  if (!bir) return (!BioAPI_OK);

  dwBytesRead = fread(&(bir->Header), dwBytesToRead, 1, hFile);

  /* Read the biometric data */
  dwBytesToRead = LocalEndian4 (bir->Header.Length) - 
					sizeof (BioAPI_BIR_HEADER);
  bir->BiometricData = (BioAPI_BIR_BIOMETRIC_DATA_PTR)malloc(dwBytesToRead);
  if (!bir->BiometricData)
    return (!BioAPI_OK);
  dwBytesRead = fread(bir->BiometricData, dwBytesToRead, 1, hFile);

  /* Read the signature if present */
  dwBytesToRead = 0;

  dwBytesRead = fread(&dwBytesToRead, 4, 1, hFile);
  if (&dwBytesRead != 0 && dwBytesToRead != 0) {
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
