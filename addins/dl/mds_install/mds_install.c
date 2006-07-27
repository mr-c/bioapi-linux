/*-----------------------------------------------------------------------
 * File: MDS_INSTALL.C
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

#include "port/bioapi_port.h"
#include "intel/installerr.h"
#include "ins_mds.h"

#if defined (WIN32)
#define MDS_BASENAME	"bioapi_mds300"
#else
#define MDS_BASENAME	"libbioapi_mds300"
#endif
#define MDS_FILENAME	MDS_BASENAME ".dll"

int main( int argc, char* argv[] )
{
	/* Linux-Port: General change; FALSE to CSSM_FALSE */
	CSSM_BOOL bDNO = CSSM_FALSE;
	CSSM_BOOL bResult = CSSM_FALSE;
	INSTALL_ERROR eError = INSTALL_SUCCESS;
	int index = 0, uninstall = 0, source = 0, dbdest = 0;
	char szSourcePath[_MAX_PATH],
		 szDbDestPath[_MAX_PATH],
		 szWinSys[_MAX_PATH],
		 szCurrDir[_MAX_PATH],
		 szMdsDbDir[_MAX_PATH],
		 szMdsEswSrc[_MAX_PATH],
		 szMdsDllDst[_MAX_PATH],
		 szMdsDllSrc[_MAX_PATH];

	memset( szSourcePath, 0, _MAX_PATH );
	memset( szDbDestPath, 0, _MAX_PATH );
	memset( szWinSys,	  0, _MAX_PATH );
	memset( szCurrDir,	  0, _MAX_PATH );
	memset( szMdsDbDir,	  0, _MAX_PATH );
	memset( szMdsEswSrc,  0, _MAX_PATH );
	memset( szMdsDllDst,  0, _MAX_PATH );
	memset( szMdsDllSrc,  0, _MAX_PATH );

	index = argc-1;

	/* parse command line */
	while( index )
	{
	  /* assign paths if necessary */
	  if( source == 1 )
	  {
		  strcpy( szSourcePath, argv[argc-index] );
		  source++;
	  }
	  if( dbdest == 1 )
	  {
		  strcpy( szDbDestPath, argv[argc-index] );
		  dbdest++;
	  }
	  if( !stricmp( argv[argc-index], "-u") && !uninstall )
	  {
		  uninstall = 1;
	  }
	  else if( !stricmp( argv[argc-index], "-s") && !source )
	  {
		  source = 1;
	  }
	  else if( !stricmp( argv[argc-index], "-d") && !dbdest )
	  {
		  dbdest = 1;
	  }

	  index--;
	}

	if( uninstall )
	{
		/* Begin uninstalling MDS */

		eError = uninstall_mds( );
		if( INSTALL_SUCCESS != eError )
		{
            printf("MDS uninstall failed (error %u).\n", eError);
			exit(1);
		}
		else
		{
			printf("MDS uninstalled successfully.\n");
			exit(0);
		}
	}
	else
	{
		/* Begin installing MDS */

#if defined (LINUX) || defined (AIX) || defined (SOLARIS)
        CreateSystemDirectory();
#endif
		GetSystemDirectory( szWinSys, sizeof( szWinSys ) );
		GetCurrentDirectory( _MAX_PATH, szCurrDir);

		/* MDS DLL Source Location */
		if( !source )
		{
			/* There is no -s source specified; search path. */

			SearchPath( NULL,		   // LPCSTR lpPath,
						MDS_BASENAME,  // LPCSTR lpFileName,
						DLL_EXTENSION, // LPCSTR lpExtension,
						_MAX_PATH,	   // DWORD nBufferLength,
						szSourcePath,  // LPSTR lpBuffer,
						NULL );		   // LPSTR *lpFilePart

			if( strlen( szSourcePath ) < 3 )
			{
				MessageBox( NULL, "Source file not found.","Install Error",
							MB_ICONEXCLAMATION|MB_OK|MB_SETFOREGROUND );
				exit(1);
			}
		}

		/* Set MDS Database Destination */
		if( !dbdest )
		{
			/* There is no -d destination specified; create default. */
			sprintf( szMdsDbDir, "%s%c%s", szWinSys, DIRECTORY_SEPARATOR, 
								"BioAPIFFDB" );
		}
		else
		{
			/* Heuristic for detection of type of path being passed in:
			 *
			 *	If there's a ":" **OR**
			 *	if \ and \ are the first two characters,
			 *	assume a network path was passed in; use as is.
			 *
			 *	Else if \ is the first character (but \ is not the second),
			 *	prepend the current drive letter and a ":" character.
			 *
			 *	Else those three cases failed, and it must be a relative path
			 *	so prepend the fully-qualified current directory path.
			 *
			 */
#if defined (WIN32)
			if ( (strcspn(szDbDestPath, ":") < strlen(szDbDestPath)) ||
				 ((szDbDestPath[0] == '\\') && (szDbDestPath[1] == '\\')) )
			{
				sprintf( szMdsDbDir, "%s", szDbDestPath );
			}
			else if (szDbDestPath[0] == '\\')
			{
				sprintf( szMdsDbDir, "%c:%s", szCurrDir[0], szDbDestPath);
			}
			else
#else
			if (szDbDestPath[0] == DIRECTORY_SEPARATOR)
			{
				sprintf( szMdsDbDir, "%s", szDbDestPath );
			}
			else
			/* If it is not an absolute path, prefix the current directory path */
#endif
			{
				sprintf( szMdsDbDir, "%s%c%s", szCurrDir, DIRECTORY_SEPARATOR, szDbDestPath);
			}
		}

		/* Copy MDS DLL and set file attributes */
#if defined (LINUX) || defined (AIX) || defined (SOLARIS)
		mkdir(szMdsDbDir, 0755);
		sprintf( szMdsDllDst, "%s%s%s", LIB_IPATH, MDS_BASENAME, DLL_EXTENSION );
#elif defined (__digital)
		sprintf( szMdsDllDst, "/usr/shlib/%s%s", MDS_BASENAME, DLL_EXTENSION );
#else
		CreateDirectory( szMdsDbDir, NULL );
		sprintf( szMdsDllDst, "%s%s%s%s",
			szWinSys, DIRECTORY_SEPARATOR_STRING, MDS_BASENAME, DLL_EXTENSION );
		/* Just in case a read-only version exists at the destination . . . */
		SetFileAttributes( szMdsDllDst, FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_ARCHIVE );
#endif
		sprintf( szMdsDllSrc, "%s%s%s%s",
			szSourcePath, DIRECTORY_SEPARATOR_STRING, MDS_BASENAME, DLL_EXTENSION);

		/* Now copy the file . . . */
		bResult = CopyFile( szMdsDllSrc, szMdsDllDst, bDNO );
		if( bResult == CSSM_FALSE )
		{
		   MessageBox( NULL, "Copy failed for MDS", "Install Error",
					   MB_ICONEXCLAMATION|MB_OK|MB_SETFOREGROUND );
		   exit(1);
		}
#if !defined (LINUX) && !defined (AIX) && !defined (SOLARIS)
		else
		{
			SetFileAttributes( szMdsDllDst, FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_ARCHIVE );
		}
#endif

		eError = install_mds( szMdsDllDst, szMdsDbDir, szMdsEswSrc );

		if( INSTALL_SUCCESS != eError )
		{
          printf("MDS install failed (error %u).\n", eError);
		  exit(1);
		}
		else
		{
		  printf("MDS installed successfully.\n");
		  exit(0);
		}
	}
}
