/*-----------------------------------------------------------------------
 * File: INTERNAL.H
 *-----------------------------------------------------------------------
 */


#ifndef __MDSUTIL_INTERNAL_H__
#define __MDSUTIL_INTERNAL_H__

#ifndef _CSSMTYPE_H
#error cssmtype.h must be included before this file
#endif
#ifndef __MDS_UTIL_H__
#error mds_util.h must be included before this file
#endif

/* Generic update and search functions */
CSSM_RETURN CSSMAPI __MDSU_UpdateSchema(
							MDSU_CONTEXT *pContext,
							const void *pSchemaData,
							_MDSU_AttributeConstructor ConstructAttributes,
							_MDSU_AttributeDestructor DestructAttributes,
							CSSM_DB_UNIQUE_RECORD_PTR *UniqueId );

CSSM_RETURN CSSMAPI __MDSU_FindFirst(
							MDSU_CONTEXT *pContext,
							void *pSchemaTemplate,
							MDSU_TEMPLATE_MASK ValidityMask,
							void *pSchemaData,
							_MDSU_AttributeConstructor ConstructAttributes,
							_MDSU_AttributeDestructor DestructAttributes,
							_MDSU_PredicateConstructor ConstructPredicate,
							_MDSU_PredicateDestructor DestructPredicate,
							_MDSU_AttributeConverter ConvertAttributes,
							CSSM_DB_UNIQUE_RECORD_PTR *UniqueId );

CSSM_RETURN CSSMAPI __MDSU_FindNext(
							MDSU_CONTEXT *pContext,
							void *pSchemaData,
							_MDSU_AttributeConstructor ConstructAttributes,
							_MDSU_AttributeDestructor DestructAttributes,
							_MDSU_AttributeConverter ConvertAttributes,
							CSSM_DB_UNIQUE_RECORD_PTR *UniqueId );

#endif /* ifndef __MDSUTIL_INTERNAL_H__ */
