#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(imyplay, 0.3, bogdandr@op.pl)
AM_INIT_AUTOMAKE
AC_CONFIG_FILES([Makefile src/Makefile doc/imyplay.txi:doc/imyplay.txc \
	po/Makefile.in:po/Makefile.inn imyplay.spc:imyplay.spi \
	src/imyp_cfg.h:src/imyp_cfg.hin doc/Makefile po/Makefile])
AC_CONFIG_SRCDIR([src/imyplay.c])
AC_CONFIG_HEADER([config.h:config.hin])

# ==================== configure arguments

AC_ARG_ENABLE(allegro,[[  --enable-allegro	Use the Allegro library (default).]],
	[if (test "x$enableval" = "xno"); then
		feat_allegro=no
	 else
		feat_allegro=yes
	 fi
	]
	,feat_allegro=yes)

AC_ARG_ENABLE(SDL,[[  --enable-SDL		Use the SDL library.]],
	[if (test "x$enableval" = "xno"); then
		feat_SDL=no
	 else
		feat_SDL=yes
	 fi
	]
	,feat_SDL=yes)

AC_ARG_ENABLE(ALSA,[[  --enable-ALSA		Use the ALSA library.]],
	[if (test "x$enableval" = "xno"); then
		feat_ALSA=no
	 else
		feat_ALSA=yes
	 fi
	]
	,feat_ALSA=yes)

AC_ARG_ENABLE(OSS,[[  --enable-OSS		Use the OSS subsystem.]],
	[if (test "x$enableval" = "xno"); then
		feat_OSS=no
	 else
		feat_OSS=yes
	 fi
	]
	,feat_OSS=yes)

AC_ARG_ENABLE(LIBAO,[[  --enable-LIBAO	Use the LIBAO library.]],
	[if (test "x$enableval" = "xno"); then
		feat_LIBAO=no
	 else
		feat_LIBAO=yes
	 fi
	]
	,feat_LIBAO=yes)

AC_ARG_ENABLE(PORTAUDIO,[[  --enable-PORTAUDIO	Use the PORTAUDIOv19 library.]],
	[if (test "x$enableval" = "xno"); then
		feat_PORTAUDIO=no
	 else
		feat_PORTAUDIO=yes
	 fi
	]
	,feat_PORTAUDIO=yes)

AC_ARG_ENABLE(PULSEAUDIO,[[  --enable-PULSEAUDIO	Use the PULSEAUDIO library.]],
	[if (test "x$enableval" = "xno"); then
		feat_PULSEAUDIO=no
	 else
		feat_PULSEAUDIO=yes
	 fi
	]
	,feat_PULSEAUDIO=yes)

AC_ARG_ENABLE(JACK,[[  --enable-JACK		Use the JACK library.]],
	[if (test "x$enableval" = "xno"); then
		feat_JACK=no
	 else
		feat_JACK=yes
	 fi
	]
	,feat_JACK=yes)

# ==================== Set of available languages (deprecated).
#ALL_LINGUAS="pl"

# ==================== Checks for programs.
AC_PROG_CC
AC_PROG_RANLIB
AM_CONDITIONAL(ISGCC, [test "x$GCC" = "xyes"])

# ==================== Checks for libraries.

if ( test "x$feat_SDL" = "xyes" ); then

	libsdl=yes
	AC_CHECK_LIB([SDL],[SDL_OpenAudio],,[libsdl=no])
fi

if ( test "x$feat_allegro" = "xyes" ); then

	AC_CHECK_LIB([alleg],[allegro_init],[liballeg=yes],[liballeg=no])
	alleg_ldflags=`allegro-config --libs`

	if (test "x$liballeg" == "xyes" || test "x$alleg_ldflags" != "x" ); then
		liballeg=yes
		AC_DEFINE([HAVE_LIBALLEG],[1],[Whether you have the Allegro library.])
	fi

	if ( test "x$alleg_ldflags" != "x" ); then
		# do NOT substitute "LIBS" here! Allegro libraries would make all tests fail
		# because of missing _mangled_main_address symbol.
		AC_SUBST(AM_LDFLAGS,"$LDFLAGS $alleg_ldflags")
	fi
fi

if ( test "x$feat_ALSA" = "xyes" ); then

	libalsa=yes
	AC_CHECK_LIB([asound],[snd_pcm_open],,[libalsa=no])
fi

if ( test "x$feat_LIBAO" = "xyes" ); then

	libao=yes
	AC_CHECK_LIB([ao],[ao_initialize],,[libao=no])
	XIPH_PATH_AO()
fi

if ( test "x$feat_PORTAUDIO" = "xyes" ); then

	libportaudio=yes
	AC_CHECK_LIB([portaudio],[Pa_Initialize],,[libportaudio=no])
fi

if ( test "x$feat_PULSEAUDIO" = "xyes" ); then

	libpulseaudio=yes
	AC_CHECK_LIB([pulse],[pa_stream_new],,[libpulseaudio=no])
	if ( test "x$libpulseaudio" = "xyes" ); then

		libpulseaudio_simple=yes
		AC_CHECK_LIB([pulse-simple],[pa_simple_new],,[libpulseaudio_simple=no])
	fi
fi

if ( test "x$feat_JACK" = "xyes" ); then

	libjack=yes
	AC_CHECK_LIB([jack],[jack_client_open],,[libjack=no])
fi

libmath=yes
AC_CHECK_LIB([m],[sin],,[libmath=no])

if (test "x$libmath" != "xyes" ); then
	AC_CHECK_FUNCS([sin])
fi

# ==================== Checks for header files.
AC_HEADER_STDC

AC_CHECK_HEADER([string.h],AC_DEFINE([HAVE_STRING_H],[1],[Whether you have the string.h header.]),
	[AC_MSG_ERROR([[I need string.h to work.]])])

AC_CHECK_HEADERS([getopt.h malloc.h sys/types.h stdlib.h locale.h math.h signal.h errno.h libgen.h])

if ( test "x$liballeg" = "xyes" || test "x$alleg_ldflags" != "x" ); then

	have_all_h=yes
	AC_CHECK_HEADER([allegro.h],AC_DEFINE([HAVE_ALLEGRO_H],[1],
		[Whether you have the allegro.h header.]), have_all_h=no)
fi

if ( test "x$libsdl" = "xyes" ); then

	have_sdl_h=yes
	AC_CHECK_HEADER([SDL.h],AC_DEFINE([HAVE_SDL_H],[1],
		[Whether you have the SDL.h header.]),
			[AC_CHECK_HEADER([SDL/SDL.h],AC_DEFINE([HAVE_SDL_SDL_H],[1],
				[Whether you have the SDL/SDL.h header.]),
			have_sdl_h=no)])
fi

if ( test "x$libalsa" = "xyes" ); then

	have_asoundlib_h=yes
	AC_CHECK_HEADER([asoundlib.h],AC_DEFINE([HAVE_ASOUNDLIB_H],[1],
		[Whether you have the asoundlib.h header.]),
			[AC_CHECK_HEADER([alsa/asoundlib.h],AC_DEFINE([HAVE_ALSA_ASOUNDLIB_H],[1],
				[Whether you have the alsa/asoundlib.h header.]),
			have_asoundlib_h=no)])
fi

if ( test "x$libao" = "xyes" ); then

	have_ao_h=yes
	AC_CHECK_HEADER([ao.h],AC_DEFINE([HAVE_AO_H],[1],
		[Whether you have the ao.h header.]),
			[AC_CHECK_HEADER([ao/ao.h],AC_DEFINE([HAVE_AO_AO_H],[1],
				[Whether you have the ao/ao.h header.]),
			have_ao_h=no)])
fi

if ( test "x$libportaudio" = "xyes" ); then

	have_portaudio_h=yes
	AC_CHECK_HEADER([portaudio.h],AC_DEFINE([HAVE_PORTAUDIO_H],[1],
		[Whether you have the portaudio.h header.]),
			[AC_CHECK_HEADER([portaudio/portaudio.h],
				AC_DEFINE([HAVE_PORTAUDIO_PORTAUDIO_H],[1],
				[Whether you have the portaudio/portaudio.h header.]),
			have_portaudio_h=no)])
fi

if ( test "x$libjack" = "xyes" ); then

	have_jack_h=yes
	AC_CHECK_HEADER([jack.h],AC_DEFINE([HAVE_JACK_H],[1],
		[Whether you have the jack.h header.]),
			[AC_CHECK_HEADER([jack/jack.h],
				AC_DEFINE([HAVE_JACK_JACK_H],[1],
				[Whether you have the jack/jack.h header.]),
			have_jack_h=no)])
fi

if ( test "x$have_asoundlib_h" = "xyes" || test "x$feat_OSS" = "xyes"		\
	|| test "x$have_ao_h" = "xyes" || test "x$have_portaudio_h" = "xyes"	\
	|| test "x$have_jack_h" = "xyes"); then

	have_select=yes
	AC_CHECK_FUNCS([select],AC_DEFINE([HAVE_SELECT],[1],
		[Whether you have the select() function.]),[have_select=no])
	if ( test "x$have_select" = "xyes" ); then

		have_sys_select_h=yes
		have_sys_time_h=yes
		have_sys_types_h=yes
		have_unistd_h=yes
		AC_HEADER_TIME()
		AC_CHECK_HEADERS([sys/select.h], AC_DEFINE([HAVE_SYS_SELECT_H],[1],
			[Whether you have the sys/select.h header.]), have_sys_select_h=no)
		AC_CHECK_HEADERS([sys/time.h], AC_DEFINE([HAVE_SYS_TIME_H],[1],
			[Whether you have the sys/time.h header.]), have_sys_time_h=no)
		AC_CHECK_HEADERS([sys/types.h], AC_DEFINE([HAVE_SYS_TYPES_H],[1],
			[Whether you have the sys/types.h header.]), have_sys_types_h=no)
		AC_CHECK_HEADERS([unistd.h], AC_DEFINE([HAVE_UNISTD_H],[1],
			[Whether you have the unistd.h header.]), have_unistd_h=no)
	fi
fi

if ( test "x$feat_OSS" = "xyes" ); then

	have_sys_soundcard_h=yes
	AC_CHECK_HEADERS([sys/soundcard.h], AC_DEFINE([HAVE_SYS_SOUNDCARD_H],[1],
		[Whether you have the sys/soundcard.h header.]), have_sys_soundcard_h=no)
	have_sys_ioctl_h=yes
	AC_CHECK_HEADERS([sys/ioctl.h], AC_DEFINE([HAVE_SYS_IOCTL_H],[1],
		[Whether you have the sys/ioctl.h header.]), have_sys_ioctl_h=no)
	have_fcntl_h=yes
	AC_CHECK_HEADERS([fcntl.h], AC_DEFINE([HAVE_FCNTL_H],[1],
		[Whether you have the fcntl.h header.]), have_fcntl_h=no)

	if ( test "x$have_sys_soundcard_h" = "xyes"	\
		&& test "x$have_sys_ioctl_h" = "xyes"	\
		&& test "x$have_fcntl_h" = "xyes"	\
	 ); then

		have_ioctl=yes
		AC_CHECK_FUNCS([ioctl],AC_DEFINE([HAVE_IOCTL],[1],
			[Whether you have the ioctl() function.]),[have_ioctl=no])
		have_open=yes
		AC_CHECK_FUNCS([open],AC_DEFINE([HAVE_OPEN],[1],
			[Whether you have the open() function.]),[have_open=no])
		have_close=yes
		AC_CHECK_FUNCS([close],AC_DEFINE([HAVE_CLOSE],[1],
			[Whether you have the close() function.]),[have_close=no])
		have_write=yes
		AC_CHECK_FUNCS([write],AC_DEFINE([HAVE_WRITE],[1],
			[Whether you have the write() function.]),[have_write=no])
	fi
fi

if ( test "x$libpulseaudio_simple" = "xyes" && test "x$libpulseaudio" = "xyes" ); then

	have_pulseaudio_simple_h=yes
	AC_CHECK_HEADER([simple.h],AC_DEFINE([HAVE_SIMPLE_H],[1],
		[Whether you have the simple.h header.]),
			[AC_CHECK_HEADER([pulse/simple.h],
				AC_DEFINE([HAVE_PULSE_SIMPLE_H],[1],
				[Whether you have the pulse/simple.h header.]),
			have_pulseaudio_simple_h=no)])
fi

AM_CONDITIONAL(ALLEGRO, [(test "x$liballeg" = "xyes" || test "x$alleg_ldflags" != "x") && \
	test "x$have_all_h" = "xyes"])

AM_CONDITIONAL(SDL, [test "x$libsdl" = "xyes" && test "x$have_sdl_h" = "xyes"])

AM_CONDITIONAL(ALSA, [test "x$libalsa" = "xyes" && test "x$have_asoundlib_h" = "xyes"	\
	&& test "x$have_select" = "xyes" && ( test "x$have_sys_select_h" = "xyes" || (	\
		test "x$have_sys_time_h" = "xyes"	&& 				\
		test "x$have_sys_types_h" = "xyes"	&& 				\
		test "x$have_unistd_h" = "xyes")					\
		)])

AM_CONDITIONAL(OSS, [test "x$have_ioctl" = "xyes" && test "x$have_sys_soundcard_h" = "xyes"	\
	&& test "x$have_sys_ioctl_h" = "xyes" && test "x$have_fcntl_h" = "xyes"			\
	&& test "x$have_unistd_h" = "xyes" && test "x$have_open" = "xyes"			\
	&& test "x$have_close" = "xyes" && test "x$have_write" = "xyes"				\
	&& test "x$have_select" = "xyes" && ( test "x$have_sys_select_h" = "xyes" || (		\
		test "x$have_sys_time_h" = "xyes"	&& 					\
		test "x$have_sys_types_h" = "xyes")						\
		)])

AM_CONDITIONAL(LIBAO, [test "x$libao" = "xyes" && test "x$have_ao_h" = "xyes"		\
	&& test "x$have_select" = "xyes" && ( test "x$have_sys_select_h" = "xyes" || (	\
		test "x$have_sys_time_h" = "xyes"	&& 				\
		test "x$have_sys_types_h" = "xyes"	&& 				\
		test "x$have_unistd_h" = "xyes")					\
		)])

AM_CONDITIONAL(PORTAUDIO, [test "x$libportaudio" = "xyes" && test "x$have_portaudio_h" = "xyes"])

AM_CONDITIONAL(PULSEAUDIO, [test "x$libpulseaudio_simple" = "xyes" \
	&& test "x$libpulseaudio" = "xyes" \
	&& test "x$have_pulseaudio_simple_h" = "xyes"])

AM_CONDITIONAL(JACK, [test "x$libjack" = "xyes" && test "x$have_jack_h" = "xyes"	\
	&& test "x$have_select" = "xyes" && ( test "x$have_sys_select_h" = "xyes" || (	\
		test "x$have_sys_time_h" = "xyes"	&& 				\
		test "x$have_sys_types_h" = "xyes"	&& 				\
		test "x$have_unistd_h" = "xyes")					\
		)])

# ===================== Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T
AC_C_VOLATILE
AC_CHECK_TYPES([off_t, size_t],,,
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

# Checks for library functions.
AC_TYPE_SIGNAL
AC_FUNC_MALLOC

AC_CHECK_FUNCS([malloc memcpy setlocale sigaction fseeko ftello snprintf basename getopt_long])

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

# ===================== Print summary

echo "***********************************"

if (	(test "x$liballeg" = "xyes" || test "x$alleg_ldflags" != "x") && \
	test "x$have_all_h" = "xyes"); then

	echo " *	Allegro: yes"
else
	if (test "x$feat_allegro" != "xyes"); then
		echo " *	Allegro: no (disabled by command line)"

	elif ( test "x$liballeg" != "xyes" && test "x$alleg_ldflags" = "x" ); then
		echo " *	Allegro: no (library missing or unusable)"

	else	#if (test "x$have_all_h" != "xyes"); then
		echo " *	Allegro: no (allegro.h header missing)"
	fi
fi

if (	test "x$libsdl" = "xyes" && test "x$have_sdl_h" = "xyes"); then

	echo " *	SDL: yes"
else
	if (test "x$feat_SDL" != "xyes"); then
		echo " *	SDL: no (disabled by command line)"

	elif ( test "x$libsdl" != "xyes" ); then
		echo " *	SDL: no (library missing or unusable)"

	else	#if (test "x$have_sdl_h" != "xyes"); then
		echo " *	SDL: no (SDL.h header missing)"
	fi
fi

if (	test "x$libalsa" = "xyes" && test "x$have_asoundlib_h" = "xyes"	\
	&& test "x$have_select" = "xyes" && ( test "x$have_sys_select_h" = "xyes" || (	\
		test "x$have_sys_time_h" = "xyes"	&& 				\
		test "x$have_sys_types_h" = "xyes"	&& 				\
		test "x$have_unistd_h" = "xyes")					\
		)); then

	echo " *	ALSA: yes"
else
	if (test "x$feat_ALSA" != "xyes"); then
		echo " *	ALSA: no (disabled by command line)"

	elif ( test "x$libalsa" != "xyes" ); then
		echo " *	ALSA: no (library missing or unusable)"

	elif ( test "x$have_select" != "xyes" ); then
		echo " *	ALSA: no (select() function missing)"

	elif ( ! ( test "x$have_sys_select_h" = "xyes" || (				\
		test "x$have_sys_time_h" = "xyes"	&& 				\
		test "x$have_sys_types_h" = "xyes"	&& 				\
		test "x$have_unistd_h" = "xyes")					\
		) ); then
		echo " *	ALSA: no (select() headers missing or unusable)"

	else	#(test "x$have_asoundlib_h" != "xyes"); then
		echo " *	ALSA: no (asoundlib.h header missing)"
	fi
fi

if (	test "x$have_ioctl" = "xyes" && test "x$have_sys_soundcard_h" = "xyes"		\
	&& test "x$have_sys_ioctl_h" = "xyes" && test "x$have_fcntl_h" = "xyes"		\
	&& test "x$have_unistd_h" = "xyes" && test "x$have_open" = "xyes"		\
	&& test "x$have_close" = "xyes" && test "x$have_write" = "xyes"			\
	&& test "x$have_select" = "xyes" && ( test "x$have_sys_select_h" = "xyes" || (	\
		test "x$have_sys_time_h" = "xyes"	&& 				\
		test "x$have_sys_types_h" = "xyes")					\
		)									\
	); then

	echo " *	OSS: yes"
else
	if (test "x$feat_OSS" != "xyes"); then
		echo " *	OSS: no (disabled by command line)"

	elif (test "x$have_sys_soundcard_h" != "xyes"); then
		echo " *	OSS: no (sys/soundcard.h header missing)"

	elif ( test "x$have_ioctl" != "xyes" ); then
		echo " *	OSS: no (ioctl() function missing)"

	elif ( test "x$have_sys_ioctl_h" != "xyes" ); then
		echo " *	OSS: no (sys/ioctl.h header missing)"

	elif ( test "x$have_fcntl_h" != "xyes" ); then
		echo " *	OSS: no (fcntl.h header missing)"

	elif ( test "x$have_unistd_h" != "xyes" ); then
		echo " *	OSS: no (unistd.h header missing)"

	elif ( test "x$have_open" != "xyes" ); then
		echo " *	OSS: no (open() function missing)"

	elif ( test "x$have_close" != "xyes" ); then
		echo " *	OSS: no (close() function missing)"

	elif ( test "x$have_write" != "xyes" ); then
		echo " *	OSS: no (write() function missing)"

	elif ( test "x$have_select" != "xyes" ); then
		echo " *	OSS: no (select() function missing)"

	elif ( ! ( test "x$have_sys_select_h" = "xyes" || (				\
		test "x$have_sys_time_h" = "xyes"	&& 				\
		test "x$have_sys_types_h" = "xyes"	&& 				\
		test "x$have_unistd_h" = "xyes")					\
		) ); then
		echo " *	OSS: no (select() headers missing or unusable)"
	else
		echo " *	OSS: no (reason unknown)"

	fi
fi

if (	test "x$libao" = "xyes" && test "x$have_ao_h" = "xyes"	\
	&& test "x$have_select" = "xyes" && ( test "x$have_sys_select_h" = "xyes" || (	\
		test "x$have_sys_time_h" = "xyes"	&& 				\
		test "x$have_sys_types_h" = "xyes"	&& 				\
		test "x$have_unistd_h" = "xyes")					\
		)); then

	echo " *	LIBAO: yes"
else
	if (test "x$feat_LIBAO" != "xyes"); then
		echo " *	LIBAO: no (disabled by command line)"

	elif ( test "x$libao" != "xyes" ); then
		echo " *	LIBAO: no (library missing or unusable)"

	elif ( test "x$have_select" != "xyes" ); then
		echo " *	LIBAO: no (select() function missing)"

	elif ( ! ( test "x$have_sys_select_h" = "xyes" || (				\
		test "x$have_sys_time_h" = "xyes"	&& 				\
		test "x$have_sys_types_h" = "xyes"	&& 				\
		test "x$have_unistd_h" = "xyes")					\
		) ); then
		echo " *	LIBAO: no (select() headers missing or unusable)"

	else	#(test "x$have_ao_h" != "xyes"); then
		echo " *	LIBAO: no (ao.h header missing)"
	fi
fi

if (	test "x$libportaudio" = "xyes" && test "x$have_portaudio_h" = "xyes" ); then

	echo " *	PortAudiov19: yes"
else
	if (test "x$feat_PORTAUDIO" != "xyes"); then
		echo " *	PortAudiov19: no (disabled by command line)"

	elif ( test "x$libportaudio" != "xyes" ); then
		echo " *	PortAudiov19: no (library missing or unusable)"

	else	#(test "x$have_portaudio_h" != "xyes"); then
		echo " *	PortAudiov19: no (portaudio.h header missing)"
	fi
fi

if (	test "x$libpulseaudio_simple" = "xyes" && test "x$libpulseaudio" = "xyes" && \
	test "x$have_pulseaudio_simple_h" = "xyes" ); then

	echo " *	PulseAudio: yes"
else
	if (test "x$feat_PULSEAUDIO" != "xyes"); then
		echo " *	PulseAudio: no (disabled by command line)"

	elif ( test "x$libpulseaudio_simple" != "xyes" 	\
		|| test "x$libpulseaudio" != "xyes" ); then
		echo " *	PulseAudio: no (library missing or unusable)"

	else	#(test "x$have_pulseaudio_simple_h" != "xyes"); then
		echo " *	PulseAudio: no (simple.h header missing)"
	fi
fi

if (	test "x$libjack" = "xyes" && test "x$have_jack_h" = "xyes"	\
	&& test "x$have_select" = "xyes" && ( test "x$have_sys_select_h" = "xyes" || (	\
		test "x$have_sys_time_h" = "xyes"	&& 				\
		test "x$have_sys_types_h" = "xyes"	&& 				\
		test "x$have_unistd_h" = "xyes")					\
		) ); then

	echo " *	JACK1: yes"
else
	if (test "x$feat_JACK" != "xyes"); then
		echo " *	JACK1: no (disabled by command line)"

	elif ( test "x$libjack" != "xyes" ); then
		echo " *	JACK1: no (library missing or unusable)"

	elif ( test "x$have_select" != "xyes" ); then
		echo " *	JACK1: no (select() function missing)"

	elif ( ! ( test "x$have_sys_select_h" = "xyes" || (				\
		test "x$have_sys_time_h" = "xyes"	&& 				\
		test "x$have_sys_types_h" = "xyes"	&& 				\
		test "x$have_unistd_h" = "xyes")					\
		) ); then
		echo " *	JACK1: no (select() headers missing or unusable)"

	else	#(test "x$have_jack_h" != "xyes"); then
		echo " *	JACK1: no (jack.h header missing)"
	fi
fi

echo "***********************************"
