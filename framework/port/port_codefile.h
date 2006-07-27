/*-----------------------------------------------------------------------
 * File: PORT_CODEFILE.H
 *
 * Copyright (c) 1995-2000 Intel Corporation. All rights reserved.
 *-----------------------------------------------------------------------
 */


/*********************************************************
 * File name: port_codefile.h                                 *
 * Contains codes associated with framework functions    *
 *********************************************************/

#ifdef  DO_TRACE

#ifndef _PORT_CODEFILE_H
#define _PORT_CODEFILE_H

#define FNCODE_GetEnvironmentVariable                                (PORT_BASE_CODE        +   1)
#define FNCODE_RegWipeKey                                            (PORT_BASE_CODE        +   2)
#define FNCODE__filelength                                           (PORT_BASE_CODE        +   3)
#define FNCODE_cssm_CloseMemoryFile                                  (PORT_BASE_CODE        +   4)
#define FNCODE_cssm_CloseMutex                                       (PORT_BASE_CODE        +   5)
#define FNCODE_cssm_CreateMemoryFile                                 (PORT_BASE_CODE        +   6)
#define FNCODE_cssm_CreateMutex                                      (PORT_BASE_CODE        +   7)
#define FNCODE_cssm_DeleteSection                                    (PORT_BASE_CODE        +   8)
#define FNCODE_cssm_FlushMemoryFile                                  (PORT_BASE_CODE        +   9)
#define FNCODE_cssm_FreeLibrary                                      (PORT_BASE_CODE        +  10)
#define FNCODE_cssm_GenSectionMap                                    (PORT_BASE_CODE        +  11)
#define FNCODE_cssm_GetIdentificationInfo                            (PORT_BASE_CODE        +  12)
#define FNCODE_cssm_GetLength                                        (PORT_BASE_CODE        +  13)
#define FNCODE_cssm_GetModuleFileName                                (PORT_BASE_CODE        +  14)
#define FNCODE_cssm_GetModuleHandle                                  (PORT_BASE_CODE        +  15)
#define FNCODE_cssm_GetProcAddress                                   (PORT_BASE_CODE        +  16)
#define FNCODE_cssm_GetSectionCount                                  (PORT_BASE_CODE        +  17)
#define FNCODE_cssm_GetSectionList                                   (PORT_BASE_CODE        +  18)
#define FNCODE_cssm_GetTime                                          (PORT_BASE_CODE        +  19)
#define FNCODE_cssm_GetUserName                                      (PORT_BASE_CODE        +  20)
#define FNCODE_cssm_GetValue                                         (PORT_BASE_CODE        +  21)
#define FNCODE_cssm_GetValueCount                                    (PORT_BASE_CODE        +  22)
#define FNCODE_cssm_GetValueList                                     (PORT_BASE_CODE        +  23)
#define FNCODE_cssm_IsBadCodePtr                                     (PORT_BASE_CODE        +  24)
#define FNCODE_cssm_IsBadReadPtr                                     (PORT_BASE_CODE        +  25)
#define FNCODE_cssm_IsBadStrPtr                                      (PORT_BASE_CODE        +  26)
#define FNCODE_cssm_IsBadWritePtr                                    (PORT_BASE_CODE        +  27)
#define FNCODE_cssm_LoadLibrary                                      (PORT_BASE_CODE        +  28)
#define FNCODE_cssm_LockMutex                                        (PORT_BASE_CODE        +  29)
#define FNCODE_cssm_MapMemoryFile                                    (PORT_BASE_CODE        +  30)
#define FNCODE_cssm_OpenMemoryFile                                   (PORT_BASE_CODE        +  31)
#define FNCODE_cssm_SetValue                                         (PORT_BASE_CODE        +  32)
#define FNCODE_cssm_TrylockMutex                                     (PORT_BASE_CODE        +  33)
#define FNCODE_cssm_UnlockMutex                                      (PORT_BASE_CODE        +  34)
#define FNCODE_cssm_UnmapMemoryFile                                  (PORT_BASE_CODE        +  35)
#define FNCODE_cssm_calloc                                           (PORT_BASE_CODE        +  36)
#define FNCODE_cssm_fread                                            (PORT_BASE_CODE        +  37)
#define FNCODE_cssm_free                                             (PORT_BASE_CODE        +  38)
#define FNCODE_cssm_fwrite                                           (PORT_BASE_CODE        +  39)
#define FNCODE_cssm_getcwd                                           (PORT_BASE_CODE        +  40)
#define FNCODE_cssm_getenv                                           (PORT_BASE_CODE        +  41)
#define FNCODE_cssm_malloc                                           (PORT_BASE_CODE        +  42)
#define FNCODE_cssm_memcmp                                           (PORT_BASE_CODE        +  43)
#define FNCODE_cssm_memcpy                                           (PORT_BASE_CODE        +  44)
#define FNCODE_cssm_memmove                                          (PORT_BASE_CODE        +  45)
#define FNCODE_cssm_memset                                           (PORT_BASE_CODE        +  46)
#define FNCODE_cssm_port_init                                        (PORT_BASE_CODE        +  47)
#define FNCODE_cssm_port_terminate                                   (PORT_BASE_CODE        +  48)
#define FNCODE_cssm_realloc                                          (PORT_BASE_CODE        +  49)
#define FNCODE_cssm_strlwr                                           (PORT_BASE_CODE        +  50)


#define PORT_BASE_CODE 2000

#define FILTER_PORT "port"

#ifndef BINARY_TRACE
extern char* port_function_names[];
typedef struct {char *filter; char **function_names;} TraceInfo;
extern TraceInfo port_trace_info;
#else 
#define port_trace_info FILTER_PORT
#endif /* BINARY_TRACE */

#endif /* _PORT_CODEFILE_H */

#endif /* DO_TRACE */
