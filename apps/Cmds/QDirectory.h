#ifndef __QDIR_H__
#define __QDIR_H__

#include <qstring.h>
#include <mds.h>

class QRecType;

class QDirectory
{
private:

  friend class QMDS;
  friend class QRecType;

public:

  QString m_strDirName;
  QMDS *m_pMds;
  MDS_DB_HANDLE m_hDb;
  
  CSSM_RETURN Open( QMDS *pMds, const char *szDirName );
  QDirectory(QMDS * pMds, const char * szDirName);
  virtual ~QDirectory();

#ifdef _DEBUG
  virtual void AssertValid() const;
#endif

protected:
  CSSM_RETURN OpenDirectory();
  CSSM_RETURN CloseDirectory();
};

#endif // __QDIR_H__
