/*-----------------------------------------------------------------------
 * File: DAL_SCHEMA.H
 *
 * Copyright (c) 1995-1999 Intel Corporation. All rights reserved.
 *-----------------------------------------------------------------------
 */

/*
 *	Contains constants and #defines for schema information for DAL
 */

#ifndef DAL_SCHEMA_H
#define DAL_SCHEMA_H

#include <cssmtype.h>


/*----------------------------------------------------------------------------
					Defines for Application Record Types
----------------------------------------------------------------------------*/

#define CSSM_DL_DB_APP_NUM_RELATIONS					(8)

#define CSSM_DL_DB_RECORD_CERT_NUM_ATTRIBUTES			(4)
#define CSSM_DL_DB_RECORD_CERT_NUM_INDEXES				(CSSM_DL_DB_RECORD_CERT_NUM_ATTRIBUTES)

#define CSSM_DL_DB_RECORD_CRL_NUM_ATTRIBUTES			(4)
#define CSSM_DL_DB_RECORD_CRL_NUM_INDEXES				(CSSM_DL_DB_RECORD_CRL_NUM_ATTRIBUTES)

#define CSSM_DL_DB_RECORD_POLICY_NUM_ATTRIBUTES			(3)
#define CSSM_DL_DB_RECORD_POLICY_NUM_INDEXES			(CSSM_DL_DB_RECORD_POLICY_NUM_ATTRIBUTES)

#define CSSM_DL_DB_RECORD_GENERIC_NUM_ATTRIBUTES		(2)
#define CSSM_DL_DB_RECORD_GENERIC_NUM_INDEXES			(CSSM_DL_DB_RECORD_GENERIC_NUM_ATTRIBUTES)

#define CSSM_DL_DB_RECORD_PUBLIC_KEY_NUM_ATTRIBUTES		(27)
#define CSSM_DL_DB_RECORD_PUBLIC_KEY_NUM_INDEXES		(CSSM_DL_DB_RECORD_PUBLIC_KEY_NUM_ATTRIBUTES)

#define CSSM_DL_DB_RECORD_PRIVATE_KEY_NUM_ATTRIBUTES	(27)
#define CSSM_DL_DB_RECORD_PRIVATE_KEY_NUM_INDEXES		(CSSM_DL_DB_RECORD_PRIVATE_KEY_NUM_ATTRIBUTES)

#define CSSM_DL_DB_RECORD_SYMMETRIC_KEY_NUM_ATTRIBUTES	(27)
#define CSSM_DL_DB_RECORD_SYMMETRIC_KEY_NUM_INDEXES		(CSSM_DL_DB_RECORD_SYMMETRIC_KEY_NUM_ATTRIBUTES)

#define CSSM_DL_DB_RECORD_ALL_KEYS_NUM_ATTRIBUTES		(27)
#define CSSM_DL_DB_RECORD_ALL_KEYS_NUM_INDEXES			(CSSM_DL_DB_RECORD_ALL_KEYS_NUM_ATTRIBUTES)


/*-----------------------------------------------------------------------------
						Declarations of Common Field's
-----------------------------------------------------------------------------*/

/* Certificate's Type (CertType) */
#define DAL_ATTRINFO_CERTTYPE { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "CertType" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_UINT32 \
}
static const CSSM_DB_ATTRIBUTE_INFO s_CssmDlDbAttrInfo_CertType = DAL_ATTRINFO_CERTTYPE;

/* Certificate's Encoding (CertEncoding) */
#define DAL_ATTRINFO_CERTENCODING { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "CertEncoding" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_UINT32 \
}
static const CSSM_DB_ATTRIBUTE_INFO s_CssmDlDbAttrInfo_CertEncoding = DAL_ATTRINFO_CERTENCODING;

/* Certificate's Subject Name (SubjectName) */
#define DAL_ATTRINFO_SUBJECTNAME { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "SubjectName" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_MULTI_UINT32 \
}
static const CSSM_DB_ATTRIBUTE_INFO s_CssmDlDbAttrInfo_SubjectName = DAL_ATTRINFO_SUBJECTNAME;

/* PrintName */
#define DAL_ATTRINFO_PRINTNAME { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "PrintName" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_STRING \
}
static const CSSM_DB_ATTRIBUTE_INFO s_CssmDlDbAttrInfo_PrintName = DAL_ATTRINFO_PRINTNAME;

/* Alias */
#define DAL_ATTRINFO_ALIAS { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "Alias" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_BLOB \
}
static const CSSM_DB_ATTRIBUTE_INFO s_CssmDlDbAttrInfo_Alias = DAL_ATTRINFO_ALIAS;

/* CrlType */
#define DAL_ATTRINFO_CRLTYPE { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "CrlType" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_UINT32 \
}
static const CSSM_DB_ATTRIBUTE_INFO s_CssmDlDbAttrInfo_CrlType = DAL_ATTRINFO_CRLTYPE;

/* CrlEncoding */
#define DAL_ATTRINFO_CRLENCODING { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "CrlEncoding" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_UINT32 \
}
static const CSSM_DB_ATTRIBUTE_INFO s_CssmDlDbAttrInfo_CrlEncoding = DAL_ATTRINFO_CRLENCODING;

/* PolicyName */
#define DAL_ATTRINFO_POLICYNAME { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "PolicyName" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_UINT32 \
}
static const CSSM_DB_ATTRIBUTE_INFO s_CssmDlDbAttrInfo_PolicyName = DAL_ATTRINFO_POLICYNAME;

/* KeyClass */
#define DAL_ATTRINFO_KEYCLASS { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "KeyClass" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_UINT32 \
}
static const CSSM_DB_ATTRIBUTE_INFO s_CssmDlDbAttrInfo_KeyClass = DAL_ATTRINFO_KEYCLASS;

/* Permanent */
#define DAL_ATTRINFO_PERMANENT { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "Permanent" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_UINT32 \
}
static const CSSM_DB_ATTRIBUTE_INFO s_CssmDlDbAttrInfo_Permanent = DAL_ATTRINFO_PERMANENT;

/* Private */
#define DAL_ATTRINFO_PRIVATE { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "Private" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_UINT32 \
}
static const CSSM_DB_ATTRIBUTE_INFO s_CssmDlDbAttrInfo_Private = DAL_ATTRINFO_PRIVATE;

/* Modifiable */
#define DAL_ATTRINFO_MODIFIABLE { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "Modifiable" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_UINT32 \
}
static const CSSM_DB_ATTRIBUTE_INFO s_CssmDlDbAttrInfo_Modifiable = DAL_ATTRINFO_MODIFIABLE;

/* Label */
#define DAL_ATTRINFO_LABEL { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "Label" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_BLOB \
}
static const CSSM_DB_ATTRIBUTE_INFO s_CssmDlDbAttrInfo_Label = DAL_ATTRINFO_LABEL;

/* ApplicationTag */
#define DAL_ATTRINFO_APPLICATIONTAG { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "ApplicationTag" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_BLOB \
}
static const CSSM_DB_ATTRIBUTE_INFO s_CssmDlDbAttrInfo_ApplicationTag = DAL_ATTRINFO_APPLICATIONTAG;

/* KeyCreator */
#define DAL_ATTRINFO_KEYCREATOR { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "KeyCreator" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_UINT32 \
}
static const CSSM_DB_ATTRIBUTE_INFO s_CssmDlDbAttrInfo_KeyCreator = DAL_ATTRINFO_KEYCREATOR;

/* KeyType */
#define DAL_ATTRINFO_KEYTYPE { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "KeyType" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_UINT32 \
}
static const CSSM_DB_ATTRIBUTE_INFO s_CssmDlDbAttrInfo_KeyType = DAL_ATTRINFO_KEYTYPE;

/* KeySizeInBits */
#define DAL_ATTRINFO_KEYSIZEINBITS { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "KeySizeInBits" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_UINT32 \
}
static const CSSM_DB_ATTRIBUTE_INFO s_CssmDlDbAttrInfo_KeySizeInBits = DAL_ATTRINFO_KEYSIZEINBITS;

/* EffectiveKeySize */
#define DAL_ATTRINFO_EFFECTIVEKEYSIZE { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "EffectiveKeySize" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_UINT32 \
}
static const CSSM_DB_ATTRIBUTE_INFO s_CssmDlDbAttrInfo_EffectiveKeySize = DAL_ATTRINFO_EFFECTIVEKEYSIZE;

/* StartDate */
#define DAL_ATTRINFO_STARTDATE { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "StartDate" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_TIME_DATE \
}
static const CSSM_DB_ATTRIBUTE_INFO s_CssmDlDbAttrInfo_StartDate = DAL_ATTRINFO_STARTDATE;

/* EndDate */
#define DAL_ATTRINFO_ENDDATE { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "EndDate" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_TIME_DATE \
}
static const CSSM_DB_ATTRIBUTE_INFO s_CssmDlDbAttrInfo_EndDate = DAL_ATTRINFO_ENDDATE;

/* Sensitive */
#define DAL_ATTRINFO_SENSITIVE { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "Sensitive" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_UINT32 \
}
static const CSSM_DB_ATTRIBUTE_INFO s_CssmDlDbAttrInfo_Sensitive = DAL_ATTRINFO_SENSITIVE;

/* AlwaysSensitive */
#define DAL_ATTRINFO_ALWAYSSENSITIVE { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "AlwaysSensitive" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_UINT32 \
}
static const CSSM_DB_ATTRIBUTE_INFO s_CssmDlDbAttrInfo_AlwaysSensitive = DAL_ATTRINFO_ALWAYSSENSITIVE;

/* Extractable */
#define DAL_ATTRINFO_EXTRACTABLE { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "Extractable" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_UINT32 \
}
static const CSSM_DB_ATTRIBUTE_INFO s_CssmDlDbAttrInfo_EXTRACTABLE = DAL_ATTRINFO_EXTRACTABLE;

/* NeverExtractable */
#define DAL_ATTRINFO_NEVEREXTRACTABLE { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "NeverExtractable" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_UINT32 \
}
static const CSSM_DB_ATTRIBUTE_INFO s_CssmDlDbAttrInfo_NeverExtractable = DAL_ATTRINFO_NEVEREXTRACTABLE;

/* Encrypt */
#define DAL_ATTRINFO_ENCRYPT { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "Encrypt" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_UINT32 \
}
static const CSSM_DB_ATTRIBUTE_INFO s_CssmDlDbAttrInfo_Encrypt = DAL_ATTRINFO_ENCRYPT;

/* Decrypt */
#define DAL_ATTRINFO_DECRYPT { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "Decrypt" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_UINT32 \
}
static const CSSM_DB_ATTRIBUTE_INFO s_CssmDlDbAttrInfo_Decrypt = DAL_ATTRINFO_DECRYPT;

/* Derive */
#define DAL_ATTRINFO_DERIVE { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "Derive" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_UINT32 \
}
static const CSSM_DB_ATTRIBUTE_INFO s_CssmDlDbAttrInfo_Derive = DAL_ATTRINFO_DERIVE;

/* Sign */
#define DAL_ATTRINFO_SIGN { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "Sign" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_UINT32 \
}
static const CSSM_DB_ATTRIBUTE_INFO s_CssmDlDbAttrInfo_Sign = DAL_ATTRINFO_SIGN;

/* Verify */
#define DAL_ATTRINFO_VERIFY { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "Verify" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_UINT32 \
}
static const CSSM_DB_ATTRIBUTE_INFO s_CssmDlDbAttrInfo_Verify = DAL_ATTRINFO_VERIFY;

/* SignRecover */
#define DAL_ATTRINFO_SIGNRECOVER { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "SignRecover" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_UINT32 \
}
static const CSSM_DB_ATTRIBUTE_INFO s_CssmDlDbAttrInfo_SignRecover = DAL_ATTRINFO_SIGNRECOVER;

/* VerifyRecover */
#define DAL_ATTRINFO_VERIFYRECOVER { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "VerifyRecover" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_UINT32 \
}
static const CSSM_DB_ATTRIBUTE_INFO s_CssmDlDbAttrInfo_VerifyRecover = DAL_ATTRINFO_VERIFYRECOVER;

/* Wrap */
#define DAL_ATTRINFO_WRAP { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "Wrap" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_UINT32 \
}
static const CSSM_DB_ATTRIBUTE_INFO s_CssmDlDbAttrInfo_Wrap = DAL_ATTRINFO_WRAP;

/* Unwrap */
#define DAL_ATTRINFO_UNWRAP { \
	CSSM_DB_ATTRIBUTE_NAME_AS_STRING, /* AttributeNameFormat */ \
	{ "Unwrap" }, /* AttributeName */ \
	CSSM_DB_ATTRIBUTE_FORMAT_UINT32 \
}
static const CSSM_DB_ATTRIBUTE_INFO s_CssmDlDbAttrInfo_Unwrap = DAL_ATTRINFO_UNWRAP;


/*-----------------------------------------------------------------------------
		Definitions of external Get Attributes and Indexes Functions
-----------------------------------------------------------------------------*/

extern void dal_CERT_GetAttributes(CSSM_DB_RECORD_ATTRIBUTE_INFO * pList);
extern void dal_CERT_GetIndexes(CSSM_DB_RECORD_INDEX_INFO * pList);

extern void dal_CRL_GetAttributes(CSSM_DB_RECORD_ATTRIBUTE_INFO * pList);
extern void dal_CRL_GetIndexes(CSSM_DB_RECORD_INDEX_INFO * pList);

extern void dal_POLICY_GetAttributes(CSSM_DB_RECORD_ATTRIBUTE_INFO * pList);
extern void dal_POLICY_GetIndexes(CSSM_DB_RECORD_INDEX_INFO * pList);

extern void dal_GENERIC_GetAttributes(CSSM_DB_RECORD_ATTRIBUTE_INFO * pList);
extern void dal_GENERIC_GetIndexes(CSSM_DB_RECORD_INDEX_INFO * pList);

extern void dal_PUBLIC_KEY_GetAttributes(CSSM_DB_RECORD_ATTRIBUTE_INFO * pList);
extern void dal_PUBLIC_KEY_GetIndexes(CSSM_DB_RECORD_INDEX_INFO * pList);

extern void dal_PRIVATE_KEY_GetAttributes(CSSM_DB_RECORD_ATTRIBUTE_INFO * pList);
extern void dal_PRIVATE_KEY_GetIndexes(CSSM_DB_RECORD_INDEX_INFO * pList);

extern void dal_SYMMETRIC_KEY_GetAttributes(CSSM_DB_RECORD_ATTRIBUTE_INFO * pList);
extern void dal_SYMMETRIC_KEY_GetIndexes(CSSM_DB_RECORD_INDEX_INFO * pList);

extern void dal_ALL_KEYS_GetAttributes(CSSM_DB_RECORD_ATTRIBUTE_INFO * pList);
extern void dal_ALL_KEYS_GetIndexes(CSSM_DB_RECORD_INDEX_INFO * pList);

extern CSSM_RETURN dal_ReplaceWithDefaultSchema(CSSM_DB_RECORD_ATTRIBUTE_INFO * AttributeInfoPtr,
												CSSM_DB_RECORD_INDEX_INFO * IndexInfoPtr);

#endif
