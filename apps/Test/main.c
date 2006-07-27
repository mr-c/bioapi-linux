/*
 * main.c
 *
 * Simple BioAPI test application
 * This test application performs some basic BioAPI calls to test if the 
 * framework and BSPs have been installed.
 *
 */

#include "bioapi.h"
#include "bioapi_schema.h"
#include "port/bioapi_port.h"
#include "bioapi_uuid.h"
#include "bioapi_util.h"

void PrintErrorCode(BioAPI_RETURN bioReturn);
void PrintModuleId(BioAPI_UUID moduleId);

int main() {

		BioAPI_RETURN bioReturn;
		BioAPI_VERSION bioVersion;
		BioAPI_BSP_SCHEMA *BspSchemaArray;
		BioAPI_BSP_SCHEMA *CurrSchema;

		int index;
		uint32 ArraySize, ElementsNeeded, NumElementsReturned, i;

		printf("Starting BioAPI Test Application\n");

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
			printf("BSP Index= %d\n", i);			
			printf("BSP Name: %s\n", CurrSchema->BSPName);
			printf("Description: %s\n", CurrSchema->Description);
			printf("Vendor: %s\n", CurrSchema->Vendor);
			PrintModuleId(CurrSchema->ModuleId);
			printf("Device ID: 0x%.8x\n", CurrSchema->DeviceId);
			CurrSchema++;
		} 
		free(BspSchemaArray);
		BioAPI_Terminate();

		printf("Ending BioAPI Test Application\n");

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
