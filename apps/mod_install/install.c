/*-----------------------------------------------------------------------
 * File: INSTALL.C
 *
 * Copyright (c) 1995-2000 Intel Corporation. All rights reserved.
 *-----------------------------------------------------------------------
 */

#include <stdio.h>
#include <stdlib.h>

/* Linux-Port: added the include files */
#if defined (LINUX) || defined (SOLARIS)
#include <assert.h>
#include <sys/stat.h>
#endif

#include "bioapi.h"
#include "port/bioapi_port.h"
#include "../../framework/h_layer/install/precomp_install.h"

#include "cmdline.h"

void Usage( char *Exe )
{
	char ExePath[_MAX_FNAME];
	_splitpath( Exe, NULL, NULL, ExePath, NULL );
#if defined (LINUX) || defined (AIX) || defined (SOLARIS)
  printf( "Usage: %s [-fiur] file\n", ExePath );
  printf( "  -f             Do not warn about unsigned or corrupt modules.\n"
	  "  -i ModulePath  Install the module.\n"
	  "  -u ModuleName  Uninstall the module.\n"
	  "  -r ModuleName  Refresh the installation information.\n"
	  "  ** Options -i, -u, and -r are mutually exclusive. **\n"
	  "  ** and almost one of them is required. **\n"
#else
	printf( "Usage: %s [-fiur] -s file [-d path]\n", ExePath );
  	printf( "  -f           Unused option reserved for future use.\n"
			"  -i       Install the module.\n"
			"  -u       Uninstall the module.\n"
			"  -r       Refresh the installation information.\n"
			"           ** Options -i, -u, and -r are mutually exclusive. **\n"
			"  -s file  Source file to install; no extension if installing, file name\n"
			"           only when uninstalling or refreshing\n"
			"  -d path  Destination path; required for install.\n"
#endif
		  );
	exit( 1 );
}

int PositiveResponse( char *Prompt, int Default )
{
	char Response;

	printf( "%s ", Prompt );
	Response = (char) getchar();
	if ( Response == '\n' )
	{
		return Default;
	}
	else
	{
		if ( 'Y' == toupper( Response ) )
		{
			return CSSM_TRUE;
		}
		else
		{
			return CSSM_FALSE;
		}
	}
}

int ReadManifest( CSSM_DATA *pCredential, char *CredentialSource, int bForce )
{
	int rv = CSSM_TRUE;
#if 0
	/* SECTION UNDER IS SO ORIGINAL CODE ISN'T LOST.  THIS ONLY PERTAINS TO
           MANIFESTS, WHICH THE BIOAPI DOES NOT USE AT THIS TIME.
        */
	uint8 *pManifestBuffer = NULL;
	struct _stat FileInfo;
	FILE *fManifest = NULL;
#endif

	assert( pCredential );
	pCredential->Data = NULL;
	pCredential->Length = 0;

#if 0
	/* SECTION UNDER IS SO ORIGINAL CODE ISN'T LOST.  THIS ONLY PERTAINS TO
           MANIFESTS, WHICH THE BIOAPI DOES NOT USE AT THIS TIME.
        */
	if ( !_stat( CredentialSource, &FileInfo ) )
	{
		/* Allocate memory for the manifest buffer */
		if ( FileInfo.st_size != 0 )
		{
			pManifestBuffer = malloc( FileInfo.st_size );
			if ( !pManifestBuffer )
			{
				printf( "Memory exhausted! Stopping installation.\n" );
				rv = CSSM_FALSE;
			}
			else
			{
				pCredential->Data = pManifestBuffer;
				pCredential->Length = FileInfo.st_size;

				/* Read the manifest contents */
				fManifest = fopen( CredentialSource, "rb" );
				if ( fManifest == NULL )
				{
					printf( "Unreadable credential. Stopping installion.\n"
							"Check file permissions and try again.\n" );
					rv = CSSM_FALSE;
				}
				else
				{
					if ( fread( pManifestBuffer, 1, FileInfo.st_size, fManifest )
							!= (size_t)FileInfo.st_size )
					{
						printf( "Credential read failed. Stopping installation.\n"
								"Check file contents and try again.\n" );
						rv = CSSM_FALSE;
					}

					fclose( fManifest );
				} /* fopen */

				if ( !rv )
				{
					free( pManifestBuffer );
					pCredential->Data = NULL;
					pCredential->Length = 0;
				}
			} /* malloc */
		}
		else
		{
			printf( "Empty credential! Stopping installation.\n" );
			rv = CSSM_FALSE;
		}
	}
	else
	{
		if ( bForce )
		{
			printf( "No credential found. Continuing installation.\n" );
		}
		else
		{
			if ( PositiveResponse( "No credential found. Install anyway? [y/N]", CSSM_FALSE ) )
			{
				printf( "Continuing installation without credential.\n" );
			}
			else
			{
				printf( "Stopping installation.\n" );
				rv = CSSM_FALSE;
			}
		}
	}
#endif /*0*/

	return rv;
}

int main(int argc,char *argv[])
{
	DWORD error=0;
	/* Configuration parameters */
	INSTALL_ACTION Action = -1;
	int bForce = CSSM_FALSE;
	char Option;
	char *OptionParam = NULL;
	unsigned ParseIndex = 0;
	char *Src = NULL;
	char *Dst = NULL;

	/* Module management and registration parameters */
/* Linux-Port: added the UNIX part */
#if defined (WIN32)
  HINSTANCE hModule = NULL;
#else /* UNIX */
  CSSM_LIB_HANDLE hModule = NULL;
#endif
	INSTALL_ENTRY_PTR InstallProc = NULL;
	BioAPI_INSTALL_ERROR RegistrationResult;

	/* Parsed source file values */
	char SourceFileDrive[_MAX_DRIVE];
	char SourceFileDir[_MAX_DIR];
	char SourceFileName[_MAX_FNAME];
	char SourceFileExt[_MAX_EXT];

	/* Parsed destination directory parameters */
	char DestPathDrive[_MAX_DRIVE];
	char DestPathDir[_MAX_DIR];
	char DestPathName[_MAX_FNAME];
	char DestPathExt[_MAX_EXT];
/* Linix-Port: added the Linux branch */
#if defined (LINUX) || defined (SOLARIS)
  char *ModuleName = NULL;
  char szSrcPath[_MAX_PATH]="./";
  char szDstPath[_MAX_PATH]=LIB_IPATH;
#endif
	/* Copy and credential locations */
	char CopySource[_MAX_PATH];
	char CopyDest[_MAX_PATH];
	char CredentialSource[_MAX_PATH];

	/* Installation values */
	char TargetPath[_MAX_PATH];
	char TargetFileName[_MAX_FNAME + _MAX_EXT];

    char buffer[100];
    char *filepart;

	/* Credential Buffer */
	BioAPI_DATA Credential = { 0, NULL };

	/* Parse the command line */
	CommandInitParse( argv, argc );

	/* Fetch the parameters */
	ParseIndex = 0;
	while ( CommandGetNextOption( &Option, &OptionParam, ParseIndex++ ) )
	{
		switch ( Option )
		{
		case 'f':
			{
				/* -f has no params */
				if ( OptionParam )
				{
					printf( "Option -%c has no parameter\n", Option );
					Usage( argv[0] );
				}
				bForce = CSSM_TRUE;
				break;
			}
		case 'i':
			{
/* Linux-Port: added the Linux branch */
#if defined (LINUX) || defined (SOLARIS)
	    if ( ModuleName || !OptionParam )
	      {
		printf( "Option -%c requires a parameter or multiple files specified\n", Option );
		Usage( argv[0] );
	      }
	    ModuleName = OptionParam;
#else
				if ( OptionParam || ( Action != -1 ) )
				{
					printf( "Option -%c has no parameter or multiple actions specified\n", Option );
					Usage( argv[0] );
				}
#endif
				Action = INSTALL_ACTION_INSTALL;
				break;
			}
		case 'u':
			{
/* Linux-Port: Added the Linux branch */
#if defined (LINUX) || defined (SOLARIS)
	    if ( ModuleName || !OptionParam )
	      {
		printf( "Option -%c requires a parameter or multiple files specified\n", Option );
		Usage( argv[0] );
	      }
	    ModuleName = OptionParam;
#else
				if ( OptionParam || ( Action != -1 ) )
				{
					printf( "Option -%c has no parameter or multiple actions specified\n", Option );
					Usage( argv[0] );
				}
#endif
				Action = INSTALL_ACTION_UNINSTALL;
				break;
			}
		case 'r':
			{
/* Linux-Port: Added the Linux branch */
#if defined (LINUX) || defined (SOLARIS)
	    if ( ModuleName || !OptionParam )
	      {
		printf( "Option -%c requires a parameter or multiple files specified\n", Option );
		Usage( argv[0] );
	      }
	    ModuleName = OptionParam;
#else
				if ( OptionParam || ( Action != -1 ) )
				{
					printf( "Option -%c has no parameter or multiple actions specified\n", Option );
					Usage( argv[0] );
				}
#endif
				Action = INSTALL_ACTION_REFRESH;
				break;
			}
/* Linux-Port: Added the Linux branch */
#if !defined (LINUX) && !defined (SOLARIS)
		case 's':
			{
				if ( Src || !OptionParam )
				{
					printf( "Option -%c requires a parameter or multiple files specified\n", Option );
					Usage( argv[0] );
				}
				Src = OptionParam;
				break;
			}
		case 'd':
			{
				if ( Dst || !OptionParam )
				{
					printf( "Option -%c requires a parameter or multiple paths specified\n", Option );
					Usage( argv[0] );
				}
				Dst = OptionParam;
				break;
			}
#endif
		case 0:
			{
				/* The first option returned is the "NULL" option with the exe name as the value */
				break;
			}
		default:
			{
				printf( "Unknown option: %c\n", Option );
				Usage( argv[0] );
			}
		} /* switch */
	} /* while */

	/* Make sure an install action and appropriate paths were specified */
/* Linux-Port: Added the Linux branch */
#if defined (LINUX) || defined (SOLARIS)
  if ( !ModuleName )
    {
      printf( "ModuleName not specified\n" );
      Usage( argv[0] );
    }
  if ( Action == -1 )
    {
      printf( "Action(install/uninstall/refresh) not specified\n" );
      Usage( argv[0] );
    }
#else
	if ( !Src )
	{
		printf( "Option -s not specified\n" );
		Usage( argv[0] );
	}
	else if ( !Dst && ( Action == INSTALL_ACTION_INSTALL ) )
	{
		printf( "Option -d not specified\n" );
		Usage( argv[0] );
	}
	if ( Action == -1 )
	{
		/* Install is the default action */
		Action = INSTALL_ACTION_INSTALL;
	}
#endif

/* Linux-Port: Added the Linux branch */
#if defined (LINUX) || defined (SOLARIS)
  if ( ( Action == INSTALL_ACTION_INSTALL ) || ( Action == INSTALL_ACTION_REFRESH ) )
  {
    strcat(szSrcPath, ModuleName);
    Src = szSrcPath;
  }
  if ( Action == INSTALL_ACTION_UNINSTALL )
  {
    strcat(szDstPath, ModuleName);
    Src = szDstPath;
  }
#endif
	/* Parse the source file */
	_splitpath( Src, SourceFileDrive, SourceFileDir, SourceFileName, SourceFileExt );
	if ( strcmp( SourceFileExt, "" ) )
	{
/* Linux-Port: Added the Linux branch */
#if !defined (LINUX) && !defined (SOLARIS)
		printf( "File extension specified. It will be ignored.\n" );
#endif
	}
/* Linux-Port: Added the Linux branch */
#if defined (LINUX) || defined (SOLARIS)
  if ( Action != INSTALL_ACTION_UNINSTALL )
  {
    strcat(szDstPath, SourceFileName);
    strcat(szDstPath, SourceFileExt);
  }
  Dst = szDstPath;
#endif
	if ( Dst )
	{
		_splitpath( Dst, DestPathDrive, DestPathDir, DestPathName, DestPathExt );
/* Linux-Port: Added the Linux branch */
#if !defined (LINUX) && !defined (SOLARIS)
		if ( strcmp( DestPathName, "" ) ||
			 strcmp( DestPathExt, "" ) )
		{
			printf( "File name specified for destination. It will be ignored.\n" );
		}
#endif
	}

	/* Generate the paths */
  _makepath( CopySource, SourceFileDrive, SourceFileDir, SourceFileName, DLL_STR );
	_makepath( CredentialSource, SourceFileDrive, SourceFileDir, SourceFileName, "esw" );
	if ( ( Action == INSTALL_ACTION_INSTALL ) || ( Action == INSTALL_ACTION_REFRESH ) )
	{
      _makepath( CopyDest, DestPathDrive, DestPathDir, SourceFileName, DLL_STR );
		_makepath( TargetPath, DestPathDrive, DestPathDir, NULL, NULL );
      _makepath( TargetFileName, NULL, NULL, SourceFileName, DLL_STR  );
	}
	else
	{
      _makepath( CopyDest, SourceFileDrive, SourceFileDir, SourceFileName, DLL_STR );
		_makepath( TargetPath, SourceFileDrive, SourceFileDir, NULL, NULL );
	}
	
	/* Read the credential contents if present and performing an installation */
	if ( ( Action != INSTALL_ACTION_INSTALL ) ||
		 ReadManifest( (CSSM_DATA *)&Credential, CredentialSource, bForce ) )
	{
		/* Verify the manifest here? */

		/* Copy the source file if performing an installation */
		if ( ( Action == INSTALL_ACTION_INSTALL ) &&
			 !CopyFile( CopySource, CopyDest, CSSM_FALSE ) )
		{
			printf( "Could not copy \"%s\" to \"%s\"!\n", CopySource, CopyDest );
		}
		else
		{
            if (SearchPath(TargetPath, SourceFileName, DLL_EXTENSION, sizeof(buffer), buffer, &filepart))
            {
/* Load the addin */
/* Linux-Port: Added the Linux branch */
#if defined (LINUX) || defined (SOLARIS)
				if (port_LoadLibrary( CopyDest, &hModule )!= CSSM_OK )
#else /* WIN32 */
				hModule = LoadLibrary( CopyDest );
				if ( !hModule )
#endif
                {
                    printf( "Could not load addin module \"%s\"!\n", CopyDest );
                }
                else
                {
/* Find the install information functions */
/* Linux-Port: Added the Linux branch */
#if defined (LINUX) || defined (SOLARIS)
					if(port_GetProcAddress( hModule, INSTALL_ENTRY_NAME_STRING,
											(CSSM_PROC_ADDR *)&InstallProc) != CSSM_OK) {
			      printf( "Could not find %s function in %s!\n", INSTALL_ENTRY_NAME_STRING, CopyDest );
					}
#else /* WIN32 */
				  InstallProc = (INSTALL_ENTRY_PTR)GetProcAddress( hModule, INSTALL_ENTRY_NAME_STRING );
					if ( InstallProc == NULL ) { 
					  printf( "Could not find " INSTALL_ENTRY_NAME_STRING " function in %s!\n", CopyDest );
				  }
#endif
					else
					{
						/* Invoke the registration */
/* Linux-Port: Added the Linux branch */
#if defined (LINUX) || defined (SOLARIS)



						if(TargetPath[strlen(TargetPath)-1] == '/')
							TargetPath[strlen(TargetPath)-1] = '\0';
#endif
						if (InstallProc( TargetFileName,
										TargetPath,
										&Credential,
										Action,
										&RegistrationResult ) != CSSM_OK )
						{
							printf( "Module: %s (Code #%X)!\n",
							    RegistrationResult.ErrorString,
								RegistrationResult.ErrorCode );
						}
						else
						{
							if ( Action == INSTALL_ACTION_INSTALL )
							{
								printf( "Module installed successfully.\n" );
							}
							else if ( Action == INSTALL_ACTION_UNINSTALL )
							{
/* Linux-Port: Added the Linux branch */
#if defined (LINUX) || defined (SOLARIS)
								DeleteFile(szDstPath);
#endif
								printf( "Module uninstalled successfully.\n" );
							}
							else if ( Action == INSTALL_ACTION_REFRESH )
							{
								printf( "Module installation information refreshed.\n" );
							}
							else  /* should never arrive here */
							{
								printf( "UNKNOWN MOD_INSTALL ERROR.\n" );
							}
						}
                    } /* GetProcAddress */
					
                    /* Unload the library */
/* Linux-Port: Added the Linux branch */
#if defined (LINUX) || defined (SOLARIS)
					port_FreeLibrary( hModule );
#else /* WIN32 */
					FreeLibrary(hModule);
#endif
                } /* LoadLibrary */
            }
			else
			{
/* Linux-Port: Added the Linux branch */
#if defined (LINUX) || defined (SOLARIS)
#else /* WIN32 */
        error=GetLastError(0;
#endif
				printf("Could not find file: %s", CopyDest);
			}
		} /* Copy */

		/* Free the manifest buffer */
		if ( Credential.Data )
		{
			free( Credential.Data );
			Credential.Data = NULL;
		}
	} /* ReadManifest */

	return 0;
}
