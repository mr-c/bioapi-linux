/*-----------------------------------------------------------------------
 * File: INS_MDS.C
 *
 * Copyright (c) 1995-2000 Intel Corporation. All rights reserved.
 *-----------------------------------------------------------------------
 */

#pragma warning (disable:4201 4514 4214 4115)
#include <stdio.h>
#ifdef WIN32
  #include <io.h>
#endif
  #include <sys/types.h>
  #include <sys/stat.h>
#ifdef WIN32
  #include <windows.h>
#endif
#pragma warning (default:4201 4214 4115)
/* note that error 4514 is "unreferenced inline function has been removed" */

#if defined(WIN32) && defined(_DEBUG)
#include <crtdbg.h>
#endif

#include "ins_mds.h"
#include "bioapi_util.h"

/* 
 * Linux-Port: typedefs already defined in mds.h which is included ins_mds.h
 */
/*
typedef CSSM_RETURN (CSSMAPI *_MDSAPI_Initialize_PTR)(
					const CSSM_GUID *pCallerGuid,
					const CSSM_DATA *pCallerManifest,
					const CSSM_MEMORY_FUNCS *pMemoryFunctions,
					MDS_FUNCS *pDlFunctions,
					MDS_HANDLE *hMds );

typedef CSSM_RETURN (CSSMAPI *_MDSAPI_Terminate_PTR)(
					MDS_HANDLE MdsHandle );

typedef CSSM_RETURN (CSSMAPI *_MDSAPI_Install_PTR)(
					MDS_HANDLE MdsHandle );

typedef CSSM_RETURN (CSSMAPI *_MDSAPI_Uninstall_PTR)(
					MDS_HANDLE MdsHandle );
*/


uint32* create_cdsadir_recordtypes()
{
	uint32 cdsadir_record_type[14];
	uint32* recordids = NULL;
	int i;
	uint32* tmpptr = NULL;
	cdsadir_record_type[0] = MDS_CDSA_SCHEMA_START + 1;
	cdsadir_record_type[1] = MDS_CDSA_SCHEMA_START + 2;
	cdsadir_record_type[2] = MDS_CDSA_SCHEMA_START + 3;
	cdsadir_record_type[3] = MDS_CDSA_SCHEMA_START + 4;
	cdsadir_record_type[4] = MDS_CDSA_SCHEMA_START + 5;
	cdsadir_record_type[5] = MDS_CDSA_SCHEMA_START + 6;
	cdsadir_record_type[6] = MDS_CDSA_SCHEMA_START + 7;
	cdsadir_record_type[7] = MDS_CDSA_SCHEMA_START + 8;
	cdsadir_record_type[8] = MDS_CDSA_SCHEMA_START + 9;
	cdsadir_record_type[9] = MDS_CDSA_SCHEMA_START + 10;
	cdsadir_record_type[10] = MDS_CDSA_SCHEMA_START + 11;
	cdsadir_record_type[11] = MDS_CDSA_SCHEMA_START + 12;
	cdsadir_record_type[12] = MDS_CDSA_SCHEMA_START + 13;
	cdsadir_record_type[13] = MDS_CDSA_SCHEMA_START + 14;
	recordids = malloc( sizeof(uint32)*14);
	if(recordids == NULL)
		return NULL;
	tmpptr = recordids;
	for( i=0; i< 14;i++)
	{
		memcpy( tmpptr, &cdsadir_record_type[i], sizeof(uint32));
		tmpptr ++;
	}
	return recordids;
}

unsigned char* ReadManifest( uint32* Size,
							 char* FileName)
{
	FILE *fi;
	struct _stat st;
	sint32 size = 0;
	unsigned char* Data = NULL;
	if (NULL == Size)
		return NULL;

	if (NULL == FileName) {
		return NULL;
	}

	/*
	 * open file for reading
	 */
	if ((fi = fopen(FileName, "rb")) == NULL)
		return NULL;
	/*
	 * stat the file and find out how much memory we need
	 */
	if ((_stat(FileName, &st)) != 0)
		return NULL;

	size = st.st_size;
	if( size == -1)
	{
		fclose( fi);
		return NULL;
	}
	*Size = (uint32)size;
	/*
	 * allocate enough memory to hold the image
	 */
	Data = (unsigned char *)malloc(size);
	if (NULL == Data)
		return NULL;

	/*
	 * Read the file
	 */
	if ((fread((void*)Data, 1, size, fi)) != (uint32)size) {
		free(Data);
		fclose(fi);
		return NULL;
	}

	fclose(fi);

	return Data;
}

INSTALL_ERROR
write_mdsinfo_to_registry( char* ManifestPath,
						   char* DBDestPath,
						   char* DestinationPath)
{
	CSSM_RETURN Rtn;
	char RegKey[_MAX_PATH];
	char dbRegKey[_MAX_PATH];
	uint32* cdsadir_record_types=NULL;
	uint32 VersionMajor = MDS_MAJOR;
	uint32 VersionMinor = MDS_MINOR;

	strcpy( RegKey, MDS_REGISTRY_PATH);

	Rtn = dlutil_port_SetValue(RegKey,
							   "VersionMajor",
							   (void*)&VersionMajor,
							   sizeof(uint32),
							   DLUTIL_VALUE_BINARY,
							   0);

	if( Rtn != CSSM_OK )
		return( INSTALL_REGISTRY_ERROR );

	Rtn = dlutil_port_SetValue(RegKey,
							   "VersionMinor",
							   (void*)&VersionMinor,
							   sizeof(uint32),
							   DLUTIL_VALUE_BINARY,
							   0);

	if( Rtn != CSSM_OK )
		return( INSTALL_REGISTRY_ERROR );

	Rtn = dlutil_port_SetValue(RegKey,
							   "MDSLocation",
							   (void*)DestinationPath,
							   strlen(DestinationPath),
							   DLUTIL_VALUE_STRING,
							   0);

	if( Rtn != CSSM_OK )
		return( INSTALL_REGISTRY_ERROR );

	strcpy( dbRegKey,RegKey);
	strcat( dbRegKey, MDS_CDSA_DIRECTORY);

	Rtn = dlutil_port_SetValue(dbRegKey,
							   "DBLocation",
							   (void*)DBDestPath,
							   strlen(DBDestPath),
							   DLUTIL_VALUE_STRING,
							   0);

	if( Rtn != CSSM_OK )
		return( INSTALL_REGISTRY_ERROR );

	cdsadir_record_types = create_cdsadir_recordtypes();
	if (!cdsadir_record_types)
		return( INSTALL_REGISTRY_ERROR );

	Rtn = dlutil_port_SetValue(dbRegKey,
							   "RecordIdentifiers",
							   (void*)&cdsadir_record_types,
							   sizeof(uint32)*14,
							   DLUTIL_VALUE_BINARY,
							   0);
	free (cdsadir_record_types);

	if( Rtn != CSSM_OK )
		return( INSTALL_REGISTRY_ERROR );

	return( INSTALL_SUCCESS );
}

INSTALL_ERROR
install_mds ( char* MDSPath,
			  char* dbpath,
			  char* CredPath )
{
	CSSM_LIB_HANDLE hMDSInst;
	CSSM_PROC_ADDR pfInit,	pfInstall, pfTerminate, pfUninstall;
	INSTALL_ERROR eError;
	CSSM_RETURN ret;

	CSSM_DL_HANDLE Handle;
	MDS_FUNCS pDlFunctions;
	char PrevDir[_MAX_PATH];

	pfInit = NULL;
	pfInstall = NULL;
	pfTerminate = NULL;
	pfUninstall = NULL;

	eError = write_mdsinfo_to_registry( CredPath, dbpath, MDSPath);
	if( INSTALL_SUCCESS != eError )
	{
		return( eError );
	}

	ret = port_LoadLibrary( MDSPath, &hMDSInst);
	if( ret != CSSM_OK )
		return( INSTALL_UNABLE_TO_LOAD_MDS );

	/* needed entry points from CSSM */
	ret = port_GetProcAddress( hMDSInst, "MDS_Initialize", &pfInit );
	if( ret != CSSM_OK )
	{
		port_FreeLibrary( hMDSInst );
		return( INSTALL_UNABLE_TO_GET_CSSM_INIT_ENTRY_PT );
	}

	ret = port_GetProcAddress( hMDSInst, "MDS_Install",&pfInstall );
	if( ret != CSSM_OK )
	{
		port_FreeLibrary( hMDSInst );
		return( INSTALL_UNABLE_TO_GET_CSSM_MODULE_INSTALL_ENTRY_PT );
	}

	ret = port_GetProcAddress( hMDSInst, "MDS_Terminate", &pfTerminate );
	if( ret != CSSM_OK )
	{
		port_FreeLibrary( hMDSInst );
		return( INSTALL_UNABLE_TO_GET_CSSM_INIT_ENTRY_PT );
	}

	ret = port_GetProcAddress( hMDSInst, "MDS_Uninstall", &pfUninstall );
	if( ret != CSSM_OK )
	{
		port_FreeLibrary( hMDSInst );
		return( INSTALL_UNABLE_TO_GET_CSSM_MODULE_INSTALL_ENTRY_PT );
	}

	/* initialize MDS */
	ret = ((_MDSAPI_Initialize_PTR)pfInit)(NULL,
									NULL,
									(CSSM_MEMORY_FUNCS *) &BioAPIMemoryFuncs,
									&pDlFunctions,
									&Handle);
	if( ret != CSSM_OK )
	{
		port_FreeLibrary( hMDSInst );
		return( INSTALL_CSSM_INIT_FAIL );
	}
    port_getcwd(PrevDir, (_MAX_PATH));

	if( dbpath != NULL )
	{
		SetCurrentDirectory( dbpath);
	}
	/* Install the module */
	if ( ((_MDSAPI_Install_PTR)pfInstall)( Handle ) != CSSM_OK )
	{
		port_FreeLibrary( hMDSInst );
		return( INSTALL_CSSM_MODULE_INSTALL_FAIL );
	}
	SetCurrentDirectory( PrevDir);
	if( ((_MDSAPI_Terminate_PTR)pfTerminate)( Handle ) != CSSM_OK )
	{
		port_FreeLibrary( hMDSInst );
		return( INSTALL_CSSM_MODULE_INSTALL_FAIL );
	}

	/* clean-up and return */
	port_FreeLibrary( hMDSInst );

	return( INSTALL_SUCCESS );
}

INSTALL_ERROR
uninstall_mds ( )
{
	int i;
	char TmpPath[_MAX_PATH];
	char TmpPath2[_MAX_PATH];
	char Default[_MAX_PATH];
	char regKey[_MAX_PATH];
	CSSM_RETURN Rtn;
	CSSM_RETURN ret;

	CSSM_LIB_HANDLE hMDSInst;
	CSSM_PROC_ADDR pfInit, pfInstall, pfTerminate, pfUninstall;

	CSSM_DL_HANDLE Handle;
	MDS_FUNCS pDlFunctions;


	memset( TmpPath, 0, _MAX_PATH );
	memset( TmpPath2, 0, _MAX_PATH );
	memset( Default, 0, _MAX_PATH );

    if (!mds_GetRegString(MDS_REGISTRY_PATH, "MDSLocation", TmpPath))
	{
		MessageBox( NULL, "MDS may not be uninstalled properly.", "MDS Location not found", MB_ICONEXCLAMATION|MB_OK );
		return( UNINSTALL_REGISTRY_ERROR );
	}

	strcpy( regKey, MDS_REGISTRY_PATH);
	i = strlen( regKey);
	regKey[i-1] = 0;
	Rtn = dlutil_port_DeleteSection( regKey, 0 );
	if(Rtn != CSSM_OK)
	{
		return( UNINSTALL_REGISTRY_ERROR );
	}

	ret = port_LoadLibrary( TmpPath, &hMDSInst);
	if( ret != CSSM_OK )
	{
		return( INSTALL_UNABLE_TO_LOAD_CSSM );
	}

	/* needed entry points from CSSM */
	ret = port_GetProcAddress( hMDSInst, "MDS_Initialize", &pfInit );
	if( ret != CSSM_OK )
	{
		port_FreeLibrary( hMDSInst );
		return( INSTALL_UNABLE_TO_GET_CSSM_INIT_ENTRY_PT );
	}

	ret = port_GetProcAddress( hMDSInst, "MDS_Install",&pfInstall );
	if( ret != CSSM_OK )
	{
		port_FreeLibrary( hMDSInst );
		return( INSTALL_UNABLE_TO_GET_CSSM_MODULE_INSTALL_ENTRY_PT );
	}

	ret = port_GetProcAddress( hMDSInst, "MDS_Terminate", &pfTerminate );
	if( ret != CSSM_OK )
	{
		port_FreeLibrary( hMDSInst );
		return( INSTALL_UNABLE_TO_GET_CSSM_INIT_ENTRY_PT );
	}

	ret = port_GetProcAddress( hMDSInst, "MDS_Uninstall", &pfUninstall );
	if( ret != CSSM_OK )
	{
		port_FreeLibrary( hMDSInst );
		return( INSTALL_UNABLE_TO_GET_CSSM_MODULE_INSTALL_ENTRY_PT );
	}

	/* initialize MDS */
	ret = ((_MDSAPI_Initialize_PTR)pfInit)(NULL,
									NULL,
									(CSSM_MEMORY_FUNCS *) &BioAPIMemoryFuncs,
									&pDlFunctions,
									&Handle);
	if( ret != CSSM_OK )
	{
		port_FreeLibrary( hMDSInst );
		return( INSTALL_CSSM_INIT_FAIL );
	}

	((_MDSAPI_Uninstall_PTR)pfUninstall)( Handle );

	((_MDSAPI_Terminate_PTR)pfTerminate)( Handle );

	/* clean-up and return */
	port_FreeLibrary( hMDSInst );
	_chmod( TmpPath, _S_IREAD | _S_IWRITE );
	DeleteFile( TmpPath );
	return INSTALL_SUCCESS;
}

CSSM_BOOL
mds_GetRegString (  char* pszKey,
                    char* pszValue,
                    char* pszData )
{
  sint32 dwDataSize = _MAX_PATH - 1;
  sint32 dwValueType = 0;

  if (dlutil_port_GetValue(pszKey,
                           pszValue,
                           pszData,
                           &dwDataSize,
                           &dwValueType,
                           DLUTIL_CONFIG_GLOBAL) != CSSM_OK)
  {
    return CSSM_FALSE;
  }
  else
  {
    if (dwDataSize >= _MAX_PATH)
        return CSSM_FALSE;

    if (pszData[dwDataSize] != '\0')
      pszData[dwDataSize] = '\0';

    return CSSM_TRUE;
  }
}
