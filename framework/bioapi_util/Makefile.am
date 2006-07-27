noinst_LTLIBRARIES = libbioapi_util.la

libbioapi_util_la_SOURCES =	bsp_install.c \
				bsp_schema.c \
				convert.c \
				device_schema.c \
				memfuncs.c

libbioapi_util_la_LIBADD = ../mds_util/libmds_util.la

CPPFLAGS =	-I${top_srcdir}/include \
		-I${top_srcdir}/imports/cdsa/v2_0/inc/ \
		-I${top_srcdir}/framework/mds_util/

