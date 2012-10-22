AC_INIT([[BioAPI 1.10 Framework Reference Implementation]],[[1.2.4]],[[michael.crusoe@gmail.com]],[[bioapi]])
AC_PREREQ([2.67])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR(framework/h_layer/bioapi_api.c)
AM_INIT_AUTOMAKE([foreign dist-bzip2 no-installinfo no-installman])
AC_SUBST(VERSION)
AC_C_BIGENDIAN([AC_DEFINE(BIG_ENDIAN)],[AC_DEFINE(LITTLE_ENDIAN)])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_LEX
AC_PROG_INSTALL
AC_PROG_YACC
LT_INIT

# Checks for libraries.
# FIXME: Replace `main' with a function in `-ldl':
AC_CHECK_LIB([dl], [main])
# FIXME: Replace `main' with a function in `-lpthread':
AC_CHECK_LIB([pthread], [main])

# Checks for header files.
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS([fcntl.h float.h limits.h malloc.h stdlib.h string.h strings.h sys/file.h sys/time.h syslog.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_MODE_T
AC_TYPE_OFF_T
AC_TYPE_SIZE_T
AC_HEADER_TIME

# Checks for library functions.
AC_FUNC_CLOSEDIR_VOID
AC_REPLACE_FNMATCH
AC_FUNC_MALLOC
AC_FUNC_MEMCMP
AC_FUNC_MMAP
AC_FUNC_REALLOC
AC_FUNC_STAT
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([bzero ftruncate getcwd gettimeofday memchr memmove memset mkdir munmap strcspn strrchr strstr])

AC_DEFINE(UNIX)
AC_DEFINE(POSIX)
AC_DEFINE(LINUX)
AC_DEFINE(_GNU_SOURCE)
AC_SUBST(MDS_PATH,${localstatedir}/bioapi)

AX_HAVE_QT

AM_CONDITIONAL(GUI_OPT, test ${have_qt}=yes)

AC_CONFIG_FILES([Makefile
		addins/bioapi_dummy_addin/Makefile
		addins/dl/mds/Makefile
		addins/dl/mds_install/Makefile
		addins/pwbsp/Makefile
		addins/qtpwbsp/Makefile
		apps/Cmds/Makefile
		apps/NonGUI_Sample/Makefile
		apps/Test/Makefile
		apps/mod_install/Makefile
		apps/Sample/Makefile
		apps/MdsEdit/Makefile
		framework/bioapi_util/Makefile
		framework/h_layer/install/Makefile
		framework/h_layer/Makefile
		framework/mds_util/Makefile
		framework/mds_util_api/Makefile
		framework/port/Makefile
		include/Makefile
		imports/cdsa/v2_0/inc/Makefile])
AC_OUTPUT

