#include "stdafx.h"
#include "QMDS.h"
#include "QDirectory.h"
#include "AppMemTrack.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

QDirectory::QDirectory(QMDS * pMds, const char * szDirName) :
	m_pMds( pMds),
	m_strDirName( szDirName )
{
	memset( &m_hDb, 0, sizeof(m_hDb) );
	OpenDirectory();
}

QDirectory::~QDirectory()
{
	CloseDirectory();
}

CSSM_RETURN QDirectory::OpenDirectory()
{
  ASSERTVALID();


  CSSM_DB_ACCESS_TYPE access =
    CSSM_DB_ACCESS_READ | CSSM_DB_ACCESS_WRITE;

  m_hDb.DLHandle = m_pMds->m_hMds;
  m_hDb.DBHandle = 0;
  
  CSSM_RETURN ret = m_pMds->DbOpen(m_strDirName,
				   NULL,
				   access,
				   NULL,
				   NULL,
				   &m_hDb.DBHandle);
  
  return ret;
}

CSSM_RETURN QDirectory::CloseDirectory()
{
  ASSERTVALID();

  CSSM_RETURN crStatus = m_pMds->DbClose( m_hDb );

  if ( crStatus == CSSM_OK )
  {
    memset( &m_hDb, 0, sizeof(MDS_DB_HANDLE) );
  }

  return crStatus;
}

CSSM_RETURN QDirectory::Open(QMDS *pMds, const char *szDirName)
{
  ASSERTVALID();

  m_pMds = pMds;
  m_strDirName = szDirName;
  return OpenDirectory();
}

#ifdef _DEBUG
void QDirectory::AssertValid() const
{
  //	CObject::ASSERTVALID();
  //	ASSERT( m_pMds );
}
#endif
