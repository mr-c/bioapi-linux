noinst_LTLIBRARIES = libCMDS.la

libCMDS_la_SOURCES =	CdsaRec.cpp \
			QMDS.cpp \
			QMDSUtil.cpp \
			QDirectory.cpp \
			QRec.cpp \
			QRecType.cpp \
			stdafx.h \
			CdsaRec.h \
			CdsaRecType.h \
			AppMemTrack.h \
			QMDS.h \
			QMDSUtil.h \
			QDirectory.h \
			QRec.h \
			QRecType.h \
			MDSRecType.h

libCMDS_la_LIBADD =	${QT_LIBS} \
			../../addins/dl/mds/libbioapi_mds300.la

CPPFLAGS =      -I${top_srcdir}/include \
                -I${top_srcdir}/imports/cdsa/v2_0/inc \
                ${QT_CXXFLAGS}

