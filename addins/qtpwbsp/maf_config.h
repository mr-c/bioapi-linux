/*
 * File: MAF_CONFIG.H
 *
 * Set all BSP and device options in this file.
 */

#ifndef MAF_CONFIG_H
#define MAF_CONFIG_H

#include "bioapi.h"

/* *******************************************************************
 * BSP options
 * *******************************************************************/

/* UUID of the BSP module */
#define BSP_UUID_INITIALIZER				{ 0xba, 0xa7, 0x48, 0xa2, 0xc0, 0xc7, 0x47, 0x97, 0xa6, 0x0d, 0x46, 0x30, 0xec, 0xc1, 0xe5, 0xfd}

/* Device ID associated with the BSP */
#define BSP_DEVICE_ID						0

/* Filename of the module */
/* Linux-Port: added branch to handle Windows/Unix library name issues */
#if defined (WIN32)
#define BSP_SELF_CHECK_SECTION				"qtpwbsp.dll"
#else
#define BSP_SELF_CHECK_SECTION				"libqtpwbsp.so"
#endif

/* Major and minor versions of the product */
#define BSP_VERSION_MAJOR					1
#define BSP_VERSION_MINOR					01

/* Name of the BSP vendor */
#define BSP_VENDOR_NAME						"BioAPI Consortium"

/* Description of the BSP */
#define BSP_DESCRIPTION						"BioAPI QT Password BSP"

/* Number of supported formats */
#define BSP_NUM_SUPPORTED_FORMATS			0

/* Supported formats.  Only required if BSP_NUM_SUPPORTED_FORMATS is non-zero.
 * Number of initializers here must match number above. */
/*#define BSP_SUPPORTED_FORMATS_INITIALIZER	{ } */

/* Forms of authentication supported by the product */
#define BSP_FACTORS_MASK					BioAPI_FACTOR_PASSWORD

/* Operations supported by the product */
#define BSP_OPERATIONS						( BioAPI_CAPTURE |			\
											  BioAPI_CREATETEMPLATE |	\
											  BioAPI_PROCESS |			\
											  BioAPI_VERIFYMATCH |		\
											  BioAPI_VERIFY )

/* Options supported by the product */
#define BSP_OPTIONS							( BioAPI_PAYLOAD | BioAPI_LOCAL_BSP )

/* Minimum FAR at which a payload will be released.
 * If payloads are not supported, this should be zero. */
#define BSP_PAYLOAD_POLICY					0

/* Maximum payload length supported.
 * If payloads are not supported, this should be zero. */
#define BSP_MAX_PAYLOAD						0xffffffff

/* Default timeouts, in milliseconds */
#define BSP_VERIFY_TIMEOUT					0
#define BSP_IDENTIFY_TIMEOUT				0
#define BSP_CAPTURE_TIMEOUT					0
#define BSP_ENROLL_TIMEOUT					0

/* Maximum size of a BSP internal database.
 * If the BSP doesn't have a database, this should be zero. */
#define BSP_MAX_DB_SIZE						0

/* Largest population supported by the Identify function.
 * If Identify functions are not supported, this should be zero.
 * Use 0xffffffff to indicate unlimited. */
#define BSP_MAX_IDENTIFY					0

/* *******************************************************************
 * Device options
 * *******************************************************************/

/* If you want the install routine to install the device record immediately,
 * define this.  You would do this in the case of a device which is assumed to
 * always be present (eg. a keyboard or built-in microphone).  BSPs which will
 * dynamically detect the presence or absence of a device should not define this,
 * and call BioAPIInstallDevice when appropriate.
 * Regardless of this option, the remaining options should all be defined. */
#define BSP_INSTALL_DEVICE

/* UUID of the device. */
#define DEVICE_UUID_INITIALIZER			{ 0xba, 0xa7, 0x48, 0xa2, 0xc0, 0xc7, 0x47, 0x97, 0xa6, 0x0d, 0x46, 0x30, 0xec, 0xc1, 0xe5, 0xfd}

/* Number of supported formats */
#define DEVICE_NUM_SUPPORTED_FORMATS		0

/* Supported formats.  Only required if DEVICE_NUM_SUPPORTED_FORMATS is non-zero.
 * Number of initializers here must match number above. */
/*#define DEVICE_SUPPORTED_FORMATS_INITIALIZER	{ } */

/* Supported events. */
#define DEVICE_SUPPORTED_EVENTS				0

/* Name of the device vendor.
 * If not defined, we default it to the same as the BSP vendor.  This helps to localize changes. */
/* #define DEVICE_VENDOR_NAME					"Example Vendor" */
#ifndef DEVICE_VENDOR_NAME
 #define DEVICE_VENDOR_NAME					BSP_VENDOR_NAME
#endif

/* Description of the device */
#define DEVICE_DESCRIPTION					"BioAPI Password device"

/* Major and minor versions of the product */
#define DEVICE_HW_VERSION_MAJOR				1
#define DEVICE_HW_VERSION_MINOR				0
#define DEVICE_FW_VERSION_MAJOR				1
#define DEVICE_FW_VERSION_MINOR				0

/* Serial number of the device. This will almost always be blank here. */
#define DEVICE_SERIAL_NUMBER				""

/* Indication of whether the device has been authenticated.  This will almost always be 0
 * here, and only 1 when BioAPIInstallDevice is called dynamically by the BSP. */
#define DEVICE_AUTHENTICATED				0
 

/* *******************************************************************
 * General options
 * *******************************************************************/

/* Is the BSP module thread safe? */
#define BSP_THREAD_SAFE						1

/* Does the addin module require the writer lock during SPI functions on the load
 * tracker node (is this node being modified?) */
#define BSP_NEED_LOAD_WRITER_LOCK			0

/* Does the addin module require the writer lock during SPI functions on the attach
 * tracker node (is this node being modified?) */
#define BSP_NEED_ATTACH_WRITER_LOCK		1

/* Does the addin module require the custom ADDIN_LOAD_DATA structure (see below) */
#define ADDIN_NEED_ADDIN_LOAD_STRUCT		0

/* Does the addin module require the custom ADDIN_ATTACH_DATA structure (see below) */
#define ADDIN_NEED_ADDIN_ATTACH_STRUCT		1

/* Macro used to define if a device is valid for an addin */
#define BSP_IS_VALID_DEVICE(DeviceId)		((DeviceId) == BSP_DEVICE_ID)


/* *******************************************************************
 * Custom options
 * *******************************************************************/

#define BIR_HEADER_VERSION					1
#define BIO_DATA_FORMAT_OWNER				0
#define BIO_DATA_FORMAT_ID					0

typedef struct BioDataHeader
{
	uint32 PWLength;
	uint32 PayloadLength;
} BIO_DATA_HEADER;



#if (ADDIN_NEED_ADDIN_LOAD_STRUCT)
/*-----------------------------------------------------------------------------
 * Description:
 * Structure containing custom fields in the BioAPI_MODULE_LOAD_TRACKER structure
 *---------------------------------------------------------------------------*/
struct addin_load_data
{
	/* put custom fields here */
	uint32 dummy;
};
typedef struct addin_load_data ADDIN_LOAD_DATA;

#endif /* if ADDIN_NEED_ADDIN_LOAD_STRUCT */


#if (ADDIN_NEED_ADDIN_ATTACH_STRUCT)

typedef struct BIRHandleNode
{
	BioAPI_BIR_HANDLE BIRHandle;
	BioAPI_BIR		  BIR;
	void			   *NextNode;

} BIR_LIST_NODE;

/*-----------------------------------------------------------------------------
 * Description:
 * Structure containing custom fields in the CSSM_MODULE_ATTACH_TRACKER structure
 *
 * BIRList	   - points to the list of linked list nodes which contain the
 *				 BIRs which currently reside in the BSPs memory space\
 *
 * HandleIndex - counter which is used to ensure the handles given to the
 *				 BIRs do not collide
 *---------------------------------------------------------------------------*/
struct addin_attach_data
{
   BIR_LIST_NODE	   *BIRList;
   BioAPI_BIR_HANDLE  HandleIndex;
};
typedef struct addin_attach_data ADDIN_ATTACH_DATA;

#endif /* if ADDIN_NEED_ADDIN_ATTACH_STRUCT */

#endif
