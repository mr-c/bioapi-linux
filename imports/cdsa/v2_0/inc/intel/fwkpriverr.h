/*-----------------------------------------------------------------------
 * File: FWLPRIVERR.H
 *
 * Copyright (c) 1995-2000 Intel Corporation. All rights reserved.
 *-----------------------------------------------------------------------
 */

/*-----------------------------------------------------------------------
 * This file contains all of the includes and several definitions needed
 * for the porting library.
 */


#ifndef _FWKPRIVERR_H_
#define _FWKPRIVERR_H_


#ifndef _CSSMERR_H
#include "cssmerr.h"
#endif  /* _CSSMERR_H */

#define CSSMERR_CSSM_CANT_GET_PROC_ADDR     (CSSM_CSSM_PRIVATE_ERROR + 1)
#define CSSMERR_CSSM_END_OF_FILE            (CSSM_CSSM_PRIVATE_ERROR + 2)
#define CSSMERR_CSSM_LOCK_TIMEOUT           (CSSM_CSSM_PRIVATE_ERROR + 3)


#endif  /* _FWKPRIVERR_H_ */
