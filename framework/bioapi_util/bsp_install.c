/*-----------------------------------------------------------------------
 * File: BSP_INSTALL.C
 *-----------------------------------------------------------------------
 */

/* Public BioAPI headers that we'll need */
#include "bioapi.h"
#include "bioapi_uuid.h"
#include "bioapi_util.h"
#include "port/bioapi_port.h"
#include "bsp_schema.h"
#include "device_schema.h"

/* CDSA header files used by this project */
#include "cssmtype.h"
#include "cssmerr.h"
#include "mds_util.h"


extern IfMdsuSchemaManipulator IfiMdsuBioAPIBspCapabilitySchema;
extern IfMdsuSchemaManipulator IfiMdsuBioAPIBspDeviceSchema;
 /* Number of MDS update operations that will be remembered by mds_util for
 * error clean-up during installation.
 */
#define BIOAPI_BSP_MDS_COMMIT_SIZE 20

static BioAPI_RETURN
__CleanMDSBioAPIDevice(MDSU_CONTEXT *pContext, const BioAPI_UUID *uuid)
{
	BioAPI_RETURN err = BioAPI_OK;
	BioAPI_DEVICE_SCHEMA DeviceTemplate;
	BioAPI_DEVICE_SCHEMA DeviceData;


	/* Execute a search on every record for this module to get a handle of all
	 * records and then delete them.
	 */

	/* Device Schema */
	BioAPI_CopyUuid (DeviceTemplate.ModuleId, uuid);
	ERR(err = MDSU_FindFirst(pContext,
							 &IfiMdsuBioAPIBspDeviceSchema,
							 &DeviceTemplate,
							 MDSU_BIOAPI_DEV_MODULEID,
							 &DeviceData,
							 NULL));
	while(BioAPI_OK == err)
	{

		ERR(err = MDSU_FindNext(pContext,
								&DeviceData,
								NULL));
	}

	if(CSSMERR_DL_ENDOFDATA == err)
	{
		/* Flush everything from the MDS */
		ERR(err = MDSU_Revert(pContext));
	}


	/* Make sure nothing gets removed if there is an error */
	if((CSSMERR_DL_ENDOFDATA != err) &&
		(BioAPI_OK != err))
	{
		MDSU_Commit( pContext );
	}

	return err;
}

static BioAPI_RETURN
__CleanMDS(MDSU_CONTEXT *pContext, const BioAPI_UUID *uuid)
{
	BioAPI_RETURN err = BioAPI_OK;
	BioAPI_BSP_SCHEMA BioAPIBspCapTemplate;
	BioAPI_BSP_SCHEMA BioAPIBspCapData;

	/* Execute a search on every record for this module to get a handle of all
	 * records and then delete them.
	 */

	/* DL Primary Schema */
	BioAPI_CopyUuid (BioAPIBspCapTemplate.ModuleId, uuid);
	ERR(err = MDSU_FindFirst(pContext,
							 &IfiMdsuBioAPIBspCapabilitySchema,
							 &BioAPIBspCapTemplate,
							 MDSU_BIOAPI_CAP_MODULE_ID,
							 &BioAPIBspCapData,
							 NULL));
	while(BioAPI_OK == err)
	{
		ERR(err = MDSU_FindNext(pContext,
								&BioAPIBspCapData,
								NULL));
	}

	if(CSSMERR_DL_ENDOFDATA == err)
	{
		/* Flush everything from the MDS */
		ERR(err = MDSU_Revert(pContext));

	} /* BioAPIBspCapability removed */

	/* Make sure nothing gets removed if there is an error */
	if((CSSMERR_DL_ENDOFDATA != err) &&
		(BioAPI_OK != err))
	{
		MDSU_Commit( pContext );
	}
	err = __CleanMDSBioAPIDevice(pContext, uuid);

	return err;
}


static void bioapi_Get_Bsp_Capability_Schema_Attributes(CSSM_DB_SCHEMA_ATTRIBUTE_INFO **pList)
{
	static CSSM_DB_SCHEMA_ATTRIBUTE_INFO rgAttributes[BIOAPI_BSP_NUM_ATTRIBUTES] = {
		BIOAPI_SCHEMA_BSP_MODULEID,
		BIOAPI_SCHEMA_BSP_DEVICEID,
		BIOAPI_SCHEMA_BSP_BSPNAME,
		BIOAPI_SCHEMA_BSP_SPECVERSION,
		BIOAPI_SCHEMA_BSP_PRODUCTVERSION,
		BIOAPI_SCHEMA_BSP_VENDOR,
		BIOAPI_SCHEMA_BSP_SUPPORTEDFORMATS,
		BIOAPI_SCHEMA_BSP_FACTORSMASK,
		BIOAPI_SCHEMA_BSP_OPERATIONS,
		BIOAPI_SCHEMA_BSP_OPTIONS,
		BIOAPI_SCHEMA_BSP_PAYLOADPOLICY,
		BIOAPI_SCHEMA_BSP_MAXPAYLOADSIZE,
		BIOAPI_SCHEMA_BSP_DEFAULTVERIFYTIMEOUT,
		BIOAPI_SCHEMA_BSP_DEFAULTIDENTIFYTIMEOUT,
		BIOAPI_SCHEMA_BSP_DEFAULTCAPTURETIMEOUT,
		BIOAPI_SCHEMA_BSP_DEFAULTENROLLTIMEOUT,
		BIOAPI_SCHEMA_BSP_MAXBSPDBSIZE,
		BIOAPI_SCHEMA_BSP_MAXIDENTIFY,
		BIOAPI_SCHEMA_BSP_DESCRIPTION,
		BIOAPI_SCHEMA_BSP_PATH
	};

	*pList = rgAttributes;
}

static void bioapi_Get_Bsp_Capability_Indexes(CSSM_DB_SCHEMA_INDEX_INFO ** pList)
{
	static CSSM_DB_SCHEMA_INDEX_INFO rgIndexes[BIOAPI_BSP_NUM_ATTRIBUTES] = {
		BIOAPI_INDEX_SCHEMA_BSP_MODULEID,
		BIOAPI_INDEX_SCHEMA_BSP_DEVICEID,
		BIOAPI_INDEX_SCHEMA_BSP_BSPNAME,
		BIOAPI_INDEX_SCHEMA_BSP_SPECVERSION,
		BIOAPI_INDEX_SCHEMA_BSP_PRODUCTVERSION,
		BIOAPI_INDEX_SCHEMA_BSP_VENDOR,
		BIOAPI_INDEX_SCHEMA_BSP_SUPPORTEDFORMATS,
		BIOAPI_INDEX_SCHEMA_BSP_FACTORSMASK,
		BIOAPI_INDEX_SCHEMA_BSP_OPERATIONS,
		BIOAPI_INDEX_SCHEMA_BSP_OPTIONS,
		BIOAPI_INDEX_SCHEMA_BSP_PAYLOADPOLICY,
		BIOAPI_INDEX_SCHEMA_BSP_MAXPAYLOADSIZE,
		BIOAPI_INDEX_SCHEMA_BSP_DEFAULTVERIFYTIMEOUT,
		BIOAPI_INDEX_SCHEMA_BSP_DEFAULTIDENTIFYTIMEOUT,
		BIOAPI_INDEX_SCHEMA_BSP_DEFAULTCAPTURETIMEOUT,
		BIOAPI_INDEX_SCHEMA_BSP_DEFAULTENROLLTIMEOUT,
		BIOAPI_INDEX_SCHEMA_BSP_MAXBSPDBSIZE,
		BIOAPI_INDEX_SCHEMA_BSP_MAXIDENTIFY,
		BIOAPI_INDEX_SCHEMA_BSP_DESCRIPTION,
		BIOAPI_INDEX_SCHEMA_BSP_PATH
	};

	*pList = rgIndexes;
}

static void bioapi_Get_Bsp_Device_Schema_Attributes(CSSM_DB_SCHEMA_ATTRIBUTE_INFO **pList)
{
	static CSSM_DB_SCHEMA_ATTRIBUTE_INFO rgDeviceAttributes[BIOAPI_BIO_DEVICE_NUM_ATTRIBUTES] = {
		BIOAPI_SCHEMA_DEVICE_MODULEID,
		BIOAPI_SCHEMA_DEVICE_DEVICEID,
		BIOAPI_SCHEMA_DEVICE_SUPPORTEDFORMATS,
		BIOAPI_SCHEMA_DEVICE_SUPPORTEDEVENTS,
		BIOAPI_SCHEMA_DEVICE_VENDOR,
		BIOAPI_SCHEMA_DEVICE_DESCRIPTION,
		BIOAPI_SCHEMA_DEVICE_SERIALNUMBER,
		BIOAPI_SCHEMA_DEVICE_HARDWAREVERSION,
		BIOAPI_SCHEMA_DEVICE_FIRMWAREVERSION,
		BIOAPI_SCHEMA_DEVICE_AUTHENTICATEDDEVICE
	};

	*pList = rgDeviceAttributes;
}

static void bioapi_Get_Bsp_Device_Schema_Indexes(CSSM_DB_SCHEMA_INDEX_INFO ** pList)
{
	static CSSM_DB_SCHEMA_INDEX_INFO rgDeviceIndexes[BIOAPI_BIO_DEVICE_NUM_ATTRIBUTES] = {
		BIOAPI_INDEX_SCHEMA_DEVICE_MODULEID,
		BIOAPI_INDEX_SCHEMA_DEVICE_DEVICEID,
		BIOAPI_INDEX_SCHEMA_DEVICE_SUPPORTEDFORMATS,
		BIOAPI_INDEX_SCHEMA_DEVICE_SUPPORTEDEVENTS,
		BIOAPI_INDEX_SCHEMA_DEVICE_VENDOR,
		BIOAPI_INDEX_SCHEMA_DEVICE_DESCRIPTION,
		BIOAPI_INDEX_SCHEMA_DEVICE_SERIALNUMBER,
		BIOAPI_INDEX_SCHEMA_DEVICE_HARDWAREVERSION,
		BIOAPI_INDEX_SCHEMA_DEVICE_FIRMWAREVERSION,
		BIOAPI_INDEX_SCHEMA_DEVICE_AUTHENTICATEDDEVICE
	};

	*pList = rgDeviceIndexes;
}


/* Creates the new relation*/
BioAPI_RETURN BioAPI bioapi_CreateMDSRelation (const char *szDirectoryName,
										CSSM_DB_RECORDTYPE RelationID,
										const char *RelationName,
										uint32 NumberOfAttributes,
										const CSSM_DB_SCHEMA_ATTRIBUTE_INFO *pAttributeInfo,
										uint32 NumberOfIndexes,
										const CSSM_DB_SCHEMA_INDEX_INFO *pIndexInfo,
										const BioAPI_UUID *uuid)
{
	static MDS_FUNCS s_bioapiMDSFuncs;
	MDS_DB_HANDLE hMds;
	BioAPI_RETURN ret;
	if ( ( ret = MDS_Initialize(uuid,
								NULL,
								&BioAPIMemoryFuncs,
								&s_bioapiMDSFuncs,
								&(hMds.DLHandle) ) ) == BioAPI_OK )
	{
		/* Open the requested MDS directory */
		if ( ( ret = s_bioapiMDSFuncs.DbOpen( hMds.DLHandle,
										 szDirectoryName,
										 NULL,
										 CSSM_DB_ACCESS_READ | CSSM_DB_ACCESS_WRITE | CSSM_DB_ACCESS_PRIVILEGED,
										 NULL,
										 NULL,
										 &hMds.DBHandle ) ) == BioAPI_OK )
		{
			ret = s_bioapiMDSFuncs.CreateRelation(
											hMds,
											RelationID,
											RelationName,
											NumberOfAttributes,
											pAttributeInfo,
											NumberOfIndexes,
											pIndexInfo);

		}

		s_bioapiMDSFuncs.DbClose ( hMds );

		MDS_Terminate( hMds.DLHandle );

	} /* MDS_Initialize */
	return (ret);
}

BioAPI_RETURN BioAPI bioapi_UpdateDeviceInfo( BioAPI_DEVICE_SCHEMA* BioAPIBspDevice,
										BioAPI_BOOL bInstall,
										const BioAPI_UUID *uuid)
{
	BioAPI_RETURN err = BioAPI_OK;
	MDSU_CONTEXT MDSUContext;

	/* Initialize MDS */
	if (ERR(err = MDSU_Init(&MDSUContext,
							&BioAPIMemoryFuncs,
							uuid,
							MDS_CDSA_DIRECTORY,
							(CSSM_DB_ACCESS_READ | CSSM_DB_ACCESS_WRITE),
							BIOAPI_BSP_MDS_COMMIT_SIZE)) == BioAPI_OK)
	{
		if( ( bInstall == CSSM_TRUE) && (BioAPIBspDevice != NULL) )
		{
			/* Insert the common schema record into the MDS */
			ERR(err = MDSU_UpdateSchema(&MDSUContext,
										&IfiMdsuBioAPIBspDeviceSchema,
										BioAPIBspDevice,
										NULL));
			if(BioAPI_OK == err)
			{
				/* commit the records */
				MDSU_Commit(&MDSUContext);

				/* reset error to BioAPI_OK */
				err = BioAPI_OK;
			}
			else
			{
				/* there's been an error, so we back out all changes */
				MDSU_Revert( &MDSUContext );
			}
		}
		else
		{
			err = __CleanMDSBioAPIDevice(&MDSUContext, uuid);
		}
		/* Shut down the MDS connection */
		MDSU_Term( &MDSUContext );
	}
	return err;
}

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
					const char *bsp_path)
{
	BioAPI_RETURN err = BioAPI_OK;
	MDSU_CONTEXT MDSUContext;
	CSSM_DB_SCHEMA_ATTRIBUTE_INFO_PTR CapAttrInfo = NULL;
	CSSM_DB_SCHEMA_INDEX_INFO_PTR CapIndexInfo = NULL;
	BioAPI_BSP_SCHEMA BioAPIBspCapability;
	uint32 format;

	if (INSTALL_ACTION_UNINSTALL != Action)
	{
		bioapi_Get_Bsp_Capability_Schema_Attributes(&CapAttrInfo);
		bioapi_Get_Bsp_Capability_Indexes(&CapIndexInfo);
		err = bioapi_CreateMDSRelation (MDS_CDSA_DIRECTORY,
							BIOAPI_BSP_RECORDTYPE,
							BIOAPI_BSP_CAPABILITIES,
							BIOAPI_BSP_NUM_ATTRIBUTES,
							CapAttrInfo,
							BIOAPI_BSP_NUM_ATTRIBUTES,
							CapIndexInfo,
							bsp_uuid);
	}
	if( err == BioAPI_OK || err == CSSMERR_DL_INVALID_RECORDTYPE )
	{
		/* Initialize MDS */
		if (ERR(err = MDSU_Init(&MDSUContext,
								&BioAPIMemoryFuncs,
								bsp_uuid,
								MDS_CDSA_DIRECTORY,
								(CSSM_DB_ACCESS_READ | CSSM_DB_ACCESS_WRITE),
								BIOAPI_BSP_MDS_COMMIT_SIZE)) == BioAPI_OK)
		{
			err = __CleanMDS(&MDSUContext, bsp_uuid);

			if((BioAPI_OK == err) &&
				(INSTALL_ACTION_UNINSTALL != Action))
			{
				/* This is a sample entry in the capabilities record. This needs to be chnaged by the BSP
				vendor, filling in real values*/
				BioAPI_CopyUuid ( BioAPIBspCapability.ModuleId, bsp_uuid );
				BioAPIBspCapability.DeviceId = device_id;
				strcpy( BioAPIBspCapability.BSPName, bsp_name);
				BioAPIBspCapability.SpecVersion.Major = BioAPI_MAJOR;
				BioAPIBspCapability.SpecVersion.Minor = BioAPI_MINOR;
				BioAPIBspCapability.ProductVersion.Major = bsp_version_major;
				BioAPIBspCapability.ProductVersion.Minor = bsp_version_minor;
				strcpy( BioAPIBspCapability.Vendor, bsp_vendor_name);
				BioAPIBspCapability.NumSupportedFormats = bsp_num_supported_formats;
				for (format = 0; format < bsp_num_supported_formats; format ++)
				{
					BioAPIBspCapability.BspSupportedFormats[format].FormatOwner =
							bsp_supported_formats[format].FormatOwner;
					BioAPIBspCapability.BspSupportedFormats[format].FormatID =
							bsp_supported_formats[format].FormatID;
				}
				BioAPIBspCapability.FactorsMask = bsp_factors_mask;
				BioAPIBspCapability.Operations = bsp_operations;
				BioAPIBspCapability.Options = bsp_options;
				BioAPIBspCapability.PayloadPolicy = bsp_payload_policy;
				BioAPIBspCapability.MaxPayloadSize = bsp_max_payload;
				BioAPIBspCapability.DefaultVerifyTimeout = bsp_verify_timeout;
				BioAPIBspCapability.DefaultIdentifyTimeout = bsp_identify_timeout;
				BioAPIBspCapability.DefaultCaptureTimeout = bsp_capture_timeout;
				BioAPIBspCapability.DefaultEnrollTimeout = bsp_enroll_timeout;
				BioAPIBspCapability.MaxBspDbSize = bsp_max_db_size;
				BioAPIBspCapability.MaxIdentify = bsp_max_identify;
				strcpy( BioAPIBspCapability.Description, bsp_description);
				strcpy( BioAPIBspCapability.Path, bsp_path);

				/* Write the data to MDS */
				ERR(err = MDSU_UpdateSchema(&MDSUContext,
											&IfiMdsuBioAPIBspCapabilitySchema,
											&BioAPIBspCapability,
											NULL));

				if(BioAPI_OK == err)
				{
					/* commit the records */
					MDSU_Commit(&MDSUContext);

					err = BioAPI_OK;
				}
				else
				{
					/* there's been an error, so we back out all changes */
					MDSU_Revert( &MDSUContext );
					sprintf(pError->ErrorString,
							"MDS Error (DL Primary): %X\n",
							err);
					pError->ErrorCode = err;
				}
			}

			/* Shut down the MDS connection */
			MDSU_Term( &MDSUContext );
		}
		else
		{
			sprintf(pError->ErrorString,
					"MDS Error (Init): %X\n",
					err);
			pError->ErrorCode = err;
		}
	}
	return err;
}

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
					BioAPI_BOOL device_authenticated)
{
	BioAPI_RETURN err = BioAPI_OK;
	BioAPI_DEVICE_SCHEMA BioAPIBspDevice;
	uint32 format;

	/* Folllowing is Sample Updating of Device schema. This should be called after
	the device is inserted*/
	BioAPI_CopyUuid (&BioAPIBspDevice.ModuleId, device_uuid);
	BioAPIBspDevice.DeviceId = device_id;
	BioAPIBspDevice.NumSupportedFormats = device_num_supported_formats;
	for (format = 0; format < device_num_supported_formats; format ++)
	{
		BioAPIBspDevice.DeviceSupportedFormats[format].FormatOwner =
				device_supported_formats[format].FormatOwner;
		BioAPIBspDevice.DeviceSupportedFormats[format].FormatID =
				device_supported_formats[format].FormatID;
	}
	BioAPIBspDevice.SupportedEvents = device_supported_events;
	strcpy( BioAPIBspDevice.DeviceVendor, device_vendor);
	strcpy( BioAPIBspDevice.DeviceDescription, device_description);
	strcpy( BioAPIBspDevice.DeviceSerialNumber, device_serial_number);
	BioAPIBspDevice.DeviceHardwareVersion.Major = device_hw_version_major;
	BioAPIBspDevice.DeviceHardwareVersion.Minor = device_hw_version_minor;
	BioAPIBspDevice.DeviceFirmwareVersion.Major = device_fw_version_major;
	BioAPIBspDevice.DeviceFirmwareVersion.Minor = device_fw_version_minor;
	BioAPIBspDevice.AuthenticatedDevice = device_authenticated;
	err = bioapi_UpdateDeviceInfo( &BioAPIBspDevice, BioAPI_TRUE, device_uuid);

	return err;
}

BioAPI_RETURN BioAPI
BioAPIRemoveDevice (const BioAPI_UUID *uuid, BioAPI_DEVICE_ID device)
{
	BioAPI_RETURN err = BioAPI_OK;
	BioAPI_DEVICE_SCHEMA DeviceTemplate;
	BioAPI_DEVICE_SCHEMA DeviceData;
	CSSM_DB_UNIQUE_RECORD_PTR TempId;

	MDSU_CONTEXT MDSUContext;

	/* Initialize MDS */
	if (ERR(err = MDSU_Init(&MDSUContext,
							&BioAPIMemoryFuncs,
							uuid,
							MDS_CDSA_DIRECTORY,
							(CSSM_DB_ACCESS_READ | CSSM_DB_ACCESS_WRITE),
							BIOAPI_BSP_MDS_COMMIT_SIZE)) == BioAPI_OK)
	{
		/* Execute a search on every record for this module to find the specified
		 * record and then delete it.
		 */

		/* Device Schema */
		BioAPI_CopyUuid (DeviceTemplate.ModuleId, uuid);
		DeviceTemplate.DeviceId = device;
		ERR(err = MDSU_FindFirst(&MDSUContext,
								 &IfiMdsuBioAPIBspDeviceSchema,
								 &DeviceTemplate,
								 MDSU_BIOAPI_DEV_MODULEID | MDSU_BIOAPI_DEV_DEVICEID,
								 &DeviceData,
								 &TempId));
		if (BioAPI_OK == err)
			err = MDSU_DeleteRecord( &MDSUContext, TempId );

		/* Shut down the MDS connection */
		MDSU_Term( &MDSUContext );
	}

	return err;
}
