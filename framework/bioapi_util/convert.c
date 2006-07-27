/*
 * convert.c
 *
 * General-purpose conversion functions.
 *
 * These are NOT a part of the BioAPI specification.
 */

#include "bioapi_util.h"
#include "bioapi_uuid.h"
#include "bioapi_err.h"
#include "port/bioapi_port.h"

/**-------------------------------------------------------------------------
 ** BioAPI_GetPrintableUUID
 **
 ** Description
 **	 Formats a BioAPI_UUID structure into a printable format. The output
 **	 matches the format used by the Win32 registry.
 **
 ** Parameters
 **	 pGUID (input) - The BioAPI_UUID structure to convert.
 **	 PrintableGUID (output) - Pointer to a character buffer with enough
 **	  space for at least BioAPI_PRINTABLE_UUID_LENGTH characters.
 **
 ** Return Value
 **	 BioAPI_ERRCODE_INVALID_INPUT_POINTER - The memory referenced by the pUUID
 **	  pointer is unreadable or the PrintableUUID parameter is not writable.
 **	 BioAPI_OK - The UUID has been converted.
 *--------------------------------------------------------------------------
 **/
BioAPI_RETURN BioAPI BioAPI_GetPrintableUUID( const BioAPI_UUID *pUUID,
		char *PrintableUUID )
{
	/* Parameter checks */
	if ( port_IsBadReadPtr( pUUID, sizeof(BioAPI_UUID) ) ||
		 port_IsBadWritePtr( PrintableUUID, BioAPI_PRINTABLE_UUID_LENGTH ) )
	{
		return BioAPI_ERRCODE_INVALID_INPUT_POINTER;
	}

	/* Format the output */
	sprintf( PrintableUUID,
			 BioAPI_UUID_FORMAT_STRING,
			 (*pUUID)[0], (*pUUID)[1], (*pUUID)[2], (*pUUID)[3],
			 (*pUUID)[4], (*pUUID)[5],
			 (*pUUID)[6], (*pUUID)[7],
			 (*pUUID)[8], (*pUUID)[9],
			 (*pUUID)[10],
			 (*pUUID)[11],
			 (*pUUID)[12],
			 (*pUUID)[13],
			 (*pUUID)[14],
			 (*pUUID)[15]);

	return BioAPI_OK;
}

BioAPI_RETURN BioAPI BioAPI_GetStructuredUUID( const char *PrintableUUID,
		BioAPI_UUID_PTR pUUID )
{
	int nCount;
	int tempUUID[16];
	uint8 *pbUUID = (uint8*) pUUID;

	/* Parameter checks */
	if ( ( port_IsBadWritePtr( pUUID, sizeof(BioAPI_UUID) ) != BioAPI_OK ) ||
		 ( port_IsBadReadPtr( PrintableUUID, BioAPI_PRINTABLE_UUID_LENGTH )
		 != BioAPI_OK ) )
	{
		return BioAPI_ERRCODE_INVALID_POINTER;
	}

	/* Scan the input into a temporary integer array */
	nCount = sscanf( PrintableUUID,
					 BioAPI_UUID_FORMAT_STRING,
					 &(tempUUID)[0], &(tempUUID)[1], &(tempUUID)[2], &(tempUUID)[3],
					 &(tempUUID)[4], &(tempUUID)[5],
					 &(tempUUID)[6], &(tempUUID)[7],
					 &(tempUUID)[8], &(tempUUID)[9],
					 &(tempUUID)[10],
					 &(tempUUID)[11],
					 &(tempUUID)[12],
					 &(tempUUID)[13],
					 &(tempUUID)[14],
					 &(tempUUID)[15]);

	if ( nCount != 16 )
	{
		return BioAPIERR_BSP_INVALID_DATA;
	}

	/* Copy the integers into the chars */
	for (nCount = 0; nCount < 16; nCount ++)
	{
		pbUUID[nCount] = (uint8) tempUUID[nCount];
	}

	return BioAPI_OK;
}

BioAPI_RETURN BioAPI BioAPI_GetPrintableVersion( const BioAPI_VERSION *pVersion,
		char *PrintableVersion )
{
	/* Parameter check */
	if ( ( pVersion == NULL ) ||
		 ( PrintableVersion == NULL ) )
	{
		return BioAPI_ERRCODE_INVALID_POINTER;
	}

	/* format the output: any minor version number other than 0 will format as two chars
	   ie. 1.01 rather than 1.1, which is easily confused with 1.10 */
	if (pVersion->Minor)
		sprintf( PrintableVersion, "%d.%02d", pVersion->Major, pVersion->Minor );
	else
		sprintf( PrintableVersion, "%d.%d", pVersion->Major, pVersion->Minor );

	return BioAPI_OK;
}

BioAPI_RETURN BioAPI BioAPI_GetStructuredVersion( const char *PrintableVersion,
		BioAPI_VERSION *pVersion )
{
	int nCount;

	/* Parameter check */
	if ( ( pVersion == NULL ) ||
		 ( PrintableVersion == NULL ) )
	{
		return BioAPI_ERRCODE_INVALID_POINTER;
	}

	/* format the output */
	nCount = sscanf( PrintableVersion, "%d.%d",
					 &pVersion->Major,
					 &pVersion->Minor );

	return BioAPI_OK;
}

#ifndef LITTLE_ENDIAN

/*
 * Right now, these functions are symmetric.  This is the case for a big-endian (aka 1234) system.
 * Little-endian (aka 4321) is handled as no-ops in the header file.  However, there are other
 * hardware architectures (e.g. 2143 and 3412) and these may not all be symmetric.  Therefore, we
 * define the functions as if they are asymmetric operations.
 */

unsigned long LittleEndian4 (unsigned long val)
{
	return ((val & 0xff000000) >> 24) | ((val & 0x00ff0000) >> 8) |
			((val & 0x0000ff00) << 8) | ((val & 0x000000ff) << 24);
}

unsigned long LocalEndian4 (unsigned long val)
{
	return ((val & 0xff000000) >> 24) | ((val & 0x00ff0000) >> 8) |
			((val & 0x0000ff00) << 8) | ((val & 0x000000ff) << 24);
}

unsigned short LittleEndian2 (unsigned short val)
{
	return ((val & 0xff00) >> 8) | ((val & 0x00ff) << 8);
}

unsigned short LocalEndian2 (unsigned short val)
{
	return ((val & 0xff00) >> 8) | ((val & 0x00ff) << 8);
}

#endif
