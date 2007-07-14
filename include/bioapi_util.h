/*-----------------------------------------------------------------------
 * File: bioapi_util.h
 *-----------------------------------------------------------------------
 */

#ifndef _BIOAPI_UTIL_H_
#define _BIOAPI_UTIL_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "bioapi_type.h"
#include "bioapi_installdefs.h"

/*
 * Format conversion utilities
 */
BioAPI_RETURN BioAPI BioAPI_GetPrintableUUID( const BioAPI_UUID *pUUID,
		char *PrintableUUID );
BioAPI_RETURN BioAPI BioAPI_GetStructuredUUID( const char *PrintableUUID,
		BioAPI_UUID_PTR pUUID );

BioAPI_RETURN BioAPI BioAPI_GetPrintableVersion( const BioAPI_VERSION *pVersion,
		char *PrintableVersion );
BioAPI_RETURN BioAPI BioAPI_GetStructuredVersion( const char *PrintableVersion,
		BioAPI_VERSION *pVersion );

#define BioAPI_PRINTABLE_UUID_LENGTH	(40)
#define BioAPI_PRINTABLE_VERSION_LENGTH	( 8 )
#define BioAPI_UUID_FORMAT_STRING		"{%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x}"

/*
 * Installation utilities
 */
BioAPI_RETURN BioAPI
BioAPIInstallBsp(INSTALL_ACTION Action,
					BioAPI_INSTALL_ERROR *pError,
					const BioAPI_UUID *bsp_uuid,
					uint32 device_id,
					const char *bsp_name,
					uint32 bsp_version_major,
					uint32 bsp_version_minor,
					const char *bsp_vendor_name,
					uint32 bsp_num_supported_formats,
					const BioAPI_BIR_BIOMETRIC_DATA_FORMAT *bsp_supported_formats,
					BioAPI_BIR_AUTH_FACTORS bsp_factors_mask,
					BioAPI_OPERATIONS_MASK bsp_operations,
					BioAPI_OPTIONS_MASK bsp_options,
					BioAPI_FAR bsp_payload_policy,
					uint32 bsp_max_payload,
					uint32 bsp_verify_timeout,
					uint32 bsp_identify_timeout,
					uint32 bsp_capture_timeout,
					uint32 bsp_enroll_timeout,
					uint32 bsp_max_db_size,
					uint32 bsp_max_identify,
					const char *bsp_description,
					const char *bsp_path);

BioAPI_RETURN BioAPI
BioAPIInstallDevice(INSTALL_ACTION Action,
					BioAPI_INSTALL_ERROR *pError,
					const BioAPI_UUID *device_uuid,
					uint32 device_id,
					uint32 device_num_supported_formats,
					const BioAPI_BIR_BIOMETRIC_DATA_FORMAT *device_supported_formats,
					BioAPI_MODULE_EVENT_MASK device_supported_events,
					const char *device_vendor,
					const char *device_description,
					const char *device_serial_number,
					uint32 device_hw_version_major,
					uint32 device_hw_version_minor,
					uint32 device_fw_version_major,
					uint32 device_fw_version_minor,
					BioAPI_BOOL device_authenticated);

BioAPI_RETURN BioAPI
BioAPIRemoveDevice (const BioAPI_UUID *uuid,
					BioAPI_DEVICE_ID device);


/*
 * Memory functions
 */
#ifndef BioAPI_MEMTRACK_ON

/* Standard memory functions */
void* BioAPI _BioAPI_malloc(uint32 size, void* ref);
void  BioAPI _BioAPI_free( void* ptr, void* ref);
void* BioAPI _BioAPI_realloc(void* ptr, uint32 size, void* ref);
void* BioAPI _BioAPI_calloc(uint32 num, uint32 size, void* ref);

/* Windows-style (GlobalAlloc, etc.) memory functions */
void* BioAPI _BioAPI_Win_malloc(uint32 size, void* ref);
void  BioAPI _BioAPI_Win_free( void* ptr, void* ref);
void* BioAPI _BioAPI_Win_realloc(void* ptr, uint32 size, void* ref);
void* BioAPI _BioAPI_Win_calloc(uint32 num, uint32 size, void* ref);

/* Some apps like to call the function without going through the pointers.
 * We'll give them the standard functions.
 */
#define BioAPI_malloc(s, t) _BioAPI_malloc((s), (t))
#define BioAPI_calloc(s, t, u) _BioAPI_calloc((s), (t) ,(u))
#define BioAPI_realloc(s, t, u) _BioAPI_realloc((s), (t), (u))
#define BioAPI_free(s, t) _BioAPI_free((s), (t))

#else	/* BioAPI_MEMTRACK_ON */

/* Standard memory functions */
void* BioAPI _BioAPI_malloc(uint32 size, void* ref,
							const char * szFilename, uint32 u32LineNumber);
void  BioAPI _BioAPI_free(void* ptr, void* ref,
							const char * szFilename, uint32 u32LineNumber);
void* BioAPI _BioAPI_realloc(void* ptr, uint32 size, void* ref,
							const char * szFilename, uint32 u32LineNumber);
void* BioAPI _BioAPI_calloc(uint32 num, uint32 size, void* ref,
							const char * szFilename, uint32 u32LineNumber);

/* Windows-style (GlobalAlloc, etc.) memory functions */
void* BioAPI _BioAPI_Win_malloc(uint32 size, void* ref,
							const char * szFilename, uint32 u32LineNumber);
void  BioAPI _BioAPI_Win_free(void* ptr, void* ref,
							const char * szFilename, uint32 u32LineNumber);
void* BioAPI _BioAPI_Win_realloc(void* ptr, uint32 size, void* ref,
							const char * szFilename, uint32 u32LineNumber);
void* BioAPI _BioAPI_Win_calloc(uint32 num, uint32 size, void* ref,
							const char * szFilename, uint32 u32LineNumber);

/* Some apps like to call the function without going through the pointers.
 * We'll give them the standard functions.
 */
#define BioAPI_malloc(s, t) _BioAPI_malloc((s), (t), __FILE__, __LINE__)
#define BioAPI_calloc(s, t, u) _BioAPI_calloc((s), (t) ,(u), __FILE__, __LINE__)
#define BioAPI_realloc(s, t, u) _BioAPI_realloc((s), (t), (u), __FILE__, __LINE__)
#define BioAPI_free(s, t) _BioAPI_free((s), (t), __FILE__, __LINE__)

#endif	/* BioAPI_MEMTRACK_ON */

/* Memory function tables */
extern const BioAPI_MEMORY_FUNCS BioAPIMemoryFuncs;
extern const BioAPI_MEMORY_FUNCS BioAPIWinMemoryFuncs;

/* Macro to count the "1" bits in a word */
#define _CountBits( _inword_, _count_ ) \
	{ \
		uint32 _tempword_ = _inword_; \
		_count_ = 0; \
		while ( _tempword_ ) \
		{ \
			if ( _tempword_ & 1 ) \
			{ \
				_count_++; \
			} \
			_tempword_ >>= 1; \
		} \
	}

/* Macros for byte-order conversion */
/* Linux-Port: added Linux as part of test to defined it as LITTLE ENDIAN */
/* Update:  not all Linux machines are PCs (i.e. little endian) !  So endian-ness
 *          is now determined by the configure script based on the CPU arch

#if defined (WIN32) || defined (LINUX)
#define LITTLE_ENDIAN
#endif
*/


#ifdef LITTLE_ENDIAN
#define LittleEndian4(a)	(a)
#define LocalEndian4(a)		(a)
#define LittleEndian2(a)	(a)
#define LocalEndian2(a)		(a)
#else
unsigned long LittleEndian4 (unsigned long val);
unsigned long LocalEndian4 (unsigned long val);
unsigned short LittleEndian2 (unsigned short val);
unsigned short LocalEndian2 (unsigned short val);
#endif

#ifdef __cplusplus
}
#endif

#endif /* _BIOAPI_UTIL_H_ */
