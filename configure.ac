#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
#
# IMYplay - A program for playing iMelody ringtones (IMY files).
#	-- configure.ac, part of the build system.
#
# Copyright (C) 2009-2023 Bogdan Drozdowski, bogdro (at) users . sourceforge . net
# License: GNU General Public License, v3+
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.
#

AC_PREREQ([2.71])
AC_INIT([IMYplay],[1.3],[bogdro@users.sourceforge.net],[imyplay],[https://imyplay.sourceforge.io])
AM_INIT_AUTOMAKE([-Wall])
AC_CONFIG_FILES([Makefile src/Makefile doc/imyplay.txi:doc/imyplay.txc \
	po/Makefile.in po/Makefile imyplay.spc:imyplay.spi \
	src/imyp_cfg.h:src/imyp_cfg.hin doc/Makefile test/Makefile])
AC_CONFIG_SRCDIR([src/imyplay.c])
AC_CONFIG_HEADERS([config.h:config.hin])

# ==================== configure arguments

AC_ARG_ENABLE(allegro,[[  --enable-allegro	Enable Allegro output (default).]],
	[if (test "x$enableval" = "xno"); then
		feat_allegro=no
	 else
		feat_allegro=yes
	 fi
	]
	,feat_allegro=yes)

AC_ARG_ENABLE(SDL,[[  --enable-SDL		Enable SDLv1 output.]],
	[if (test "x$enableval" = "xno"); then
		feat_SDL=no
	 else
		feat_SDL=yes
	 fi
	]
	,feat_SDL=yes)

AC_ARG_ENABLE(SDL2,[[  --enable-SDL2		Enable SDLv2 output.]],
	[if (test "x$enableval" = "xno"); then
		feat_SDL2=no
	 else
		feat_SDL2=yes
	 fi
	]
	,feat_SDL2=yes)

AC_ARG_ENABLE(ALSA,[[  --enable-ALSA		Enable ALSA output.]],
	[if (test "x$enableval" = "xno"); then
		feat_ALSA=no
	 else
		feat_ALSA=yes
	 fi
	]
	,feat_ALSA=yes)

AC_ARG_ENABLE(OSS,[[  --enable-OSS		Enable OSS output.]],
	[if (test "x$enableval" = "xno"); then
		feat_OSS=no
	 else
		feat_OSS=yes
	 fi
	]
	,feat_OSS=yes)

AC_ARG_ENABLE(LIBAO,[[  --enable-LIBAO	Enable LIBAO output.]],
	[if (test "x$enableval" = "xno"); then
		feat_LIBAO=no
	 else
		feat_LIBAO=yes
	 fi
	]
	,feat_LIBAO=yes)

AC_ARG_ENABLE(PORTAUDIO,[[  --enable-PORTAUDIO	Enable PORTAUDIOv19 output.]],
	[if (test "x$enableval" = "xno"); then
		feat_PORTAUDIO=no
	 else
		feat_PORTAUDIO=yes
	 fi
	]
	,feat_PORTAUDIO=yes)

AC_ARG_ENABLE(PULSEAUDIO,[[  --enable-PULSEAUDIO	Enable PULSEAUDIO output.]],
	[if (test "x$enableval" = "xno"); then
		feat_PULSEAUDIO=no
	 else
		feat_PULSEAUDIO=yes
	 fi
	]
	,feat_PULSEAUDIO=yes)

AC_ARG_ENABLE(JACK,[[  --enable-JACK		Enable JACK output.]],
	[if (test "x$enableval" = "xno"); then
		feat_JACK=no
	 else
		feat_JACK=yes
	 fi
	]
	,feat_JACK=yes)

AC_ARG_ENABLE(MIDI,[[  --enable-MIDI		Enable MIDI output.]],
	[if (test "x$enableval" = "xno"); then
		feat_MIDI=no
	 else
		feat_MIDI=yes
	 fi
	]
	,feat_MIDI=yes)

AC_ARG_ENABLE(EXEC,[[  --enable-EXEC		Enable executing commands as output.]],
	[if (test "x$enableval" = "xno"); then
		feat_EXEC=no
	 else
		feat_EXEC=yes
	 fi
	]
	,feat_EXEC=yes)

AC_ARG_ENABLE(GSTREAMER,[[  --enable-GSTREAMER	Enable GStreamer output.]],
	[if (test "x$enableval" = "xno"); then
		feat_GST=no
	 else
		feat_GST=yes
	 fi
	]
	,feat_GST=yes)

AC_ARG_ENABLE(FILE,[[  --enable-FILE		Enable raw file output.]],
	[if (test "x$enableval" = "xno"); then
		feat_FILE=no
	 else
		feat_FILE=yes
	 fi
	]
	,feat_FILE=yes)

AC_ARG_ENABLE(SPKR,[[  --enable-SPKR		Enable PC-Speaker output.]],
	[if (test "x$enableval" = "xno"); then
		feat_SPKR=no
	 else
		feat_SPKR=yes
	 fi
	]
	,feat_SPKR=yes)

AC_ARG_ENABLE(FLEXPARSER,[[  --enable-FLEXPARSER	Enable flex-based parser.]],
	[if (test "x$enableval" = "xno"); then
		feat_FLEX=no
	 else
		feat_FLEX=yes
	 fi
	]
	,feat_FLEX=yes)

AC_ARG_ENABLE(LIBHIDEIP,[[  --enable-LIBHIDEIP	Enable LibHideIP for IMYplay.]],
	[if (test "x$enableval" = "xno"); then
		feat_LIBHIDEIP=no
	 else
		feat_LIBHIDEIP=yes
	 fi
	]
	,feat_LIBHIDEIP=no)

AC_ARG_ENABLE(LIBNETBLOCK,[[  --enable-LIBNETBLOCK	Enable LibNetBlock for IMYplay.]],
	[if (test "x$enableval" = "xno"); then
		feat_LIBNETBLOCK=no
	 else
		feat_LIBNETBLOCK=yes
	 fi
	]
	,feat_LIBNETBLOCK=no)

# ==================== Set of available languages (deprecated).
#ALL_LINGUAS="pl"

# ==================== Checks for programs.
AC_LANG(C)
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_RANLIB
AM_CONDITIONAL(ISGCC, [test "x$GCC" = "xyes"])

have_flex=no
# flex should be searched for unconditionally, because it is
# used in 'make dist'
#if ( test "x$feat_FLEX" = "xyes" ); then

	# required by AM_PROG_LEX, otherwise called without
	# any arguments and that's deprecated:
	AC_PROG_LEX([noyywrap])
	AM_PROG_LEX
	if ( test "x$LEX" = "xflex" ); then

		have_flex=yes
	fi
#fi

# ==================== Checks for libraries.

# SDLv2 has functions with the same names as those in SDLv1,
# so enable at most one of the libraries at the same time
if ( test "x$feat_SDL2" = "xyes" ); then

	libsdl2=yes
	AC_CHECK_LIB([SDL2],[SDL_OpenAudio],,[libsdl2=no])
fi
if ( test "x$feat_SDL" = "xyes" && test "x$libsdl2" != "xyes" ); then

	libsdl=yes
	AC_CHECK_LIB([SDL],[SDL_OpenAudio],,[libsdl=no])
fi


if ( test "x$feat_allegro" = "xyes" ); then

	AC_CHECK_LIB([alleg],[allegro_init],[liballeg=yes],[liballeg=no])
	alleg_ldflags=`allegro-config --libs 2>/dev/null`

	if (test "x$liballeg" == "xyes" || test "x$alleg_ldflags" != "x" ); then
		liballeg=yes
		AC_MSG_NOTICE([Using "$alleg_ldflags" as Allegro library flags.])
		AC_DEFINE([HAVE_LIBALLEG],[1],[Whether you have the Allegro library.])
	fi

	if ( test "x$alleg_ldflags" != "x" ); then
		# do NOT substitute "LIBS" here! Allegro libraries would make all tests fail
		# because of missing _mangled_main_address symbol.
		#AC_SUBST(AM_LDFLAGS,"$AM_LDFLAGS $LDFLAGS $alleg_ldflags")
		AC_SUBST(ALLEGRO_LDFLAGS,"$alleg_ldflags")
	else
		AC_MSG_NOTICE([Allegro disabled, both the library and allegro-config are missing.])
	fi
fi

if ( test "x$feat_ALSA" = "xyes" ); then

	libalsa=yes
	AC_CHECK_LIB([asound],[snd_pcm_open],,[libalsa=no])
fi

if ( test "x$feat_LIBAO" = "xyes" ); then

	libao=yes
	AC_CHECK_LIB([ao],[ao_initialize],,[libao=no])
# 	XIPH_PATH_AO()
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
	if ( test "x$libjack" = "xyes" ); then
		AC_CHECK_LIB([jack],[jack_free],AC_DEFINE([HAVE_JACK_FREE],[1],
			[Whether you have the jack_free() function in the JACK library.]))
		AC_CHECK_FUNCS([jack_get_version_string])
	fi
fi

libmath=yes
AC_CHECK_LIB([m],[sin],,[libmath=no])

if (test "x$libmath" != "xyes" ); then
	AC_CHECK_FUNCS([sin])
fi

if ( test "x$feat_EXEC" = "xyes" ); then

	have_system=yes
	AC_CHECK_FUNCS([system],AC_DEFINE([HAVE_SYSTEM],[1],
		[Whether you have the system() function.]),[have_system=no])
fi

if ( test "x$feat_GST" = "xyes" ); then

	PKG_PROG_PKG_CONFIG([])
	if ( test "x$PKG_CONFIG" != "x" ); then

		libgstreamer=no
		PKG_CHECK_MODULES([GST], [gstreamer-controller-1.0], [
			libgstreamer=yes
			AC_DEFINE([HAVE_GST_GST_H],[1],
				[Whether you have the gst/gst.h header.])
			], [libgstreamer=no]);
		if ( test "x$libgstreamer" = "xno" ); then

			PKG_CHECK_MODULES([GST], [gstreamer-controller >= 1.0], [
				libgstreamer=yes
				AC_DEFINE([HAVE_GST_GST_H],[1],
					[Whether you have the gst/gst.h header.])
				], [libgstreamer=no]);
		fi
		if ( test "x$libgstreamer" = "xno" ); then

			PKG_CHECK_MODULES([GST], [gstreamer-controller-0.10], [
				libgstreamer=yes
				AC_DEFINE([HAVE_GST_GST_H],[1],
					[Whether you have the gst/gst.h header.])
				], [libgstreamer=no]);
		fi
		if ( test "x$libgstreamer" = "xno" ); then

			PKG_CHECK_MODULES([GST], [gstreamer-controller >= 0.10], [
				libgstreamer=yes
				AC_DEFINE([HAVE_GST_GST_H],[1],
					[Whether you have the gst/gst.h header.])
				], [libgstreamer=no]);
		fi
		if ( test "x$libgstreamer" = "xno" ); then

			PKG_CHECK_MODULES([GST], [gstreamer-0.10], [
				libgstreamer=yes
				AC_DEFINE([HAVE_GST_GST_H],[1],
					[Whether you have the gst/gst.h header.])
				], [libgstreamer=no]);
		fi
		if ( test "x$libgstreamer" = "xno" ); then

			PKG_CHECK_MODULES([GST], [gstreamer >= 0.10], [
				libgstreamer=yes
				AC_DEFINE([HAVE_GST_GST_H],[1],
					[Whether you have the gst/gst.h header.])
				], [libgstreamer=no]);
		fi
		if ( test "x$libgstreamer" = "xyes" ); then

			CFLAGS="$CFLAGS $GST_CFLAGS"
			LDFLAGS="$LDFLAGS $GST_LIBS"
			LIBS="$LIBS $GST_LIBS"
		fi
	fi
fi

if ( test "x$feat_LIBHIDEIP" = "xyes" ); then

	libhip=yes
	AC_CHECK_LIB(hideip,[libhideip_enable],,[libhip=no])
fi

if ( test "x$feat_LIBNETBLOCK" = "xyes" ); then

	libnb=yes
	AC_CHECK_LIB(netblock,[libnetblock_enable],,[libnb=no])
fi

# ==================== Checks for header files.
# AC_ HEADER_ STDC
AC_CHECK_INCLUDES_DEFAULT
AC_PROG_EGREP

AC_HEADER_SYS_WAIT()

has_sys_wait_h=yes
AC_CHECK_HEADER([sys/wait.h],AC_DEFINE([HAVE_SYS_WAIT_H],[1],
	[Whether you have the sys/wait.h header.]),[AC_CHECK_HEADER([wait.h],
		AC_DEFINE([HAVE_WAIT_H],[1],
		[Whether you have the wait.h header.]),[has_wait_h=no])])

AC_CHECK_HEADER([string.h],AC_DEFINE([HAVE_STRING_H],[1],[Whether you have the string.h header.]),
	[AC_MSG_ERROR([[I need string.h to work.]])])

have_unistd_h=yes
AC_CHECK_HEADERS([unistd.h], AC_DEFINE([HAVE_UNISTD_H],[1],
	[Whether you have the unistd.h header.]), have_unistd_h=no)

have_errno_h=yes
AC_CHECK_HEADERS([errno.h], AC_DEFINE([HAVE_ERRNO_H],[1],
	[Whether you have the errno.h header.]), have_errno_h=no)

have_stdlib_h=yes
AC_CHECK_HEADERS([stdlib.h], AC_DEFINE([HAVE_STDLIB_H],[1],
	[Whether you have the stdlib.h header.]), have_stdlib_h=no)

if ( test "x$feat_FLEX" = "xyes" && test "x$have_flex" = "xyes" ); then

	have_inttypes_h=yes
	AC_CHECK_HEADERS([inttypes.h], AC_DEFINE([HAVE_INTTYPES_H],[1],
		[Whether you have the inttypes.h header.]), have_inttypes_h=no)

fi

AC_CHECK_HEADERS([getopt.h malloc.h sys/types.h locale.h math.h signal.h \
	libgen.h strings.h])
AX_INCLUDE_STRCASECMP

if ( test "x$feat_EXEC" = "xyes" && test "x$have_system" = "xyes" ); then

	have_stdlib_h=yes
	AC_CHECK_HEADER([stdlib.h],AC_DEFINE([HAVE_STDLIB_H],[1],
		[Whether you have the stdlib.h header.]), have_stdlib_h=no)
fi

if ( test "x$liballeg" = "xyes" || test "x$alleg_ldflags" != "x" ); then

	have_all_h=yes
	AC_CHECK_HEADER([allegro.h],AC_DEFINE([HAVE_ALLEGRO_H],[1],
		[Whether you have the allegro.h header.]), have_all_h=no)
fi

if ( test "x$libsdl2" = "xyes" ); then

	have_sdl2_h=yes
	AC_CHECK_HEADER([SDL2/SDL.h],AC_DEFINE([HAVE_SDL2_SDL_H],[1],
		[Whether you have the SDL2/SDL.h header.]),
			have_sdl2_h=no)
fi

# SDLv2 has functions with the same names as those in SDLv1,
# so enable at most one of the libraries at the same time
if ( test "x$libsdl" = "xyes" && test "x$libsdl2" != "xyes" ); then

	have_sdl_h=yes
	AC_CHECK_HEADER([SDL/SDL.h],AC_DEFINE([HAVE_SDL_SDL_H],[1],
		[Whether you have the SDL/SDL.h header.]),
			have_sdl_h=no)
fi

if ( test "x$libalsa" = "xyes" ); then

	have_asoundlib_h=yes
	AC_CHECK_HEADER([alsa/asoundlib.h],AC_DEFINE([HAVE_ALSA_ASOUNDLIB_H],[1],
		[Whether you have the alsa/asoundlib.h header.]),
			[AC_CHECK_HEADER([asoundlib.h],AC_DEFINE([HAVE_ASOUNDLIB_H],[1],
				[Whether you have the asoundlib.h header.]),
			have_asoundlib_h=no)])
fi

if ( test "x$libao" = "xyes" ); then

	have_ao_h=yes
	AC_CHECK_HEADER([ao/ao.h],AC_DEFINE([HAVE_AO_AO_H],[1],
		[Whether you have the ao/ao.h header.]),
			[AC_CHECK_HEADER([ao.h],AC_DEFINE([HAVE_AO_H],[1],
				[Whether you have the ao.h header.]),
			have_ao_h=no)])
fi

if ( test "x$libportaudio" = "xyes" ); then

	have_portaudio_h=yes
	AC_CHECK_HEADER([portaudio/portaudio.h],AC_DEFINE([HAVE_PORTAUDIO_PORTAUDIO_H],[1],
		[Whether you have the portaudio/portaudio.h header.]),
			[AC_CHECK_HEADER([portaudio.h],
				AC_DEFINE([HAVE_PORTAUDIO_H],[1],
				[Whether you have the portaudio.h header.]),
			have_portaudio_h=no)])
fi

if ( test "x$libjack" = "xyes" ); then

	have_jack_h=yes
	AC_CHECK_HEADER([jack/jack.h],AC_DEFINE([HAVE_JACK_JACK_H],[1],
		[Whether you have the jack/jack.h header.]),
			[AC_CHECK_HEADER([jack.h],
				AC_DEFINE([HAVE_JACK_H],[1],
				[Whether you have the jack.h header.]),
			have_jack_h=no)])
fi

#if ( test "x$have_asoundlib_h" = "xyes" || test "x$feat_OSS" = "xyes"		\
#	|| test "x$have_ao_h" = "xyes" || test "x$have_portaudio_h" = "xyes"	\
#	|| test "x$have_jack_h" = "xyes" || test "x$feat_SPKR" = "xyes"); then

	have_select=yes
	# No need for AC_FUNC_SELECT_ARGTYPES - we use only
	# ( 0, NULL, NULL, NULL, &tv ) and the first parameter can be any
	# kind of int or a size_t and the last parameter can be const or not.
	AC_CHECK_FUNCS([select],AC_DEFINE([HAVE_SELECT],[1],
		[Whether you have the select() function.]),[have_select=no])
	if ( test "x$have_select" = "xyes" ); then

		have_sys_select_h=yes
		have_sys_time_h=yes
		have_sys_types_h=yes

		# AC_ HEADER_ TIME()
		# m4_warn([obsolete],
		# [Update your code to rely only on HAVE_SYS_TIME_H,
		# then remove this warning and the obsolete code below it.
		# All current systems provide time.h; it need not be checked for.
		# Not all systems provide sys/time.h, but those that do, all allow
		# you to include it and time.h simultaneously.])dnl
		# AC_CHECK_HEADERS_ONCE([sys/time.h])
		# Obsolete code to be removed.
		# if test $ac_cv_header_sys_time_h = yes; then
		#   AC_DEFINE([TIME_WITH_SYS_TIME],[1],[Define to 1 if you can safely include both <sys/time.h>
		# 	     and <time.h>.  This macro is obsolete.])
		# fi
		# End of obsolete code.

		AC_CHECK_HEADERS([sys/select.h], AC_DEFINE([HAVE_SYS_SELECT_H],[1],
			[Whether you have the sys/select.h header.]), have_sys_select_h=no)
		AC_CHECK_HEADERS([sys/time.h], AC_DEFINE([HAVE_SYS_TIME_H],[1],
			[Whether you have the sys/time.h header.]), have_sys_time_h=no)
		AC_CHECK_HEADERS([sys/types.h], AC_DEFINE([HAVE_SYS_TYPES_H],[1],
			[Whether you have the sys/types.h header.]), have_sys_types_h=no)
	fi
#fi


if ( test "x$feat_SPKR" = "xyes" ); then

	have_sys_kd_h=yes
	AC_CHECK_HEADERS([sys/kd.h], AC_DEFINE([HAVE_SYS_KD_H],[1],
		[Whether you have the sys/kd.h header.]), have_sys_kd_h=no)
	have_kiocsound=no
	if ( test "x$have_sys_kd_h" = "xyes" ); then

		have_kiocsound=yes
		AC_CHECK_DECL([KIOCSOUND],[AC_DEFINE([HAVE_DECL_KIOCSOUND], [1], [Whether KIOCSOUND is defined])],
			[have_kiocsound=no],
			[
			#ifdef HAVE_SYS_KD_H
			#include <sys/kd.h>
			#endif
			])
	fi
fi

if ( test "x$feat_SPKR" = "xyes" || test "x$feat_OSS" = "xyes" ); then

	have_sys_ioctl_h=yes
	AC_CHECK_HEADERS([sys/ioctl.h], AC_DEFINE([HAVE_SYS_IOCTL_H],[1],
		[Whether you have the sys/ioctl.h header.]), have_sys_ioctl_h=no)
	have_fcntl_h=yes
	AC_CHECK_HEADERS([fcntl.h], AC_DEFINE([HAVE_FCNTL_H],[1],
		[Whether you have the fcntl.h header.]), have_fcntl_h=no)

	if ( test "x$have_sys_ioctl_h" = "xyes"	\
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
	fi
fi

if ( test "x$feat_OSS" = "xyes" ); then

	have_sys_soundcard_h=yes
	AC_CHECK_HEADERS([sys/soundcard.h], AC_DEFINE([HAVE_SYS_SOUNDCARD_H],[1],
		[Whether you have the sys/soundcard.h header.]), have_sys_soundcard_h=no)
	if ( test "x$have_sys_soundcard_h" = "xyes"	\
		&& test "x$have_fcntl_h" = "xyes"	\
	 ); then

		have_write=yes
		AC_CHECK_FUNCS([write],AC_DEFINE([HAVE_WRITE],[1],
			[Whether you have the write() function.]),[have_write=no])
	fi
fi

if ( test "x$libpulseaudio_simple" = "xyes" && test "x$libpulseaudio" = "xyes" ); then

	have_pulseaudio_simple_h=yes
	AC_CHECK_HEADER([pulse/simple.h],AC_DEFINE([HAVE_PULSE_SIMPLE_H],[1],
		[Whether you have the pulse/simple.h header.]),
			[AC_CHECK_HEADER([simple.h],
				AC_DEFINE([HAVE_SIMPLE_H],[1],
				[Whether you have the simple.h header.]),
			have_pulseaudio_simple_h=no)])
fi

if ( test "x$feat_LIBHIDEIP" = "xyes" && test "x$libhip" = "xyes" ); then

	have_libhideip_h=yes
	AC_CHECK_HEADER([libhideip.h],AC_DEFINE([HAVE_LIBHIDEIP_H],[1],
		[Whether you have the libhideip.h header.]), have_libhideip_h=no)
fi

if ( test "x$feat_LIBNETBLOCK" = "xyes" && test "x$libnb" = "xyes" ); then

	have_libnetblock_h=yes
	AC_CHECK_HEADER([libnetblock.h],AC_DEFINE([HAVE_LIBNETBLOCK_H],[1],
		[Whether you have the libnetblock.h header.]), have_libnetblock_h=no)
fi

if ( test "x$feat_FILE" = "xyes" ); then

	# check for a marker function (that is always present), so we can detect it
	have_fwrite=yes
	AC_CHECK_FUNCS([fwrite],AC_DEFINE([HAVE_FWRITE],[1],
		[Whether you have the fwrite() function.]),[have_fwrite=no])

fi

have_malloc=yes
AC_CHECK_FUNCS([malloc],AC_DEFINE([HAVE_MALLOC],[1],
	[Whether you have the malloc() function.]),[have_malloc=no])
have_memset=yes
AC_CHECK_FUNCS([memset],AC_DEFINE([HAVE_MEMSET],[1],
	[Whether you have the memset() function.]),[have_memset=no])
have_memcpy=yes
AC_CHECK_FUNCS([memcpy],AC_DEFINE([HAVE_MEMCPY],[1],
	[Whether you have the memcpy() function.]),[have_memcpy=no])

if ( test "x$feat_MIDI" == "xyes" || test "x$feat_FLEX" = "xyes" ); then
	have_realloc=yes
	AC_CHECK_FUNCS([realloc],AC_DEFINE([HAVE_REALLOC],[1],
		[Whether you have the realloc() function.]),[have_realloc=no])
fi

if ( test "x$feat_MIDI" == "xyes" ); then
	have_qsort=yes
	AC_CHECK_FUNCS([qsort],AC_DEFINE([HAVE_QSORT],[1],
		[Whether you have the qsort() function.]),[have_qsort=no])
	AC_CHECK_HEADERS([stdint.h])
fi


AM_CONDITIONAL(ALLEGRO, [test "x$feat_allegro" = "xyes" && 				\
	(test "x$liballeg" = "xyes" || test "x$alleg_ldflags" != "x") &&		\
	test "x$have_all_h" = "xyes"])

# SDLv2 has functions with the same names as those in SDLv1,
# so enable at most one of the libraries at the same time
AM_CONDITIONAL(SDL2, [test "x$feat_SDL2" = "xyes" && test "x$libsdl2" = "xyes"		\
	&& test "x$have_sdl2_h" = "xyes"])

AM_CONDITIONAL(SDL, [test "x$feat_SDL" = "xyes" && test "x$libsdl" = "xyes"		\
	&& test "x$have_sdl_h" = "xyes" && test "x$libsdl2" != "xyes" ])

AM_CONDITIONAL(ALSA, [test "x$feat_ALSA" = "xyes" && test "x$libalsa" = "xyes"		\
	&& test "x$have_asoundlib_h" = "xyes"	\
	&& test "x$have_select" = "xyes" && ( test "x$have_sys_select_h" = "xyes" || (	\
		test "x$have_sys_time_h" = "xyes"	&& 				\
		test "x$have_sys_types_h" = "xyes"	&& 				\
		test "x$have_unistd_h" = "xyes")					\
		)])

AM_CONDITIONAL(OSS, [test "x$feat_OSS" = "xyes" && test "x$have_ioctl" = "xyes"	\
	&& test "x$have_sys_soundcard_h" = "xyes"	\
	&& test "x$have_sys_ioctl_h" = "xyes" && test "x$have_fcntl_h" = "xyes"			\
	&& test "x$have_unistd_h" = "xyes" && test "x$have_open" = "xyes"			\
	&& test "x$have_close" = "xyes" && test "x$have_write" = "xyes"				\
	&& test "x$have_select" = "xyes" && ( test "x$have_sys_select_h" = "xyes" || (		\
		test "x$have_sys_time_h" = "xyes"	&& 					\
		test "x$have_sys_types_h" = "xyes")						\
		)])

AM_CONDITIONAL(LIBAO, [test "x$feat_LIBAO" = "xyes" && test "x$libao" = "xyes" 	\
	&& test "x$have_ao_h" = "xyes"		\
	&& test "x$have_select" = "xyes" && ( test "x$have_sys_select_h" = "xyes" || (	\
		test "x$have_sys_time_h" = "xyes"	&& 				\
		test "x$have_sys_types_h" = "xyes"	&& 				\
		test "x$have_unistd_h" = "xyes")					\
		)])

AM_CONDITIONAL(PORTAUDIO, [test "x$feat_PORTAUDIO" = "xyes" && test "x$libportaudio" = "xyes" && \
	test "x$have_portaudio_h" = "xyes"])

AM_CONDITIONAL(PULSEAUDIO, [test "x$feat_PULSEAUDIO" = "xyes"				\
	&& test "x$libpulseaudio_simple" = "xyes" && test "x$libpulseaudio" = "xyes"	\
	&& test "x$have_pulseaudio_simple_h" = "xyes"])

AM_CONDITIONAL(JACK, [test "x$feat_JACK" = "xyes" && test "x$libjack" = "xyes"		\
	&& test "x$have_jack_h" = "xyes"	\
	&& test "x$have_select" = "xyes" && ( test "x$have_sys_select_h" = "xyes" || (	\
		test "x$have_sys_time_h" = "xyes"	&& 				\
		test "x$have_sys_types_h" = "xyes"	&& 				\
		test "x$have_unistd_h" = "xyes")					\
		)])

AM_CONDITIONAL(MIDI, [test "x$feat_MIDI" = "xyes" && test "x$have_malloc" = "xyes"	\
	&& test "x$have_realloc" = "xyes" && test "x$have_memset" = "xyes"		\
	&& test "x$have_memcpy" = "xyes" && test "x$have_qsort" = "xyes"])

AM_CONDITIONAL(EXEC, [test "x$feat_EXEC" = "xyes" && test "x$have_stdlib_h" = "xyes"	\
	&& test "x$have_system" = "xyes"])

AM_CONDITIONAL(GSTREAMER, [test "x$feat_GST" = "xyes" && test "x$libgstreamer" = "xyes"])

AM_CONDITIONAL(FILE, [test "x$feat_FILE" = "xyes" && test "x$have_fwrite" = "xyes"])

AM_CONDITIONAL(SPKR, [test "x$feat_SPKR" = "xyes" && test "x$have_ioctl" = "xyes"		\
	&& test "x$have_sys_kd_h" = "xyes" && test "x$have_kiocsound" = "xyes"			\
	&& test "x$have_sys_ioctl_h" = "xyes" && test "x$have_fcntl_h" = "xyes"			\
	&& test "x$have_unistd_h" = "xyes" && test "x$have_open" = "xyes"			\
	&& test "x$have_close" = "xyes"	&& test "x$have_select" = "xyes" 			\
	&& ( test "x$have_sys_select_h" = "xyes" || (						\
		test "x$have_sys_time_h" = "xyes"	&& 					\
		test "x$have_sys_types_h" = "xyes")						\
		)])

imyp_flex_enabled=no
if (       test "x$feat_FLEX" = "xyes" && test "x$have_flex" = "xyes"	\
	&& test "x$have_unistd_h" = "xyes" &&  test "x$have_errno_h" = "xyes"		\
	&& test "x$have_stdlib_h" = "xyes" &&  test "x$have_inttypes_h" = "xyes"	\
	&& test "x$have_malloc" = "xyes" && test "x$have_realloc" = "xyes"); then

        imyp_flex_enabled=yes
fi


AM_CONDITIONAL(FLEX, [test "x$imyp_flex_enabled" = "xyes" ])

# ===================== Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_VOLATILE
AC_TYPE_LONG_LONG_INT
AC_TYPE_OFF_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_TYPE_UINT32_T
AC_CHECK_TYPES([sig_atomic_t],,,
	[
	#ifdef HAVE_SIGNAL_H
	#include <signal.h>
	#endif
	])

# Checks for library functions.
# AC_ TYPE_ SIGNAL
# m4_warn([obsolete],
# [your code may safely assume C89 semantics that RETSIGTYPE is void.
# Remove this warning and the AC_CACHE_CHECK when you adjust the code.])dnl
AC_CACHE_CHECK([return type of signal handlers],[ac_cv_type_signal],[AC_COMPILE_IFELSE(
[AC_LANG_PROGRAM([#include <sys/types.h>
#include <signal.h>
],
		 [return *(signal (0, 0)) (0) == 1;])],
		   [ac_cv_type_signal=int],
		   [ac_cv_type_signal=void])])
AC_DEFINE_UNQUOTED([RETSIGTYPE],[$ac_cv_type_signal],[Define as the return type of signal handlers
		    (int or void).])

AC_FUNC_MALLOC

AC_CHECK_FUNCS([setlocale sigaction snprintf basename getopt_long \
	strcasecmp strdup])
if ( test "x$imyp_flex_enabled" != "xyes" ); then

	AC_FUNC_FSEEKO
	AC_CHECK_FUNCS([ftello])
fi

AM_GNU_GETTEXT_VERSION([0.21])
AM_GNU_GETTEXT([external])

# Can be /usr/local/include/libintl.h. Use --with-libintl-prefix=DIR then.
# The macros above set the correct CFLAGS if this option is given.

AC_CHECK_HEADERS([libintl.h])

have_check=no
#ACTION-IF-NOT-FOUND is required, otherwise 'configure' fails
PKG_CHECK_MODULES([CHECK], [check >= 0.9.4], [have_check=yes], [have_check=no])
#AM_PATH_CHECK([], [have_check=yes])

AM_CONDITIONAL([IMY_TESTS_ENABLED], [test "x$have_check" = "xyes"])

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
	AX_C_CHECK_FLAG([-Wformat -Wformat-overflow=2], [CFLAGS="$CFLAGS -Wformat -Wformat-overflow=2"], [CFLAGS="$CFLAGS"])
	AX_C_CHECK_FLAG([-Wnested-externs], [CFLAGS="$CFLAGS -Wnested-externs"], [CFLAGS="$CFLAGS"])
	AX_C_CHECK_FLAG([-Wshadow], [CFLAGS="$CFLAGS -Wshadow"], [CFLAGS="$CFLAGS"])
	AX_C_CHECK_FLAG([-Wconversion], [CFLAGS="$CFLAGS -Wconversion"], [CFLAGS="$CFLAGS"])
	AX_C_CHECK_FLAG([-Wdeclaration-after-statement], [CFLAGS="$CFLAGS -Wdeclaration-after-statement"], [CFLAGS="$CFLAGS"])
	AX_C_CHECK_FLAG([-Wundef], [CFLAGS="$CFLAGS -Wundef"], [CFLAGS="$CFLAGS"])
	#AX_C_CHECK_FLAG([-Wpadded], [CFLAGS="$CFLAGS -Wpadded"], [CFLAGS="$CFLAGS"])
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
	AX_C_CHECK_FLAG([-O3], [CFLAGS="$CFLAGS -O3"], [CFLAGS="$CFLAGS"])
	AX_C_CHECK_FLAG([-Waggregate-return], [CFLAGS="$CFLAGS -Waggregate-return"], [CFLAGS="$CFLAGS"])
	AX_C_CHECK_FLAG([-Wwrite-strings], [CFLAGS="$CFLAGS -Wwrite-strings"], [CFLAGS="$CFLAGS"])
        AX_C_CHECK_FLAG([-Wchkp], [CFLAGS="$CFLAGS -Wchkp"], [CFLAGS="$CFLAGS"])
        AX_C_CHECK_FLAG([-Wduplicated-cond], [CFLAGS="$CFLAGS -Wduplicated-cond"], [CFLAGS="$CFLAGS"])
        AX_C_CHECK_FLAG([-Wrestrict], [CFLAGS="$CFLAGS -Wrestrict"], [CFLAGS="$CFLAGS"])
        AX_C_CHECK_FLAG([-Woverlength-strings], [CFLAGS="$CFLAGS -Woverlength-strings"], [CFLAGS="$CFLAGS"])
        AX_C_CHECK_FLAG([-Wno-nonnull-compare], [CFLAGS="$CFLAGS -Wno-nonnull-compare"], [CFLAGS="$CFLAGS"])
        AX_C_CHECK_FLAG([-Wstringop-truncation], [CFLAGS="$CFLAGS -Wstringop-truncation"], [CFLAGS="$CFLAGS"])
        AX_C_CHECK_FLAG([-Wduplicated-branches], [CFLAGS="$CFLAGS -Wduplicated-branches"], [CFLAGS="$CFLAGS"])
        AX_C_CHECK_FLAG([-fstack-check], [CFLAGS="$CFLAGS -fstack-check"], [CFLAGS="$CFLAGS"])
        # disabled, because of too much noise from the generated flex output file:
        #AX_C_CHECK_FLAG([-fanalyzer], [CFLAGS="$CFLAGS -fanalyzer"], [CFLAGS="$CFLAGS"])
        AX_C_CHECK_FLAG([-g], [CFLAGS="$CFLAGS -g"], [CFLAGS="$CFLAGS"])	# needed for the 'debuginfo' rpm package

#	if (test "x$have_ll" = "xyes"); then
#
#		AX_C_CHECK_FLAG([-std=gnu99], [CFLAGS="$CFLAGS -std=gnu99"], [CFLAGS="$CFLAGS"])
#		AX_C_CHECK_FLAG([-Wno-long-long], [CFLAGS="$CFLAGS -Wno-long-long"], [CFLAGS="$CFLAGS"])
#	fi
fi

# this doubles the CFLAGS on compile:
#AC_SUBST(AM_CFLAGS,$CFLAGS)

# =====================

AC_OUTPUT

# ===================== Print summary

echo "***********************************"

if (	test "x$feat_allegro" = "xyes" && (test "x$liballeg" = "xyes" || test "x$alleg_ldflags" != "x") && \
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

# SDLv2 has functions with the same names as those in SDLv1,
# so enable at most one of the libraries at the same time
if (	test "x$feat_SDL2" = "xyes" && test "x$libsdl2" = "xyes" && test "x$have_sdl2_h" = "xyes"); then

	echo " *	SDLv2: yes"
else
	if (test "x$feat_SDL2" != "xyes"); then
		echo " *	SDLv2: no (disabled by command line)"

	elif ( test "x$libsdl2" != "xyes" ); then
		echo " *	SDLv2: no (library missing or unusable)"

	else	#if (test "x$have_sdl2_h" != "xyes"); then
		echo " *	SDLv2: no (SDL.h header missing)"
	fi
fi

AM_CONDITIONAL(SDL2, [test "x$feat_SDL2" = "xyes" && test "x$libsdl2" = "xyes"		\
	&& test "x$have_sdl2_h" = "xyes"])

if (	test "x$feat_SDL" = "xyes" && test "x$libsdl" = "xyes" \
	&& test "x$have_sdl_h" = "xyes" && 
	(test "x$feat_SDL2" != "xyes" || test "x$libsdl2" != "xyes"		\
	|| test "x$have_sdl2_h" != "xyes")); then

	echo " *	SDLv1: yes"
else
	if (test "x$feat_SDL2" = "xyes" && test "x$libsdl2" = "xyes"		\
		&& test "x$have_sdl2_h" = "xyes"); then
		echo " *	SDLv1: no (disabled by SDLv2)"

	elif (test "x$feat_SDL" != "xyes"); then
		echo " *	SDLv1: no (disabled by command line)"

	elif ( test "x$libsdl" != "xyes" ); then
		echo " *	SDLv1: no (library missing or unusable)"

	else	#if (test "x$have_sdl_h" != "xyes"); then
		echo " *	SDLv1: no (SDL.h header missing)"
	fi
fi

if (	test "x$feat_ALSA" = "xyes" && test "x$libalsa" = "xyes" &&			\
	test "x$have_asoundlib_h" = "xyes"	\
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

if (	test "x$feat_OSS" = "xyes" && test "x$have_ioctl" = "xyes"			\
	&& test "x$have_sys_soundcard_h" = "xyes"		\
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

if (	test "x$feat_LIBAO" = "xyes" && test "x$libao" = "xyes" && test "x$have_ao_h" = "xyes"	\
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

if (	test "x$feat_PORTAUDIO" = "xyes" && test "x$libportaudio" = "xyes" &&		\
	 test "x$have_portaudio_h" = "xyes" ); then

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

if (	test "x$feat_PULSEAUDIO" = "xyes" && test "x$libpulseaudio_simple" = "xyes" &&	\
	 test "x$libpulseaudio" = "xyes" && test "x$have_pulseaudio_simple_h" = "xyes" ); then

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

if (	test "x$feat_JACK" = "xyes" && test "x$libjack" = "xyes" 			\
	&& test "x$have_jack_h" = "xyes" && test "x$have_select" = "xyes" &&		\
	 ( test "x$have_sys_select_h" = "xyes" || (	\
		test "x$have_sys_time_h" = "xyes"	&& 				\
		test "x$have_sys_types_h" = "xyes"	&& 				\
		test "x$have_unistd_h" = "xyes")					\
		) ); then

	echo " *	JACK: yes"
else
	if (test "x$feat_JACK" != "xyes"); then
		echo " *	JACK: no (disabled by command line)"

	elif ( test "x$libjack" != "xyes" ); then
		echo " *	JACK: no (library missing or unusable)"

	elif ( test "x$have_select" != "xyes" ); then
		echo " *	JACK: no (select() function missing)"

	elif ( ! ( test "x$have_sys_select_h" = "xyes" || (				\
		test "x$have_sys_time_h" = "xyes"	&& 				\
		test "x$have_sys_types_h" = "xyes"	&& 				\
		test "x$have_unistd_h" = "xyes")					\
		) ); then
		echo " *	JACK: no (select() headers missing or unusable)"

	else	#(test "x$have_jack_h" != "xyes"); then
		echo " *	JACK: no (jack.h header missing)"
	fi
fi

if (	test "x$feat_MIDI" = "xyes" && test "x$have_malloc" = "xyes"	\
	&& test "x$have_realloc" = "xyes" && test "x$have_memset" = "xyes" \
	&& test "x$have_memcpy" = "xyes" && test "x$have_qsort" = "xyes"); then

	echo " *	MIDI: yes"
else
	if (test "x$feat_MIDI" != "xyes"); then
		echo " *	MIDI: no (disabled by command line)"

	elif ( test "x$have_malloc" != "xyes" ); then
		echo " *	MIDI: no (malloc() function missing)"

	elif ( test "x$have_realloc" != "xyes" ); then
		echo " *	MIDI: no (realloc() function missing)"

	elif ( test "x$have_memset" != "xyes" ); then
		echo " *	MIDI: no (memset() function missing)"

	elif ( test "x$have_memcpy" != "xyes" ); then
		echo " *	MIDI: no (memcpy() function missing)"

	else	#( test "x$have_qsort" != "xyes" ); then
		echo " *	MIDI: no (qsort() function missing)"

	fi
fi

if (	test "x$feat_EXEC" = "xyes" && test "x$have_stdlib_h" = "xyes" \
	&& test "x$have_system" = "xyes" ); then

	echo " *	EXEC: yes"
else
	if (test "x$feat_EXEC" != "xyes"); then
		echo " *	EXEC: no (disabled by command line)"

	elif ( test "x$have_stdlib_h" != "xyes" ); then
		echo " *	EXEC: no (stdlib.h header missing)"

	else	#( test "x$have_system" != "xyes" ); then
		echo " *	EXEC: no (system() function missing)"

	fi
fi

if (	test "x$feat_GST" = "xyes" && test "x$libgstreamer" = "xyes" ); then

	echo " *	GStreamer: yes"
else
	if (test "x$feat_GST" != "xyes"); then
		echo " *	GStreamer: no (disabled by command line)"

	elif ( test "x$PKG_CONFIG" = "x" ); then
		echo " *	GStreamer: no (pkg-config missing)"

	else	#( test "x$libgstreamer" != "xyes" ); then
		echo " *	GStreamer: no (library or headers missing)"

	fi
fi

if (	test "x$feat_FILE" = "xyes" && test "x$have_fwrite" = "xyes" ); then

	echo " *	FILE: yes"
else
	if (test "x$feat_FILE" != "xyes"); then
		echo " *	FILE: no (disabled by command line)"

	fi
fi

if (	test "x$feat_SPKR" = "xyes" && test "x$have_ioctl" = "xyes"			\
	&& test "x$have_sys_kd_h" = "xyes" && test "x$have_kiocsound" = "xyes"		\
	&& test "x$have_sys_ioctl_h" = "xyes" && test "x$have_fcntl_h" = "xyes"		\
	&& test "x$have_unistd_h" = "xyes" && test "x$have_open" = "xyes"		\
	&& test "x$have_close" = "xyes" && test "x$have_select" = "xyes" &&		\
	 ( test "x$have_sys_select_h" = "xyes" || (	\
		test "x$have_sys_time_h" = "xyes"	&& 				\
		test "x$have_sys_types_h" = "xyes")					\
		)									\
	); then

	echo " *	PC-Speaker: yes"
else
	if (test "x$feat_SPKR" != "xyes"); then
		echo " *	PC-Speaker: no (disabled by command line)"

	elif (test "x$have_sys_kd_h" != "xyes"); then
		echo " *	PC-Speaker: no (sys/kd.h header missing)"

	elif (test "x$have_kiocsound" != "xyes"); then
		echo " *	PC-Speaker: no (KIOCSOUND not defined)"

	elif ( test "x$have_ioctl" != "xyes" ); then
		echo " *	PC-Speaker: no (ioctl() function missing)"

	elif ( test "x$have_sys_ioctl_h" != "xyes" ); then
		echo " *	PC-Speaker: no (sys/ioctl.h header missing)"

	elif ( test "x$have_fcntl_h" != "xyes" ); then
		echo " *	PC-Speaker: no (fcntl.h header missing)"

	elif ( test "x$have_unistd_h" != "xyes" ); then
		echo " *	PC-Speaker: no (unistd.h header missing)"

	elif ( test "x$have_open" != "xyes" ); then
		echo " *	PC-Speaker: no (open() function missing)"

	elif ( test "x$have_close" != "xyes" ); then
		echo " *	PC-Speaker: no (close() function missing)"

	elif ( test "x$have_select" != "xyes" ); then
		echo " *	PC-Speaker: no (select() function missing)"

	elif ( ! ( test "x$have_sys_select_h" = "xyes" || (				\
		test "x$have_sys_time_h" = "xyes"	&& 				\
		test "x$have_sys_types_h" = "xyes"	&& 				\
		test "x$have_unistd_h" = "xyes")					\
		) ); then
		echo " *	PC-Speaker: no (select() headers missing or unusable)"
	else
		echo " *	PC-Speaker: no (reason unknown)"

	fi
fi

echo "***********************************"

if (	test "x$feat_FLEX" = "xyes" && test "x$have_flex" = "xyes"			\
	&& test "x$have_unistd_h" = "xyes" &&  test "x$have_errno_h" = "xyes"		\
	&& test "x$have_stdlib_h" = "xyes" &&  test "x$have_inttypes_h" = "xyes"	\
	&& test "x$have_malloc" = "xyes" && test "x$have_realloc" = "xyes" ); then

	echo " *	Flex parser: yes"
	AC_MSG_NOTICE([[Flex-based parser enabled. If it does not compile for]])
	AC_MSG_NOTICE([[you or works incorrectly, re-configure IMYplay with]])
	AC_MSG_NOTICE([[./configure --enable-FLEXPARSER=no]])
else
	if (test "x$feat_FLEX" != "xyes"); then
		echo " *	Flex parser: no (disabled by command line)"

	elif ( test "x$have_flex" != "xyes" ); then
		echo " *	Flex parser: no (flex missing)"

	elif ( test "x$have_unistd_h" != "xyes" ); then
		echo " *	Flex parser: no (unistd.h missing)"

	elif ( test "x$have_errno_h" != "xyes" ); then
		echo " *	Flex parser: no (errno.h missing)"

	elif ( test "x$have_stdlib_h" != "xyes" ); then
		echo " *	Flex parser: no (stdlib.h missing)"

	elif ( test "x$have_inttypes_h" != "xyes" ); then
		echo " *	Flex parser: no (inttypes.h missing)"

	elif ( test "x$have_malloc" != "xyes" ); then
		echo " *	Flex parser: no (malloc() function missing)"

	elif ( test "x$have_realloc" != "xyes" ); then
		echo " *	Flex parser: no (realloc() function missing)"

	else	#( test "x$have_flex" != "xyes" ); then
		echo " *	Flex parser: no (unknown reason)"

	fi
fi

if (	test "x$feat_LIBHIDEIP" = "xyes" && test "x$libhip" = "xyes" \
	&& test "x$have_libhideip_h" = "xyes" ); then

	echo " *	LibHideIP enabled: yes"
	AC_MSG_WARN([[LibHideIP may cause network sound output not to work.]])
	AC_MSG_WARN([[If you do not wish to enable LibHideIP, re-configure IMYplay with]])
	AC_MSG_WARN([[./configure --enable-LIBHIDEIP=no]])
else
	if (test "x$feat_LIBHIDEIP" != "xyes"); then
		echo " *	LibHideIP enabled: no (default/disabled by command line)"

	elif ( test "x$libhip" != "xyes" ); then
		echo " *	LibHideIP enabled: no (library missing)"

	else	#( test "x$have_libhideip_h" != "xyes" ); then
		echo " *	LibHideIP enabled: no (libhideip.h header missing)"

	fi
fi

if (	test "x$feat_LIBNETBLOCK" = "xyes" && test "x$libnb" = "xyes" \
	&& test "x$have_libnetblock_h" = "xyes" ); then

	echo " *	LibNetBlock enabled: yes"
	AC_MSG_WARN([[LibNetBlock may cause network sound output not to work.]])
	AC_MSG_WARN([[If you do not wish to enable LibNetBlock, re-configure IMYplay with]])
	AC_MSG_WARN([[./configure --enable-LIBNETBLOCK=no]])
else
	if (test "x$feat_LIBNETBLOCK" != "xyes"); then
		echo " *	LibNetBlock enabled: no (default/disabled by command line)"

	elif ( test "x$libnb" != "xyes" ); then
		echo " *	LibNetBlock enabled: no (library missing)"

	else	#( test "x$have_libnetblock_h" != "xyes" ); then
		echo " *	LibNetBlock enabled: no (libnetblock.h header missing)"

	fi
fi

echo "***********************************"
