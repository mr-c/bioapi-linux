#include "stdafx.h"
#include "CdsaRec.h"
#include "CdsaRecType.h"
#include "AppMemTrack.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
//////////////////////////////////////////////////////////////////////

template <> void QCdsaRecType< CCssmRecInfoType >::SpecifyAttributes()
{
  m_outputAttributeData[CCssmRecInfoType::IDX_MODULEID].Info     = s_BioApiAttrInfo_ModuleId;
  m_outputAttributeData[CCssmRecInfoType::IDX_MODULENAME].Info	 = s_BioApiAttrInfo_ModuleName;
  m_outputAttributeData[CCssmRecInfoType::IDX_PROD_VERSION].Info = s_BioApiAttrInfo_ProductVersion;
  m_outputAttributeData[CCssmRecInfoType::IDX_SPEC_VERSION].Info = s_BioApiAttrInfo_SpecVersion;
  m_outputAttributeData[CCssmRecInfoType::IDX_VENDOR].Info	 = s_BioApiAttrInfo_Vendor;
  m_outputAttributeData[CCssmRecInfoType::IDX_DESC].Info         = s_BioApiAttrInfo_Description;
}

template <> void QCdsaRecType< CEmmRecInfoType >::SpecifyAttributes()
{
  m_outputAttributeData[CEmmRecInfoType::IDX_MODULEID].Info	= s_BioApiAttrInfo_ModuleId;
  m_outputAttributeData[CEmmRecInfoType::IDX_DEVICEID].Info	= s_BioApiAttrInfo_DeviceId;
  m_outputAttributeData[CEmmRecInfoType::IDX_BSPNAME].Info	= s_BioApiAttrInfo_BspName;
  m_outputAttributeData[CEmmRecInfoType::IDX_PROD_VERSION].Info	= s_BioApiAttrInfo_ProductVersion;
  m_outputAttributeData[CEmmRecInfoType::IDX_SPEC_VERSION].Info	= s_BioApiAttrInfo_SpecVersion;
  m_outputAttributeData[CEmmRecInfoType::IDX_VENDOR].Info	= s_BioApiAttrInfo_Vendor;
  
  m_outputAttributeData[CEmmRecInfoType::IDX_SUPPFMT].Info	= s_BioApiAttrInfo_BspSupportedFormats;
  m_outputAttributeData[CEmmRecInfoType::IDX_FACTORMASK].Info	= s_BioApiAttrInfo_FactorsMask;
  m_outputAttributeData[CEmmRecInfoType::IDX_OPERATIONS].Info	= s_BioApiAttrInfo_Operations;
  m_outputAttributeData[CEmmRecInfoType::IDX_OPTIONS].Info	= s_BioApiAttrInfo_Options;
  m_outputAttributeData[CEmmRecInfoType::IDX_PAYLOADPOLICY].Info= s_BioApiAttrInfo_PayloadPolicy;
  m_outputAttributeData[CEmmRecInfoType::IDX_MAXPAYLOADSZ].Info	= s_BioApiAttrInfo_MaxPayloadSize;
  m_outputAttributeData[CEmmRecInfoType::IDX_VERIFYTMOUT].Info	= s_BioApiAttrInfo_DefaultVerifyTimeout;
  m_outputAttributeData[CEmmRecInfoType::IDX_IDENTIFYIMOUT].Info= s_BioApiAttrInfo_DefaultIdentifyTimeout;
  m_outputAttributeData[CEmmRecInfoType::IDX_DEFCAPTURE].Info	= s_BioApiAttrInfo_DefaultCaptureTimeout;
  m_outputAttributeData[CEmmRecInfoType::IDX_ENROLLTMOUT].Info	= s_BioApiAttrInfo_DefaultEnrollTimeout;
  m_outputAttributeData[CEmmRecInfoType::IDX_MAXBSPDBSZ].Info	= s_BioApiAttrInfo_MaxBspDbSize;
  m_outputAttributeData[CEmmRecInfoType::IDX_MAXIDENTIFY].Info	= s_BioApiAttrInfo_MaxIdentify;
  m_outputAttributeData[CEmmRecInfoType::IDX_DESCRIPTION].Info	= s_BioApiAttrInfo_Description;
  m_outputAttributeData[CEmmRecInfoType::IDX_PATH].Info		= s_BioApiAttrInfo_Path;
}



template <> void QCdsaRecType< CBioAPI_DeviceInfoType >::SpecifyAttributes()
{
  m_outputAttributeData[CBioAPI_DeviceInfoType::IDX_MODULE_ID].Info		= s_BioApiAttrInfo_ModuleId;
  m_outputAttributeData[CBioAPI_DeviceInfoType::IDX_DEVICE_ID].Info		= s_BioApiAttrInfo_DeviceId;
  m_outputAttributeData[CBioAPI_DeviceInfoType::IDX_SUPPORTEDFORMATS].Info	= s_BioApiAttrInfo_DeviceSupportedFormats;
  m_outputAttributeData[CBioAPI_DeviceInfoType::IDX_SUPPORTEDEVENTS].Info	= s_BioApiAttrInfo_SupportedEvents;
  m_outputAttributeData[CBioAPI_DeviceInfoType::IDX_DEVICEVENDOR].Info		= s_BioApiAttrInfo_DeviceVendor;
  m_outputAttributeData[CBioAPI_DeviceInfoType::IDX_DEVICEDESCRIPTION].Info	= s_BioApiAttrInfo_DeviceDescription;
  m_outputAttributeData[CBioAPI_DeviceInfoType::IDX_DEVICESERALNUMBER].Info	= s_BioApiAttrInfo_DeviceSerialNumber;
  m_outputAttributeData[CBioAPI_DeviceInfoType::IDX_DEVICEHWVERSION].Info	= s_BioApiAttrInfo_DeviceHardwareVersion;
  m_outputAttributeData[CBioAPI_DeviceInfoType::IDX_DEVICEFIRMWAREVERSION].Info	= s_BioApiAttrInfo_DeviceFirmwareVersion;
  m_outputAttributeData[CBioAPI_DeviceInfoType::IDX_AUTHENTICATEDDEVICE].Info	= s_BioApiAttrInfo_AuthenticatedDevice;
}
