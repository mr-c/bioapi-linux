#include "stdafx.h"
#include "MdsEditDoc.h"
#include "QMDS.h"
#include "AppMemTrack.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

QMdsEditDoc::QMdsEditDoc()
{
  m_pMds = new QMDS(NULL);
}

QMdsEditDoc::~QMdsEditDoc()
{
  delete m_pMds;
}
