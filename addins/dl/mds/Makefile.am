lib_LTLIBRARIES=libbioapi_mds300.la

libbioapi_mds300_la_SOURCES=	dal_database.cpp \
				dal_database_list.cpp \
				dal_internal.cpp \
				dal_preparefield.cpp \
				dal_recordtable.cpp \
				dal_relation.cpp \
				dal_src.cpp \
				dal_translation.cpp \
				ff_data.c \
				ff_freelist.c \
				ff_func.cpp \
				ff_index.c \
				ff_port.c \
				ff_table.cpp \
				ff_util.c \
				maf_collectn.c \
				mds.cpp \
				mds_create.cpp \
				mds_emulate.cpp \
				mds_init_fini.c \
				dl_file.c \
				dl_fileread.c \
				dl_filewrite.c \
				dl_isbad.c \
				dl_memutil.c \
				dl_portreg.c \
				dl_reg.c \
				dlutil.h \
				dlutil_reg.h \
				ff_port.h \
				ff_data.h \
				ff_freelist.h \
				ff_index.h \
				ff_util.h \
				maf_collectn.h \
				maf_util.h \
				ff_port.h \
				ff_common.h \
				maf_include.h \
				dal_classes.h \
				dal_internal.h \
				dal_src.h \
				maf_config.h \
				maf_interface.h \
				dal_translation.h \
				dal_schema.h \
				flatfile.h \
				mds_internal.h \
				dal_dlbe.h \
				dal_common.h \
				defines.h \
				dal_typearray.h \
				dlutil_struct.h \
				maf_callouts.h

EXTRA_DIST = libbioapi_mds300.ver

libbioapi_mds300_la_LIBADD =	../../../framework/port/libport.la \
				../../../framework/bioapi_util/libbioapi_util.la

libbioapi_mds300_la_LDFLAGS = -Wl,--version-script=${srcdir}/libbioapi_mds300.ver

AM_CPPFLAGS =	-I${top_srcdir}/include/ \
		-I${top_srcdir}/imports/cdsa/v2_0/inc/ \
		-DSTRONG_TYPES \
		-DMODULE_DIRECTORY_SERVICES
