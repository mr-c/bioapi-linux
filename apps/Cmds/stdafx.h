// Some 'Windows' typedefs and macros

typedef unsigned int DWORD;
typedef bool BOOL;
typedef const char *LPCTSTR;

#define _T(X) X
#define __min(X, Y) ((X < Y) ? X : Y)

// Qt includes
#include <qstring.h>

// Other includes

#include "cssmtype.h"
#include "mds.h"
#include "bioapi.h"
#include "bsp_schema.h"
#include "device_schema.h"

#include "cssmerr.h"
#include "mds_schema.h"
#include "QMDSUtil.h"
#include "QRecType.h"

#ifdef _DEBUG
#define ASSERTVALID()	AssertValid()
#else
#define ASSERTVALID()
#endif
