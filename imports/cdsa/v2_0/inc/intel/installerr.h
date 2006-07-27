/*-----------------------------------------------------------------------
 * File: INSTALLERR.H
 *
 * Copyright (c) 1999-2000 Intel Corporation. All rights reserved.
 *-----------------------------------------------------------------------
 */

#ifndef _INSTALLERR_H
#define _INSTALLERR_H

typedef unsigned int INSTALL_ERROR;

#define INSTALL_SUCCESS \
                        (0)

#define INSTALL_CSSM_INIT_FAIL \
                        (INSTALL_SUCCESS + 1)

#define INSTALL_UNABLE_TO_CREATE_MODULE_INFO \
                        (INSTALL_SUCCESS + 2)

#define INSTALL_UNABLE_TO_LOAD_CSSM \
                        (INSTALL_SUCCESS + 3)

#define INSTALL_UNABLE_TO_GET_CSSM_INIT_ENTRY_PT \
                        (INSTALL_SUCCESS + 4)

#define INSTALL_UNABLE_TO_GET_CSSM_MODULE_INSTALL_ENTRY_PT \
                        (INSTALL_SUCCESS + 5)

#define INSTALL_UNABLE_TO_GET_CSSM_GET_MODULE_LOCATION_ENTRY_PT \
                        (INSTALL_SUCCESS + 6)

#define INSTALL_CREDENTIAL_FILE_OPEN_ERROR \
                        (INSTALL_SUCCESS + 7)

#define INSTALL_CSSM_MODULE_INSTALL_FAIL \
                        (INSTALL_SUCCESS + 8)

#define INSTALL_CSSM_MODULE_UNINSTALL_FAIL \
                        (INSTALL_SUCCESS + 9)

#define INSTALL_CSSM_GET_MODULE_LOCATION_FAIL \
                        (INSTALL_SUCCESS + 10)

#define INSTALL_FILE_ALREADY_EXISTS \
                        (INSTALL_SUCCESS + 11)

#define INSTALL_FILE_NOT_FOUND \
                        (INSTALL_SUCCESS + 12)

#define INSTALL_ACCESS_DENIED \
                        (INSTALL_SUCCESS + 13)

#define INSTALL_PATH_CONVERSION_ERROR \
                        (INSTALL_SUCCESS + 14)

#define INSTALL_REGISTRY_ERROR \
                        (INSTALL_SUCCESS + 15)

#define UNINSTALL_REGISTRY_ERROR \
                        (INSTALL_SUCCESS + 16)

#define INSTALL_UNKNOWN_ERROR \
                        (INSTALL_SUCCESS + 17)

#define INSTALL_PARAM_ERROR \
                        (INSTALL_SUCCESS + 18)

#define INSTALL_MDS_REGISTRY_ERROR \
                        (INSTALL_SUCCESS + 19)

#define INSTALL_MDS_DATABASE_ERROR \
                        (INSTALL_SUCCESS + 20)

#define INSTALL_UNABLE_TO_LOAD_MDS \
                        (INSTALL_SUCCESS + 21)


#endif /* _INSTALLERR_H */
