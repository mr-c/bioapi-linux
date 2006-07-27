/*
 * INSTALL.C
 *
 * Install the BSP.
 *
 * This file should not generally need to be changed, unless it is not sufficient to
 * #define values (in the case of values determined at install time, for example).
 * Also, any BSP that is creating a device record at install time, and is able to
 * determine the serial number of the device at this time, should insert whatever
 * code is required to find the serial number before the call to BioAPIInstallDevice.
 */

#include "maf_config.h"
#include "bioapi_util.h"
#include "port/bioapi_port.h"

/* Initialize a global variable with the BSP UUID */
const BioAPI_UUID BSP_UUID = BSP_UUID_INITIALIZER;

BioAPI_RETURN BioAPI
INSTALL_ENTRY_NAME(const char *szModuleName,
				   const char *szSearchPath,
				   BioAPI_DATA *reserved,
				   INSTALL_ACTION Action,
				   BioAPI_INSTALL_ERROR *pError)
{
#if BSP_NUM_SUPPORTED_FORMATS
	/* Initialize a variable with the supported BSP formats.
	 * If you get a "too many initializers" error here, check maf_config.h and ensure that
	 * the number of pairs in BSP_SUPPORTED_FORMATS_INITIALIZER is the same as specified in
	 * BSP_NUM_SUPPORTED_FORMATS. */
	const BioAPI_BIR_BIOMETRIC_DATA_FORMAT BSP_SUPPORTED_FORMATS[BSP_NUM_SUPPORTED_FORMATS] =
			BSP_SUPPORTED_FORMATS_INITIALIZER;
#else
	/* No supported BSP formats. */
	#define BSP_SUPPORTED_FORMATS	NULL
#endif

#if DEVICE_NUM_SUPPORTED_FORMATS
	/* Initialize a variable with the supported device formats.
	 * If you get a "too many initializers" error here, check maf_config.h and ensure that
	 * the number of pairs in DEVICE_SUPPORTED_FORMATS_INITIALIZER is the same as specified in
	 * DEVICE_NUM_SUPPORTED_FORMATS. */
	const BioAPI_BIR_BIOMETRIC_DATA_FORMAT DEVICE_SUPPORTED_FORMATS[DEVICE_NUM_SUPPORTED_FORMATS] =
			DEVICE_SUPPORTED_FORMATS_INITIALIZER;
#else
	/* No supported device formats. */
	#define DEVICE_SUPPORTED_FORMATS	NULL
#endif

	BioAPI_RETURN err;

	/* Check that the filename matches - very rudimentary integrity. */
	if (INSTALL_ACTION_UNINSTALL != Action && stricmp (BSP_SELF_CHECK_SECTION, szModuleName))
	{
		sprintf(pError->ErrorString,
				"Install error: filename mismatch.\n");
		pError->ErrorCode = BioAPI_ERRCODE_INVALID_DATA;
		return BioAPI_ERRCODE_INVALID_DATA;
	}

	/* All of the constants passed below are defined in maf_config.h.  Change
	 * them there, not here.  Once that's done, this is easy! */
	err = BioAPIInstallBsp(Action,
							pError,
							&BSP_UUID,
							BSP_DEVICE_ID,
							szModuleName,
							BSP_VERSION_MAJOR,
							BSP_VERSION_MINOR,
							BSP_VENDOR_NAME,
							BSP_NUM_SUPPORTED_FORMATS,
							BSP_SUPPORTED_FORMATS,
							BSP_FACTORS_MASK,
							BSP_OPERATIONS,
							BSP_OPTIONS,
							BSP_PAYLOAD_POLICY,
							BSP_MAX_PAYLOAD,
							BSP_VERIFY_TIMEOUT,
							BSP_IDENTIFY_TIMEOUT,
							BSP_CAPTURE_TIMEOUT,
							BSP_ENROLL_TIMEOUT,
							BSP_MAX_DB_SIZE,
							BSP_MAX_IDENTIFY,
							BSP_DESCRIPTION,
							szSearchPath);
	if (BioAPI_OK != err)
	{
		sprintf(pError->ErrorString,
				"MDS Error (Init): %X\n",
				err);
		pError->ErrorCode = err;
	}

#ifdef BSP_INSTALL_DEVICE
	else
	{
		/* Initialize a variable with the device UUID */
		const BioAPI_UUID DEVICE_UUID = DEVICE_UUID_INITIALIZER;
		err = BioAPIInstallDevice(Action,
								pError,
								&DEVICE_UUID,
								BSP_DEVICE_ID,
								DEVICE_NUM_SUPPORTED_FORMATS,
								DEVICE_SUPPORTED_FORMATS,
								DEVICE_SUPPORTED_EVENTS,
								DEVICE_VENDOR_NAME,
								DEVICE_DESCRIPTION,
								DEVICE_SERIAL_NUMBER,
								DEVICE_HW_VERSION_MAJOR,
								DEVICE_HW_VERSION_MINOR,
								DEVICE_FW_VERSION_MAJOR,
								DEVICE_FW_VERSION_MINOR,
								DEVICE_AUTHENTICATED);
	}
#endif

	return err;
}
