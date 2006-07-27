/*-----------------------------------------------------------------------
 * File: BioAPI_SPI.H
 *
 *-----------------------------------------------------------------------
 */

#ifndef _BioAPISPI_H
#define _BioAPISPI_H

#ifdef __cplusplus
extern "C" {
#endif


/* Define a function pointer type for each SPI function. The pointer types
 * are named _BioAPI_XXX_PTR, where XXX is the value of the SPI.
 */
typedef BioAPI_RETURN (BioAPI *_BioSPI_FreeBIRHandle_PTR)(
						BioAPI_HANDLE ModuleHandle,
						BioAPI_BIR_HANDLE BIRHandle );

typedef BioAPI_RETURN (BioAPI *_BioSPI_GetBIRFromHandle_PTR)(
						BioAPI_HANDLE ModuleHandle,
						BioAPI_BIR_HANDLE BIRHandle,
						BioAPI_BIR_PTR *BIR );

typedef BioAPI_RETURN (BioAPI *_BioSPI_GetHeaderFromHandle_PTR)(
						BioAPI_HANDLE ModuleHandle,
						BioAPI_BIR_HANDLE BIRHandle,
						BioAPI_BIR_HEADER_PTR Header );

typedef BioAPI_RETURN (BioAPI *_BioSPI_EnableEvents_PTR)(
						BioAPI_HANDLE ModuleHandle,
						BioAPI_MODULE_EVENT_MASK *Events );

typedef BioAPI_RETURN (BioAPI *_BioSPI_SetGUICallbacks_PTR)(
						BioAPI_HANDLE ModuleHandle,
						BioAPI_GUI_STREAMING_CALLBACK GuiStreamingCallback,
						void *GuiStreamingCallbackCtx,
						BioAPI_GUI_STATE_CALLBACK GuiStateCallback,
						void *GuiStateCallbackCtx );

typedef BioAPI_RETURN (BioAPI *_BioSPI_SetStreamCallback_PTR)(
						BioAPI_HANDLE ModuleHandle,
						BioAPI_STREAM_CALLBACK StreamCallback,
						void *StreamCallbackCtx );

typedef BioAPI_RETURN (BioAPI *_BioSPI_StreamInputOutput_PTR)(
						BioAPI_HANDLE ModuleHandle,
						BioAPI_DATA_PTR InMessage,
						BioAPI_DATA_PTR OutMessage );

typedef BioAPI_RETURN (BioAPI *_BioSPI_Capture_PTR)(
						BioAPI_HANDLE ModuleHandle,
						BioAPI_BIR_PURPOSE Purpose,
						BioAPI_BIR_HANDLE_PTR CapturedBIR,
						sint32 Timeout,
						BioAPI_BIR_HANDLE_PTR AuditData );

typedef BioAPI_RETURN (BioAPI *_BioSPI_CreateTemplate_PTR)(
						BioAPI_HANDLE ModuleHandle,
						const BioAPI_INPUT_BIR *CapturedBIR,
						const BioAPI_INPUT_BIR *StoredTemplate,
						BioAPI_BIR_HANDLE_PTR NewTemplate,
						const BioAPI_DATA *Payload );

typedef BioAPI_RETURN (BioAPI *_BioSPI_Process_PTR)(
						BioAPI_HANDLE ModuleHandle,
						const BioAPI_INPUT_BIR *CapturedBIR,
						BioAPI_BIR_HANDLE_PTR ProcessedBIR );

typedef BioAPI_RETURN (BioAPI *_BioSPI_VerifyMatch_PTR)(
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
						BioAPI_DATA_PTR *Payload );

typedef BioAPI_RETURN (BioAPI *_BioSPI_IdentifyMatch_PTR)(
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
						sint32 Timeout );

typedef BioAPI_RETURN (BioAPI *_BioSPI_Enroll_PTR)(
						BioAPI_HANDLE ModuleHandle,
						BioAPI_BIR_PURPOSE Purpose,
						const BioAPI_INPUT_BIR *StoredTemplate,
						BioAPI_BIR_HANDLE_PTR NewTemplate,
						const BioAPI_DATA *Payload,
						sint32 Timeout,
						BioAPI_BIR_HANDLE_PTR AuditData );

typedef BioAPI_RETURN (BioAPI *_BioSPI_Verify_PTR)(
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
						BioAPI_BIR_HANDLE_PTR AuditData );

typedef BioAPI_RETURN (BioAPI *_BioSPI_Identify_PTR)(
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
						BioAPI_BIR_HANDLE_PTR AuditData );

typedef BioAPI_RETURN (BioAPI *_BioSPI_Import_PTR)(
						BioAPI_HANDLE ModuleHandle,
						const BioAPI_DATA *InputData,
						BioAPI_BIR_BIOMETRIC_DATA_FORMAT InputFormat,
						BioAPI_BIR_PURPOSE Purpose,
						BioAPI_BIR_HANDLE_PTR ConstructedBIR );

typedef BioAPI_RETURN (BioAPI *_BioSPI_SetPowerMode_PTR)(
						BioAPI_HANDLE ModuleHandle,
						BioAPI_POWER_MODE PowerMode );

typedef BioAPI_RETURN (BioAPI *_BioSPI_DbOpen_PTR)(
						BioAPI_HANDLE ModuleHandle,
						const uint8 *DbName,
						BioAPI_DB_ACCESS_TYPE AccessRequest,
						BioAPI_DB_HANDLE_PTR DbHandle,
						BioAPI_DB_CURSOR_PTR Cursor );

typedef BioAPI_RETURN (BioAPI *_BioSPI_DbClose_PTR)(
						BioAPI_HANDLE ModuleHandle,
						BioAPI_DB_HANDLE DbHandle );

typedef BioAPI_RETURN (BioAPI *_BioSPI_DbCreate_PTR)(
						BioAPI_HANDLE ModuleHandle,
						const uint8 *DbName,
						BioAPI_DB_ACCESS_TYPE AccessRequest,
						BioAPI_DB_HANDLE_PTR DbHandle );

typedef BioAPI_RETURN (BioAPI *_BioSPI_DbDelete_PTR)(
						BioAPI_HANDLE ModuleHandle,
						const uint8 *DbName );

typedef BioAPI_RETURN (BioAPI *_BioSPI_DbSetCursor_PTR)(
						BioAPI_HANDLE ModuleHandle,
						BioAPI_DB_HANDLE DbHandle,
						const BioAPI_UUID *KeyValue,
						BioAPI_DB_CURSOR_PTR Cursor );

typedef BioAPI_RETURN (BioAPI *_BioSPI_DbFreeCursor_PTR)(
						BioAPI_HANDLE ModuleHandle,
						BioAPI_DB_CURSOR_PTR Cursor );

typedef BioAPI_RETURN (BioAPI *_BioSPI_DbStoreBIR_PTR)(
						BioAPI_HANDLE ModuleHandle,
						const BioAPI_INPUT_BIR *BIRToStore,
						BioAPI_DB_HANDLE DbHandle,
						BioAPI_UUID_PTR Uuid );

typedef BioAPI_RETURN (BioAPI *_BioSPI_DbGetBIR_PTR)(
						BioAPI_HANDLE ModuleHandle,
						BioAPI_DB_HANDLE DbHandle,
						const BioAPI_UUID *KeyValue,
						BioAPI_BIR_HANDLE_PTR RetrievedBIR,
						BioAPI_DB_CURSOR_PTR Cursor );

typedef BioAPI_RETURN (BioAPI *_BioSPI_DbGetNextBIR_PTR)(
						BioAPI_HANDLE ModuleHandle,
						BioAPI_DB_CURSOR_PTR Cursor,
						BioAPI_BIR_HANDLE_PTR RetievedBIR,
						BioAPI_UUID_PTR Uuid );

typedef BioAPI_RETURN (BioAPI *_BioSPI_DbQueryBIR_PTR)(
						BioAPI_HANDLE ModuleHandle,
						BioAPI_DB_HANDLE DbHandle,
						const BioAPI_INPUT_BIR *BIRToQuery,
						BioAPI_UUID_PTR Uuid );

typedef BioAPI_RETURN (BioAPI *_BioSPI_DbDeleteBIR_PTR)(
						BioAPI_HANDLE ModuleHandle,
						BioAPI_DB_HANDLE DbHandle,
						const BioAPI_UUID *KeyValue );


/* Define a function pointer interface table for the SPI.
 */
typedef struct bioapi_bsp_funcs {
_BioSPI_FreeBIRHandle_PTR FreeBIRHandle;
_BioSPI_GetBIRFromHandle_PTR GetBIRFromHandle;
_BioSPI_GetHeaderFromHandle_PTR GetHeaderFromHandle;
_BioSPI_EnableEvents_PTR EnableEvents;
_BioSPI_SetGUICallbacks_PTR SetGUICallbacks;
_BioSPI_SetStreamCallback_PTR SetStreamCallback;
_BioSPI_StreamInputOutput_PTR StreamInputOutput;
_BioSPI_Capture_PTR Capture;
_BioSPI_CreateTemplate_PTR CreateTemplate;
_BioSPI_Process_PTR Process;
_BioSPI_VerifyMatch_PTR VerifyMatch;
_BioSPI_IdentifyMatch_PTR IdentifyMatch;
_BioSPI_Enroll_PTR Enroll;
_BioSPI_Verify_PTR Verify;
_BioSPI_Identify_PTR Identify;
_BioSPI_Import_PTR Import;
_BioSPI_SetPowerMode_PTR SetPowerMode;
_BioSPI_DbOpen_PTR DbOpen;
_BioSPI_DbClose_PTR DbClose;
_BioSPI_DbCreate_PTR DbCreate;
_BioSPI_DbDelete_PTR DbDelete;
_BioSPI_DbSetCursor_PTR DbSetCursor;
_BioSPI_DbFreeCursor_PTR DbFreeCursor;
_BioSPI_DbStoreBIR_PTR DbStoreBIR;
_BioSPI_DbGetBIR_PTR DbGetBIR;
_BioSPI_DbGetNextBIR_PTR DbGetNextBIR;
_BioSPI_DbQueryBIR_PTR DbQueryBIR;
_BioSPI_DbDeleteBIR_PTR DbDeleteBIR;
} BioAPI_BSP_FUNCS, *BioAPI_BSP_FUNCS_PTR;

#ifdef __cplusplus
}
#endif

#endif /* ifndef _BioAPISPI_H */
