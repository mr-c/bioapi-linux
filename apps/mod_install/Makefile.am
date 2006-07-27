bin_PROGRAMS = mod_install

mod_install_SOURCES =	install.c \
			cmdline.c \
			cmdline.h

mod_install_LDADD = ../../framework/port/libport.la

CPPFLAGS =	-I${top_srcdir}/include/ \
		-I${top_srcdir}/imports/cdsa/v2_0/inc/ \
		-I${top_srcdir}/framework/mds_util/ \
		-DLIB_IPATH=\"${libdir}/\"
