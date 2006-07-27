#ifndef __MDSEDITDOC_H__
#define __MDSEDITDOC_H__

#include <QMDS.h>

class QMdsEditDoc
{
public:

  QMdsEditDoc();
  virtual ~QMdsEditDoc();

  QMDS *m_pMds;

#ifdef _DEBUG
  virtual void AssertValid() const;
#endif
};

#endif // __MDSEDITDOC_H__
