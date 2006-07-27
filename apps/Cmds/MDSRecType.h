/*-----------------------------------------------------------------------
 * File: MDSRECTYPE.H
 *-----------------------------------------------------------------------
 */


#ifndef MDSRECTYPE_H
#define MDSRECTYPE_H

#include "bioapi_schema.h"

class QBaseRecInfoType 
{
public:

	QBaseRecInfoType( LPCTSTR szName, unsigned int dwRecType, unsigned int dwNumAttr ) :
	  m_strName( szName ),
	  m_dwRecType( dwRecType ),
	  m_dwNumAttrs( dwNumAttr )
	{
	}

	QString m_strName;
	CSSM_DB_RECORDTYPE m_dwRecType;
	unsigned int m_dwNumAttrs;
};

class CCssmRecInfoType : public QBaseRecInfoType
{
public:
  enum
  {
    IDX_MODULEID,
    IDX_MODULENAME,
    IDX_PROD_VERSION,
    IDX_SPEC_VERSION,
    IDX_VENDOR,
    IDX_DESC
  };

  CCssmRecInfoType() :
    QBaseRecInfoType("BIOAPI_H_LAYER_RECORDTYPE",
		     BIOAPI_H_LAYER_RECORDTYPE,
		     BIOAPI_H_LAYER_NUM_ATTRIBUTES)
  {
  }
};


class CEmmRecInfoType : public QBaseRecInfoType
{
public:
  enum
  {
    IDX_MODULEID,
    IDX_DEVICEID,
    IDX_BSPNAME,
    IDX_PROD_VERSION,
    IDX_SPEC_VERSION,
    IDX_VENDOR,
    IDX_SUPPFMT,
    IDX_FACTORMASK,
    IDX_OPERATIONS,
    IDX_OPTIONS,
    IDX_PAYLOADPOLICY,
    IDX_MAXPAYLOADSZ,
    IDX_VERIFYTMOUT,
    IDX_IDENTIFYIMOUT,
    IDX_DEFCAPTURE,
    IDX_ENROLLTMOUT,
    IDX_MAXBSPDBSZ,
    IDX_MAXIDENTIFY,
    IDX_DESCRIPTION,
    IDX_PATH
  };

  CEmmRecInfoType() :
    QBaseRecInfoType("BIOAPI_BSP_RECORDTYPE",
		     BIOAPI_BSP_RECORDTYPE,
		     BIOAPI_BSP_NUM_ATTRIBUTES)
  {
  }
};


class CBioAPI_DeviceInfoType: public QBaseRecInfoType
{
public:
  enum
  {
    IDX_MODULE_ID,
    IDX_DEVICE_ID,
    IDX_SUPPORTEDFORMATS,
    IDX_SUPPORTEDEVENTS,
    IDX_DEVICEVENDOR,
    IDX_DEVICEDESCRIPTION,
    IDX_DEVICESERALNUMBER,
    IDX_DEVICEHWVERSION,
    IDX_DEVICEFIRMWAREVERSION,
    IDX_AUTHENTICATEDDEVICE
  };

  CBioAPI_DeviceInfoType() :
    QBaseRecInfoType("BIOAPI_BIO_DEVICE_RECORDTYPE",
		     BIOAPI_BIO_DEVICE_RECORDTYPE,
		     BIOAPI_BIO_DEVICE_NUM_ATTRIBUTES)
  {
  }
};



#endif
