/*-----------------------------------------------------------------------
 * File: PORTFILE.C
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


/* File I/O routines */
/*----------------------------------------------------------
 *Name: port_fopen
 *
 *Description: Opens the file 'filename' in the specified 'mode'.
 * 
 *Parameters:
 *  filename (input) - the name of the file to be opened
 *  mode (input) - the desired mode (read/write, text/binary)
 * 
 *Returns: 
 *  stream - pointer to the open file or NULL if an an error occurred
 * 
 *Error Codess:
 *  CSSMERR_CSSM_INVALID_POINTER
 *  CSSMERR_CSSM_INVALID_POINTER
 *  CSSMERR_CSSM_OS_ACCESS_DENIED
 *--------------------------------------------------------*/
CSSM_RETURN CSSMAPI 
port_fopen(
    const char * filename, 
    const char * mode,
    FILE **file_ptr)
{
    if (file_ptr == NULL)
    {
        return CSSMERR_CSSM_INVALID_POINTER;
    }
    *file_ptr = NULL;
    
    if (filename == NULL)
    {
        return CSSMERR_CSSM_INVALID_POINTER;
    }
    if (port_IsBadStrPtr(filename, strlen(filename)))
    {
        return CSSMERR_CSSM_INVALID_POINTER;
    }
    if (mode == NULL)
    {
        return CSSMERR_CSSM_INVALID_POINTER;
    }
    if (port_IsBadStrPtr(mode, strlen(mode)))
    {
        return CSSMERR_CSSM_INVALID_POINTER;
    }

    *file_ptr = fopen(filename, mode);
    if (*file_ptr == 0)
    {
        return CSSMERR_CSSM_OS_ACCESS_DENIED;
    }

    return CSSM_OK;
}



/*----------------------------------------------------------
 *Name: port_fclose
 *
 *Description: Opens the file 'filename' in the specified 'mode'.
 * 
 *Parameters:
 *  stream (input) - pointer to an open file
 * 
 *Returns: 
 *  CSSM_OK - The clsoe was successfully completed
 *  CSSM_FAIL - A problem was encountered during the file close.
 *     Call cssm_GetError for more info.
 * 
 *Error Codess:
 *  CSSMERR_CSSM_INVALID_POINTER
 *  CSSM_FCLOSE_FAILED
 *--------------------------------------------------------*/
CSSM_RETURN CSSMAPI port_fclose (FILE * stream)
{
    int fclose_rtn = 0;

    if (stream == NULL)
    {
        return CSSMERR_CSSM_INVALID_POINTER;
    }

    fclose_rtn = fclose(stream);

    if (fclose_rtn != 0)
    {
        return CSSMERR_CSSM_OS_ACCESS_DENIED;
    }
    return CSSM_OK;
}



/*----------------------------------------------------------
 *Name: CSSM_fwrite
 *
 *Description: Writes the contents of 'buffer' to the file 
 *  pointed to by 'stream' one byte at a time.  The file should
 *  have been opened in binary mode.
 * 
 *Parameters:
 *  buffer (input) - pointer to data to be written
 *  size (input) - size of each item to be written
 *  num (input) - number of items to be written
 *  stream (input) - pointer to a file opened in binary mode
 * 
 *Returns: 
 *  CSSM_OK - The write was successfully completed
 *  CSSM_FAIL - A problem was encountered during the write.
 *     Call cssm_GetError for more info.
 * 
 *Error Codess:
 *  CSSMERR_CSSM_INVALID_POINTER
 *  CSSM_FWRITE_FAILED
 *--------------------------------------------------------*/
CSSM_RETURN CSSMAPI 
port_fwrite(
    const void *buffer, 
    uint32 size, 
    uint32  num, 
    FILE *stream )
{
    
    uint32  num_written, total_size, i;
    TRACE(port_trace_info, FNCODE_cssm_fwrite);
    if (stream == NULL)
    {
        RETURN(CSSMERR_CSSM_INVALID_POINTER);
    }

    total_size = size * num;

    if (port_IsBadReadPtr(buffer, total_size))
    {
        RETURN(CSSMERR_CSSM_INVALID_POINTER);
    }
    
    for (i = 0; i < total_size; i++)
    {
    
        num_written = fwrite(buffer, 1, 1, stream);

        if (num_written != 1)
        {
            RETURN(CSSMERR_CSSM_OS_ACCESS_DENIED);
        }
            
        buffer = (uint8 *) buffer + sizeof(uint8);
    }
    RETURN(CSSM_OK)
}



/*----------------------------------------------------------
 *Name: CSSM_fread
 *
 *Description: Reads the contents of the file pointed to by 
 *  'stream' to 'buffer' one byte at a time.  The file should
 *  have been opened in binary mode.
 * 
 *Parameters:
 *  buffer (output) - pointer to area to place data
 *  size (input) - size of each item to be read
 *  num (input) - number of items to be read
 *  stream (input) - pointer to a file opened in binary mode
 * 
 *Returns: 
 *  CSSM_OK - The read was successfully completed
 *  CSSM_FAIL - A problem was encountered during the read.
 *     Call CSSM_GetError for more info.
 * 
 *Error Codes:
 *  CSSMERR_CSSM_INVALID_POINTER - 'buffer' is not valid for writing data to,
 *      or 'stream' is NULL
 *  CSSM_FREAD_FAILED - can't read from file
 *  CSSMERR_CSSM_END_OF_FILE - end of file encountered before all requested
 *    data was read.  caller can determine severity of this error
 *--------------------------------------------------------*/
CSSM_RETURN CSSMAPI 
port_fread(
    void *buffer, 
    uint32 size, 
    uint32  num, 
    FILE *stream )
{

    uint32  num_read, total_size, i;
    TRACE(port_trace_info, FNCODE_cssm_fread);
    if (stream == NULL)
    {
        RETURN(CSSMERR_CSSM_INVALID_POINTER);
    }

    total_size = size * num;

    if (port_IsBadWritePtr(buffer, total_size))
    {
        RETURN(CSSMERR_CSSM_INVALID_POINTER);
    }
    
    for (i = 0; i < total_size; i++)
    {
        num_read = fread(buffer, 1, 1, stream);
        
        if (num_read != 1)
        {
            if (feof(stream))
            {
                /* May not be an error case, less data in the file
                   then expected */
                RETURN(CSSMERR_CSSM_END_OF_FILE);
            }
            else
            {
                RETURN(CSSMERR_CSSM_OS_ACCESS_DENIED);
            }
        }
            
        buffer = (uint8 *) buffer + sizeof(uint8);
    }
    RETURN(CSSM_OK)
    
}



/*----------------------------------------------------------
 *Name: CSSM_GetLength
 *
 *Description: Returns the length of the file pointed to by 'stream'
 * 
 *Parameters:
 *  stream (input) - pointer to a file 
 * 
 *Returns: 
 *  -1:  Error encountered.  Call CSSM_GetError for more info
 *  0 <= file_size:  Size of file
 * 
 *Error Codes:
 *  CSSMERR_CSSM_OS_ACCESS_DENIED
 *  CSSMERR_CSSM_INVALID_POINTER
 *--------------------------------------------------------*/
CSSM_RETURN CSSMAPI 
port_GetLength(
    FILE *stream,
    sint32 *len)
{
    sint32 bytes;
    sint32 current_pos;

    TRACE(port_trace_info, FNCODE_cssm_GetLength);

    if (len == NULL)
    {
        RETURN(CSSMERR_CSSM_INVALID_POINTER);
    }
    *len = -1;
    
    /* Check that file pointer passed is valid */
    if (stream == NULL)
    {
        RETURN(CSSMERR_CSSM_INVALID_POINTER);
    }

    /* Record current file cursor position */
    current_pos = ftell(stream);
    
    /* Verify current position is valid, if not set to 0 */
    if (current_pos < 0)
    {
        current_pos = 0;
    }
    
    /* Seek to the end of the file */
    if (fseek(stream, 0, SEEK_END))
    {
        RETURN(CSSMERR_CSSM_OS_ACCESS_DENIED);
    }
    
    /* Record position (at end of file) */
    bytes = ftell(stream);

    /* Verify the end of file position found is valid (not negative) */
    if (bytes == -1)
    {
        RETURN(CSSMERR_CSSM_OS_ACCESS_DENIED);
    }

    /* Return file pointer to original position */
    fseek(stream, current_pos, SEEK_SET);
    
    /* Return file length  -> IBM_CHANGE */
    /* RETURN(bytes + 1) */
    *len = bytes;
    
    RETURN(CSSM_OK);
}
