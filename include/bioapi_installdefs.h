/*-----------------------------------------------------------------------
 * File: INSTALLDEFS.H
 *-----------------------------------------------------------------------
 */

#ifndef __INSTALLDEFS_H__
#define __INSTALLDEFS_H__

#include "cssmtype.h"

typedef enum INSTALL_ACTION
{
	INSTALL_ACTION_INSTALL,
	INSTALL_ACTION_REFRESH,
	INSTALL_ACTION_UNINSTALL
} INSTALL_ACTION;

typedef struct _install_error
{
	BioAPI_RETURN ErrorCode;
	CSSM_STRING ErrorString;
}
BioAPI_INSTALL_ERROR;

#define INSTALLERR_BASE				( 0x80000000 )
#define INSTALLERR_NO_DEVICE_SLOTS	( INSTALLERR_BASE + 1 )

/* Name of the installation entry point */
#define INSTALL_ENTRY_NAME			RegisterBioAPIModule
#define INSTALL_ENTRY_NAME_STRING	"RegisterBioAPIModule"

/* The entry point in the module must have this definition */
BioAPI_RETURN BioAPI INSTALL_ENTRY_NAME(
						const char *szModuleName,
						const char *szSearchPath,
						BioAPI_DATA *pCredential,
						INSTALL_ACTION Action,
						BioAPI_INSTALL_ERROR *pError );

/* Pointer type to the above function */
typedef BioAPI_RETURN (BioAPI *INSTALL_ENTRY_PTR) (
						const char *szModuleName,
						const char *szSearchPath,
						BioAPI_DATA *pCredential,
						INSTALL_ACTION Action,
						BioAPI_INSTALL_ERROR *pError );

#endif /* ifndef __INSTALDEFS_H__ */
