/*-----------------------------------------------------------------------
 * File: DL_PORTREG.C
 *
 * Copyright (c) 1995-2000 Intel Corporation. All rights reserved.
 *-----------------------------------------------------------------------
 */

#if defined (UNIX)
#include <pthread.h>			/* Must be the first include file */
#include <dlfcn.h>
#include <errno.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <dirent.h>
#endif
#if defined (__digital__)
#include "compaq/dprintf.h"
#include <loader.h>
#endif
#if defined (UNIX)
#ifndef DPRINTF
#define DPRINTF
#endif
#endif
#include "dlutil_reg.h"
#include "port/bioapi_port.h"

#if defined (WIN32)
static CSSM_RETURN RegWipeKey(HKEY hkey, const char *szSubKey);
#endif

/* Registry routines */

#if defined (UNIX)
/* XXX Helper routines for prototyping the registry routines. */
static int get_registry_path(char *path, size_t pathlen,
				 const char *section, sint32 PersonalPref)

{
	const char *config;
	char *p;

	switch (PersonalPref) {
	case DLUTIL_CONFIG_GLOBAL:
	config = "HKEY_LOCAL_MACHINE";
	break;
	case DLUTIL_CONFIG_PERSONAL:
	config = "HKEY_CURRENT_USER";
	break;
	default:
	return -1;
	}

    GetSystemDirectory(path, pathlen);

    if (strlen(path)+strlen(config)+strlen(section)+12 > pathlen)
        return -1;

    strcat(path, "/registry/");
    strcat(path, config); 
    strcat(path, "/");
    strcat(path, section); 

	for (p = path; *p; p++)
	if (*p == '\\')
		*p = '/';

	return 0;
}

static int get_registry_value(const char *dirpath,
				  const char *name,
				  void	 *value,
				  sint32 *value_len,
				  sint32 *value_type)
{
	char path[PATH_MAX + 1];
	FILE *fp = NULL;
	unsigned char *v = (unsigned char *) value;
	int i, n, c;
	char buf[3];

	snprintf(path, sizeof path, "%s/%s", dirpath, name);

	if ((fp = fopen(path, "r")) == NULL)
	goto error;

	if ((n = fscanf(fp, "%2s", buf)) != 1)
	goto error;

	if (strncmp(buf, "0x", 2) == 0)
	{
	*value_type = DLUTIL_VALUE_BINARY;

	i = 0;
	while ((n = fscanf(fp, "%2x", &c)) == 1)
	{
		if (i >= *value_len)
		goto error;
		v[i++] = c;
	}

	if (n != EOF)
		goto error;
	}
	else
	{
	*value_type = DLUTIL_VALUE_STRING;

	i = strlen(buf);
	if (i >= *value_len)
		goto error;
	memcpy(v, buf, i);

	while ((c = getc(fp)) != EOF)
	{
		if (i >= *value_len)
		goto error;
		v[i++] = c;
	}

	if (v[i-1] == '\n')
		--i;

	if (i >= *value_len)
		goto error;
	v[i++] = '\0';
	}

	*value_len = i;

	fclose(fp);

	return 0;

 error:

	if (fp)
	fclose(fp);

	bzero(value, *value_len);
	*value_len = 0;

	return -1;
}

static int set_registry_value(const char *dirpath,
				  const char *name,
				  const void *value,
				  sint32 value_len,
				  sint32 value_type)
{
	char path[PATH_MAX + 1];
	FILE *fp = NULL;
	unsigned char *p = (unsigned char *) value;
	int i;

	snprintf(path, sizeof path, "%s/%s", dirpath, name);

	if ((fp = fopen(path, "w")) == NULL)
	goto error;

	switch (value_type) {
	case DLUTIL_VALUE_BINARY:
	fputs("0x", fp);
	for (i = 0; i < value_len; i++)
		fprintf(fp, "%02x", p[i]);
	break;
	case DLUTIL_VALUE_STRING:
	fputs((char *) p, fp);
	break;
	default:
	goto error;
	}

	fputc('\n', fp);
	fclose(fp);

	return 0;

 error:

	if (fp)
	fclose(fp);

	return -1;
}
#endif

#if defined (WIN32)

static CSSM_RETURN s_dlutil_port_RegCreateKeyEx(HKEY hKey,					/* handle to an open key */
												LPCTSTR lpSubKey,			/* address of subkey name */
												PHKEY phkResult,			/* address of buffer for opened handle */
												LPDWORD lpdwDisposition)	/* address of disposition value buffer */
{
	CSSM_RETURN crStatus = CSSM_OK;
	LONG lStatus = 0;

	OSVERSIONINFO osVersionInfo;
	memset( &osVersionInfo, 0, sizeof(osVersionInfo) );

	osVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	if ( !GetVersionEx(&osVersionInfo) )
	{
		return (CSSMERR_CSSM_OS_ACCESS_DENIED);
	}

	switch ( osVersionInfo.dwPlatformId )
	{
		case VER_PLATFORM_WIN32_NT:
		{
			ACL *pAcl = NULL;
			PSID pSidWorldSid = NULL;

			do
			{
				BOOL bStatus = FALSE;

				DWORD cbAcl = 0;
				SECURITY_DESCRIPTOR sd;
				SECURITY_ATTRIBUTES sa;

				/*First create a SID (secure ID) that represent everyone, aka we are the world*/
				SID_IDENTIFIER_AUTHORITY siaWorldSidAuthority = SECURITY_WORLD_SID_AUTHORITY;

				pSidWorldSid = (PSID)malloc( GetSidLengthRequired(1) );
				InitializeSid( pSidWorldSid, &siaWorldSidAuthority, 1 );
				*(GetSidSubAuthority( pSidWorldSid, 0)) = SECURITY_WORLD_RID;

				/*Now create the ACL by appending an ACE after a ACL header*/
				cbAcl = sizeof(ACL) + sizeof(ACCESS_ALLOWED_ACE)
									- sizeof(DWORD)		/*the size of DWORD SidStart*/
									+ GetLengthSid(pSidWorldSid);

				pAcl = (ACL*)malloc( cbAcl );
				if ( !pAcl )
				{
					/*Out of memory*/
					crStatus = CSSMERR_CSSM_MEMORY_ERROR;
					break;
				}

				bStatus = InitializeAcl(pAcl, cbAcl, ACL_REVISION);
				if ( !bStatus )
				{
					/*Can't initializeAcl!*/
					crStatus = CSSMERR_CSSM_OS_ACCESS_DENIED;
					break;
				}

				bStatus = AddAccessAllowedAce(pAcl,
											  ACL_REVISION,
											  STANDARD_RIGHTS_ALL | SPECIFIC_RIGHTS_ALL,
											  pSidWorldSid);
				if ( !bStatus )
				{
					/*Can't give access to everyone :(*/
					crStatus = CSSMERR_CSSM_OS_ACCESS_DENIED;
					break;
				}

				/*Let's make a security descriptor for our registry key*/
				memset( &sd, 0, sizeof(sd) );

				bStatus = InitializeSecurityDescriptor(&sd,
													   SECURITY_DESCRIPTOR_REVISION);
				if ( !bStatus )
				{
					/*Error in initializing security descriptor.*/
					crStatus = CSSMERR_CSSM_OS_ACCESS_DENIED;
					break;
				}

				/*It's time to bind the DACL to the security descriptor*/
				bStatus = SetSecurityDescriptorDacl(&sd,
													TRUE,	/*flag for presence of discretionary ACL*/
													pAcl,
													FALSE); /*flag for default discretionary ACL*/
				if ( !bStatus )
				{
					/*Error in setting DACL in security descriptor.*/
					crStatus = CSSMERR_CSSM_OS_ACCESS_DENIED;
					break;
				}

				/*Now, for the Big Enchilada.*/
				memset( &sa, 0, sizeof(sa) );

				sa.nLength = sizeof(SECURITY_ATTRIBUTES);
				sa.bInheritHandle = FALSE;
				sa.lpSecurityDescriptor = &sd;

				/*Finally, we are ready to create the registry key!*/
				lStatus = RegCreateKeyEx(hKey,
										 lpSubKey,
										 0,		/*reserved*/
										 0,
										 REG_OPTION_NON_VOLATILE,
										 KEY_ALL_ACCESS,
										 &sa,	/*security attribute*/
										 phkResult,
										 lpdwDisposition);

				if ( lStatus != ERROR_SUCCESS )
				{
					/*Can't create reg key.*/
					crStatus = CSSMERR_CSSM_OS_ACCESS_DENIED;
					break;
				}

#pragma warning(disable: 4127)
			} while( FALSE );
#pragma warning(default: 4127)

			/*Let's clean up our mess!*/
			if ( pSidWorldSid )
			{
				free( pSidWorldSid );
				pSidWorldSid = NULL;
			}
			if ( pAcl )
			{
				free( pAcl );
				pAcl = NULL;
			}
		} /*WinNT*/
		break;

		case VER_PLATFORM_WIN32_WINDOWS: /*Win95 or Win98*/
		{
			lStatus = RegCreateKeyEx(hKey,
									 lpSubKey,
									 0,		/*reserved*/
									 0,
									 REG_OPTION_NON_VOLATILE,
									 KEY_ALL_ACCESS,
									 NULL,
									 phkResult,
									 lpdwDisposition);

			if ( lStatus != ERROR_SUCCESS )
			{
				/*Can't create reg key.*/
				crStatus = CSSMERR_CSSM_OS_ACCESS_DENIED;
			}
		}
		break;

		default:
		{
			/*Not WinNT, Win95 nor Win98*/
			crStatus = CSSMERR_CSSM_INTERNAL_ERROR;
		}
		break;
	} /*switch*/

	return (crStatus);
}

#endif /* WIN32 */

/*-----------------------------------------------------------------------------
 * Name: dlutil_port_SetValue
 *
 * Description:	 Sets a value for Name under Section in the registry
 *
 * Parameters:
 * Section (input): Name of the section in the configuration file that
 *			will contain the entry.	 May include a path to mimic
 *			registry paths such as "CSSM\AddIn\blah". This will
 *			map directly to the section name in Windows 3.1 and
 *			to the registry path under Win32, but can be easily
 *			adapted for other platforms.
 * Name (input):	Name of the value to set in the specified section.
 * Value (input):	Void pointer to the data to write to the configuration
 *					file/database.
 * ValueLength (input): Length of the data to write to the registry.
 * ValueType (input):	Indicator to the type of data that is being written
 *			to the configuration file. Valid types are
 *			DLUTIL_CONFIG_STRING and DLUTIL_CONFIG_BINARY. Binary
 *			values should converted to an ASCII representation of
 *			the data before being written into a text registry.
 *			They are stored as-is in the Win32 registry
 * PersonalPref (input):Boolean indicator as to whether the value should be
 *			written to the system configuration data or to that
 *			of the user. Valid values: DLUTIL_CONFIG_GLOBAL and
 *			DLUTIL_CONFIG_PERSONAL
 *
 * Returns:
 * CSSM_OK if the operation was successful
 *
 * Error Codes:
 * CSSM_CANT_CREATE_KEY - Unable to create a key for Section
 * CSSM_CANT_SET_VALUE - Unable to set the value for Name under Section
 *---------------------------------------------------------------------------*/
CSSM_RETURN dlutil_port_SetValue(const char *Section,
								 const char *Name,
								 const void *Value,
								 sint32 ValueLength,
								 sint32 ValueType,
								 sint32 PersonalPref)
{

#if defined (WIN32)

	HKEY hkey;
	uint32 ret_val;
	if (PersonalPref)
	{
		if (s_dlutil_port_RegCreateKeyEx(HKEY_CURRENT_USER,
										 Section,
										 &hkey,
										 (unsigned long *)&ret_val) != CSSM_OK)
		{
			return (CSSMERR_CSSM_OS_ACCESS_DENIED);
		}
	}
	else
	{
		if (s_dlutil_port_RegCreateKeyEx(HKEY_LOCAL_MACHINE,
										 Section,
										 &hkey,
										 (unsigned long *)&ret_val) != CSSM_OK)
		{
			return (CSSMERR_CSSM_OS_ACCESS_DENIED);
		}
	}

	ValueType = (ValueType == DLUTIL_VALUE_BINARY) ? REG_BINARY : REG_SZ;
	if (RegSetValueEx(hkey,
					  Name,
					  0,
					  (unsigned long)ValueType,
					  (unsigned char *)Value,
					  (unsigned long)ValueLength) != ERROR_SUCCESS)
	{
		return (CSSMERR_CSSM_OS_ACCESS_DENIED);
	}

	/* close the key when done */
	RegCloseKey (hkey);
	return (CSSM_OK);

#elif defined (UNIX)
	char dirpath[PATH_MAX + 1];
#if defined (LINUX) || defined (AIX) || defined (SOLARIS)
    const char mkdir_fmt[] = "/bin/mkdir -p '%s'";
#else
	const char mkdir_fmt[] = "/usr/bin/mkdir -p '%s'";
#endif
	char mkdir_buf[sizeof mkdir_fmt + sizeof dirpath + 1];
	int status;
	struct stat statbuf;

	DPRINTF("registry", "(dlutil_port_SetValue) Section = %s\n", Section);
	DPRINTF("registry", "(dlutil_port_SetValue) Name = %s\n", Name);

	if (get_registry_path(dirpath, sizeof dirpath, Section,PersonalPref))
	{
		return CSSMERR_CSSM_OS_ACCESS_DENIED;
	}

	if (stat(dirpath, &statbuf))
	{
	if (errno != ENOENT)
		return CSSMERR_CSSM_OS_ACCESS_DENIED;

	sprintf(mkdir_buf, mkdir_fmt, dirpath);
	status = system(mkdir_buf);
	if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
		return CSSMERR_CSSM_OS_ACCESS_DENIED;
	}

	if (set_registry_value(dirpath, Name, Value, ValueLength, ValueType))
	return CSSMERR_CSSM_OS_ACCESS_DENIED;

	return CSSM_OK;

#endif
}

/*-----------------------------------------------------------------------------
 * Name: dlutil_port_GetValue
 *
 * Description:	 Gets a value for Name under Section in the registry
 *
 * Parameters:
 * Section (input)			:	Name of the section in the configuration file that
 *								contains the entry.	 May include a path to mimic
 *								registry paths such as "CSSM\AddIn\blah". This will
 *								map directly to the section name in Windows 3.1 and
 *								to the registry path under Win32, but can be easily
 *								adapted for other platforms.
 * Name (input)				:	Name of the value to get in the specified section.
 * Value (output)			:	Void pointer to the data to read from the configuration
 *								file/database.
 * ValueLength (output)		:	Length of the data to read from the registry.
 * ValueType (output)		:	Indicator to the type of data that is being read
 *								from the configuration file. Valid types are
 *								DLUTIL_CONFIG_STRING and DLUTIL_CONFIG_BINARY. Binary
 *								values should converted to an ASCII representation of
 *								the data before being written into a text registry.
 *								They are stored as-is in the Win32 registry
 * PersonalPref (input)		:	Boolean indicator as to whether the value should be
 *								read from the system configuration data or to that
 *								of the user. Valid values are DLUTIL_CONFIG_GLOBAL and
 *								DLUTIL_CONFIG_PERSONAL
 *
 * Returns:
 * CSSM_OK if the operation was successful
 *
 * Error Codes:
 * CSSM_CANT_OPEN_KEY - Unable to open a key for Section
 * CSSM_CANT_SET_VALUE - Unable to get the value for Name under Section
 *---------------------------------------------------------------------------*/
CSSM_RETURN dlutil_port_GetValue(const char *Section,
								 const char *Name,
								 void *Value,
								 sint32 *ValueLength,
								 sint32 *ValueType,
								 sint32 PersonalPref)
{
#if defined (WIN32)

	HKEY hkey;

	if (PersonalPref)
	{
		if (RegOpenKeyEx(HKEY_CURRENT_USER,
						 Section,
						 0,
						 KEY_ALL_ACCESS,
						 &hkey) != ERROR_SUCCESS)
		{
			return (CSSMERR_CSSM_OS_ACCESS_DENIED);
		}
	}
	else
	{
		if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,
						 Section,
						 0,
						 KEY_ALL_ACCESS,
						 &hkey) != ERROR_SUCCESS)
		{
			return (CSSMERR_CSSM_OS_ACCESS_DENIED);
		}

	}

	if (RegQueryValueEx(hkey,
						Name,
						NULL,
						(unsigned long *)ValueType,
						(unsigned char *)Value,
						(unsigned long *)ValueLength) != ERROR_SUCCESS)
	{
		RegCloseKey(hkey);
		return (CSSMERR_CSSM_OS_ACCESS_DENIED);
	}

	*ValueType = (*ValueType == REG_BINARY) ? DLUTIL_VALUE_BINARY : DLUTIL_VALUE_STRING;
	RegCloseKey(hkey);

	return (CSSM_OK);

#elif defined (UNIX)

	char dirpath[PATH_MAX + 1];
	DIR *dir;

	DPRINTF("registry", "(dlutil_port_GetValue) Section = %s\n", Section);
	DPRINTF("registry", "(dlutil_port_GetValue) Name = %s\n", Name);

	if (get_registry_path(dirpath, sizeof dirpath, Section, PersonalPref))
		return CSSMERR_CSSM_OS_ACCESS_DENIED;

	if ((dir = opendir(dirpath)) == NULL)
		return CSSMERR_CSSM_OS_ACCESS_DENIED;

	closedir(dir);

	if (get_registry_value(dirpath, Name, Value, ValueLength, ValueType))
		return CSSMERR_CSSM_OS_ACCESS_DENIED;

	return CSSM_OK;

#endif
}

/*-----------------------------------------------------------------------------
 * Name: dlutil_port_DeleteSection
 *
 * Description:	 Deletes a	Section in the registry
 *
 * Parameters:
 * Section (input)			:	Name of the section in the configuration file to
 *								delete.	 May include a path to mimic
 *								registry paths such as "CSSM\AddIn\blah". This will
 *								map directly to the section name in Windows 3.1 and
 *								to the registry path under Win32, but can be easily
 *								adapted for other platforms.
 * PersonalPref (input)		:	Boolean indicator as to whether the value should be
 *								written to the system configuration data or to that
 *								of the user. Valid values are DLUTIL_CONFIG_GLOBAL and
 *								DLUTIL_CONFIG_PERSONAL
 *
 * Returns:
 * CSSM_OK if the operation was successful
 *
 * Error Codes:
 * CSSM_CANT_OPEN_KEY - Unable to open the key for section
 * CSSM_CANT_ENUM_KEY - Unable to enumerate the key for section
 * CSSM_CANT_DELETE_KEY - Unable to delte the key for section
 *---------------------------------------------------------------------------*/
CSSM_RETURN dlutil_port_DeleteSection(const char* Section,
									  sint32 PersonalPref)
{
#if defined (WIN32)

	if (PersonalPref)
	{
		if (RegWipeKey(HKEY_CURRENT_USER, Section) != CSSM_OK)
		{
			return (CSSMERR_CSSM_OS_ACCESS_DENIED);
		}
	}
	else
	{
		if (RegWipeKey(HKEY_LOCAL_MACHINE, Section) != CSSM_OK)
		{
			return (CSSMERR_CSSM_OS_ACCESS_DENIED);
		}
	}

	return (CSSM_OK);

#elif defined (UNIX)
	char dirpath[PATH_MAX + 1];
#if defined (LINUX) || defined (AIX) || defined (SOLARIS)
    const char rm_fmt[] = "/bin/rm -rf '%s'";
#else
	const char rm_fmt[] = "/usr/bin/rm -rf '%s'";
#endif
	char rm_buf[sizeof rm_fmt + sizeof dirpath + 1];
	int status;

	DPRINTF("registry", "(RegWipeKey) Section = %s\n", Section);

	if (get_registry_path(dirpath, sizeof dirpath, Section, PersonalPref))
	{
		return CSSMERR_CSSM_OS_ACCESS_DENIED;
	}

	sprintf(rm_buf, rm_fmt, dirpath);
	status = system(rm_buf);

	if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
	{
		return CSSMERR_CSSM_OS_ACCESS_DENIED;
	}

	return CSSM_OK;

#endif
}

#if defined (WIN32)
/*-----------------------------------------------------------------------------
 * Name: RegWipeKey
 *
 * Description:	 Recursively deletes a	Section in the registry
 *
 * Parameters:
 * hkey (input)			:	HKEY_CURRENT_USER or HKEY_LOCAL_MACHINE
 * szSubKey (input)		:	Key to delete
 *
 * Returns:
 * CSSM_OK if the operation was successful
 *
 * Error Codes:
 * CSSM_CANT_OPEN_KEY - Unable to open the key for section
 * CSSM_CANT_ENUM_KEY - Unable to enumerate the key for section
 * CSSM_CANT_DELETE_KEY - Unable to delte the key for section
 *---------------------------------------------------------------------------*/
CSSM_RETURN RegWipeKey(HKEY hkey,
					   const char* szSubKey)
{

	HKEY htempkey;
	char keyname[CSSM_MAX_REG];
	char tempkeyname[CSSM_MAX_REG];
	uint32 keynamelength;
	CSSM_RETURN Result;
	sint32 regResult;
	sint32 done = 0;

	do
	{
		/* open the key for enumeration */
		regResult = RegOpenKeyEx(hkey, szSubKey, 0, KEY_ALL_ACCESS, &htempkey);
		if (regResult != ERROR_SUCCESS)
		{
			return (CSSMERR_CSSM_OS_ACCESS_DENIED);
		}

		keynamelength = CSSM_MAX_REG;
		regResult = RegEnumKeyEx(htempkey,
								 0,
								 keyname,
								 (unsigned long *)&keynamelength,
								 0,
								 NULL,
								 NULL,
								 NULL);
		RegCloseKey(htempkey);

		if (regResult != ERROR_NO_MORE_ITEMS && regResult == ERROR_SUCCESS)
		{
			strcpy(tempkeyname,szSubKey);
			strcat(tempkeyname,"\\");
			strcat(tempkeyname,keyname);

			Result = RegWipeKey(hkey,tempkeyname);
			if (Result != CSSM_OK)
			{
				done = 1;
			}
		}
		else if (regResult == ERROR_NO_MORE_ITEMS)
		{
			done = 1;
		}
		else
		{
			return (CSSMERR_CSSM_OS_ACCESS_DENIED);
		}
	} while (!done);

	/* Nuke the requested key */
	regResult = RegDeleteKey(hkey,szSubKey);
	if (regResult != ERROR_SUCCESS)
	{
		return (CSSMERR_CSSM_OS_ACCESS_DENIED);
	}

	return (CSSM_OK);

}
#endif

/*-----------------------------------------------------------------------------
 * Name: dlutil_port_GetSectionList
 *
 * Description:	 Get a list of the sections under the specified section
 *
 * Parameters:
 * Section (input)			:	Name of the section in the configuration file to
 *								delete.	 May include a path to mimic
 *								registry paths such as "CSSM\AddIn\blah". This will
 *								map directly to the section name in Windows 3.1 and
 *								to the registry path under Win32, but can be easily
 *								adapted for other platforms.
 * list (output)			:	List of sections. This structure will be filled with
 *								an array of pointers to NAMES of sections (char **). The
 *								list will be terminated with a NULL name pointer.
 *								This structure and each name will be allocated with
 *								local_malloc.
 * PersonalPref (input)		:	Boolean indicator as to whether the value should be
 *								written to the system configuration data or to that
 *								of the user. Valid values are DLUTIL_CONFIG_GLOBAL and
 *								DLUTIL_CONFIG_PERSONAL
 *
 * Returns:
 * CSSM_OK if the operation was successful
 *
 * Error Codes:
 * CSSM_CANT_OPEN_KEY - Unable to open the key for section
 * CSSM_CANT_ENUM_KEY - Unable to enumerate the key for section
 * CSSM_CANT_QUERY_KEY - Unable to query the key for section
 *---------------------------------------------------------------------------*/

CSSM_RETURN dlutil_port_GetSectionList(const char* section,
									   CSSM_REG_LIST_PTR list,
									   sint32 PersonalPref)
{
#if defined (WIN32)

	HKEY hkey;
	uint32 numkeys;
	char name[CSSM_MAX_REG];
	uint32 namelength;
	uint32 index;
	sint32 regResult;

	if (PersonalPref)
	{
		regResult = RegOpenKeyEx(HKEY_CURRENT_USER,
								 section,
								 0,
								 KEY_ALL_ACCESS,
								 &hkey);
	}
	else
	{
		regResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
								 section,
								 0,
								 KEY_ALL_ACCESS,
								 &hkey);
	}

	if (regResult != ERROR_SUCCESS)
	{
		return (CSSMERR_CSSM_OS_ACCESS_DENIED);
	}

	/* Get the count of keys */
	regResult = RegQueryInfoKey(hkey,
								NULL,
								NULL,
								NULL,
								(unsigned long *)&numkeys,
								NULL,
								NULL,
								NULL,
								NULL,
								NULL,
								NULL,
								NULL);

	if (regResult != ERROR_SUCCESS)
	{
		RegCloseKey(hkey);
		return (CSSMERR_CSSM_OS_ACCESS_DENIED);
	}

	/* Allocate memory for the names */
	if ((*list = (CSSM_REG_LIST) local_malloc((sizeof(char *) * (numkeys + 1)),
											   NULL)) == NULL)
	{
		RegCloseKey(hkey);
		return (CSSMERR_CSSM_MEMORY_ERROR);
	}
	port_memset(*list,0,sizeof(char *) * (numkeys + 1));

	/* Enumerate the keys and place in string list */
	for (index = 0; index < numkeys; index++)
	{
		namelength = CSSM_MAX_REG;
		regResult = RegEnumKeyEx(hkey,
								 index,
								 name,
								 (unsigned long *)&namelength,
								 NULL,
								 NULL,
								 NULL,
								 NULL);

		if (regResult != ERROR_SUCCESS)
		{
			char* *ppChar;

			(*list)[index] = NULL;
			RegCloseKey(hkey);
			ppChar = *list;
			while (*ppChar)
			{
				local_free((*ppChar)++, NULL);
			}

			return (CSSMERR_CSSM_OS_ACCESS_DENIED);
		}

		/* Allocate memory for the name string */
		if (((*list)[index] = (char *) local_malloc((namelength + 1),
													 NULL)) == NULL)
		{
			char* *ppChar;

			(*list)[index] = NULL;
			RegCloseKey(hkey);
			ppChar = *list;
			while (*ppChar)
			{
				local_free((*ppChar)++, NULL);
			}

			return (CSSMERR_CSSM_MEMORY_ERROR);
		}

		/* Copy the CSP name into the new storage area */
		strcpy((*list)[index],name);
	}
	(*list)[index] = NULL;

	RegCloseKey(hkey);
	return (CSSM_OK);

#elif defined (UNIX)

	char dirpath[PATH_MAX + 1];
	char entpath[PATH_MAX + 1];
	DIR *dir;
	struct dirent *ent;
	struct stat statbuf;
	CSSM_REG_LIST dir_list = NULL;
	int num_dirs = 0;
	char *name;

	DPRINTF("registry", "(dlutil_port_GetSectionList) section = %s\n", section);

	if (get_registry_path(dirpath, sizeof dirpath, section, PersonalPref))
		return CSSMERR_CSSM_OS_ACCESS_DENIED;

	if ((dir = opendir(dirpath)) == NULL)
		return CSSMERR_CSSM_OS_ACCESS_DENIED;

	/*
	 * The list always needs room for a null-terminator, even if the
	 * section is empty.
	 */
	if ((dir_list = local_malloc(sizeof(char *), NULL)) == NULL)
		return CSSMERR_CSSM_OS_ACCESS_DENIED;

	for (ent = readdir(dir); ent; ent = readdir(dir))
	{
	if (strcmp(ent->d_name, ".")  == 0 ||
		strcmp(ent->d_name, "..") == 0)
		continue;

	snprintf(entpath, sizeof entpath, "%s/%s", dirpath, ent->d_name);
	if (stat(entpath, &statbuf))
	{
		closedir(dir);
		return CSSMERR_CSSM_OS_ACCESS_DENIED;
	}

	if ((statbuf.st_mode & S_IFMT) != S_IFDIR)
		continue;

	dir_list = local_realloc(dir_list,
				 (num_dirs + 2) * sizeof(char *), NULL);
	if (dir_list == NULL)
	{
		/*
		 * XXX If this wasn't just a prototype, we'd also free any
		 * dir_list entries that we had allocated.
		 */
		return CSSMERR_CSSM_OS_ACCESS_DENIED;
	}

	name = local_malloc(strlen(ent->d_name) + 1, NULL);
	if (name == NULL)
	{
		/*
		 * XXX If this wasn't just a prototype, we'd also free any
		 * dir_list entries that we had allocated.
		 */
		return CSSMERR_CSSM_OS_ACCESS_DENIED;
	}

	strcpy(name, ent->d_name);
	dir_list[num_dirs++] = name;
	}

	dir_list[num_dirs] = NULL;
	*list = dir_list;

	closedir(dir);

	return CSSM_OK;

#endif
}

/*-----------------------------------------------------------------------------
 * Name: dlutil_port_GetValueCount
 *
 * Description:	 Gets the number of values under the specified section
 *
 * Parameters:
 * Section (input)			:	Name of the section in the configuration file to
 *								delete.	 May include a path to mimic
 *								registry paths such as "CSSM\AddIn\blah". This will
 *								map directly to the section name in Windows 3.1 and
 *								to the registry path under Win32, but can be easily
 *								adapted for other platforms.
 * count (output)			:	Number of values under section
 * PersonalPref (input)		:	Boolean indicator as to whether the value should be
 *								written to the system configuration data or to that
 *								of the user. Valid values are DLUTIL_CONFIG_GLOBAL and
 *								DLUTIL_CONFIG_PERSONAL
 *
 * Returns:
 * CSSM_OK if the operation was successful
 *
 * Error Codes:
 * CSSM_CANT_OPEN_KEY - Unable to open the key for section
 * CSSM_CANT_QUERY_KEY - Unable to query the key for section
 *---------------------------------------------------------------------------*/

CSSM_RETURN dlutil_port_GetValueCount(const char* section,
									  sint32* count,
									  sint32 PersonalPref)
{
#if defined (WIN32)

	HKEY hkey;
	sint32 numkeys;
	sint32 regResult;

	if (PersonalPref)
	{
		regResult = RegOpenKeyEx(HKEY_CURRENT_USER,
								 section,
								 0,
								 KEY_ALL_ACCESS,
								 &hkey);
	}
	else
	{
		regResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
								 section,
								 0,
								 KEY_ALL_ACCESS,
								 &hkey);
	}

	if (regResult != ERROR_SUCCESS)
	{
		return (CSSMERR_CSSM_OS_ACCESS_DENIED);
	}

	/* Get the count of values */
	regResult = RegQueryInfoKey(hkey,
								NULL,
								NULL,
								NULL,
								NULL,
								NULL,
								NULL,
								(unsigned long *)&numkeys,
								NULL,
								NULL,
								NULL,
								NULL);

	RegCloseKey(hkey);
	if (regResult != ERROR_SUCCESS)
	{
		return (CSSMERR_CSSM_OS_ACCESS_DENIED);
	}

	*count = numkeys;

	return (CSSM_OK);

#elif defined (UNIX)

	char dirpath[PATH_MAX + 1];
	char valpath[PATH_MAX + 1];
	DIR *dir;
	struct dirent *ent;
	struct stat statbuf;
	int num_files = 0;

	DPRINTF("registry", "(dlutil_port_GetValueCount) section = %s\n", section);

	if (get_registry_path(dirpath, sizeof dirpath, section, PersonalPref))
		return CSSMERR_CSSM_OS_ACCESS_DENIED;

	if ((dir = opendir(dirpath)) == NULL)
		return CSSMERR_CSSM_OS_ACCESS_DENIED;

	for (ent = readdir(dir); ent; ent = readdir(dir))
	{
	if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
		continue;

	snprintf(valpath, sizeof valpath, "%s/%s", dirpath, ent->d_name);
	if (stat(valpath, &statbuf))
	{
		closedir(dir);
		return CSSMERR_CSSM_OS_ACCESS_DENIED;
	}

	if ((statbuf.st_mode & S_IFMT) == S_IFREG)
		num_files++;
	}

	closedir(dir);

	*count = num_files;

	DPRINTF("registry", "(cssm_GetValueCount) count = %d\n", *count);

	return CSSM_OK;

#endif
}

