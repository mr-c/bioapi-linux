lib_LTLIBRARIES = libbioapi100.la

libbioapi100_la_SOURCES =	addmgr.c \
				addutil.c \
				bioapi_api.c \
				manage_interface.c \
				spi_callbacks.c	\
				internal.h \
				precomp.h

libbioapi100_la_LIBADD =	../mds_util/libmds_util.la \
				../mds_util_api/libmds_util_api.la \
				install/libinstall.la \
				../../addins/dl/mds/libbioapi_mds300.la \
				../bioapi_util/libbioapi_util.la \
				../port/libport.la

AM_CPPFLAGS =	-I${top_srcdir}/include \
		-I${top_srcdir}/imports/cdsa/v2_0/inc/ 
