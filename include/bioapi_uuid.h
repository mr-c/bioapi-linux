/*
 * BIOAPI_UUID.H
 *
 * Functions for UUID handling.
 *
 * These are NOT a part of the BioAPI specification.
 */

#ifndef _BIOAPIUUID_H
#define _BIOAPIUUID_H

#ifdef __cplusplus
extern "C"
{
#endif

#define BioAPI_CompareUuids( uuid1, uuid2 ) \
	( (port_memcmp (uuid1, uuid2, sizeof (BioAPI_UUID)) == 0) ? BioAPI_TRUE : BioAPI_FALSE )

#define BioAPI_CopyUuid( uuid1, uuid2 ) \
	( port_memcpy (uuid1, uuid2, sizeof (BioAPI_UUID)) )

#define BioAPI_ClearUuid( uuid ) \
	( memset (uuid, 0, sizeof (BioAPI_UUID)) )



/**************************************************************************
 * All UUID values are defined as follows. The sample name used in the
 * example is "my_module_uuid".
 *
 * 1) A variable initializer string for type BioAPI_UUID is defined with the
 *	  target name with "_init" appended. (i.e. my_module_uuid_init). It is
 *	  used to initialize individual instances of a variable. For instance,
 *	  to declare a BioAPI_UUID variable with the my_module_uuid value you
 *	  would use the following code:
 *		 BioAPI_UUID TempUuid = my_module_uuid_init;
 *
 * 2) The macro _BioAPI_DEFINE_UUID (defined below) is used to declare a
 *	  variable instance with the value of the corresponding initializer.
 *
 *		 _BioAPI_DEFINE_UUID( my_module_uuid, my_module_uuid_init );
 *
 *	  If the value BioAPI_INITUUID is defined, then the macro will expand to
 *	  produce a global constant variable with the appropriate value. If
 *	  BioAPI_INITUUID is not defined, then the macro expands to an external
 *	  declaration of a global constant.
 *
 *	  For instance, if a source file needs the	global constants to be
 *	  defined, the following code is used:
 *
 *		 #define BioAPI_INITUUID
 *		 #include "bioapi_uuid.h"
 *
 *	  If only external references are required, just include the header
 *	  normally.
 *
 *	  NOTE: ALL STATIC LIBRARIES SHOULD AVOID DECLARING BioAPI_INITUUID. THIS
 *	  WILL CAUSE A LINKER COLLISION IF ANOTHER STATIC LIBRARY OR THE
 *	  EXECUTABLE ITSELF DEFINES BioAPI_INITUUID.
 *
 *	  NOTE: ONLY ONE SOURCE FILE IN AN EXECUTABLE SHOULD DEFINE
 *	  BioAPI_INITUUID.
 *
 * NOTE: The preferred method of using the UUID values in this file is to
 * use the initializer values described in (1) above. It allows the finest
 * control over the set of constants instantiated in your executable.
 **************************************************************************
 */
#ifndef BioAPI_INITUUID
#define _BioAPI_DEFINE_UUID(name, value) \
	extern BioAPI_UUID name
#else
#define _BioAPI_DEFINE_UUID(name, value) \
	BioAPI_UUID name = value
#endif /* BioAPI_INITUUID */


/****************************************************************************/
/*************************	 CURRENT UUIDS	  *******************************/
/****************************************************************************/
/* Version independant references to global BioAPI_UUID constants */
#define ref_hlayer_uuid					reference_h_layer_uuid_1_0_0


/* Version independant references to BioAPI_UUID initializer values */
#define ref_h_layer_uuid_init			reference_h_layer_1_0_0_uuid_init


/* ----- H Layer ----- */
/* reference implementation v1.0: */
/* {AEC05377-3132-412f-A6F5-35E48735B762} */
#define reference_h_layer_1_0_0_uuid_init { 0xae, 0xc0, 0x53, 0x77, 0x31, 0x32, 0x41, 0x2f, 0xa6, 0xf5, 0x35, 0xe4, 0x87, 0x35, 0xb7, 0x62 }
_BioAPI_DEFINE_UUID( reference_h_layer_uuid_1_0_0, reference_h_layer_1_0_0_uuid_init );



#ifdef __cplusplus
}
#endif

#endif /* _BIOAPIUUID_H */
