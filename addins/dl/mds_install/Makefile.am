bin_PROGRAMS = mds_install

mds_install_SOURCES =	dl_portreg.c \
			ins_mds.c \
			mds_install.c \
			dlutil_reg.h \
			ins_mds.h

mds_install_LDADD =	../../../framework/port/libport.la \
			../mds/libbioapi_mds300.la \
			../../../framework/bioapi_util/libbioapi_util.la

AM_CPPFLAGS =	-I${top_srcdir}/imports/cdsa/v2_0/inc/ \
		-I${top_srcdir}/include/ -DLIB_IPATH=\"${libdir}/\"
