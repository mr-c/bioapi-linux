/*-----------------------------------------------------------------------
 * File: PORTDLL.C
 *
 * Copyright (c) 1995-2000 Intel Corporation. All rights reserved.
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
#include <dlfcn.h>
/* Dynamic Library Loading routines */

/*-----------------------------------------------------------------------------
 * Name: port_LoadLibrary
 *
 * Description:  Loads a dynamically linked library
 * 
 * Parameters: 
 * LibraryPathName (input)      :  Path name of library
 *
 * Returns:
 * Handle to the loaded library
 * NULL if an error occurs
 * 
 * Error Codes:
 * CSSMERR_CSSM_OS_ACCESS_DENIED - Unable to load the specified library
 * CSSMERR_CSSM_INVALID_POINTER - LibraryPathName pointer is not valid
 *---------------------------------------------------------------------------*/
CSSM_RETURN CSSMAPI 
port_LoadLibrary(
    const char *LibraryPathName,
    CSSM_LIB_HANDLE *lib_handle)
{
    CSSM_LIB_HANDLE tmp;

    TRACE(port_trace_info, FNCODE_cssm_LoadLibrary);
    if (lib_handle == NULL)
    {
        RETURN(CSSMERR_CSSM_INVALID_POINTER);
    }
    *lib_handle = NULL;
    
    if (NULL == LibraryPathName || port_IsBadReadPtr(LibraryPathName, 1))
    {
        RETURN(CSSMERR_CSSM_INVALID_POINTER);
    }
    if (port_IsBadReadPtr(LibraryPathName, strlen(LibraryPathName)))
    {
        RETURN(CSSMERR_CSSM_INVALID_POINTER);
    }

#if defined (WIN32)
    /* LoadLibrary will call DllMain() to init */
    if ((tmp = LoadLibrary(LibraryPathName)) == NULL)
	{
		RETURN(CSSMERR_CSSM_OS_ACCESS_DENIED);
    }
#elif defined (UNIX)
    /* dlopen will call _init() to init */
    tmp = dlopen(LibraryPathName, RTLD_LAZY);
    if (tmp == NULL)
    {
			fputs (dlerror(), stderr);
#ifdef _DBG
        printf ("%s", dlerror());
#endif
#ifdef LINUX
      port_syslog(CSSM_LOG_PORT, CSSM_LOG_ERR, 
		  "port_LoadLibrary: dlopen(%s) failed, error = %s\n", 
		  LibraryPathName, dlerror());
#endif
        RETURN(CSSMERR_CSSM_OS_ACCESS_DENIED);
    }
#endif
    *lib_handle = tmp;

    RETURN(CSSM_OK);
}



/*-----------------------------------------------------------------------------
 * Name: port_FreeLibrary
 *
 * Description:  Frees a dynamically linked library
 * 
 * Parameters: 
 * handle (input)   :  Handle of loaded library 
 *
 * Returns:
 * CSSM_OK if the operation is successful
 * CSSM_FAIL if there is an error
 * 
 * Error Codes:
 * CSSMERR_CSSM_OS_ACCESS_DENIED - Unable to free the specified library
 *---------------------------------------------------------------------------*/
CSSM_RETURN CSSMAPI port_FreeLibrary(CSSM_LIB_HANDLE handle)
{
    TRACE(port_trace_info, FNCODE_cssm_FreeLibrary);

    if (handle == NULL)
    {
        RETURN(CSSMERR_CSSM_INVALID_POINTER);
    }

#if defined(WIN32)
    if (FreeLibrary((HMODULE)handle) != TRUE)
    {
        RETURN(CSSMERR_CSSM_OS_ACCESS_DENIED);
    }
#elif defined (UNIX)
#if defined (UNIX) && defined (ELF_FORMAT)
    /* see if we are trying to close an executable */
    if (!((struct link_map *)handle)->l_ld)
    {
        free(handle);
    }
    else
#endif /* LINUX && ELF_FORMAT */
    if (dlclose(handle))
    {
#ifdef _DBG
        printf ("%s", dlerror());
#endif
#ifdef LINUX
	port_syslog(CSSM_LOG_PORT, CSSM_LOG_ERR,
		    "FreeLibrary(): dlclose(%x) failed, error = %s\n",
		    handle, dlerror());
#endif
        RETURN(CSSMERR_CSSM_OS_ACCESS_DENIED);
    }
#endif
    RETURN(CSSM_OK);
}



/*-----------------------------------------------------------------------------
 * Name: port_GetProcAddress
 *
 * Description:  Returns the address of the specified function
 * 
 * Parameters: 
 * handle (input)       :  Handle for a loaded library
 * func_name (input)    :  Name of function to get the address for
 *
 * Returns:
 * Address of the specified function
 * NULL if an error occurs
 * 
 * Error Codes:
 * CSSMERR_CSSM_CANT_GET_PROC_ADDR - Unable to get the procedure address
 * CSSMERR_CSSM_OS_ACCESS_DENIED - handle is invalid
 * CSSMERR_CSSM_INVALID_POINTER - func_name ptr is invalid
 *---------------------------------------------------------------------------*/
CSSM_RETURN CSSMAPI 
port_GetProcAddress(
    CSSM_LIB_HANDLE handle, 
    const char * func_name,
    CSSM_PROC_ADDR *proc)
{
	CSSM_PROC_ADDR tmp;

    TRACE(port_trace_info, FNCODE_cssm_GetProcAddress);
    if (proc == NULL)
    {
        RETURN(CSSMERR_CSSM_INVALID_POINTER);
    }
    *proc = NULL;

    if (handle == NULL)
    {
        RETURN(CSSMERR_CSSM_OS_ACCESS_DENIED);
    }
    if (NULL == func_name || port_IsBadReadPtr(func_name, 1))
    {
        RETURN(CSSMERR_CSSM_INVALID_POINTER);
    }
    if (port_IsBadReadPtr(func_name, strlen(func_name)))
    {
        RETURN(CSSMERR_CSSM_INVALID_POINTER);
    }

#if defined (WIN32)
    tmp = GetProcAddress(handle, func_name);    
    if (tmp == NULL)
    {
        RETURN(CSSMERR_CSSM_CANT_GET_PROC_ADDR);
    }
#elif defined (UNIX)
    tmp = (CSSM_PROC_ADDR) dlsym(handle, func_name);
    if (tmp == NULL)
    {
#ifdef _DBG
        printf ("%s", dlerror());
#endif
#ifdef LINUX
	port_syslog(CSSM_LOG_PORT, CSSM_LOG_ERR, 
		    "port_GetProcAddress(): dlsym(%s) failed, error = %s\n",
		    func_name, dlerror());
#endif
        RETURN(CSSMERR_CSSM_CANT_GET_PROC_ADDR);
    }
#endif
    *proc = tmp;

    RETURN(CSSM_OK);
}



/*-----------------------------------------------------------------------------
 * Name: port_GetModuleHandle
 *
 * Description:  Returns the handle of the specified module
 * 
 * Parameters: 
 * module_name - Name of file to return a handle to
 *
 * Returns:
 * Handle to the specified module
 * NULL if an error occurs
 * 
 * Error Codes:
 * CSSMERR_CSSM_OS_ACCESS_DENIED - Unable to load the specified library
 * CSSMERR_CSSM_INVALID_POINTER - module_name pointer is not valid
 *---------------------------------------------------------------------------*/
CSSM_RETURN CSSMAPI 
port_GetModuleHandle(
    const char *module_name,
    CSSM_LIB_HANDLE *m_handle)
{
#if defined (WIN32)
    HMODULE module;
#elif defined (UNIX)
    CSSM_LIB_HANDLE module;
#endif

    TRACE(port_trace_info, FNCODE_cssm_GetModuleHandle);
    
    if (m_handle == NULL)
    {
        RETURN(CSSMERR_CSSM_INVALID_POINTER);
    }
    *m_handle = NULL;

#if defined (WIN32)
    if (NULL == module_name || port_IsBadReadPtr(module_name, 1))
    {
        RETURN(CSSMERR_CSSM_INVALID_POINTER);
    }

    if (port_IsBadReadPtr(module_name, strlen(module_name)))
    {
        RETURN(CSSMERR_CSSM_INVALID_POINTER);
    }

    module = GetModuleHandle(module_name);
    if (module == NULL)
    {
        RETURN(CSSMERR_CSSM_OS_ACCESS_DENIED);
    }
    *m_handle = module;
#elif defined (UNIX)
    /*
     * GetModuleHandle() leaves the module's ref count the same,
     * so we must too.  Otherwise, CSSM_ModuleUnload() won't actually
     * unload the module when CDSA is built with integrity checking.
     */
    if (port_LoadLibrary(module_name, &module) != CSSM_OK)
        RETURN(CSSMERR_CSSM_OS_ACCESS_DENIED);
    if (port_FreeLibrary(module) != CSSM_OK)
        RETURN(CSSMERR_CSSM_OS_ACCESS_DENIED);
    *m_handle = module;
#endif

    RETURN(CSSM_OK);
}


#if defined(LINUX) 
int     
dlnam(void *handle, char *filename, int len)
{
	Dl_info info;
	int result;
	result = dladdr(handle, &info);
	if (result == 0) {
		strncpy(filename, info.dli_fname, len);
	}
	return result;
}
void *
dlladdr(void *handle)
{
  
  Dl_info info;
  int result;
  result = dladdr(handle, &info);
  if (result == 0) {
	  return info.dli_fbase;
  } else {
	  return NULL;
  }
}
#endif



#if defined(SOLARIS) 
extern int _dlerror (void (*operate) (void *), void *args);
/*extern void _dlerror (int errcode,const char *objname,const char *errstring);*/

int     
dlnam(void *handle, char *filename, int len)
{
  struct link_map *map = handle;
  
  void doit (void *a)
  {
    if (handle == NULL)
      /*      _dlerror (0, NULL, "Null dll handle is invalid")*/;
    else
    {
      if ( (strlen(map->l_name) + 1) <= len )
	strcpy(filename, map->l_name);
      else
	/*_dlerror (0, NULL, "Not enough memory to return the dll name")*/;
    }
  }
  
  return -1 /*_dlerror (doit,(void *)NULL) ? -1 : strlen(map->l_name)*/;
}
void *
dlladdr(void *handle)
{
  struct link_map *map = handle;
  
  void doit (void *a)
  {
    if (handle == NULL)
      /*_dlerror (0, NULL, "Null as dll handle is invalid")*/;
  }
  
  return -1 /*_dlerror (doit,(void *)NULL) ? NULL : (void *)(map->l_addr)*/;
}
#endif

#if defined (__digital__)
#include <sys/types.h>
#include <loader.h>
/*----------------------------------------------------------------------------
 * Name: dlnam
 *
 * Description:  Given a handle to a shared object from dlopen(), gets
 *               the filename of the object.
 *
 * Parameters:
 * handle (input)   : Handle to shared object
 * buf (output)     : Filename buffer
 * buflen (input)   : Length in bytes of filename buffer
 *
 * Returns:
 *  0 if successful
 * -1 if an error occured
 *---------------------------------------------------------------------------*/
static int dlnam(void *handle, char *buf, int buflen)
{
    void *sym;
    ldr_process_t process;
    ldr_module_t mod_id;
    ldr_region_t reg_id;
    ldr_module_info_t mod_info;
    ldr_region_info_t reg_info;
    size_t size;
    void *endaddr;

    if ((sym = dlsym(handle, ".text")) == NULL)
    return -1;

    process = ldr_my_process();
    mod_id  = LDR_NULL_MODULE;

    for (;;)
    {
        if (ldr_next_module(process, &mod_id))
        return -1;
    if (mod_id == LDR_NULL_MODULE)
        break;

        if (ldr_inq_module(process, mod_id, &mod_info,
               sizeof mod_info, &size))
        return -1;
        if (size < sizeof mod_info)
        continue;

        for (reg_id = 0; reg_id < mod_info.lmi_nregion; reg_id++)
    {
            if (ldr_inq_region(process, mod_id, reg_id, &reg_info,
                               sizeof reg_info, &size))
        return -1;
        if (size < sizeof reg_info)
        continue;

        endaddr = (char *) reg_info.lri_mapaddr + reg_info.lri_size;
        if (sym >= reg_info.lri_mapaddr && sym < endaddr)
        {
        if (strlen(mod_info.lmi_name) >= buflen)
        {
            errno = ERANGE;
            return -1;
        }

        strcpy(buf, mod_info.lmi_name);
        return 0;
        }
        }
    }

    return -1;
}
#endif

/*-----------------------------------------------------------------------------
 * Name: port_GetModuleFileName
 *
 * Description:  Gets the filename of the specifed module
 * 
 * Parameters: 
 * module (input)   : Handle of a module
 * filename (output)    : Name of file
 * buf_length (input)   : Lenght of filename buffer
 *
 * Returns:
 * CSSM_OK if successful
 * CSSM_FAIL if an error occured
 * 
 * Error Codes:
 * CSSMERR_CSSM_OS_ACCESS_DENIED - Unable to get the module filename
 * CSSMERR_CSSM_INVALID_POINTER - LibraryPathName pointer is not valid
 * CSSMERR_CSSM_OS_ACCESS_DENIED - module handle is invalid
 *---------------------------------------------------------------------------*/
CSSM_RETURN CSSMAPI 
port_GetModuleFileName(
    CSSM_LIB_HANDLE module, 
    char *filename, 
    sint32 buf_length)
{
#if defined (WIN32)
    unsigned long  ret_val;
#endif

    TRACE(port_trace_info, FNCODE_cssm_GetModuleFileName);

#if defined (WIN32)
    if (module == NULL)
    {
        RETURN(CSSMERR_CSSM_OS_ACCESS_DENIED);
    }
    if (NULL == filename || port_IsBadReadPtr(filename, 1))
    {
        RETURN(CSSMERR_CSSM_INVALID_POINTER);
    }
    if (port_IsBadReadPtr(filename, strlen(filename)))
    {
        RETURN(CSSMERR_CSSM_INVALID_POINTER);
    }

    ret_val = GetModuleFileName(module, filename, (unsigned long)buf_length);

    if (ret_val == 0)
    {
        RETURN(CSSMERR_CSSM_OS_ACCESS_DENIED);
    }

#elif defined (AIX) 
    RETURN(CSSM_ERRCODE_FUNCTION_NOT_IMPLEMENTED);
#else
    if (dlnam(module, filename, buf_length)<0)
    {
        RETURN(CSSMERR_CSSM_OS_ACCESS_DENIED);
    }
#endif

    RETURN(CSSM_OK);
}


/*
 * getNTHeader
 *
 */
#if defined (WIN32)
PIMAGE_NT_HEADERS
port_getNTHeader(
    const unsigned char *image)
{
      PIMAGE_DOS_HEADER dosHeader;
      PIMAGE_NT_HEADERS pNTHeader;

      dosHeader = (PIMAGE_DOS_HEADER) image;
      if ( dosHeader->e_magic != IMAGE_DOS_SIGNATURE ){
            return NULL;
      }

      pNTHeader = (PIMAGE_NT_HEADERS)((DWORD) dosHeader + 
                  (DWORD) dosHeader->e_lfanew);
      if (pNTHeader->Signature != IMAGE_NT_SIGNATURE ){
            /* well, we can't figure this out */
            return NULL;
      }

      return pNTHeader;
}
#endif

CSSM_RETURN CSSMAPI
port_ComputeAddressInModule(CSSM_LIB_HANDLE LibHandle,
                            void* *StartAddress,
                            void* *EndAddress)
{
#if defined (WIN32)
    unsigned char *         pImage = NULL;
    PIMAGE_NT_HEADERS       pNTHeader = NULL;
    PIMAGE_SECTION_HEADER   SecHead = NULL;
    uint32                  SectionCount = 0;
    unsigned char *start = NULL;
    unsigned char *temp = NULL;
    unsigned char *end   = NULL;

    if( LibHandle == 0)
        return CSSMERR_CSSM_INTERNAL_ERROR;

    if (!StartAddress || !EndAddress)
        return CSSMERR_CSSM_INTERNAL_ERROR;

    pImage = (unsigned char *)LibHandle;
    if (!pImage)
        return CSSMERR_CSSM_INTERNAL_ERROR;

    pNTHeader = port_getNTHeader(pImage);
    if (!pNTHeader)
        return CSSMERR_CSSM_INTERNAL_ERROR;

    SecHead = IMAGE_FIRST_SECTION(pNTHeader);
    if (!SecHead)
        return CSSMERR_CSSM_INTERNAL_ERROR;

    start = pImage + SecHead->VirtualAddress;
    for(SectionCount = 0;
        SectionCount< (DWORD) pNTHeader->FileHeader.NumberOfSections;
        SectionCount++, SecHead++)
    {   
        temp = pImage + SecHead->VirtualAddress;       
        end   = temp +  SecHead->Misc.VirtualSize;
    }
    *StartAddress = start;
    *EndAddress = end;
#elif defined (LINUX) || defined (SOLARIS)
    struct link_map *module = (struct link_map *)LibHandle;

    if (!LibHandle || !StartAddress || !EndAddress)
        return CSSMERR_CSSM_INTERNAL_ERROR;

    if (module->l_addr)
    {
	uint8 *pSecTable;
        Elf_Ehdr *elf_hdr = (Elf_Ehdr *)module->l_addr;
	Elf_Half i, shnum = elf_hdr->e_shnum;
	Elf_Shdr *sec_hdr;
	Elf_Addr start = 0;
	Elf_Addr end = 0;
	Elf_Addr delta = (elf_hdr->e_type == ET_DYN) ? module->l_addr : 0;
	size_t st_size = elf_hdr->e_shentsize * shnum;
	int fd;

	if ((fd = open(module->l_name, O_RDONLY)) == -1)
	    return CSSMERR_CSSM_INTERNAL_ERROR;

	if (lseek(fd, elf_hdr->e_shoff, SEEK_SET) == -1)
	{
	    close(fd);
	    return CSSMERR_CSSM_INTERNAL_ERROR;
	}

	if ((pSecTable = malloc(st_size)) == NULL)
	{
	    close(fd);
	    return CSSMERR_CSSM_INTERNAL_ERROR;
	}

	if (read(fd, pSecTable, st_size) != st_size)
	{
	    free(pSecTable);
	    close(fd);
	    return CSSMERR_CSSM_INTERNAL_ERROR;
	}

	for (i = 0, sec_hdr = (Elf_Shdr *)pSecTable; i < shnum; sec_hdr++, i++)
	{
	    /* 
	     * The section does not appear in the process 
	     * memory image if sh_addr member is 0.
	     */
	    if (!sec_hdr->sh_addr)
	        continue; 

	    if (!start)
	        start = sec_hdr->sh_addr + delta;
	    end = sec_hdr->sh_addr + sec_hdr->sh_size;
	}
	end += delta;

	free(pSecTable);
	close(fd);
	*StartAddress = (void *)start;
	*EndAddress = (void *)end;
    }
    else
        return CSSMERR_CSSM_INTERNAL_ERROR;
#endif
    return CSSM_OK;
}

