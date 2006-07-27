noinst_LTLIBRARIES = libport.la

libport_la_SOURCES = 	filelock.c \
			portdll.c \
			port.c \
			portfile.c \
			portinit.c \
			portmem.c \
			portmemm.c \
			portmutx.c \
			portsock.c \
			portswmr.c \
			threadlocal.c \
			threads.c \
			internal.h \
			port_codefile.h

CPPFLAGS =	-I${top_srcdir}/imports/cdsa/v2_0/inc/ \
		-I${top_srcdir}/imports/cdsa/v2_0/inc/port/ \
		-I${top_srcdir}/imports/cdsa/v2_0/inc/intel/ \
		-DELF_FORMAT \
		-DMDS_PATH=\"${MDS_PATH}\"

