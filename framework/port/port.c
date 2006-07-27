/*-----------------------------------------------------------------------
 * File: PORT.C
 *
 * Copyright (c) 1995-2000 Intel Corporation. All rights reserved.
 * Port on Linux by Bull 1999-2000 (afchine.madjlessi@bull.net)
 *-----------------------------------------------------------------------
 */

/* Portions:
 * (C) COPYRIGHT International Business Machines Corp. 1996, 1997
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */

#include "internal.h"

#ifdef UNIX
/* Syslog facility */
#define LOGLEN 1024
void port_syslog(int Facility, int Priority, char* format, ...)
{
  va_list arglist;
  char sysmsg[LOGLEN];

  openlog(CSSM_CDSA_SYSLOG_IDENT, LOG_PID, Facility);
  va_start(arglist, format);
  vsnprintf(sysmsg, LOGLEN, format, arglist);
  va_end(arglist);
  syslog(Priority, "%s", sysmsg);
  closelog();
}

CSSM_RETURN CreateSystemDirectory(void)
{
  char	cmd[128];

  strcpy(cmd, "/bin/mkdir -p ");
  strcat(cmd,  BioAPI_SYSDIR);
  if (system(cmd) != 0)
  {
    port_syslog(CSSM_LOG_PORT, CSSM_LOG_ERR, "CreateSystemDirectory(): mkdir -p %s failed, errno = %d\n", BioAPI_SYSDIR, errno);
    RETURN(CSSM_ERRCODE_INTERNAL_ERROR)
  }
  chmod(BioAPI_SYSDIR, 00777);
  RETURN(CSSM_OK) 
}

CSSM_RETURN DeleteSystemDirectory(void)
{
  char	cmd[128];

  strcpy(cmd, "/bin/rm -rf ");
  strcat(cmd, BioAPI_SYSDIR);
  if (system(cmd) != 0)
  {
    port_syslog(CSSM_LOG_PORT, CSSM_LOG_ERR, "DeleteSystemDirectory(): rm -rf %s failed, errno = %d\n", BioAPI_SYSDIR, errno);
    RETURN(CSSM_ERRCODE_INTERNAL_ERROR)
  }
  RETURN(CSSM_OK) 
}

CSSM_RETURN GetSystemDirectory(char *lpBuffer, size_t uSize)
{ 
  if (uSize > 0)
  {
    if (strlen(BioAPI_SYSDIR) <= uSize - 1)
    {
      strcpy(lpBuffer, BioAPI_SYSDIR);
      RETURN(strlen(BioAPI_SYSDIR));
    }
    else
    {
      strncpy(lpBuffer, BioAPI_SYSDIR, uSize - 1);
      lpBuffer[uSize - 1] = '\0';
      RETURN(strlen(BioAPI_SYSDIR) + 1);
    }
  }
  else
  {
    RETURN(strlen(BioAPI_SYSDIR) + 1);
  }
}

CSSM_RETURN SearchPath(char* lpPath, char* lpFileName, char* lpExtension, 
		 size_t nBufferLength, char* lpBuffer, char** lpFilePart)
{
  char			currentPath[CSSM_MAX_PATH];
  char			systemPath[CSSM_MAX_PATH];
  char 			dirName[CSSM_MAX_PATH];
  struct dirent		dirEntry;
  struct dirent*	pDirEnt;
  DIR*			pDir;
  char*			p0;
  char*			pDelim0;
  char*			p;
  char*			pDelim;
  char*			fileName;
  char*			fullPath;
  char*			envPathList;
  char*			pathList;
  size_t		size;

  pDirEnt		= &dirEntry;
  pDir			= NULL;
  p0			= NULL;
  pDelim0		= NULL;
  p			= NULL;
  pDelim		= NULL;
  fileName		= NULL;
  fullPath		= NULL;
  envPathList		= NULL;
  pathList		= NULL;

  lpBuffer[0] = '\0';
  if ((lpExtension != NULL) && (strcmp(lpExtension, "") != 0))
    if (strstr(lpExtension, ".") == NULL)
      RETURN(CSSM_ERRCODE_INTERNAL_ERROR)
  if ((lpPath == NULL) || (strcmp(lpPath, "") == 0))
  {
    GetCurrentDirectory(nBufferLength, currentPath);
    GetSystemDirectory(systemPath, nBufferLength);
    envPathList = getenv("PATH");
    if ((pathList = (char*)internal_malloc(strlen(currentPath)+ 1 + 
				       strlen(systemPath) + 1 +
				       strlen(envPathList) +
				       1, NULL)) == NULL)
    {
      RETURN(CSSM_ERRCODE_INTERNAL_ERROR)
    }
    strcpy(pathList, currentPath);
    strcat(pathList, ":");
    strcat(pathList, systemPath);
    strcat(pathList, ":");
    strcat(pathList, envPathList);
  }
  else
  {
    if ((pathList = (char*)internal_malloc(strlen(lpPath)+ 1, NULL)) == NULL)
    {
      RETURN(CSSM_ERRCODE_INTERNAL_ERROR)
    }
    strcpy(pathList, lpPath);
  }
  p0		= (char*)pathList;
  pDelim0	= NULL;
  while(p0 - pathList < strlen(pathList))
  {
    pDelim0 = strstr(p0, ":");
    if (pDelim0 == NULL)
    {
      strcpy(dirName, p0);
    }
    else
    {
      strncpy(dirName, p0, pDelim0 - p0);
      dirName[pDelim0 - p0] = '\0';
      p0 = pDelim0 + 1;
    }
    if (strcmp(dirName + (strlen(dirName) - 1), ":") == 0)
      dirName[strlen(dirName) - 1] = '\0';
    if ((fileName = (char*)internal_malloc(strlen(lpFileName) 	+ 
				       strlen(lpExtension) 	+ 
				       1, NULL)) == NULL)
    {
      internal_free(pathList, NULL);
      RETURN(0)
    }
    strcpy(fileName, lpFileName);
    if (strstr(fileName, ".") == NULL)
      strcat(fileName, lpExtension);
    size = strlen(dirName) + 1 + strlen(fileName);
    if ((fullPath = (char*)internal_malloc(size + 1, NULL)) == NULL)
    {
      internal_free(pathList, NULL);
      internal_free(fileName, NULL);
      RETURN(CSSM_ERRCODE_INTERNAL_ERROR)
    }
    strcpy(fullPath, dirName);
    strcat(fullPath, "/");
    strcat(fullPath, fileName);
    if ((pDir = opendir(dirName)) == NULL)
    {
      port_syslog(CSSM_LOG_PORT, CSSM_LOG_ERR, "SearchPath(): opendir(%s) failed, errno = %d\n", dirName, errno);
      internal_free(pathList, NULL);
      internal_free(fileName, NULL);
      internal_free(fullPath, NULL);
      RETURN(CSSM_ERRCODE_INTERNAL_ERROR)
    }

    while (CSSM_TRUE)
    {
      readdir_r(pDir, pDirEnt, &pDirEnt);
      if (pDirEnt == NULL)
	break;
      if (pDirEnt->d_name[0] != '.')
      {
	if (strcmp(pDirEnt->d_name, fileName) == 0)		/* Found in this dirName! */
	{
	  strncpy(lpBuffer, fullPath, nBufferLength - 1);
	  lpBuffer[nBufferLength - 1] = '\0';
								/* File part computation */
	  if (lpFilePart != NULL && *lpFilePart != NULL)
	  {
	    p = lpBuffer;
	    while(p - lpBuffer < strlen(lpBuffer))
	    {
	      if ((pDelim = strstr(p, "/")) != NULL)
	      {
		*lpFilePart = pDelim + 1;
		p = pDelim + 1;
	      }
	      else
		break;
	    }
	  }
	  internal_free(pathList, NULL);
	  internal_free(fileName, NULL);
	  internal_free(fullPath, NULL);
	  closedir(pDir);
	  if (p - lpBuffer >= strlen(lpBuffer))
	  {
	    RETURN(CSSM_ERRCODE_INTERNAL_ERROR)
	  }
	  if (nBufferLength -1 >= size)
	  {
	    RETURN(size)
	  }
	  else
	  {
	    RETURN(size + 1)
	  }
	}
      }
    }
    internal_free(pathList, NULL);
    internal_free(fileName, NULL);
    internal_free(fullPath, NULL);
    closedir(pDir);
    RETURN(CSSM_OK)
  }
}

CSSM_BOOL CopyFile(char* lpExistingFileName, char* lpNewFileName, CSSM_BOOL bFailIfExists)
{
  struct stat	statbuf;	
  char*		buffer;
  int 		existing_file_fd;
  int 		new_file_fd;
  size_t	num_read;
  size_t	num_written;
  int		flags;

  if ((lpExistingFileName == NULL) || (lpNewFileName == NULL))
  {
    RETURN(CSSM_ERRCODE_INTERNAL_ERROR)
  }
  if ((existing_file_fd = open(lpExistingFileName, O_RDONLY)) < 0)
  {
    port_syslog(CSSM_LOG_PORT, CSSM_LOG_ERR, "CopyFile(): open(%s) failed, errno = %d\n", lpExistingFileName, errno);
    RETURN(CSSM_ERRCODE_INTERNAL_ERROR)
  }
  if (fstat(existing_file_fd, &statbuf) != 0)
  {
    port_syslog(CSSM_LOG_PORT, CSSM_LOG_ERR, "CopyFile(): fstat() failed, errno = %d\n", errno);
    close(existing_file_fd);
    RETURN(CSSM_ERRCODE_INTERNAL_ERROR)
  }
  if ((buffer = internal_malloc(statbuf.st_size, NULL)) == NULL)
  {
    close(existing_file_fd);
    RETURN(CSSM_ERRCODE_INTERNAL_ERROR)
  }
  flags = O_WRONLY | O_CREAT;
  if (bFailIfExists)
  {
    flags = flags | O_EXCL;
  }
  if ((new_file_fd = open(lpNewFileName, flags, statbuf.st_mode)) < 0)
  {
    port_syslog(CSSM_LOG_PORT, CSSM_LOG_ERR, "CopyFile(): open(%s) failed, errno = %d\n", lpNewFileName, errno);
    internal_free(buffer, NULL);
    close(existing_file_fd);
    RETURN(CSSM_ERRCODE_INTERNAL_ERROR)
  }
  if ((num_read = read(existing_file_fd, buffer, statbuf.st_size)) != statbuf.st_size)
  {
    port_syslog(CSSM_LOG_PORT, CSSM_LOG_ERR, "CopyFile(): read() failed, errno = %d, num_read = %d\n", errno, num_read);
    internal_free(buffer, NULL);
    close(existing_file_fd);
    RETURN(CSSM_ERRCODE_INTERNAL_ERROR)
  }

  if ((num_written = write(new_file_fd, buffer, statbuf.st_size)) != statbuf.st_size)
  {
    port_syslog(CSSM_LOG_PORT, CSSM_LOG_ERR, "CopyFile(): write() failed, num_read = %d\n", num_read);
    internal_free(buffer, NULL);
    close(existing_file_fd);
    close(new_file_fd);
    RETURN(CSSM_ERRCODE_INTERNAL_ERROR)
  }
  close(existing_file_fd);
  close(new_file_fd);
  RETURN(CSSM_TRUE)
}

char *_fullpath( char *absPath, const char *relPath, size_t maxLength )
{
    char curDir[_MAX_PATH];
    char *bufp, *curp, *relp;
    size_t curLen, relLen = strlen(relPath);
    int level = 0;

    /* If it is an absolute path, just copy. */
    if (relPath[0] == '/')
    {
        if (absPath == NULL)
	{
	    bufp = (char *) malloc(relLen+1);
	    if (bufp != NULL)
	    {
	        strcpy(bufp, relPath);
	    }
	    return bufp;
	}

	if (relLen+1 > maxLength)
	{
	    return NULL;
	}
	else
	{
	    strcpy(absPath, relPath);
	    return absPath;
	}
    }

    getcwd(curDir, _MAX_PATH);
    curLen = strlen(curDir);
    if (curDir[curLen-1] == '/')
    {
        curDir[--curLen] = '\0';
    }

    /* 
     * If it does not start with a '.' character, prepend the 
     * current directory path.
     */
    if (relPath[0] != '.')
    {
        if (absPath == NULL)
	{
	    bufp = (char *) malloc(curLen+relLen+2);
	    if (bufp != NULL)
	    {
	        sprintf(bufp, "%s/%s", curDir, relPath);
	    }
	    return bufp;
	}

	if (curLen+relLen+2 > maxLength)
	{
	    return NULL;
	}
	else
	{
	    sprintf(absPath, "%s/%s", curDir, relPath);
	    return absPath;
	}
    }

    /* Case when relPath starts with ".." */
    relp = (char *)relPath;
    while (relp)
    {
        if (relp[0] == '.' && relp[1] == '.' && relp[2] == '/')
	{
	    level++;
	    relp += 3;
	}
        else if (relp[0] == '.' && relp[1] == '/')
	{
	    relp += 2;
	}
	else
	{
	    break;
	}
    }

    while (level--)
    {
        if ((curp = strrchr(curDir, '/')) != NULL)
	{
	    *curp = '\0';
	}
    }

    curLen = strlen(curDir);
    relLen = strlen(relp);

    if (absPath == NULL)
    {
        bufp = (char *) malloc(curLen+relLen+2);
	if (bufp != NULL)
	{
	    sprintf(bufp, "%s/%s", curDir, relp);
	}
	return bufp;
    }

    if (curLen+relLen+2 > maxLength)
    {
        return NULL;
    }
    else
    {
        sprintf(absPath, "%s/%s", curDir, relp);
        return absPath;
    }
}

void _splitpath( char* path,  char* drive,  char* directory, 
		     char* filename,  char* extension )
{
	/* drive includes :
     * directory includes leading and trailing (forward and backward slashes)
     * filename without dot and slashes
     * extension with leading dot
     */
	char *drivechar, *dirchar, *namechar;

	dirchar    = strrchr(path, '/');
	namechar   = strrchr(path, '\\');
	dirchar = max(dirchar, namechar);
	if (dirchar)
		namechar   = strrchr(dirchar, '.');
	else
		namechar   = strrchr(path, '.');

	if (directory)
	{
		*directory = 0x00;
		if (dirchar)
		{
			strncat(directory, path, dirchar-path+1);
			path = dirchar+1;
		}
	}

	if (filename)
	{
		*filename = 0x00;
		if (namechar)
		{
			strncat(filename, path, namechar-path);
			if (extension)
			{
				*extension = 0x00;
				strcat(extension, namechar);
			}
		}
	}
} 

void _makepath(char* path, char* drive,
		   char* directory, char* filename,
		   char* extension )
{
    char ch;

    if ( !path )
		return;
    
    path[0] = 0;
    if ( directory )
	{
		if ( directory[0] )
		{
			strcat(path, directory);
			ch = path[strlen(path)-1];
			if (ch != '/' && ch != '\\')
			{
				strcat(path,"/");
			}
		}
	}

    if ( filename )
	{
		if ( filename[0] )
		{
			strcat(path, filename);
			if ( extension )
			{
				if ( extension[0] )
				{
					if ( extension[0] != '.' )
					{
						strcat(path, ".");
					}
					strcat(path, extension);
				}
			}
		}
	}
}
#endif

/*-----------------------------------------------------------------------------
 * Name: port_strlwr
 *
 * Description:  Converts a string to all lowercase
 * 
 * Parameters: 
 * str (input)           : Pointer to a string
 *
 * Returns:
 * Pointer to a string - same pointer that was passed in.
 * 
 * Error Codes:
 * None
 *---------------------------------------------------------------------------*/
char * CSSMAPI port_strlwr (char * str)
{
#if defined(WIN32)
    TRACE(port_trace_info, FNCODE_cssm_strlwr);
    _strlwr(str);
    RETURN(str)

#else 

    char *p;

    for(p = str; p < str + strlen( str ); p++)
    {
        *p = tolower(*p);
    }
    
    RETURN(str)

#endif

}



/* Misc */

/*-----------------------------------------------------------------------------
 * Name: port_getcwd
 *
 * Description:  Gets the current working directory
 * 
 * Parameters: 
 * dir (input)                 : Pointer 
 * size (input)                : Length of string pointer
 *
 * Returns:
 * Pointer to string containing directory name.
 * NULL if an error occured.
 * 
 * Error Codes:
 * CSSMERR_CSSM_OS_ACCESS_DENIED - unable to get the current working directory
 * CSSMERR_CSSM_INVALID_POINTER - dir pointer is not valid
 *---------------------------------------------------------------------------*/
CSSM_RETURN CSSMAPI 
port_getcwd(
    char *dir, 
    uint32 size)
{
    TRACE(port_trace_info, FNCODE_cssm_getcwd);

    if (port_IsBadWritePtr(dir, size))
    {
        RETURN(CSSMERR_CSSM_INVALID_POINTER);
    }

    if (getcwd(dir, (int)size) == NULL)
    {
        RETURN(CSSMERR_CSSM_OS_ACCESS_DENIED);
    }

    RETURN(CSSM_OK);
}


/*-----------------------------------------------------------------------------
 * Name: port_getenv
 *
 * Description:  Gets the value of the specified environment variable
 * 
 * Parameters: 
 * env_var (input)           : Pointer 
 *
 * Returns:
 * Pointer to string containing value of environment value
 * NULL if an error occured.
 * 
 * Error Codes:
 * CSSMERR_CSSM_OS_ACCESS_DENIED - specified environment variable not found
 * CSSMERR_CSSM_INVALID_POINTER - env_var pointer is not valid
 *---------------------------------------------------------------------------*/
CSSM_RETURN CSSMAPI 
port_getenv(
    char *env_var,
    char **env)
{
    uint32  ret_val;
    char *ptr;
    char szbuf[1];

    TRACE(port_trace_info, FNCODE_cssm_getenv);
    if (env == NULL)
    {
        RETURN(CSSMERR_CSSM_INVALID_POINTER);
    }
    *env = NULL;

    if (env_var == NULL || port_IsBadReadPtr(env_var, 1))
    {
        RETURN(CSSMERR_CSSM_INVALID_POINTER);
    }
    if (port_IsBadReadPtr(env_var, strlen(env_var)))
    {
        RETURN(CSSMERR_CSSM_INVALID_POINTER);
    }
    ret_val = GetEnvironmentVariable(env_var, szbuf, 0);
    if (ret_val == 0)
    {
        RETURN(CSSMERR_CSSM_OS_ACCESS_DENIED);
    }

    ptr = (char *) local_malloc(ret_val, NULL);
    if (ptr == NULL)
    {
        RETURN(CSSMERR_CSSM_MEMORY_ERROR);
    }
    GetEnvironmentVariable(env_var, ptr, ret_val);
    *env = ptr;
    
    RETURN(CSSM_OK);
}


/*
 * This routine is currently only used to convert 4-byte
 * privileges from big-endian to the host's byte order.  That
 * conversion is needed when getting privileges from signed
 * manifest credentials.
 *
 * The implementation works on both big and little-endian hosts.
 */

CSSM_RETURN CSSMAPI
port_BigToHostEndian(CSSM_DATA_PTR Value,
                     CSSM_DATA_PTR ConvertedValue)
{
    uint32 one = 1;
    CSSM_BOOL bigEndian = ( one != *((uint8*)&one) );
    uint32 i,j;

    if( ( Value->Length > 1 ) && (Value->Data != NULL) )
    {
        ConvertedValue->Length = Value->Length;
        ConvertedValue->Data = (uint8*)local_malloc( ConvertedValue->Length, NULL);
        if( ConvertedValue->Data == NULL )
            RETURN (CSSMERR_CSSM_MEMORY_ERROR)
        if ( bigEndian )
        {
            memcpy( ConvertedValue->Data, Value->Data, Value->Length );
        }
        else
        {    
            for( i=0,j=ConvertedValue->Length-1; i<ConvertedValue->Length;i++,j-- )
            {
                ConvertedValue->Data[i] = Value->Data[j];
            }
        }
        RETURN (CSSM_OK)
    }
    else
        RETURN (CSSMERR_CSSM_INVALID_POINTER)
}



/* Shared Memory File Functions */


#if defined (AIX) || defined (UNIX)
/*-----------------------------------------------------------------------------
 * Name: GetEnvironmentVariable
 *
 * Description:  Emulates Win32 GetEnvironmentVariable. 
 * 
 * Parameters: 
 * lpName (input)   : Points to a null-terminated string that 
 *                    specifies the environment variable. 
 * lpBuffer (input) : Points to a buffer to receive the value of the specified 
 *                    environment variable.
 * nSize (input)    : Specifies the size, in characters, of the buffer pointed 
 *                    to by the lpBuffer parameter. 
 *
 * Returns:
 *    - Number of characters stored into the buffer pointed to by lpBuffer,
 *    not including the terminating null character. 
 *    - Zero if name was not found.
 *    - Buffer size, in characters, required to hold the value string and its
 *    terminating null character, if he buffer pointed to by lpBuffer is not 
 *    large enough.
 * 
 * Error Codes: None.
 *
 *---------------------------------------------------------------------------*/
uint32 GetEnvironmentVariable(const char *lpName, char *lpBuffer, uint32 nSize)
{
    char *envptr = getenv(lpName);
    uint32 envsize = strlen (envptr); 
    TRACE(port_trace_info, FNCODE_GetEnvironmentVariable);
    if (envsize == 0)
        RETURN ( envsize)
    else if (envsize+1 <= nSize)
    {
        strcpy (lpBuffer, envptr);
        RETURN ( envsize)
    }
    else
        RETURN(envsize+1)
}     

/*-----------------------------------------------------------------------------
 * Name: _filelength 
 *
 * Description:  Emulates Win32 _filelength by returning the length, in bytes,
 *               of target file. 
 *
 * Parameters:
 * fd (input)  : file descriptor of target file 
 *
 * Returns:
 *    - file length in bytes. 
 *    - -1L to indicate errors
 * Error Codes: None.
 *---------------------------------------------------------------------------*/
sint32 _filelength (int fd)
{
    sint32 bytes;
    sint32 current_pos;
   
    TRACE(port_trace_info, FNCODE__filelength);

    if ((current_pos = lseek (fd, 0L, SEEK_CUR)) == -1)
    RETURN ( -1)
    bytes = lseek (fd, 0L, SEEK_END);
    lseek (fd, current_pos, SEEK_SET);
    RETURN(bytes)
}
#endif /* AIX */



#ifdef _DEBUG
#ifdef WIN32

CSSM_RETURN
DebugError(const char *File,
           uint32 Line,
           CSSM_RETURN error)
{
    if(CSSM_OK != error)
    {
        printf("File %s; Line %d # Error %x\n", File, Line, error);
    }

    return error;
}

#endif  /* WIN32 */
#endif  /* _DEBUG */
