lib_LTLIBRARIES = libbioapi_dummy100.la

libbioapi_dummy100_la_SOURCES = install.c \
				bioapi_dummy_addini.c \
				mod_manage.c \
				maf_collectn.c \
				maf_dllmain.c \
				precomp.h \
				maf_config.h \
				bioapi_dummy_addini_err.h \
				maf_collectn.h \
				maf_interface.h \
				maf_util.h \
				maf_include.h \
				maf_callouts.h

EXTRA_DIST = libbioapi_dummy100.ver

libbioapi_dummy100_la_LIBADD = 	../../framework/port/libport.la \
				../../framework/bioapi_util/libbioapi_util.la \
				../dl/mds/libbioapi_mds300.la \
				../../framework/mds_util/libmds_util.la \
				../../framework/mds_util_api/libmds_util_api.la
				
libbioapi_dummy100_la_LDFLAGS =	-Wl,--version-script=${srcdir}/libbioapi_dummy100.ver \
				-XCClinker -nostartfiles

CPPFLAGS =	-I${top_srcdir}/include/ \
		-I${top_srcdir}/imports/cdsa/v2_0/inc/
