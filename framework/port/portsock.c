/*-----------------------------------------------------------------------
 *      File:   portsock.c
 *
 * Copyright (c) 1995-2000 Intel Corporation. All rights reserved.
 *-----------------------------------------------------------------------
 */

#include "internal.h"



/*-----------------------------------------------------------------------------
 * Name: port_GetUserName
 *
 * Description:  Gets the name of the current user
 * 
 * Parameters: 
 * user_name (input)           : Pointer 
 * size (input)                : Length of string pointer
 *
 * Returns:
 * Pointer to string containing user's name.
 * NULL if an error occured.
 * 
 * Error Codes:
 * CSSM_CANT_GET_USER_NAME - unable to get the user's name
 * CSSMERR_CSSM_INVALID_POINTER - user_name pointer is not valid
 *---------------------------------------------------------------------------*/
CSSM_RETURN CSSMAPI 
port_GetUserName(
    char *user_name, 
    uint32 size)
{
#if defined (UNIX)
    struct passwd *pwptr = 0;
#endif

    TRACE(port_trace_info, FNCODE_cssm_GetUserName);

    if (port_IsBadWritePtr(user_name, size))
    {
        RETURN(CSSMERR_CSSM_INVALID_POINTER);
    }
#if defined (UNIX)
    pwptr = getpwuid(getuid());

    if ((pwptr != NULL) && (strlen(pwptr->pw_name) + 1 <= size))
    {
        strcpy(user_name, pwptr->pw_name);
        RETURN(CSSM_OK);
    }
#else
    if (GetUserName(user_name, (unsigned long *)&size)) 
    {
        RETURN(CSSM_OK);
    }
#endif

    RETURN(CSSMERR_CSSM_OS_ACCESS_DENIED);
}
