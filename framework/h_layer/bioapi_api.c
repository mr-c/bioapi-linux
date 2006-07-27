/*-----------------------------------------------------------------------
 * File: BioAPI_API.C
 *
 *-----------------------------------------------------------------------
 */

#define BioAPI_INITUUID
#include "precomp.h"
#include "mds.h"
#include "../mds_util/mds_util.h"
#include "bioapi_schema.h"
#include "bsp_schema.h"
#include "device_schema.h"
#include "cssmerr.h"

#define BIOAPI_MDS_COMMIT_SIZE 20
extern IfMdsuSchemaManipulator IfiMdsuBioAPIBspCapabilitySchema;
extern IfMdsuSchemaManipulator IfiMdsuBioAPIBspDeviceSchema;

/* Service Functions */

/*************************************************************************/
/*** BioAPI Service Functions ********************************************/
/*************************************************************************/

BioAPI_RETURN BioAPI
BioAPI_FreeBIRHandle(BioAPI_HANDLE ModuleHandle,
					 BioAPI_BIR_HANDLE BIRHandle)
{
	BioAPI_BSP_FUNCS_PTR CallBack = NULL;
	bioapi_INTERNAL_ATTACH_LIST_PTR AttachRecord = NULL;
	BioAPI_RETURN err = BioAPI_OK;

	/* check the caller and do other setup for this SPI call */
	ERR(err = bioapi_SetupStandardSPICall(ModuleHandle,
										(void**)(&CallBack),
										&AttachRecord));
	if(BioAPI_OK == err)
	{
		/*
		 * Call the callback function.
		 * Make sure that function pointer is valid.
		 */
		if(BioAPI_FALSE == port_IsBadCodePtr((BioAPI_PROC_ADDR)CallBack->FreeBIRHandle))
		{
			ERR(err = CallBack->FreeBIRHandle(ModuleHandle,
											  BIRHandle));
		}
		else
		{
			ERR(err = BioAPI_ERRCODE_FUNCTION_NOT_IMPLEMENTED);
		}

		/* cleanup from the SPI call */
		ERR(bioapi_CleanupStandardSPICall(AttachRecord));
	}

	return err;
}

BioAPI_RETURN BioAPI
BioAPI_GetBIRFromHandle(BioAPI_HANDLE ModuleHandle,
						BioAPI_BIR_HANDLE BIRHandle,
						BioAPI_BIR_PTR *BIR)
{
	BioAPI_BSP_FUNCS_PTR CallBack = NULL;
	bioapi_INTERNAL_ATTACH_LIST_PTR AttachRecord = NULL;
	BioAPI_RETURN err = BioAPI_OK;

	/* check the caller and do other setup for this SPI call */
	ERR(err = bioapi_SetupStandardSPICall(ModuleHandle,
										(void**)(&CallBack),
										&AttachRecord));
	if(BioAPI_OK == err)
	{
		/*
		 * Call the callback function.
		 * Make sure that function pointer is valid.
		 */
		if(BioAPI_FALSE == port_IsBadCodePtr((BioAPI_PROC_ADDR)CallBack->GetBIRFromHandle))
		{
			ERR(err = CallBack->GetBIRFromHandle(ModuleHandle,
												 BIRHandle,
												 BIR));
		}
		else
		{
			ERR(err = BioAPI_ERRCODE_FUNCTION_NOT_IMPLEMENTED);
		}

		/* cleanup from the SPI call */
		ERR(bioapi_CleanupStandardSPICall(AttachRecord));
	}

	return err;
}

BioAPI_RETURN BioAPI
BioAPI_GetHeaderFromHandle(BioAPI_HANDLE ModuleHandle,
						   BioAPI_BIR_HANDLE BIRHandle,
						   BioAPI_BIR_HEADER_PTR Header)
{
	BioAPI_BSP_FUNCS_PTR CallBack = NULL;
	bioapi_INTERNAL_ATTACH_LIST_PTR AttachRecord = NULL;
	BioAPI_RETURN err = BioAPI_OK;

	/* check the caller and do other setup for this SPI call */
	ERR(err = bioapi_SetupStandardSPICall(ModuleHandle,
										(void**)(&CallBack),
										&AttachRecord));
	if(BioAPI_OK == err)
	{
		/*
		 * Call the callback function.
		 * Make sure that function pointer is valid.
		 */
		if(BioAPI_FALSE == port_IsBadCodePtr((BioAPI_PROC_ADDR)CallBack->GetHeaderFromHandle))
		{
			ERR(err = CallBack->GetHeaderFromHandle(ModuleHandle,
													BIRHandle,
													Header));
		}
		else
		{
			ERR(err = BioAPI_ERRCODE_FUNCTION_NOT_IMPLEMENTED);
		}

		/* cleanup from the SPI call */
		ERR(bioapi_CleanupStandardSPICall(AttachRecord));
	}

	return err;
}

BioAPI_RETURN BioAPI
BioAPI_EnableEvents(BioAPI_HANDLE ModuleHandle,
					BioAPI_MODULE_EVENT_MASK *Events)
{
	BioAPI_BSP_FUNCS_PTR CallBack = NULL;
	bioapi_INTERNAL_ATTACH_LIST_PTR AttachRecord = NULL;
	BioAPI_RETURN err = BioAPI_OK;

	/* check the caller and do other setup for this SPI call */
	ERR(err = bioapi_SetupStandardSPICall(ModuleHandle,
										(void**)(&CallBack),
										&AttachRecord));
	if(BioAPI_OK == err)
	{
		/*
		 * Call the callback function.
		 * Make sure that function pointer is valid.
		 */
		if(BioAPI_FALSE == port_IsBadCodePtr((BioAPI_PROC_ADDR)CallBack->EnableEvents))
		{
			ERR(err = CallBack->EnableEvents(ModuleHandle,
											 Events));
		}
		else
		{
			ERR(err = BioAPI_ERRCODE_FUNCTION_NOT_IMPLEMENTED);
		}

		/* cleanup from the SPI call */
		ERR(bioapi_CleanupStandardSPICall(AttachRecord));
	}

	return err;
}

BioAPI_RETURN BioAPI
BioAPI_SetGUICallbacks(BioAPI_HANDLE ModuleHandle,
					   BioAPI_GUI_STREAMING_CALLBACK GuiStreamingCallback,
					   void *GuiStreamingCallbackCtx,
					   BioAPI_GUI_STATE_CALLBACK GuiStateCallback,
					   void *GuiStateCallbackCtx)
{
	BioAPI_BSP_FUNCS_PTR CallBack = NULL;
	bioapi_INTERNAL_ATTACH_LIST_PTR AttachRecord = NULL;
	BioAPI_RETURN err = BioAPI_OK;

	/* check the caller and do other setup for this SPI call */
	ERR(err = bioapi_SetupStandardSPICall(ModuleHandle,
										(void**)(&CallBack),
										&AttachRecord));
	if(BioAPI_OK == err)
	{
		/*
		 * Call the callback function.
		 * Make sure that function pointer is valid.
		 */
		if(BioAPI_FALSE == port_IsBadCodePtr((BioAPI_PROC_ADDR)CallBack->SetGUICallbacks))
		{
			ERR(err = CallBack->SetGUICallbacks(ModuleHandle,
												GuiStreamingCallback,
												GuiStreamingCallbackCtx,
												GuiStateCallback,
												GuiStateCallbackCtx));
		}
		else
		{
			ERR(err = BioAPI_ERRCODE_FUNCTION_NOT_IMPLEMENTED);
		}

		/* cleanup from the SPI call */
		ERR(bioapi_CleanupStandardSPICall(AttachRecord));
	}

	return err;
}

BioAPI_RETURN BioAPI
BioAPI_SetStreamCallback(BioAPI_HANDLE ModuleHandle,
						 BioAPI_STREAM_CALLBACK StreamCallback,
						 void *StreamCallbackCtx)
{
	BioAPI_BSP_FUNCS_PTR CallBack = NULL;
	bioapi_INTERNAL_ATTACH_LIST_PTR AttachRecord = NULL;
	BioAPI_RETURN err = BioAPI_OK;

	/* check the caller and do other setup for this SPI call */
	ERR(err = bioapi_SetupStandardSPICall(ModuleHandle,
										(void**)(&CallBack),
										&AttachRecord));
	if(BioAPI_OK == err)
	{
		/*
		 * Call the callback function.
		 * Make sure that function pointer is valid.
		 */
		if(BioAPI_FALSE == port_IsBadCodePtr((BioAPI_PROC_ADDR)CallBack->SetStreamCallback))
		{
			ERR(err = CallBack->SetStreamCallback(ModuleHandle,
												  StreamCallback,
												  StreamCallbackCtx));
		}
		else
		{
			ERR(err = BioAPI_ERRCODE_FUNCTION_NOT_IMPLEMENTED);
		}

		/* cleanup from the SPI call */
		ERR(bioapi_CleanupStandardSPICall(AttachRecord));
	}

	return err;
}

BioAPI_RETURN BioAPI
BioAPI_StreamInputOutput(BioAPI_HANDLE ModuleHandle,
						 BioAPI_DATA_PTR InMessage,
						 BioAPI_DATA_PTR OutMessage)
{
	BioAPI_BSP_FUNCS_PTR CallBack = NULL;
	bioapi_INTERNAL_ATTACH_LIST_PTR AttachRecord = NULL;
	BioAPI_RETURN err = BioAPI_OK;

	/* check the caller and do other setup for this SPI call */
	ERR(err = bioapi_SetupStandardSPICall(ModuleHandle,
										(void**)(&CallBack),
										&AttachRecord));
	if(BioAPI_OK == err)
	{
		/*
		 * Call the callback function.
		 * Make sure that function pointer is valid.
		 */
		if(BioAPI_FALSE == port_IsBadCodePtr((BioAPI_PROC_ADDR)CallBack->StreamInputOutput))
		{
			ERR(err = CallBack->StreamInputOutput(ModuleHandle,
												  InMessage,
												  OutMessage));
		}
		else
		{
			ERR(err = BioAPI_ERRCODE_FUNCTION_NOT_IMPLEMENTED);
		}

		/* cleanup from the SPI call */
		ERR(bioapi_CleanupStandardSPICall(AttachRecord));
	}

	return err;
}

BioAPI_RETURN BioAPI
BioAPI_Capture(BioAPI_HANDLE ModuleHandle,
			   BioAPI_BIR_PURPOSE Purpose,
			   BioAPI_BIR_HANDLE_PTR CapturedBIR,
			   sint32 Timeout,
			   BioAPI_BIR_HANDLE_PTR AuditData)
{
	BioAPI_BSP_FUNCS_PTR CallBack = NULL;
	bioapi_INTERNAL_ATTACH_LIST_PTR AttachRecord = NULL;
	BioAPI_RETURN err = BioAPI_OK;

	/* check the caller and do other setup for this SPI call */
	ERR(err = bioapi_SetupStandardSPICall(ModuleHandle,
										(void**)(&CallBack),
										&AttachRecord));
	if(BioAPI_OK == err)
	{
		/*
		 * Call the callback function.
		 * Make sure that function pointer is valid.
		 */
		if(BioAPI_FALSE == port_IsBadCodePtr((BioAPI_PROC_ADDR)CallBack->Capture))
		{
			ERR(err = CallBack->Capture(ModuleHandle,
										Purpose,
										CapturedBIR,
										Timeout,
										AuditData));
		}
		else
		{
			ERR(err = BioAPI_ERRCODE_FUNCTION_NOT_IMPLEMENTED);
		}

		/* cleanup from the SPI call */
		ERR(bioapi_CleanupStandardSPICall(AttachRecord));
	}

	return err;
}

BioAPI_RETURN BioAPI
BioAPI_CreateTemplate(BioAPI_HANDLE ModuleHandle,
					  const BioAPI_INPUT_BIR *CapturedBIR,
					  const BioAPI_INPUT_BIR *StoredTemplate,
					  BioAPI_BIR_HANDLE_PTR NewTemplate,
					  const BioAPI_DATA *Payload)
{
	BioAPI_BSP_FUNCS_PTR CallBack = NULL;
	bioapi_INTERNAL_ATTACH_LIST_PTR AttachRecord = NULL;
	BioAPI_RETURN err = BioAPI_OK;

	/* check the caller and do other setup for this SPI call */
	ERR(err = bioapi_SetupStandardSPICall(ModuleHandle,
										(void**)(&CallBack),
										&AttachRecord));
	if(BioAPI_OK == err)
	{
		/*
		 * Call the callback function.
		 * Make sure that function pointer is valid.
		 */
		if(BioAPI_FALSE == port_IsBadCodePtr((BioAPI_PROC_ADDR)CallBack->CreateTemplate))
		{
			ERR(err = CallBack->CreateTemplate(ModuleHandle,
											   CapturedBIR,
											   StoredTemplate,
											   NewTemplate,
											   Payload));
		}
		else
		{
			ERR(err = BioAPI_ERRCODE_FUNCTION_NOT_IMPLEMENTED);
		}

		/* cleanup from the SPI call */
		ERR(bioapi_CleanupStandardSPICall(AttachRecord));
	}

	return err;
}

BioAPI_RETURN BioAPI
BioAPI_Process(BioAPI_HANDLE ModuleHandle,
			   const BioAPI_INPUT_BIR *CapturedBIR,
			   BioAPI_BIR_HANDLE_PTR ProcessedBIR)
{
	BioAPI_BSP_FUNCS_PTR CallBack = NULL;
	bioapi_INTERNAL_ATTACH_LIST_PTR AttachRecord = NULL;
	BioAPI_RETURN err = BioAPI_OK;

	/* check the caller and do other setup for this SPI call */
	ERR(err = bioapi_SetupStandardSPICall(ModuleHandle,
										(void**)(&CallBack),
										&AttachRecord));
	if(BioAPI_OK == err)
	{
		/*
		 * Call the callback function.
		 * Make sure that function pointer is valid.
		 */
		if(BioAPI_FALSE == port_IsBadCodePtr((BioAPI_PROC_ADDR)CallBack->Process))
		{
			ERR(err = CallBack->Process(ModuleHandle,
										CapturedBIR,
										ProcessedBIR));
		}
		else
		{
			ERR(err = BioAPI_ERRCODE_FUNCTION_NOT_IMPLEMENTED);
		}

		/* cleanup from the SPI call */
		ERR(bioapi_CleanupStandardSPICall(AttachRecord));
	}

	return err;
}

BioAPI_RETURN BioAPI
BioAPI_VerifyMatch(BioAPI_HANDLE ModuleHandle,
				   const BioAPI_FAR *MaxFARRequested,
				   const BioAPI_FRR *MaxFRRRequested,
				   const BioAPI_BOOL *FARPrecedence,
				   const BioAPI_INPUT_BIR *ProcessedBIR,
				   const BioAPI_INPUT_BIR *StoredTemplate,
				   BioAPI_BIR_HANDLE *AdaptedBIR,
				   BioAPI_BOOL *Result,
				   BioAPI_FAR_PTR FARAchieved,
				   BioAPI_FRR_PTR FRRAchieved,
				   BioAPI_DATA_PTR *Payload)
{
	BioAPI_BSP_FUNCS_PTR CallBack = NULL;
	bioapi_INTERNAL_ATTACH_LIST_PTR AttachRecord = NULL;
	BioAPI_RETURN err = BioAPI_OK;

	/* check the caller and do other setup for this SPI call */
	ERR(err = bioapi_SetupStandardSPICall(ModuleHandle,
										(void**)(&CallBack),
										&AttachRecord));
	if(BioAPI_OK == err)
	{
		/*
		 * Call the callback function.
		 * Make sure that function pointer is valid.
		 */
		if(BioAPI_FALSE == port_IsBadCodePtr((BioAPI_PROC_ADDR)CallBack->VerifyMatch))
		{
			ERR(err = CallBack->VerifyMatch(ModuleHandle,
											MaxFARRequested,
											MaxFRRRequested,
											FARPrecedence,
											ProcessedBIR,
											StoredTemplate,
											AdaptedBIR,
											Result,
											FARAchieved,
											FRRAchieved,
											Payload));
		}
		else
		{
			ERR(err = BioAPI_ERRCODE_FUNCTION_NOT_IMPLEMENTED);
		}

		/* cleanup from the SPI call */
		ERR(bioapi_CleanupStandardSPICall(AttachRecord));
	}

	return err;
}

BioAPI_RETURN BioAPI
BioAPI_IdentifyMatch(BioAPI_HANDLE ModuleHandle,
					 const BioAPI_FAR *MaxFARRequested,
					 const BioAPI_FRR *MaxFRRRequested,
					 const BioAPI_BOOL *FARPrecedence,
					 const BioAPI_INPUT_BIR *ProcessedBIR,
					 const BioAPI_IDENTIFY_POPULATION *Population,
					 BioAPI_BOOL Binning,
					 uint32 MaxNumberOfResults,
					 uint32 *NumberOfResults,
					 BioAPI_CANDIDATE_ARRAY_PTR *Candidates,
					 sint32 Timeout)
{
	BioAPI_BSP_FUNCS_PTR CallBack = NULL;
	bioapi_INTERNAL_ATTACH_LIST_PTR AttachRecord = NULL;
	BioAPI_RETURN err = BioAPI_OK;

	/* check the caller and do other setup for this SPI call */
	ERR(err = bioapi_SetupStandardSPICall(ModuleHandle,
										(void**)(&CallBack),
										&AttachRecord));
	if(BioAPI_OK == err)
	{
		/*
		 * Call the callback function.
		 * Make sure that function pointer is valid.
		 */
		if(BioAPI_FALSE == port_IsBadCodePtr((BioAPI_PROC_ADDR)CallBack->IdentifyMatch))
		{
			ERR(err = CallBack->IdentifyMatch(ModuleHandle,
											  MaxFARRequested,
											  MaxFRRRequested,
											  FARPrecedence,
											  ProcessedBIR,
											  Population,
											  Binning,
											  MaxNumberOfResults,
											  NumberOfResults,
											  Candidates,
											  Timeout));
		}
		else
		{
			ERR(err = BioAPI_ERRCODE_FUNCTION_NOT_IMPLEMENTED);
		}

		/* cleanup from the SPI call */
		ERR(bioapi_CleanupStandardSPICall(AttachRecord));
	}

	return err;
}

BioAPI_RETURN BioAPI
BioAPI_Enroll(BioAPI_HANDLE ModuleHandle,
			  BioAPI_BIR_PURPOSE Purpose,
			  const BioAPI_INPUT_BIR *StoredTemplate,
			  BioAPI_BIR_HANDLE_PTR NewTemplate,
			  const BioAPI_DATA *Payload,
			  sint32 Timeout,
			  BioAPI_BIR_HANDLE_PTR AuditData)
{
	BioAPI_BSP_FUNCS_PTR CallBack = NULL;
	bioapi_INTERNAL_ATTACH_LIST_PTR AttachRecord = NULL;
	BioAPI_RETURN err = BioAPI_OK;

	/* check the caller and do other setup for this SPI call */
	ERR(err = bioapi_SetupStandardSPICall(ModuleHandle,
										(void**)(&CallBack),
										&AttachRecord));
	if(BioAPI_OK == err)
	{
		/*
		 * Call the callback function.
		 * Make sure that function pointer is valid.
		 */
		if(BioAPI_FALSE == port_IsBadCodePtr((BioAPI_PROC_ADDR)CallBack->Enroll))
		{
			ERR(err = CallBack->Enroll(ModuleHandle,
									   Purpose,
									   StoredTemplate,
									   NewTemplate,
									   Payload,
									   Timeout,
									   AuditData));
		}
		else
		{
			ERR(err = BioAPI_ERRCODE_FUNCTION_NOT_IMPLEMENTED);
		}

		/* cleanup from the SPI call */
		ERR(bioapi_CleanupStandardSPICall(AttachRecord));
	}

	return err;
}

BioAPI_RETURN BioAPI
BioAPI_Verify(BioAPI_HANDLE ModuleHandle,
			  const BioAPI_FAR *MaxFARRequested,
			  const BioAPI_FRR *MaxFRRRequested,
			  const BioAPI_BOOL *FARPrecedence,
			  const BioAPI_INPUT_BIR *StoredTemplate,
			  BioAPI_BIR_HANDLE_PTR AdaptedBIR,
			  BioAPI_BOOL *Result,
			  BioAPI_FAR_PTR FARAchieved,
			  BioAPI_FRR_PTR FRRAchieved,
			  BioAPI_DATA_PTR *Payload,
			  sint32 Timeout,
			  BioAPI_BIR_HANDLE_PTR AuditData)
{
	BioAPI_BSP_FUNCS_PTR CallBack = NULL;
	bioapi_INTERNAL_ATTACH_LIST_PTR AttachRecord = NULL;
	BioAPI_RETURN err = BioAPI_OK;

	/* check the caller and do other setup for this SPI call */
	ERR(err = bioapi_SetupStandardSPICall(ModuleHandle,
										(void**)(&CallBack),
										&AttachRecord));
	if(BioAPI_OK == err)
	{
		/*
		 * Call the callback function.
		 * Make sure that function pointer is valid.
		 */
		if(BioAPI_FALSE == port_IsBadCodePtr((BioAPI_PROC_ADDR)CallBack->Verify))
		{
			ERR(err = CallBack->Verify(ModuleHandle,
									   MaxFARRequested,
									   MaxFRRRequested,
									   FARPrecedence,
									   StoredTemplate,
									   AdaptedBIR,
									   Result,
									   FARAchieved,
									   FRRAchieved,
									   Payload,
									   Timeout,
									   AuditData));
		}
		else
		{
			ERR(err = BioAPI_ERRCODE_FUNCTION_NOT_IMPLEMENTED);
		}

		/* cleanup from the SPI call */
		ERR(bioapi_CleanupStandardSPICall(AttachRecord));
	}

	return err;
}

BioAPI_RETURN BioAPI
BioAPI_Identify(BioAPI_HANDLE ModuleHandle,
				const BioAPI_FAR *MaxFARRequested,
				const BioAPI_FRR *MaxFRRRequested,
				const BioAPI_BOOL *FARPrecedence,
				const BioAPI_IDENTIFY_POPULATION *Population,
				BioAPI_BOOL Binning,
				uint32 MaxNumberOfResults,
				uint32 *NumberOfResults,
				BioAPI_CANDIDATE_ARRAY_PTR *Candidates,
				sint32 Timeout,
				BioAPI_BIR_HANDLE_PTR AuditData)
{
	BioAPI_BSP_FUNCS_PTR CallBack = NULL;
	bioapi_INTERNAL_ATTACH_LIST_PTR AttachRecord = NULL;
	BioAPI_RETURN err = BioAPI_OK;

	/* check the caller and do other setup for this SPI call */
	ERR(err = bioapi_SetupStandardSPICall(ModuleHandle,
										(void**)(&CallBack),
										&AttachRecord));
	if(BioAPI_OK == err)
	{
		/*
		 * Call the callback function.
		 * Make sure that function pointer is valid.
		 */
		if(BioAPI_FALSE == port_IsBadCodePtr((BioAPI_PROC_ADDR)CallBack->Identify))
		{
			ERR(err = CallBack->Identify(ModuleHandle,
										 MaxFARRequested,
										 MaxFRRRequested,
										 FARPrecedence,
										 Population,
										 Binning,
										 MaxNumberOfResults,
										 NumberOfResults,
										 Candidates,
										 Timeout,
										 AuditData));
		}
		else
		{
			ERR(err = BioAPI_ERRCODE_FUNCTION_NOT_IMPLEMENTED);
		}

		/* cleanup from the SPI call */
		ERR(bioapi_CleanupStandardSPICall(AttachRecord));
	}

	return err;
}

BioAPI_RETURN BioAPI
BioAPI_Import(BioAPI_HANDLE ModuleHandle,
			  const BioAPI_DATA *InputData,
			  BioAPI_BIR_BIOMETRIC_DATA_FORMAT InputFormat,
			  BioAPI_BIR_PURPOSE Purpose,
			  BioAPI_BIR_HANDLE_PTR ConstructedBIR)
{
	BioAPI_BSP_FUNCS_PTR CallBack = NULL;
	bioapi_INTERNAL_ATTACH_LIST_PTR AttachRecord = NULL;
	BioAPI_RETURN err = BioAPI_OK;

	/* check the caller and do other setup for this SPI call */
	ERR(err = bioapi_SetupStandardSPICall(ModuleHandle,
										(void**)(&CallBack),
										&AttachRecord));
	if(BioAPI_OK == err)
	{
		/*
		 * Call the callback function.
		 * Make sure that function pointer is valid.
		 */
		if(BioAPI_FALSE == port_IsBadCodePtr((BioAPI_PROC_ADDR)CallBack->Import))
		{
			ERR(err = CallBack->Import(ModuleHandle,
									   InputData,
									   InputFormat,
									   Purpose,
									   ConstructedBIR));
		}
		else
		{
			ERR(err = BioAPI_ERRCODE_FUNCTION_NOT_IMPLEMENTED);
		}

		/* cleanup from the SPI call */
		ERR(bioapi_CleanupStandardSPICall(AttachRecord));
	}

	return err;
}

BioAPI_RETURN BioAPI
BioAPI_SetPowerMode(BioAPI_HANDLE ModuleHandle,
					BioAPI_POWER_MODE PowerMode)
{
	BioAPI_BSP_FUNCS_PTR CallBack = NULL;
	bioapi_INTERNAL_ATTACH_LIST_PTR AttachRecord = NULL;
	BioAPI_RETURN err = BioAPI_OK;

	/* check the caller and do other setup for this SPI call */
	ERR(err = bioapi_SetupStandardSPICall(ModuleHandle,
										(void**)(&CallBack),
										&AttachRecord));
	if(BioAPI_OK == err)
	{
		/*
		 * Call the callback function.
		 * Make sure that function pointer is valid.
		 */
		if(BioAPI_FALSE == port_IsBadCodePtr((BioAPI_PROC_ADDR)CallBack->SetPowerMode))
		{
			ERR(err = CallBack->SetPowerMode(ModuleHandle,
											 PowerMode));
		}
		else
		{
			ERR(err = BioAPI_ERRCODE_FUNCTION_NOT_IMPLEMENTED);
		}

		/* cleanup from the SPI call */
		ERR(bioapi_CleanupStandardSPICall(AttachRecord));
	}

	return err;
}

BioAPI_RETURN BioAPI
BioAPI_DbOpen(BioAPI_HANDLE ModuleHandle,
			  const uint8 *DbName,
			  BioAPI_DB_ACCESS_TYPE AccessRequest,
			  BioAPI_DB_HANDLE_PTR DbHandle,
			  BioAPI_DB_CURSOR_PTR Cursor)
{
	BioAPI_BSP_FUNCS_PTR CallBack = NULL;
	bioapi_INTERNAL_ATTACH_LIST_PTR AttachRecord = NULL;
	BioAPI_RETURN err = BioAPI_OK;

	/* check the caller and do other setup for this SPI call */
	ERR(err = bioapi_SetupStandardSPICall(ModuleHandle,
										(void**)(&CallBack),
										&AttachRecord));
	if(BioAPI_OK == err)
	{
		/*
		 * Call the callback function.
		 * Make sure that function pointer is valid.
		 */
		if(BioAPI_FALSE == port_IsBadCodePtr((BioAPI_PROC_ADDR)CallBack->DbOpen))
		{
			ERR(err = CallBack->DbOpen(ModuleHandle,
									   DbName,
									   AccessRequest,
									   DbHandle,
									   Cursor));
		}
		else
		{
			ERR(err = BioAPI_ERRCODE_FUNCTION_NOT_IMPLEMENTED);
		}

		/* cleanup from the SPI call */
		ERR(bioapi_CleanupStandardSPICall(AttachRecord));
	}

	return err;
}

BioAPI_RETURN BioAPI
BioAPI_DbClose(BioAPI_HANDLE ModuleHandle,
			   BioAPI_DB_HANDLE DbHandle)
{
	BioAPI_BSP_FUNCS_PTR CallBack = NULL;
	bioapi_INTERNAL_ATTACH_LIST_PTR AttachRecord = NULL;
	BioAPI_RETURN err = BioAPI_OK;

	/* check the caller and do other setup for this SPI call */
	ERR(err = bioapi_SetupStandardSPICall(ModuleHandle,
										(void**)(&CallBack),
										&AttachRecord));
	if(BioAPI_OK == err)
	{
		/*
		 * Call the callback function.
		 * Make sure that function pointer is valid.
		 */
		if(BioAPI_FALSE == port_IsBadCodePtr((BioAPI_PROC_ADDR)CallBack->DbClose))
		{
			ERR(err = CallBack->DbClose(ModuleHandle,
										DbHandle));
		}
		else
		{
			ERR(err = BioAPI_ERRCODE_FUNCTION_NOT_IMPLEMENTED);
		}

		/* cleanup from the SPI call */
		ERR(bioapi_CleanupStandardSPICall(AttachRecord));
	}

	return err;
}

BioAPI_RETURN BioAPI
BioAPI_DbCreate(BioAPI_HANDLE ModuleHandle,
				const uint8 *DbName,
				BioAPI_DB_ACCESS_TYPE AccessRequest,
				BioAPI_DB_HANDLE_PTR DbHandle)
{
	BioAPI_BSP_FUNCS_PTR CallBack = NULL;
	bioapi_INTERNAL_ATTACH_LIST_PTR AttachRecord = NULL;
	BioAPI_RETURN err = BioAPI_OK;

	/* check the caller and do other setup for this SPI call */
	ERR(err = bioapi_SetupStandardSPICall(ModuleHandle,
										(void**)(&CallBack),
										&AttachRecord));
	if(BioAPI_OK == err)
	{
		/*
		 * Call the callback function.
		 * Make sure that function pointer is valid.
		 */
		if(BioAPI_FALSE == port_IsBadCodePtr((BioAPI_PROC_ADDR)CallBack->DbCreate))
		{
			ERR(err = CallBack->DbCreate(ModuleHandle,
										 DbName,
										 AccessRequest,
										 DbHandle));
		}
		else
		{
			ERR(err = BioAPI_ERRCODE_FUNCTION_NOT_IMPLEMENTED);
		}

		/* cleanup from the SPI call */
		ERR(bioapi_CleanupStandardSPICall(AttachRecord));
	}

	return err;
}

BioAPI_RETURN BioAPI
BioAPI_DbDelete(BioAPI_HANDLE ModuleHandle,
				const uint8 *DbName)
{
	BioAPI_BSP_FUNCS_PTR CallBack = NULL;
	bioapi_INTERNAL_ATTACH_LIST_PTR AttachRecord = NULL;
	BioAPI_RETURN err = BioAPI_OK;

	/* check the caller and do other setup for this SPI call */
	ERR(err = bioapi_SetupStandardSPICall(ModuleHandle,
										(void**)(&CallBack),
										&AttachRecord));
	if(BioAPI_OK == err)
	{
		/*
		 * Call the callback function.
		 * Make sure that function pointer is valid.
		 */
		if(BioAPI_FALSE == port_IsBadCodePtr((BioAPI_PROC_ADDR)CallBack->DbDelete))
		{
			ERR(err = CallBack->DbDelete(ModuleHandle,
										 DbName));
		}
		else
		{
			ERR(err = BioAPI_ERRCODE_FUNCTION_NOT_IMPLEMENTED);
		}

		/* cleanup from the SPI call */
		ERR(bioapi_CleanupStandardSPICall(AttachRecord));
	}

	return err;
}

BioAPI_RETURN BioAPI
BioAPI_DbSetCursor(BioAPI_HANDLE ModuleHandle,
				   BioAPI_DB_HANDLE DbHandle,
				   const BioAPI_UUID *KeyValue,
				   BioAPI_DB_CURSOR_PTR Cursor)
{
	BioAPI_BSP_FUNCS_PTR CallBack = NULL;
	bioapi_INTERNAL_ATTACH_LIST_PTR AttachRecord = NULL;
	BioAPI_RETURN err = BioAPI_OK;

	/* check the caller and do other setup for this SPI call */
	ERR(err = bioapi_SetupStandardSPICall(ModuleHandle,
										(void**)(&CallBack),
										&AttachRecord));
	if(BioAPI_OK == err)
	{
		/*
		 * Call the callback function.
		 * Make sure that function pointer is valid.
		 */
		if(BioAPI_FALSE == port_IsBadCodePtr((BioAPI_PROC_ADDR)CallBack->DbSetCursor))
		{
			ERR(err = CallBack->DbSetCursor(ModuleHandle,
											DbHandle,
											KeyValue,
											Cursor));
		}
		else
		{
			ERR(err = BioAPI_ERRCODE_FUNCTION_NOT_IMPLEMENTED);
		}

		/* cleanup from the SPI call */
		ERR(bioapi_CleanupStandardSPICall(AttachRecord));
	}

	return err;
}

BioAPI_RETURN BioAPI
BioAPI_DbFreeCursor(BioAPI_HANDLE ModuleHandle,
					BioAPI_DB_CURSOR_PTR Cursor)
{
	BioAPI_BSP_FUNCS_PTR CallBack = NULL;
	bioapi_INTERNAL_ATTACH_LIST_PTR AttachRecord = NULL;
	BioAPI_RETURN err = BioAPI_OK;

	/* check the caller and do other setup for this SPI call */
	ERR(err = bioapi_SetupStandardSPICall(ModuleHandle,
										(void**)(&CallBack),
										&AttachRecord));
	if(BioAPI_OK == err)
	{
		/*
		 * Call the callback function.
		 * Make sure that function pointer is valid.
		 */
		if(BioAPI_FALSE == port_IsBadCodePtr((BioAPI_PROC_ADDR)CallBack->DbFreeCursor))
		{
			ERR(err = CallBack->DbFreeCursor(ModuleHandle,
											 Cursor));
		}
		else
		{
			ERR(err = BioAPI_ERRCODE_FUNCTION_NOT_IMPLEMENTED);
		}

		/* cleanup from the SPI call */
		ERR(bioapi_CleanupStandardSPICall(AttachRecord));
	}

	return err;
}

BioAPI_RETURN BioAPI
BioAPI_DbStoreBIR(BioAPI_HANDLE ModuleHandle,
				  const BioAPI_INPUT_BIR *BIRToStore,
				  BioAPI_DB_HANDLE DbHandle,
				  BioAPI_UUID_PTR Uuid)
{
	BioAPI_BSP_FUNCS_PTR CallBack = NULL;
	bioapi_INTERNAL_ATTACH_LIST_PTR AttachRecord = NULL;
	BioAPI_RETURN err = BioAPI_OK;

	/* check the caller and do other setup for this SPI call */
	ERR(err = bioapi_SetupStandardSPICall(ModuleHandle,
										(void**)(&CallBack),
										&AttachRecord));
	if(BioAPI_OK == err)
	{
		/*
		 * Call the callback function.
		 * Make sure that function pointer is valid.
		 */
		if(BioAPI_FALSE == port_IsBadCodePtr((BioAPI_PROC_ADDR)CallBack->DbStoreBIR))
		{
			ERR(err = CallBack->DbStoreBIR(ModuleHandle,
										   BIRToStore,
										   DbHandle,
										   Uuid));
		}
		else
		{
			ERR(err = BioAPI_ERRCODE_FUNCTION_NOT_IMPLEMENTED);
		}

		/* cleanup from the SPI call */
		ERR(bioapi_CleanupStandardSPICall(AttachRecord));
	}

	return err;
}

BioAPI_RETURN BioAPI
BioAPI_DbGetBIR(BioAPI_HANDLE ModuleHandle,
				BioAPI_DB_HANDLE DbHandle,
				const BioAPI_UUID *KeyValue,
				BioAPI_BIR_HANDLE_PTR RetrievedBIR,
				BioAPI_DB_CURSOR_PTR Cursor)
{
	BioAPI_BSP_FUNCS_PTR CallBack = NULL;
	bioapi_INTERNAL_ATTACH_LIST_PTR AttachRecord = NULL;
	BioAPI_RETURN err = BioAPI_OK;

	/* check the caller and do other setup for this SPI call */
	ERR(err = bioapi_SetupStandardSPICall(ModuleHandle,
										(void**)(&CallBack),
										&AttachRecord));
	if(BioAPI_OK == err)
	{
		/*
		 * Call the callback function.
		 * Make sure that function pointer is valid.
		 */
		if(BioAPI_FALSE == port_IsBadCodePtr((BioAPI_PROC_ADDR)CallBack->DbGetBIR))
		{
			ERR(err = CallBack->DbGetBIR(ModuleHandle,
										 DbHandle,
										 KeyValue,
										 RetrievedBIR,
										 Cursor));
		}
		else
		{
			ERR(err = BioAPI_ERRCODE_FUNCTION_NOT_IMPLEMENTED);
		}

		/* cleanup from the SPI call */
		ERR(bioapi_CleanupStandardSPICall(AttachRecord));
	}

	return err;
}

BioAPI_RETURN BioAPI
BioAPI_DbGetNextBIR(BioAPI_HANDLE ModuleHandle,
					BioAPI_DB_CURSOR_PTR Cursor,
					BioAPI_BIR_HANDLE_PTR RetievedBIR,
					BioAPI_UUID_PTR Uuid)
{
	BioAPI_BSP_FUNCS_PTR CallBack = NULL;
	bioapi_INTERNAL_ATTACH_LIST_PTR AttachRecord = NULL;
	BioAPI_RETURN err = BioAPI_OK;

	/* check the caller and do other setup for this SPI call */
	ERR(err = bioapi_SetupStandardSPICall(ModuleHandle,
										(void**)(&CallBack),
										&AttachRecord));
	if(BioAPI_OK == err)
	{
		/*
		 * Call the callback function.
		 * Make sure that function pointer is valid.
		 */
		if(BioAPI_FALSE == port_IsBadCodePtr((BioAPI_PROC_ADDR)CallBack->DbGetNextBIR))
		{
			ERR(err = CallBack->DbGetNextBIR(ModuleHandle,
											 Cursor,
											 RetievedBIR,
											 Uuid));
		}
		else
		{
			ERR(err = BioAPI_ERRCODE_FUNCTION_NOT_IMPLEMENTED);
		}

		/* cleanup from the SPI call */
		ERR(bioapi_CleanupStandardSPICall(AttachRecord));
	}

	return err;
}

BioAPI_RETURN BioAPI
BioAPI_DbQueryBIR(BioAPI_HANDLE ModuleHandle,
				  BioAPI_DB_HANDLE DbHandle,
				  const BioAPI_INPUT_BIR *BIRToQuery,
				  BioAPI_UUID_PTR Uuid)
{
	BioAPI_BSP_FUNCS_PTR CallBack = NULL;
	bioapi_INTERNAL_ATTACH_LIST_PTR AttachRecord = NULL;
	BioAPI_RETURN err = BioAPI_OK;

	/* check the caller and do other setup for this SPI call */
	ERR(err = bioapi_SetupStandardSPICall(ModuleHandle,
										(void**)(&CallBack),
										&AttachRecord));
	if(BioAPI_OK == err)
	{
		/*
		 * Call the callback function.
		 * Make sure that function pointer is valid.
		 */
		if(BioAPI_FALSE == port_IsBadCodePtr((BioAPI_PROC_ADDR)CallBack->DbQueryBIR))
		{
			ERR(err = CallBack->DbQueryBIR(ModuleHandle,
										   DbHandle,
										   BIRToQuery,
										   Uuid));
		}
		else
		{
			ERR(err = BioAPI_ERRCODE_FUNCTION_NOT_IMPLEMENTED);
		}

		/* cleanup from the SPI call */
		ERR(bioapi_CleanupStandardSPICall(AttachRecord));
	}

	return err;
}

BioAPI_RETURN BioAPI
BioAPI_DbDeleteBIR(BioAPI_HANDLE ModuleHandle,
				   BioAPI_DB_HANDLE DbHandle,
				   const BioAPI_UUID *KeyValue)
{
	BioAPI_BSP_FUNCS_PTR CallBack = NULL;
	bioapi_INTERNAL_ATTACH_LIST_PTR AttachRecord = NULL;
	BioAPI_RETURN err = BioAPI_OK;

	/* check the caller and do other setup for this SPI call */
	ERR(err = bioapi_SetupStandardSPICall(ModuleHandle,
										(void**)(&CallBack),
										&AttachRecord));
	if(BioAPI_OK == err)
	{
		/*
		 * Call the callback function.
		 * Make sure that function pointer is valid.
		 */
		if(BioAPI_FALSE == port_IsBadCodePtr((BioAPI_PROC_ADDR)CallBack->DbDeleteBIR))
		{
			ERR(err = CallBack->DbDeleteBIR(ModuleHandle,
											DbHandle,
											KeyValue));
		}
		else
		{
			ERR(err = BioAPI_ERRCODE_FUNCTION_NOT_IMPLEMENTED);
		}

		/* cleanup from the SPI call */
		ERR(bioapi_CleanupStandardSPICall(AttachRecord));
	}

	return err;
}

BioAPI_RETURN BioAPI
BioAPI_EnumModules(BioAPI_BSP_SCHEMA * BspSchemaArray,
									uint32 ArraySize,
									uint32 * ElementsNeeded,
									uint32 * NumElementsReturned)

{
	BioAPI_RETURN err;
	MDSU_CONTEXT MdsuContext;
	BioAPI_BSP_SCHEMA BioAPICapTemplate;
	BioAPI_BSP_SCHEMA BioAPICapData;
	BioAPI_BSP_SCHEMA * CurrentSchemaPtr;
	uint32 Count;

	/* Initialize in case of early return (usually caused by no installed modules) */
	*NumElementsReturned = 0;
	*ElementsNeeded = 0;

	if((err = MDSU_Init(&MdsuContext,
					&BioAPIMemoryFuncs,
					&bioapi_UUID,
					MDS_CDSA_DIRECTORY,
					(CSSM_DB_ACCESS_READ | CSSM_DB_ACCESS_WRITE),
					BIOAPI_MDS_COMMIT_SIZE)) != BioAPI_OK)
		return err;

	/* Handle the simple case first...NULL passed for BspSchemaArray */
	if(BspSchemaArray == NULL)
	{
		Count = 0;
		BioAPICapTemplate.SpecVersion.Major = BioAPI_MAJOR;
		BioAPICapTemplate.SpecVersion.Minor = BioAPI_MINOR;
		err = MDSU_FindFirst(&MdsuContext,
								 &IfiMdsuBioAPIBspCapabilitySchema,
								 &BioAPICapTemplate,
								 MDSU_BIOAPI_BSP_CAP_SPECVERSION,
								 &BioAPICapData,
								 NULL);
		if (CSSM_OK != err)
		{
			MDSU_Term( &MdsuContext );
			if (CSSMERR_DL_ENDOFDATA == err)
				return BioAPIERR_H_FRAMEWORK_FUNCTION_FAILED;
			return err;
		}
		while(CSSM_OK == err)
		{
			Count ++;
			err = MDSU_FindNext(&MdsuContext,
								&BioAPICapData,
								NULL);
		}
		*ElementsNeeded = Count;
	}
	else
	{
		if(ArraySize == 0)
			return BioAPI_ERRCODE_INVALID_DATA;
		Count = 0;
		CurrentSchemaPtr = BspSchemaArray;
		BioAPICapTemplate.SpecVersion.Major = BioAPI_MAJOR;
		BioAPICapTemplate.SpecVersion.Minor = BioAPI_MINOR;
		err = MDSU_FindFirst(&MdsuContext,
								 &IfiMdsuBioAPIBspCapabilitySchema,
								 &BioAPICapTemplate,
								 MDSU_BIOAPI_BSP_CAP_SPECVERSION,
								 &BioAPICapData,
								 NULL);
		if (CSSM_OK != err)
		{
			MDSU_Term( &MdsuContext );
			return err;
		}
		while(CSSM_OK == err)
		{
			Count ++;
			if(Count <= ArraySize)
			{
				memcpy(CurrentSchemaPtr, &BioAPICapData, sizeof(BioAPI_BSP_SCHEMA));
				CurrentSchemaPtr++;
			}
			err = MDSU_FindNext(&MdsuContext,
								&BioAPICapData,
								NULL);
		}
		if(Count > ArraySize)
		{
			*NumElementsReturned = ArraySize;
		}
		else
		{
			*NumElementsReturned = Count;
		}
		*ElementsNeeded = Count;
	}
	MDSU_Term( &MdsuContext );
	return (BioAPI_OK);

}

BioAPI_RETURN BioAPI
BioAPI_EnumDevices(const BioAPI_UUID *uuid,
									BioAPI_DEVICE_SCHEMA * DeviceSchemaArray,
									uint32 ArraySize,
									uint32 * ElementsNeeded,
									uint32 * NumElementsReturned)

{
	BioAPI_RETURN err;
	MDSU_CONTEXT MdsuContext;
	BioAPI_DEVICE_SCHEMA DeviceTemplate;
	BioAPI_DEVICE_SCHEMA DeviceData;
	BioAPI_DEVICE_SCHEMA * CurrentSchemaPtr;
	uint32 Count;

	/* Initialize in case of early return (usually caused by no installed devices) */
	*NumElementsReturned = 0;
	*ElementsNeeded = 0;

	if((err = MDSU_Init(&MdsuContext,
					&BioAPIMemoryFuncs,
					uuid,
					MDS_CDSA_DIRECTORY,
					(CSSM_DB_ACCESS_READ | CSSM_DB_ACCESS_WRITE),
					BIOAPI_MDS_COMMIT_SIZE)) != BioAPI_OK)
		return err;

	/* Handle the simple case first...NULL passed for DeviceSchemaArray */
	if(DeviceSchemaArray == NULL)
	{
		Count = 0;
		BioAPI_CopyUuid (DeviceTemplate.ModuleId, uuid);
		ERR(err = MDSU_FindFirst(&MdsuContext,
								 &IfiMdsuBioAPIBspDeviceSchema,
								 &DeviceTemplate,
								 MDSU_BIOAPI_DEV_MODULEID,
								 &DeviceData,
								 NULL));
		if (CSSM_OK != err)
		{
			MDSU_Term( &MdsuContext );
			return err;
		}
		while(CSSM_OK == err)
		{
			Count ++;
			err = MDSU_FindNext(&MdsuContext,
								&DeviceData,
								NULL);
		}
		*ElementsNeeded = Count;
	}
	else
	{
		if(ArraySize == 0)
			return BioAPI_ERRCODE_INVALID_DATA;
		Count = 0;
		CurrentSchemaPtr = DeviceSchemaArray;
		BioAPI_CopyUuid (DeviceTemplate.ModuleId, uuid);
		err = MDSU_FindFirst(&MdsuContext,
								 &IfiMdsuBioAPIBspCapabilitySchema,
								 &DeviceTemplate,
								 MDSU_BIOAPI_DEV_MODULEID,
								 &DeviceData,
								 NULL);
		if (CSSM_OK != err)
		{
			MDSU_Term( &MdsuContext );
			return err;
		}
		while(CSSM_OK == err)
		{
			Count ++;
			if(Count <= ArraySize)
			{
				memcpy(CurrentSchemaPtr, &DeviceData, sizeof(BioAPI_DEVICE_SCHEMA));
				CurrentSchemaPtr++;
			}
			err = MDSU_FindNext(&MdsuContext,
								&DeviceData,
								NULL);
		}
		if(Count > ArraySize)
		{
			*NumElementsReturned = ArraySize;
		}
		else
		{
			*NumElementsReturned = Count;
		}
		*ElementsNeeded = Count;
	}
	MDSU_Term( &MdsuContext );
	return (BioAPI_OK);

}

/*************************************************************************/
