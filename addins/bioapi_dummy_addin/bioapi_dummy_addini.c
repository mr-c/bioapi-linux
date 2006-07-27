/*-----------------------------------------------------------------------
 * File: bioapi_dummy_addini.c
 *-----------------------------------------------------------------------
 */

#include "precomp.h"
#include "bioapi_spi.h"

/* Define each SPI function.
 */
BioAPI_RETURN BioAPI BSP_FreeBIRHandle(
					BioAPI_HANDLE ModuleHandle,
					BioAPI_BIR_HANDLE BIRHandle )
{
	BioAPI_RETURN ret = BioAPI_OK;
	void *pAddinContext = NULL;
	ADDIN_SPI_BEGIN(ret, ModuleHandle, pAddinContext);
	if (ret != BioAPI_OK) return ret;
	Addin_SPIEnd(pAddinContext);
	return BioAPIERR_BSP_FREE_BIRHANDLE;
}

BioAPI_RETURN BioAPI BSP_GetBIRFromHandle(
					BioAPI_HANDLE ModuleHandle,
					BioAPI_BIR_HANDLE BIRHandle,
					BioAPI_BIR_PTR *BIR )
{
	BioAPI_RETURN ret = BioAPI_OK;
	void *pAddinContext = NULL;
	ADDIN_SPI_BEGIN(ret, ModuleHandle, pAddinContext);
	if (ret != BioAPI_OK) return ret;
	Addin_SPIEnd(pAddinContext);
	return BioAPIERR_BSP_GET_BIRFROM_HANDLE;
}

BioAPI_RETURN BioAPI BSP_GetHeaderFromHandle(
					BioAPI_HANDLE ModuleHandle,
					BioAPI_BIR_HANDLE BIRHandle,
					BioAPI_BIR_HEADER_PTR Header )
{
	BioAPI_RETURN ret = BioAPI_OK;
	void *pAddinContext = NULL;
	ADDIN_SPI_BEGIN(ret, ModuleHandle, pAddinContext);
	if (ret != BioAPI_OK) return ret;
	Addin_SPIEnd(pAddinContext);
	return BioAPIERR_BSP_GET_HEADER_FROM_HANDLE;
}

BioAPI_RETURN BioAPI BSP_EnableEvents(
					BioAPI_HANDLE ModuleHandle,
					BioAPI_MODULE_EVENT_MASK *Events )
{
	BioAPI_RETURN ret = BioAPI_OK;
	void *pAddinContext = NULL;
	ADDIN_SPI_BEGIN(ret, ModuleHandle, pAddinContext);
	if (ret != BioAPI_OK) return ret;
	Addin_SPIEnd(pAddinContext);
	return BioAPIERR_BSP_ENABLE_EVENTS;
}

BioAPI_RETURN BioAPI BSP_SetGUICallbacks(
					BioAPI_HANDLE ModuleHandle,
					BioAPI_GUI_STREAMING_CALLBACK GuiStreamingCallback,
					void *GuiStreamingCallbackCtx,
					BioAPI_GUI_STATE_CALLBACK GuiStateCallback,
					void *GuiStateCallbackCtx )
{
	BioAPI_RETURN ret = BioAPI_OK;
	void *pAddinContext = NULL;
	ADDIN_SPI_BEGIN(ret, ModuleHandle, pAddinContext);
	if (ret != BioAPI_OK) return ret;
	Addin_SPIEnd(pAddinContext);
	return BioAPIERR_BSP_SET_GUICALLBACKS;
}

BioAPI_RETURN BioAPI BSP_SetStreamCallback(
					BioAPI_HANDLE ModuleHandle,
					BioAPI_STREAM_CALLBACK StreamCallback,
					void *StreamCallbackCtx )
{
	BioAPI_RETURN ret = BioAPI_OK;
	void *pAddinContext = NULL;
	ADDIN_SPI_BEGIN(ret, ModuleHandle, pAddinContext);
	if (ret != BioAPI_OK) return ret;
	Addin_SPIEnd(pAddinContext);
	return BioAPIERR_BSP_SET_STREAM_CALLBACK;
}

BioAPI_RETURN BioAPI BSP_StreamInputOutput(
					BioAPI_HANDLE ModuleHandle,
					BioAPI_DATA_PTR InMessage,
					BioAPI_DATA_PTR OutMessage )
{
	BioAPI_RETURN ret = BioAPI_OK;
	void *pAddinContext = NULL;
	ADDIN_SPI_BEGIN(ret, ModuleHandle, pAddinContext);
	if (ret != BioAPI_OK) return ret;
	Addin_SPIEnd(pAddinContext);
	return BioAPIERR_BSP_STREAM_INPUT_OUTPUT;
}

BioAPI_RETURN BioAPI BSP_Capture(
					BioAPI_HANDLE ModuleHandle,
					BioAPI_BIR_PURPOSE Purpose,
					BioAPI_BIR_HANDLE_PTR CapturedBIR,
					sint32 Timeout,
					BioAPI_BIR_HANDLE_PTR AuditData )
{
	BioAPI_RETURN ret = BioAPI_OK;
	void *pAddinContext = NULL;
	ADDIN_SPI_BEGIN(ret, ModuleHandle, pAddinContext);
	if (ret != BioAPI_OK) return ret;
	Addin_SPIEnd(pAddinContext);
	return BioAPIERR_BSP_CAPTURE;
}

BioAPI_RETURN BioAPI BSP_CreateTemplate(
					BioAPI_HANDLE ModuleHandle,
					const BioAPI_INPUT_BIR *CapturedBIR,
					const BioAPI_INPUT_BIR *StoredTemplate,
					BioAPI_BIR_HANDLE_PTR NewTemplate,
					const BioAPI_DATA *Payload )
{
	BioAPI_RETURN ret = BioAPI_OK;
	void *pAddinContext = NULL;
	ADDIN_SPI_BEGIN(ret, ModuleHandle, pAddinContext);
	if (ret != BioAPI_OK) return ret;
	Addin_SPIEnd(pAddinContext);
	return BioAPIERR_BSP_CREATE_TEMPLATE;
}

BioAPI_RETURN BioAPI BSP_Process(
					BioAPI_HANDLE ModuleHandle,
					const BioAPI_INPUT_BIR *CapturedBIR,
					BioAPI_BIR_HANDLE_PTR ProcessedBIR )
{
	BioAPI_RETURN ret = BioAPI_OK;
	void *pAddinContext = NULL;
	ADDIN_SPI_BEGIN(ret, ModuleHandle, pAddinContext);
	if (ret != BioAPI_OK) return ret;
	Addin_SPIEnd(pAddinContext);
	return BioAPIERR_BSP_PROCESS;
}

BioAPI_RETURN BioAPI BSP_VerifyMatch(
					BioAPI_HANDLE ModuleHandle,
					const BioAPI_FAR *MaxFARRequested,
					const BioAPI_FRR *MaxFRRRequested,
					const BioAPI_BOOL *FARPrecedence,
					const BioAPI_INPUT_BIR *ProcessedBIR,
					const BioAPI_INPUT_BIR *StoredTemplate,
					BioAPI_BIR_HANDLE *AdaptedBIR,
					BioAPI_BOOL *Result,
					BioAPI_FAR_PTR FARAchieved,
					BioAPI_FRR_PTR FRRAchieved,
					BioAPI_DATA_PTR *Payload )
{
	BioAPI_RETURN ret = BioAPI_OK;
	void *pAddinContext = NULL;
	ADDIN_SPI_BEGIN(ret, ModuleHandle, pAddinContext);
	if (ret != BioAPI_OK) return ret;
	Addin_SPIEnd(pAddinContext);
	return BioAPIERR_BSP_VERIFY_MATCH;
}

BioAPI_RETURN BioAPI BSP_IdentifyMatch(
					BioAPI_HANDLE ModuleHandle,
					const BioAPI_FAR *MaxFARRequested,
					const BioAPI_FRR *MaxFRRRequested,
					const BioAPI_BOOL *FARPrecedence,
					const BioAPI_INPUT_BIR *ProcessedBIR,
					const BioAPI_IDENTIFY_POPULATION *Population,
					BioAPI_BOOL Binning,
					uint32 MaxNumberOfResults,
					uint32 *NumberOfResults,
					BioAPI_CANDIDATE_ARRAY_PTR *Candidates,
					sint32 Timeout )
{
	BioAPI_RETURN ret = BioAPI_OK;
	void *pAddinContext = NULL;
	ADDIN_SPI_BEGIN(ret, ModuleHandle, pAddinContext);
	if (ret != BioAPI_OK) return ret;
	Addin_SPIEnd(pAddinContext);
	return BioAPIERR_BSP_IDENTIFY_MATCH;
}

BioAPI_RETURN BioAPI BSP_Enroll(
					BioAPI_HANDLE ModuleHandle,
					BioAPI_BIR_PURPOSE Purpose,
					const BioAPI_INPUT_BIR *StoredTemplate,
					BioAPI_BIR_HANDLE_PTR NewTemplate,
					const BioAPI_DATA *Payload,
					sint32 Timeout,
					BioAPI_BIR_HANDLE_PTR AuditData )
{
	BioAPI_RETURN ret = BioAPI_OK;
	void *pAddinContext = NULL;
	ADDIN_SPI_BEGIN(ret, ModuleHandle, pAddinContext);
	if (ret != BioAPI_OK) return ret;
	Addin_SPIEnd(pAddinContext);
	return BioAPIERR_BSP_ENROLL;
}

BioAPI_RETURN BioAPI BSP_Verify(
					BioAPI_HANDLE ModuleHandle,
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
					BioAPI_BIR_HANDLE_PTR AuditData )
{
	BioAPI_RETURN ret = BioAPI_OK;
	void *pAddinContext = NULL;
	ADDIN_SPI_BEGIN(ret, ModuleHandle, pAddinContext);
	if (ret != BioAPI_OK) return ret;
	Addin_SPIEnd(pAddinContext);
	return BioAPIERR_BSP_VERIFY;
}

BioAPI_RETURN BioAPI BSP_Identify(
					BioAPI_HANDLE ModuleHandle,
					const BioAPI_FAR *MaxFARRequested,
					const BioAPI_FRR *MaxFRRRequested,
					const BioAPI_BOOL *FARPrecedence,
					const BioAPI_IDENTIFY_POPULATION *Population,
					BioAPI_BOOL Binning,
					uint32 MaxNumberOfResults,
					uint32 *NumberOfResults,
					BioAPI_CANDIDATE_ARRAY_PTR *Candidates,
					sint32 Timeout,
					BioAPI_BIR_HANDLE_PTR AuditData )
{
	BioAPI_RETURN ret = BioAPI_OK;
	void *pAddinContext = NULL;
	ADDIN_SPI_BEGIN(ret, ModuleHandle, pAddinContext);
	if (ret != BioAPI_OK) return ret;
	Addin_SPIEnd(pAddinContext);
	return BioAPIERR_BSP_IDENTIFY;
}

BioAPI_RETURN BioAPI BSP_Import(
					BioAPI_HANDLE ModuleHandle,
					const BioAPI_DATA *InputData,
					BioAPI_BIR_BIOMETRIC_DATA_FORMAT InputFormat,
					BioAPI_BIR_PURPOSE Purpose,
					BioAPI_BIR_HANDLE_PTR ConstructedBIR )
{
	BioAPI_RETURN ret = BioAPI_OK;
	void *pAddinContext = NULL;
	ADDIN_SPI_BEGIN(ret, ModuleHandle, pAddinContext);
	if (ret != BioAPI_OK) return ret;
	Addin_SPIEnd(pAddinContext);
	return BioAPIERR_BSP_IMPORT;
}

BioAPI_RETURN BioAPI BSP_SetPowerMode(
					BioAPI_HANDLE ModuleHandle,
					BioAPI_POWER_MODE PowerMode )
{
	BioAPI_RETURN ret = BioAPI_OK;
	void *pAddinContext = NULL;
	ADDIN_SPI_BEGIN(ret, ModuleHandle, pAddinContext);
	if (ret != BioAPI_OK) return ret;
	Addin_SPIEnd(pAddinContext);
	return BioAPIERR_BSP_SET_POWER_MODE;
}

BioAPI_RETURN BioAPI BSP_DbOpen(
					BioAPI_HANDLE ModuleHandle,
					const uint8 *DbName,
					BioAPI_DB_ACCESS_TYPE AccessRequest,
					BioAPI_DB_HANDLE_PTR DbHandle,
					BioAPI_DB_CURSOR_PTR Cursor )
{
	BioAPI_RETURN ret = BioAPI_OK;
	void *pAddinContext = NULL;
	ADDIN_SPI_BEGIN(ret, ModuleHandle, pAddinContext);
	if (ret != BioAPI_OK) return ret;
	Addin_SPIEnd(pAddinContext);
	return BioAPIERR_BSP_DB_OPEN;
}

BioAPI_RETURN BioAPI BSP_DbClose(
					BioAPI_HANDLE ModuleHandle,
					BioAPI_DB_HANDLE DbHandle )
{
	BioAPI_RETURN ret = BioAPI_OK;
	void *pAddinContext = NULL;
	ADDIN_SPI_BEGIN(ret, ModuleHandle, pAddinContext);
	if (ret != BioAPI_OK) return ret;
	Addin_SPIEnd(pAddinContext);
	return BioAPIERR_BSP_DB_CLOSE;
}

BioAPI_RETURN BioAPI BSP_DbCreate(
					BioAPI_HANDLE ModuleHandle,
					const uint8 *DbName,
					BioAPI_DB_ACCESS_TYPE AccessRequest,
					BioAPI_DB_HANDLE_PTR DbHandle )
{
	BioAPI_RETURN ret = BioAPI_OK;
	void *pAddinContext = NULL;
	ADDIN_SPI_BEGIN(ret, ModuleHandle, pAddinContext);
	if (ret != BioAPI_OK) return ret;
	Addin_SPIEnd(pAddinContext);
	return BioAPIERR_BSP_DB_CREATE;
}

BioAPI_RETURN BioAPI BSP_DbDelete(
					BioAPI_HANDLE ModuleHandle,
					const uint8 *DbName )
{
	BioAPI_RETURN ret = BioAPI_OK;
	void *pAddinContext = NULL;
	ADDIN_SPI_BEGIN(ret, ModuleHandle, pAddinContext);
	if (ret != BioAPI_OK) return ret;
	Addin_SPIEnd(pAddinContext);
	return BioAPIERR_BSP_DB_DELETE;
}

BioAPI_RETURN BioAPI BSP_DbSetCursor(
					BioAPI_HANDLE ModuleHandle,
					BioAPI_DB_HANDLE DbHandle,
					const BioAPI_UUID *KeyValue,
					BioAPI_DB_CURSOR_PTR Cursor )
{
	BioAPI_RETURN ret = BioAPI_OK;
	void *pAddinContext = NULL;
	ADDIN_SPI_BEGIN(ret, ModuleHandle, pAddinContext);
	if (ret != BioAPI_OK) return ret;
	Addin_SPIEnd(pAddinContext);
	return BioAPIERR_BSP_DB_SET_CURSOR;
}

BioAPI_RETURN BioAPI BSP_DbFreeCursor(
					BioAPI_HANDLE ModuleHandle,
					BioAPI_DB_CURSOR_PTR Cursor )
{
	BioAPI_RETURN ret = BioAPI_OK;
	void *pAddinContext = NULL;
	ADDIN_SPI_BEGIN(ret, ModuleHandle, pAddinContext);
	if (ret != BioAPI_OK) return ret;
	Addin_SPIEnd(pAddinContext);
	return BioAPIERR_BSP_DB_FREE_CURSOR;
}

BioAPI_RETURN BioAPI BSP_DbStoreBIR(
					BioAPI_HANDLE ModuleHandle,
					const BioAPI_INPUT_BIR *BIRToStore,
					BioAPI_DB_HANDLE DbHandle,
					BioAPI_UUID_PTR Uuid )
{
	BioAPI_RETURN ret = BioAPI_OK;
	void *pAddinContext = NULL;
	ADDIN_SPI_BEGIN(ret, ModuleHandle, pAddinContext);
	if (ret != BioAPI_OK) return ret;
	Addin_SPIEnd(pAddinContext);
	return BioAPIERR_BSP_DB_STORE_BIR;
}

BioAPI_RETURN BioAPI BSP_DbGetBIR(
					BioAPI_HANDLE ModuleHandle,
					BioAPI_DB_HANDLE DbHandle,
					const BioAPI_UUID *KeyValue,
					BioAPI_BIR_HANDLE_PTR RetrievedBIR,
					BioAPI_DB_CURSOR_PTR Cursor )
{
	BioAPI_RETURN ret = BioAPI_OK;
	void *pAddinContext = NULL;
	ADDIN_SPI_BEGIN(ret, ModuleHandle, pAddinContext);
	if (ret != BioAPI_OK) return ret;
	Addin_SPIEnd(pAddinContext);
	return BioAPIERR_BSP_DB_GET_BIR;
}

BioAPI_RETURN BioAPI BSP_DbGetNextBIR(
					BioAPI_HANDLE ModuleHandle,
					BioAPI_DB_CURSOR_PTR Cursor,
					BioAPI_BIR_HANDLE_PTR RetievedBIR,
					BioAPI_UUID_PTR Uuid )
{
	BioAPI_RETURN ret = BioAPI_OK;
	void *pAddinContext = NULL;
	ADDIN_SPI_BEGIN(ret, ModuleHandle, pAddinContext);
	if (ret != BioAPI_OK) return ret;
	Addin_SPIEnd(pAddinContext);
	return BioAPIERR_BSP_DB_GET_NEXT_BIR;
}

BioAPI_RETURN BioAPI BSP_DbQueryBIR(
					BioAPI_HANDLE ModuleHandle,
					BioAPI_DB_HANDLE DbHandle,
					const BioAPI_INPUT_BIR *BIRToQuery,
					BioAPI_UUID_PTR Uuid )
{
	BioAPI_RETURN ret = BioAPI_OK;
	void *pAddinContext = NULL;
	ADDIN_SPI_BEGIN(ret, ModuleHandle, pAddinContext);
	if (ret != BioAPI_OK) return ret;
	Addin_SPIEnd(pAddinContext);
	return BioAPIERR_BSP_DB_QUERY_BIR;
}

BioAPI_RETURN BioAPI BSP_DbDeleteBIR(
					BioAPI_HANDLE ModuleHandle,
					BioAPI_DB_HANDLE DbHandle,
					const BioAPI_UUID *KeyValue )
{
	BioAPI_RETURN ret = BioAPI_OK;
	void *pAddinContext = NULL;
	ADDIN_SPI_BEGIN(ret, ModuleHandle, pAddinContext);
	if (ret != BioAPI_OK) return ret;
	Addin_SPIEnd(pAddinContext);
	return BioAPIERR_BSP_DB_DELETE_BIR;
}


/* Define the attach function.
 */
BioAPI_RETURN BSP_ModuleAttach(
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
	Functions.FreeBIRHandle = BSP_FreeBIRHandle;
	Functions.GetBIRFromHandle = BSP_GetBIRFromHandle;
	Functions.GetHeaderFromHandle = BSP_GetHeaderFromHandle;
	Functions.EnableEvents = BSP_EnableEvents;
	Functions.SetGUICallbacks = BSP_SetGUICallbacks;
	Functions.SetStreamCallback = BSP_SetStreamCallback;
	Functions.StreamInputOutput = BSP_StreamInputOutput;
	Functions.Capture = BSP_Capture;
	Functions.CreateTemplate = BSP_CreateTemplate;
	Functions.Process = BSP_Process;
	Functions.VerifyMatch = BSP_VerifyMatch;
	Functions.IdentifyMatch = BSP_IdentifyMatch;
	Functions.Enroll = BSP_Enroll;
	Functions.Verify = BSP_Verify;
	Functions.Identify = BSP_Identify;
	Functions.Import = BSP_Import;
	Functions.SetPowerMode = BSP_SetPowerMode;
	Functions.DbOpen = BSP_DbOpen;
	Functions.DbClose = BSP_DbClose;
	Functions.DbCreate = BSP_DbCreate;
	Functions.DbDelete = BSP_DbDelete;
	Functions.DbSetCursor = BSP_DbSetCursor;
	Functions.DbFreeCursor = BSP_DbFreeCursor;
	Functions.DbStoreBIR = BSP_DbStoreBIR;
	Functions.DbGetBIR = BSP_DbGetBIR;
	Functions.DbGetNextBIR = BSP_DbGetNextBIR;
	Functions.DbQueryBIR = BSP_DbQueryBIR;
	Functions.DbDeleteBIR = BSP_DbDeleteBIR;
	*FuncTbl = &FunctionList;
	return BioAPI_OK;
}
