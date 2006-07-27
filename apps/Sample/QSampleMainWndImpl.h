#ifndef __QSAMPLEMAINWNDIMPL_H__
#define __QSAMPLEMAINWNDIMPL_H__

#include "mainwindow.h"

#include "bioapi.h"
#include "bioapi_schema.h"
#include "port/bioapi_port.h"
#include "bioapi_uuid.h"
#include "bioapi_util.h"

class QSampleMainWndImpl : public QSampleMainWnd
{
private:

  BioAPI_HANDLE           m_moduleHandle;
  BioAPI_BSP_SCHEMA      *m_bspSchemaArray;
  unsigned int            m_arraySize;
  static BioAPI_UUID_PTR  s_uuid;
  BioAPI_BSP_SCHEMA      *m_currSchema;

  void PrintErrorCode(BioAPI_RETURN bioReturn);

  BioAPI_RETURN OutputToFile(QString lptszUserName, BioAPI_BIR_HANDLE BirHandle);
  BioAPI_RETURN InputFromFile(QString lptszUserName, BioAPI_INPUT_BIR * InputBir);

public:

  QSampleMainWndImpl();

  int initializeBioAPI();

  virtual void slotVerifyButtonPressed();
  virtual void slotEnrollButtonPressed();
  virtual void slotSelectedBSPChanged(int);
};

#endif
