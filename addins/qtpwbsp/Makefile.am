lib_LTLIBRARIES = libqtpwbsp.la

BUILT_SOURCES = moc_qpwbspdialog.cpp

libqtpwbsp_la_SOURCES =	install.c \
			pwbspi.c \
			mod_manage.c \
			maf_collectn.h \
			maf_config.h \
			maf_interface.h \
			maf_util.h \
			precomp.h \
			qpwbspdialog.h \
			qpwbspdialog.cpp \
			maf_collectn.c \
			maf_dllmain.c \
			maf_include.h \
			maf_callouts.h

EXTRA_DIST =	libqtpwbsp.ver \
		moc_qpwbspdialog.cpp

libqtpwbsp_la_LIBADD =	../../framework/port/libport.la \
			../../framework/bioapi_util/libbioapi_util.la \
			../dl/mds/libbioapi_mds300.la \
			../../framework/mds_util/libmds_util.la \
			../../framework/mds_util_api/libmds_util_api.la

AM_LDFLAGS =	-Wl,--version-script=${srcdir}/libqtpwbsp.ver \
		${QT_LIBS}

AM_CPPFLAGS =	-I${top_srcdir}/include \
		-I${top_srcdir}/imports/cdsa/v2_0/inc \
		${QT_CXXFLAGS}

%.h: %.ui
	${QT_UIC} $< -o $@

%.cpp: %.ui
	${QT_UIC} -impl $*.h $< -o $@

moc_%.cpp: %.h
	${QT_MOC} $< -o $@
