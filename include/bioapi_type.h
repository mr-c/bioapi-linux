/*-----------------------------------------------------------------------
 * File: BIOAPITYPE.H
 *
 *-----------------------------------------------------------------------
 */

#ifndef _BIOAPITYPE_H_
#define _BIOAPITYPE_H_


/*
	Operating System Dependent Primitive Declarations
	Not in TOG standard: added from cssmtype.h
*/

#if defined (WIN32)

#define BioAPI __stdcall

#elif defined (UNIX)

#define BioAPI
#define CALLBACK

#endif

#if defined (WIN32)

/* Disable Warnings due to non-ANSI content of windows.h */
/* Warning 4100 "unreferenced formal parameter", not flagged in gcc */
#pragma warning (disable:4201 4514 4214 4115 4100)
#include <windows.h>
#pragma warning (default:4201 4214 4115)

/* Turn parameter list mismatch warnings into errors */
#pragma warning (error:4113 4020 4021)
#endif /* defined(WIN32) */

/* End of: Not in TOG */

/* Linux_Port: most of these are already defined in 
 * imports/cdsa/v2_0/inc/cssmtype.h. The branch __BASIC_TYPES__ has 
 * been added here and in cssmtype.h 
 */

/* Linux_Port: sint8 needs to be defined here */
typedef char sint8;

/* Basic Types */
#if !defined (__BASIC_TYPES__)
#define __BASIC_TYPES__
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef short sint16;
typedef unsigned int uint32;
typedef int sint32;
/* typedef char sint8; see Linux-Port comments above */


#if defined(WIN32)
typedef unsigned __int64 uint64; /* MSVC++ 5/6 declaration */
#else
#ifdef __arch64__
typedef unsigned long uint64;
#else
typedef unsigned long long uint64; /* gcc 2.7.2 declaration */
#endif
#endif
#endif /* __BASIC_TYPES__ */

/*****************************************************************************
	Core Services API Datatypes
*****************************************************************************/

/*
	BioAPI_BOOL
	This data type is used to indicate a true or false condition
*/
typedef uint32 BioAPI_BOOL;
#define BioAPI_FALSE (0)
#define BioAPI_TRUE (!BioAPI_FALSE)

/*
	BioAPI_RETURN
	This data type is returned by most BioAPI functions.
*/
typedef uint32 BioAPI_RETURN;
#define BioAPI_OK (0)

typedef uint32 BioAPI_DEVICE_ID, *BioAPI_DEVICE_ID_PTR;

/*
	BioAPI_DATA
	The BioAPI_DATA structure is used to associate a length, in bytes, with
	an arbitrary block of contiguous memory.
*/
typedef struct bioapi_data
{
	uint32 Length; /* in bytes */
	uint8 *Data;
} BioAPI_DATA, *BioAPI_DATA_PTR;

typedef uint8 BioAPI_UUID[16];
typedef BioAPI_UUID *BioAPI_UUID_PTR;

/*
	BioAPI_VERSION
	This structure is used to represent the version of BioAPI components.
	The major number begins at 1 and is incremented by 1 for each major
	release.

	The minor number uses two digits to represent minor releases and revisions.
	The revision number is represented in the least significant digit. The
	remaining more significant digits represent minor numbers.	The first release
	has the value of zero.	There can be 9 subsequent releases then the minor number
	must be incremented.  For example, the minor number for the very first release
	of a product would be represented as "00".	Subsequent releases would "01",
	"02", "03", etc... to "09".	 If version number changes at each release then the
	minor numbers would increment from "00", "10", "20", etc... to "90".  A minor
	version of 0 release 1 would be "100".
 */

typedef struct bioapi_version
{
	uint32 Major;
	uint32 Minor;
} BioAPI_VERSION, *BioAPI_VERSION_PTR;

/*
	BioAPI_SERVICE_UID
	This structure uniquely identifies a biometric service provider and a device
	currently attached to it.
*/
typedef struct bioapi_uuid
{
	BioAPI_UUID Uuid;
	BioAPI_VERSION Version;
	BioAPI_DEVICE_ID DeviceId;
	uint32 Reserved;
} BioAPI_SERVICE_UID, *BioAPI_SERVICE_UID_PTR;


/*
	BioAPI_HANDLE
	A unique identifier, returned on BioAPI_ModuleAttach, that identifies an
	attached BioAPI service provider.
*/

typedef uint32 BioAPI_HANDLE, *BioAPI_HANDLE_PTR;

#define BioAPI_INVALID_HANDLE		0

/*
	BioAPI_DB_HANDLE
	A Handle to an open BIR database.
*/

typedef sint32 BioAPI_DB_HANDLE, *BioAPI_DB_HANDLE_PTR;
#define BioAPI_DB_INVALID_HANDLE	(-1)

/*
	BioAPI_MODULE_EVENT
	This enumeration defines the event types that can be raised by any
	service module.
*/

typedef uint32 BioAPI_MODULE_EVENT;
#define BioAPI_NOTIFY_INSERT			(1)
#define BioAPI_NOTIFY_REMOVE			(2)
#define BioAPI_NOTIFY_FAULT				(3)
#define BioAPI_NOTIFY_SOURCE_PRESENT	(4)
#define BioAPI_NOTIFY_SOURCE_REMOVED	(5)

/*
	BioAPI_MODULE_EVENT_MASK
	This enumeration defines a mask with bit positions for event type.	The mask
	is used to enable/disable events, and to indicate what events are supported.
*/

typedef uint32 BioAPI_MODULE_EVENT_MASK;
#define BioAPI_NOTIFY_INSERT_BIT			(0x0001)
#define BioAPI_NOTIFY_REMOVE_BIT			(0x0002)
#define BioAPI_NOTIFY_FAULT_BIT				(0x0004)
#define BioAPI_NOTIFY_SOURCE_PRESENT_BIT	(0x0008)
#define BioAPI_NOTIFY_SOURCE_REMOVED_BIT	(0x0010)

/*
	BioAPI_API_ModuleEventHandler
	This defines the event handler interface that an application must define
	and implement to receive asynchronous notification of events such as
	insertion or removal of a hardware service module, or a fault detected
	by the service module.
*/

typedef BioAPI_RETURN (BioAPI *BioAPI_ModuleEventHandler)
	(const BioAPI_UUID *BSPUuid,
	void* AppNotifyCallbackCtx,
	BioAPI_DEVICE_ID DeviceID,
	uint32 Reserved,
	BioAPI_MODULE_EVENT EventType);

typedef struct bioapi_gui_bitmap
{
	uint32 Width;
	uint32 Height;
	BioAPI_DATA_PTR Bitmap;
} BioAPI_GUI_BITMAP, *BioAPI_GUI_BITMAP_PTR;

typedef uint32 BioAPI_GUI_MESSAGE, *BioAPI_GUI_MESSAGE_PTR;

typedef uint8 BioAPI_GUI_PROGRESS, *BioAPI_GUI_PROGESS_PTR;

typedef uint8 BioAPI_GUI_RESPONSE, *BioAPI_GUI_RESPONSE_PTR;

#define BioAPI_CAPTURE_SAMPLE		(1)
#define BioAPI_CANCEL				(2)
#define BioAPI_CONTINUE				(3)
#define BioAPI_VALID_SAMPLE			(4)
#define BioAPI_INVALID_SAMPLE		(5)

/*
	BioAPI_GUI_STATE
	A mask that indicates GUI state, and alo what other parameter values are provided
	in the GUI State Callback.
*/

typedef uint32 BioAPI_GUI_STATE;

#define BioAPI_SAMPLE_AVAILABLE		(0x0001)
#define BioAPI_MESSAGE_PROVIDED		(0x0002)
#define BioAPI_PROGRESS_PROVIDED	(0x0004)

/*
	BioAPI_GUI_STATE_CALLBACK
	A Callback function that an application supplies to allow the service provider to
	indicate GUI state information to the application, and to receive responses back.
*/

typedef BioAPI_RETURN (BioAPI *BioAPI_GUI_STATE_CALLBACK)
			(void *GuiStateCallbackCtx,
			BioAPI_GUI_STATE GuiState,
			BioAPI_GUI_RESPONSE_PTR Response,
			BioAPI_GUI_MESSAGE Message,
			BioAPI_GUI_PROGRESS Progress,
			BioAPI_GUI_BITMAP_PTR SampleBuffer);

/*
	BioAPI_GUI_STREAMING_CALLBACK
	A callback function that an application supplies to allow the service provider to
	stream data in the form of a sequence of bitmaps.
*/

typedef BioAPI_RETURN (BioAPI *BioAPI_GUI_STREAMING_CALLBACK)
			(void *GuiStreamingCallbackCtx,
			BioAPI_GUI_BITMAP_PTR Bitmap);






/* BioAPI_MEMTRACK_ON defined if doing memtracking: i.e., pass __FILE__ and __LINE__ */
/* as the last two parameters into the memory functions */

#ifndef BioAPI_MEMTRACK_ON
typedef void * (BioAPI *BioAPI_MALLOC) (uint32 size, void * Allocref);
typedef void (BioAPI *BioAPI_FREE) (void * memblock, void * Allocref);
typedef void * (BioAPI *BioAPI_REALLOC)(void * Memblock, uint32 Size, void * Allocref);
typedef void * (BioAPI *BioAPI_CALLOC)(uint32 Num, uint32 Size, void * Allocref);
#else
typedef void * (BioAPI *BioAPI_MALLOC) ( uint32 Size,
										void * Allocref,
										const char * File,
										uint32 Line);

typedef void (BioAPI *BioAPI_FREE) (	 void * Memblock,
										void * Allocref,
										const char * File,
										uint32 Line);

typedef void * (BioAPI *BioAPI_REALLOC)( void * Memblock,
										uint32 Size,
										void * Allocref,
										const char * File,
										uint32 Line);

typedef void * (BioAPI *BioAPI_CALLOC)(	 uint32 Num,
										uint32 Size,
										void * Allocref,
										const char * File,
										uint32 Line);
#endif


/*
	BioAPI_MEMORY_FUNCS and BioAPI_MEMORY_FUNCS_PTR
	This structure is used by applications to supply memory functions for the
	BioAPI and the biometric service modules.
*/

typedef struct bioapi_memory_funcs
{
	BioAPI_MALLOC Malloc_func;
	BioAPI_FREE Free_func;
	BioAPI_REALLOC Realloc_func;
	BioAPI_CALLOC Calloc_func;
	void *AllocRef;
} BioAPI_MEMORY_FUNCS, *BioAPI_MEMORY_FUNCS_PTR;



/*
	BioAPI_PROC_ADDR
	Generic pointer to a BioAPI function.
*/
#if defined (WIN32)
	typedef FARPROC BioAPI_PROC_ADDR;
#else
	typedef void (BioAPI *BioAPI_PROC_ADDR) ();
#endif

typedef BioAPI_PROC_ADDR *BioAPI_PROC_ADDR_PTR;


/*
	BioAPI_FUNC_NAME_ADDR
	This structure binds a function to the runtime address of the procedure
	that implements the named function. Function names are limited in length
	to the size of BioAPI_MAX_STR_LEN.
*/

#define BioAPI_MAX_STR_LEN	255
typedef struct bioapi_func_name_addr
{
	char Name[BioAPI_MAX_STR_LEN];
	BioAPI_PROC_ADDR Address;
}BioAPI_FUNC_NAME_ADDR, *BioAPI_FUNC_NAME_ADDR_PTR;




typedef sint32 BioAPI_BIR_HANDLE, *BioAPI_BIR_HANDLE_PTR;

#define BioAPI_INVALID_BIR_HANDLE					  (-1)
#define BioAPI_UNSUPPORTED_BIR_HANDLE				  (-2)



typedef uint32 BioAPI_DB_CURSOR, *BioAPI_DB_CURSOR_PTR;



/* Linux-Port: this typedef is already defined in this file */
/* typedef sint32 BioAPI_DB_HANDLE, *BioAPI_DB_HANDLE_PTR;  */

#define	BioAPI_DB_DEFAULT_DB_HANDLE					  (0)
#define BioAPI_DB_INVALID_HANDLE					  (-1)



/*******************************************/
/* Mask types and the values they can have */
/*******************************************/

typedef uint32 BioAPI_BIR_AUTH_FACTORS;

#define BioAPI_FACTOR_MULTIPLE						  (0x00000001)
#define BioAPI_FACTOR_FACIAL_FEATURES				  (0x00000002)
#define BioAPI_FACTOR_VOICE							  (0x00000004)
#define BioAPI_FACTOR_FINGERPRINT					  (0x00000008)
#define BioAPI_FACTOR_IRIS							  (0x00000010)
#define BioAPI_FACTOR_RETINA						  (0x00000020)
#define BioAPI_FACTOR_HAND_GEOMETRY					  (0x00000040)
#define BioAPI_FACTOR_SIGNATURE_DYNAMICS			  (0x00000080)
#define BioAPI_FACTOR_KEYSTOKE_DYNAMICS				  (0x00000100)
#define BioAPI_FACTOR_LIP_MOVEMENT					  (0x00000200)
#define BioAPI_FACTOR_THERMAL_FACE_IMAGE			  (0x00000400)
#define BioAPI_FACTOR_THERMAL_HAND_IMAGE			  (0x00000800)
#define BioAPI_FACTOR_GAIT							  (0x00001000)
#define BioAPI_FACTOR_PASSWORD						  (0x80000000)



typedef uint8 BioAPI_BIR_DATA_TYPE;

#define BioAPI_BIR_DATA_TYPE_RAW					  (0x01)
#define BioAPI_BIR_DATA_TYPE_INTERMEDIATE			  (0x02)
#define BioAPI_BIR_DATA_TYPE_PROCESSED				  (0x04)
#define BioAPI_BIR_DATA_TYPE_ENCRYPTED				  (0x10)
#define BioAPI_BIR_DATA_TYPE_SIGNED					  (0x20)



typedef uint32 BioAPI_DB_ACCESS_TYPE, *BioAPI_DB_ACCESS_TYPE_PTR;

#define BioAPI_DB_ACCESS_READ						  (0x1)
#define BioAPI_DB_ACCESS_WRITE						  (0x2)

/****************************************************************/
/* Integer-based types and the pre-defined values they can have */
/****************************************************************/

typedef uint8 BioAPI_BIR_PURPOSE;

#define BioAPI_PURPOSE_VERIFY						  (1)
#define BioAPI_PURPOSE_IDENTIFY						  (2)
#define BioAPI_PURPOSE_ENROLL						  (3)
#define BioAPI_PURPOSE_ENROLL_FOR_VERIFICATION_ONLY	  (4)
#define BioAPI_PURPOSE_ENROLL_FOR_IDENTIFICATION_ONLY (5)
#define BioAPI_PURPOSE_AUDIT						  (6)

typedef uint8 BioAPI_IDENTIFY_POPULATION_TYPE;

#define BioAPI_DB_TYPE								  (1)
#define BioAPI_ARRAY_TYPE							  (2)



typedef uint8 BioAPI_INPUT_BIR_FORM;

#define BioAPI_DATABASE_ID_INPUT						(1)
#define BioAPI_BIR_HANDLE_INPUT							(2)
#define BioAPI_FULLBIR_INPUT							(3)



typedef uint32 BioAPI_POWER_MODE;

#define BioAPI_POWER_NORMAL							  (1)
#define BioAPI_POWER_DETECT							  (2)
#define BioAPI_POWER_SLEEP							  (3)


/*
	BioAPI_FAR
	A 32-bit integer value (N) that indicates a probable False Accept Rate of N/(2^31 - 1).
	The larger the value, the worse the result.
*/
typedef sint32 BioAPI_FAR, *BioAPI_FAR_PTR;

#define BioAPI_NOT_SET							  (-1)


/*
	BioAPI_FRR
	A 32-bit integer value (N) that indicates a probable False Reject Rate of N/(2^31 - 1).
	The larger the value, the worse the result.
*/

typedef sint32 BioAPI_FRR, *BioAPI_FRR_PTR;

#define BioAPI_NOT_SUPPORTED					  (-2)


typedef uint8 BioAPI_BIR_BIOMETRIC_DATA, *BioAPI_BIR_BIOMETRIC_DATA_PTR;

typedef uint8 BioAPI_BIR_VERSION, *BioAPI_BIR_VERSION_PTR;

typedef sint8 BioAPI_QUALITY;


/**************************/
/* C-structure data types */
/**************************/

typedef struct bioapi_bir_biometric_data_format
{
	uint16 FormatOwner;
	uint16 FormatID;
} BioAPI_BIR_BIOMETRIC_DATA_FORMAT, *BioAPI_BIR_BIOMETRIC_DATA_FORMAT_PTR;



typedef struct bioapi_bir_header
{
	uint32 Length;	 /* Length of Header + Opaque Data */
	BioAPI_BIR_VERSION HeaderVersion;
	BioAPI_BIR_DATA_TYPE Type;
	BioAPI_BIR_BIOMETRIC_DATA_FORMAT Format;
	BioAPI_QUALITY Quality;
	BioAPI_BIR_PURPOSE Purpose;
	BioAPI_BIR_AUTH_FACTORS FactorsMask;
} BioAPI_BIR_HEADER, *BioAPI_BIR_HEADER_PTR;



typedef struct bioapi_hrs_bir
{
	BioAPI_BIR_HEADER  Header;
	BioAPI_BIR_BIOMETRIC_DATA_PTR BiometricData; /* length indicated in */
												   /* header			  */
	BioAPI_DATA_PTR Signature;	/* NULL if no signature; length is inherent */
							  /* in this type							  */
} BioAPI_BIR, *BioAPI_BIR_PTR;



typedef struct bioapi_bir_array_population
{
	uint32 NumberOfMembers;
	BioAPI_BIR_PTR *Members;	/* A pointer to an array of BIR pointers */
} BioAPI_BIR_ARRAY_POPULATION, *BioAPI_BIR_ARRAY_POPULATION_PTR;



typedef struct bioapi_candidate
{
	BioAPI_IDENTIFY_POPULATION_TYPE Type;
	union
	{
		BioAPI_UUID_PTR BIRInDataBase;
		uint32 *BIRInArray;
	} BIR;
	BioAPI_FAR FARAchieved;
	BioAPI_FRR FRRAchieved;
} BioAPI_CANDIDATE, *BioAPI_CANDIDATE_PTR;

typedef BioAPI_CANDIDATE_PTR BioAPI_CANDIDATE_ARRAY, *BioAPI_CANDIDATE_ARRAY_PTR;



typedef struct bioapi_hrs_dbbir_id
{
	BioAPI_DB_HANDLE DbHandle;
	BioAPI_UUID KeyValue;
} BioAPI_DBBIR_ID, *BioAPI_DBBIR_ID_PTR;



typedef struct bioapi_identify_population
{
	BioAPI_IDENTIFY_POPULATION_TYPE Type;
	union
	{
		BioAPI_DB_HANDLE_PTR BIRDataBase;
		BioAPI_BIR_ARRAY_POPULATION_PTR BIRArray;
	} BIRs;
} BioAPI_IDENTIFY_POPULATION, *BioAPI_IDENTIFY_POPULATION_PTR;



typedef struct bioapi_input_bir
{
	BioAPI_INPUT_BIR_FORM Form;
	union
	{
		BioAPI_DBBIR_ID_PTR BIRinDb;
		BioAPI_BIR_HANDLE_PTR BIRinBSP;
		BioAPI_BIR_PTR BIR;
	} InputBIR;
} BioAPI_INPUT_BIR, *BioAPI_INPUT_BIR_PTR;



/***********************************/
/* typedefs for callback functions */
/***********************************/

typedef BioAPI_RETURN (BioAPI *BioAPI_STREAM_CALLBACK)
										(void *StreamCallbackCtx,
										 BioAPI_DATA_PTR OutMessage,
										 BioAPI_DATA_PTR InMessage);


/****************************************************************************/
/* these are data types used only for reading fields from the HRS relations */
/* relation within MDS														*/
/****************************************************************************/

typedef uint32 BioAPI_OPERATIONS_MASK;

#define BioAPI_CAPTURE							  (0x0001)
#define BioAPI_CREATETEMPLATE					  (0x0002)
#define BioAPI_PROCESS							  (0x0004)
#define BioAPI_VERIFYMATCH						  (0x0008)
#define BioAPI_IDENTIFYMATCH					  (0x0010)
#define BioAPI_ENROLL							  (0x0020)
#define BioAPI_VERIFY							  (0x0040)
#define BioAPI_IDENTIFY							  (0x0080)
#define BioAPI_IMPORT							  (0x0100)
#define BioAPI_SETPOWERMODE						  (0x0200)
#define BioAPI_DATABASEOPERATIONS				  (0x0400)



typedef uint32 BioAPI_OPTIONS_MASK;

#define BioAPI_RAW								  (0x00000001)
#define BioAPI_QUALITY_RAW						  (0x00000002)
#define BioAPI_QUALITY_INTERMEDIATE				  (0x00000004)
#define BioAPI_QUALITY_PROCESSED				  (0x00000008)
#define BioAPI_APP_GUI							  (0x00000010)
#define BioAPI_STREAMINGDATA					  (0x00000020)
#define BioAPI_USERVALIDATESSAMPLES				  (0x00000040)
#define BioAPI_VERIFYSAMPLES					  (0x00000080)
#define BioAPI_SOURCEPRESENT					  (0x00000100)
#define BioAPI_PAYLOAD							  (0x00001000)
#define BioAPI_BIR_SIGN							  (0x00002000)
#define BioAPI_BIR_ENCRYPT						  (0x00004000)
#define BioAPI_FRR_SUPPORTED					  (0x00010000)
#define BioAPI_ADAPTATION						  (0x00020000)
#define BioAPI_BINNING							  (0x00040000)
#define BioAPI_DEFAULTDATABASE					  (0x00080000)
#define BioAPI_LOCAL_BSP						  (0x01000000)
#define BioAPI_CLIENT_BSP						  (0x02000000)
#define BioAPI_SERVER_BSP						  (0x04000000)
#define BioAPI_STREAMINGCALLBACK				  (0x08000000)
#define BioAPI_PROGRESS							  (0x10000000)
#define BioAPI_SELFCONTAINEDDEVICE				  (0x20000000)



#endif /* _BIOAPITYPE_H_ */

