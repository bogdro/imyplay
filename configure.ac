#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(imyplay, 0.2, bogdandr@op.pl)
AM_INIT_AUTOMAKE
AC_CONFIG_FILES([Makefile src/Makefile doc/imyplay.txi:doc/imyplay.txc \
	po/Makefile.in:po/Makefile.inn imyplay.spc:imyplay.spi \
	src/imyp_cfg.h:src/imyp_cfg.hin doc/Makefile po/Makefile])
AC_CONFIG_SRCDIR([src/imyplay.c])
AC_CONFIG_HEADER([config.h:config.hin])

# ==================== configure arguments

#AC_ARG_ENABLE(ALLEGRO,[[  --enable-allegro	Use the Allegro library (default).]],
#	[if (test "x$enableval" = "xno"); then
#		feat_allegro=no
#	 else
#		feat_allegro=yes
#	 fi
#	]
#	,feat_allegro=yes)

# ==================== Set of available languages (deprecated).
#ALL_LINGUAS="pl"

# ==================== Checks for programs.
AC_PROG_CC
AC_PROG_RANLIB
AM_CONDITIONAL(ISGCC, [test "x$GCC" = "xyes"])

# ==================== Checks for libraries.

#liballeg=yes
#AC_CHECK_LIB([`allegro-config --libs`],[allegro_init],
#	[AC_DEFINE([HAVE_LIBALLEG],[1],[Whether you have the Allegro library.])]
#	,[liballeg=no])
#AC_CHECK_PROG([allegro-config], [liballeg], [yes], [no])

#if (test "x$liballeg" != "xyes" ); then
	AC_CHECK_LIB([alleg],[allegro_init],[liballeg=yes],[liballeg=no])
#else
#	AC_SUBST(AM_LDFLAGS,"$LDFLAGS `allegro-config --libs`")
#fi

alleg_ldflags=`allegro-config --libs`

if (test "x$liballeg" == "xyes" || test "x$alleg_ldflags" != "x" ); then
	liballeg=yes
	AC_DEFINE([HAVE_LIBALLEG],[1],[Whether you have the Allegro library.])
#else
#	AC_MSG_ERROR([[I need the Allegro library to work. See http://alleg.sf.net.]])
fi

if ( test "x$alleg_ldflags" != "x" ); then
	# do NOT substitute "LIBS" here! Allegro libraries would make all tests fail
	# because of missing _mangled_main_address symbol.
	AC_SUBST(AM_LDFLAGS,"$LDFLAGS $alleg_ldflags")
fi

libmath=yes
AC_CHECK_LIB([m],[sin],,[libmath=no])

if (test "x$libmath" != "xyes" ); then
	AC_CHECK_FUNCS([sin])
fi

# ==================== Checks for header files.
AC_HEADER_STDC

AC_CHECK_HEADER([string.h],AC_DEFINE([HAVE_STRING_H],[1],[Whether you have the string.h header.]),
	[AC_MSG_ERROR([[I need string.h to work.]])]
	)

AC_CHECK_HEADERS([getopt.h malloc.h sys/types.h stdlib.h locale.h math.h\
	signal.h errno.h libgen.h allegro.h])

# ===================== Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T
AC_C_VOLATILE
AC_CHECK_TYPES([off_t size_t],,,
	[
	#ifdef HAVE_SYS_TYPES_H
	#include <sys/types.h>
	#endif
	#include <stdio.h>
	])
AC_CHECK_TYPES([sig_atomic_t],,,
	[
	#ifdef HAVE_SIGNAL_H
	#include <signal.h>
	#endif
	])

#have_ll=yes
#AC_CHECK_TYPE([long long],AC_DEFINE([HAVE_LONG_LONG],[1],[Whether you have the long long type.]),
#	[have_ll=no])

# Checks for library functions.
AC_TYPE_SIGNAL
AC_FUNC_MALLOC

AC_CHECK_FUNCS([malloc memcpy setlocale sigaction fseeko ftello snprintf basename])

has_getopt=yes
AC_CHECK_FUNC([getopt_long],
	AC_DEFINE([HAVE_GETOPT_LONG],[1],[Whether you have the getopt_long function.]),
	[has_getopt=no
	AC_MSG_ERROR([[I need the getopt_long() function.]])])

AM_GNU_GETTEXT_VERSION([0.14.3])
AM_GNU_GETTEXT([external])

# Can be /usr/local/include/libintl.h. Use --with-libintl-prefix=DIR then.
# The macros above set the correct CFLAGS if this option is given.

AC_CHECK_HEADERS([libintl.h])

if (test "x$GCC" = "xyes" ); then

	AX_C_CHECK_FLAG([-Wall], [CFLAGS="$CFLAGS -Wall"], [CFLAGS="$CFLAGS"])
	AX_C_CHECK_FLAG([-Wextra], [CFLAGS="$CFLAGS -Wextra"], [CFLAGS="$CFLAGS"])
	AX_C_CHECK_FLAG([-W], [CFLAGS="$CFLAGS -W"], [CFLAGS="$CFLAGS"])
	AX_C_CHECK_FLAG([-Wfloat-equal], [CFLAGS="$CFLAGS -Wfloat-equal"], [CFLAGS="$CFLAGS"])
	AX_C_CHECK_FLAG([-Wbad-function-cast], [CFLAGS="$CFLAGS -Wbad-function-cast"], [CFLAGS="$CFLAGS"])
	AX_C_CHECK_FLAG([-Wsign-compare], [CFLAGS="$CFLAGS -Wsign-compare"], [CFLAGS="$CFLAGS"])
	AX_C_CHECK_FLAG([-Wunreachable-code], [CFLAGS="$CFLAGS -Wunreachable-code"], [CFLAGS="$CFLAGS"])
	AX_C_CHECK_FLAG([-Wpointer-arith], [CFLAGS="$CFLAGS -Wpointer-arith"], [CFLAGS="$CFLAGS"])
	AX_C_CHECK_FLAG([-Wcast-qual], [CFLAGS="$CFLAGS -Wcast-qual"], [CFLAGS="$CFLAGS"])
	AX_C_CHECK_FLAG([-Wcast-align], [CFLAGS="$CFLAGS -Wcast-align"], [CFLAGS="$CFLAGS"])
	AX_C_CHECK_FLAG([-Wstrict-prototypes], [CFLAGS="$CFLAGS -Wstrict-prototypes"], [CFLAGS="$CFLAGS"])
	AX_C_CHECK_FLAG([-Wformat -Wformat-security], [CFLAGS="$CFLAGS -Wformat -Wformat-security"], [CFLAGS="$CFLAGS"])
	AX_C_CHECK_FLAG([-Wformat -Wformat-nonliteral], [CFLAGS="$CFLAGS -Wformat -Wformat-nonliteral"], [CFLAGS="$CFLAGS"])
	AX_C_CHECK_FLAG([-Wnested-externs], [CFLAGS="$CFLAGS -Wnested-externs"], [CFLAGS="$CFLAGS"])
	AX_C_CHECK_FLAG([-Wshadow], [CFLAGS="$CFLAGS -Wshadow"], [CFLAGS="$CFLAGS"])
	AX_C_CHECK_FLAG([-Wconversion], [CFLAGS="$CFLAGS -Wconversion"], [CFLAGS="$CFLAGS"])
	AX_C_CHECK_FLAG([-Wdeclaration-after-statement], [CFLAGS="$CFLAGS -Wdeclaration-after-statement"], [CFLAGS="$CFLAGS"])
	AX_C_CHECK_FLAG([-Wundef], [CFLAGS="$CFLAGS -Wundef"], [CFLAGS="$CFLAGS"])
	AX_C_CHECK_FLAG([-Wpadded], [CFLAGS="$CFLAGS -Wpadded"], [CFLAGS="$CFLAGS"])
	AX_C_CHECK_FLAG([-Wredundant-decls], [CFLAGS="$CFLAGS -Wredundant-decls"], [CFLAGS="$CFLAGS"])
	AX_C_CHECK_FLAG([-Wfatal-errors], [CFLAGS="$CFLAGS -Wfatal-errors"], [CFLAGS="$CFLAGS"])
	AX_C_CHECK_FLAG([-pedantic], [CFLAGS="$CFLAGS -pedantic"], [CFLAGS="$CFLAGS"])
	AX_C_CHECK_FLAG([-Wno-strict-aliasing], [CFLAGS="$CFLAGS -Wno-strict-aliasing"], [CFLAGS="$CFLAGS"])
	AX_C_CHECK_FLAG([-Wmissing-prototypes], [CFLAGS="$CFLAGS -Wmissing-prototypes"], [CFLAGS="$CFLAGS"])
	AX_C_CHECK_FLAG([-Wmissing-declarations], [CFLAGS="$CFLAGS -Wmissing-declarations"], [CFLAGS="$CFLAGS"])
	AX_C_CHECK_FLAG([-O1 -Wuninitialized], [CFLAGS="$CFLAGS -O1 -Wuninitialized"], [CFLAGS="$CFLAGS"])
	AX_C_CHECK_FLAG([-Winit-self], [CFLAGS="$CFLAGS -Winit-self"], [CFLAGS="$CFLAGS"])
	AX_C_CHECK_FLAG([-Wlogical-op], [CFLAGS="$CFLAGS -Wlogical-op"], [CFLAGS="$CFLAGS"])
	AX_C_CHECK_FLAG([-fstack-protector-all], [CFLAGS="$CFLAGS -fstack-protector-all"], [CFLAGS="$CFLAGS"])
	AX_C_CHECK_FLAG([-O2], [CFLAGS="$CFLAGS -O2"], [CFLAGS="$CFLAGS"])

#	if (test "x$have_ll" = "xyes"); then
#
#		AX_C_CHECK_FLAG([-std=gnu99], [CFLAGS="$CFLAGS -std=gnu99"], [CFLAGS="$CFLAGS"])
#		AX_C_CHECK_FLAG([-Wno-long-long], [CFLAGS="$CFLAGS -Wno-long-long"], [CFLAGS="$CFLAGS"])
#	fi
fi

AC_SUBST(AM_CFLAGS,$CFLAGS)

# =====================

AC_OUTPUT
