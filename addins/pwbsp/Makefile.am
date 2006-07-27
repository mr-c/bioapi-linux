lib_LTLIBRARIES = libpwbsp.la

libpwbsp_la_SOURCES =	install.c \
			pwbspi.c \
			mod_manage.c \
			maf_collectn.c \
			maf_dllmain.c \
			maf_config.h \
			maf_include.h \
			maf_collectn.h \
			maf_util.h \
			maf_interface.h \
			maf_callouts.h \
			precomp.h

EXTRA_DIST = libpwbsp.ver

libpwbsp_la_LIBADD =	../../framework/port/libport.la \
			../../framework/bioapi_util/libbioapi_util.la \
			../dl/mds/libbioapi_mds300.la \
			../../framework/mds_util/libmds_util.la \
			../../framework/mds_util_api/libmds_util_api.la

LDFLAGS =	-Wl,--version-script=${srcdir}/libpwbsp.ver

CPPFLAGS =	-I${top_srcdir}/include/ \
		-I${top_srcdir}/imports/cdsa/v2_0/inc/
