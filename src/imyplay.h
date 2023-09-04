/*
 * IMYplay - A program for playing iMelody ringtones (IMY files).
 *	-- main file's header file.
 *
 * Copyright (C) 2009-2023 Bogdan Drozdowski, bogdro (at) users.sourceforge.net
 * License: GNU General Public License, v3+
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _IMYPLAYER_H
# define _IMYPLAYER_H 1

/* define _GNU_SOURCE before any headers */
# define _GNU_SOURCE	1	/* getopt_long() */

# include "imyp_cfg.h"

# define IMYP_ROUND(x) ( (((x) - (int)(x)) >= 0.5)? ((x)+1) : (x) )

# undef IMYP_ATTR
# ifdef __GNUC__
#  define IMYP_ATTR(x)	__attribute__(x)
# else
#  define IMYP_ATTR(x)
# endif

# define IMYP_SAMPBUFSIZE (128*1024)
# define IMYP_MAX_IMY_VOLUME 15
# define IMYP_MAX(a,b) ( ( (a) > (b) )? (a) : (b) )
# define IMYP_MIN(a,b) ( ( (a) < (b) )? (a) : (b) )

# ifdef HAVE_SYS_TYPES_H
#  include <sys/types.h>
# endif

# ifdef HAVE_GETTEXT
#  ifndef _
#   define 	_(String)		gettext (String)
#  endif
# else
#  define 	_(String)		String
# endif

# define	gettext_noop(String)	String
# define	N_(String)		String

# ifdef HAVE_SIGNAL_H
#  include <signal.h>
# endif
# ifndef HAVE_SIG_ATOMIC_T
typedef int sig_atomic_t;
# endif

/* IMYP_PARAMS is a macro used to wrap function prototypes, so that
   compilers that don't understand ANSI C prototypes still work,
   and ANSI C compilers can issue warnings about type mismatches. */
# undef IMYP_PARAMS
# undef IMYP_ANSIC
# if defined (__STDC__) || defined (_AIX) \
	|| (defined (__mips) && defined (_SYSTYPE_SVR4)) \
	|| defined (WIN32) || defined (__cplusplus)
#  define IMYP_PARAMS(protos) protos
#  define IMYP_ANSIC
# else
#  define IMYP_PARAMS(protos) ()
#  undef IMYP_ANSIC
# endif

# undef IMYP_HAVE_SELECT
# if ((defined HAVE_SYS_SELECT_H) || (defined TIME_WITH_SYS_TIME)\
	|| (defined HAVE_SYS_TIME_H) || (defined HAVE_TIME_H))	\
	&& (defined HAVE_SELECT)
#  define IMYP_HAVE_SELECT 1
# endif

# define IMYP_TOUPPER(c) ((char)( ((c) >= 'a' && (c) <= 'z')? ((c) & 0x5F) : (c) ))
# define IMYP_IS_DIGIT(c) ( ((c) >= '0') && ((c) <= '9') )
# define IMYP_DEF_BPM 120


typedef int imyp_error_type;

extern void
# ifdef IMYP_ANSIC
	IMYP_ATTR ((nonnull))
# endif
	imyp_show_error IMYP_PARAMS ((const imyp_error_type err, const char * const msg, const char * const extra));

extern const char * const err_msg;
extern const char * const imyp_sig_unk;
extern const char * const imyp_err_msg_signal;
extern const char * const ver_msg_compiled;
extern const char * const ver_msg_runtime;

enum IMYP_CURR_LIB
{
	IMYP_CURR_NONE,
	IMYP_CURR_ALLEGRO,
	IMYP_CURR_MIDI,
	IMYP_CURR_SDL,
	IMYP_CURR_ALSA,
	IMYP_CURR_OSS,
	IMYP_CURR_LIBAO,
	IMYP_CURR_PORTAUDIO,
	IMYP_CURR_JACK,
	IMYP_CURR_PULSEAUDIO,
	IMYP_CURR_EXEC,
	IMYP_CURR_GSTREAMER,
	IMYP_CURR_FILE,
	IMYP_CURR_SPKR
};

struct imyp_backend_data
{
	char dummy;
};

typedef struct imyp_backend_data imyp_backend_data_t;

struct imyp_backend
{
	imyp_backend_data_t * imyp_data;
	enum IMYP_CURR_LIB imyp_curr_lib;
};

typedef struct imyp_backend imyp_backend_t;

/* ================= Available libraries ====================== */
# if (defined HAVE_LIBALLEG) && (defined HAVE_ALLEGRO_H)
#  define IMYP_HAVE_ALLEGRO	1
# else
#  undef IMYP_HAVE_ALLEGRO
# endif

# if (defined HAVE_LIBSDL2) && (defined HAVE_SDL2_SDL_H)
#  define IMYP_HAVE_SDL2		1
# else
#  undef IMYP_HAVE_SDL2
# endif

# if (defined HAVE_LIBSDL) && (defined HAVE_SDL_SDL_H) && (!defined IMYP_HAVE_SDL2)
#  define IMYP_HAVE_SDL		1
# else
#  undef IMYP_HAVE_SDL
# endif

# if (defined HAVE_LIBASOUND) && ((defined HAVE_ASOUNDLIB_H) || (defined HAVE_ALSA_ASOUNDLIB_H))
#  define IMYP_HAVE_ALSA	1
# else
#  undef IMYP_HAVE_ALSA
# endif

#if (defined HAVE_IOCTL) && (defined HAVE_SYS_SOUNDCARD_H) && (defined HAVE_SYS_IOCTL_H) \
 && (defined HAVE_FCNTL_H) && (defined HAVE_UNISTD_H) && (defined HAVE_OPEN)		\
 && (defined HAVE_CLOSE) && (defined HAVE_WRITE) && (defined HAVE_SELECT)		\
 && ((defined HAVE_SYS_SELECT_H) || (((defined TIME_WITH_SYS_TIME)				\
	|| (defined HAVE_SYS_TIME_H) || (defined HAVE_TIME_H))				\
 && (defined HAVE_UNISTD_H)))
#  define IMYP_HAVE_OSS		1
# else
#  undef IMYP_HAVE_OSS
# endif

# if (defined HAVE_LIBAO) && ((defined HAVE_AO_H) || (defined HAVE_AO_AO_H))
#  define IMYP_HAVE_LIBAO	1
# else
#  undef IMYP_HAVE_LIBAO
# endif

# if (defined HAVE_LIBPORTAUDIO) && ((defined HAVE_PORTAUDIO_H) || (defined HAVE_PORTAUDIO_PORTAUDIO_H))
#  define IMYP_HAVE_PORTAUDIO	1
# else
#  undef IMYP_HAVE_PORTAUDIO
# endif

# if (defined HAVE_LIBJACK) && ((defined HAVE_JACK_H) || (defined HAVE_JACK_JACK_H))
#  define IMYP_HAVE_JACK	1
# else
#  undef IMYP_HAVE_JACK
# endif

# if (defined HAVE_LIBPULSE) && (defined HAVE_LIBPULSE_SIMPLE) && \
	((defined HAVE_SIMPLE_H) || (defined HAVE_PULSE_SIMPLE_H))
#  define IMYP_HAVE_PULSEAUDIO	1
# else
#  undef IMYP_HAVE_PULSEAUDIO
# endif

# if (defined HAVE_MALLOC) && (defined HAVE_REALLOC) && (defined HAVE_MEMSET) \
	&& (defined HAVE_MEMCPY) && (defined HAVE_QSORT)
#  define IMYP_HAVE_MIDI	1
# else
#  undef IMYP_HAVE_MIDI
# endif

# if (defined HAVE_STDLIB_H) && (defined HAVE_SYSTEM)
#  define IMYP_HAVE_EXEC	1
# else
#  undef IMYP_HAVE_EXEC
# endif

# ifdef HAVE_GST_GST_H
#  define IMYP_HAVE_GST		1
# else
#  undef IMYP_HAVE_GST
# endif

/*
Raw file output uses standard C functions, so should be available everywhere.
But check if it was enabled on the command line of ./configure. If HAVE_FWRITE
is defined, then the FILE backend has been enabled on the command line.
*/
# ifdef HAVE_FWRITE
#  define IMYP_HAVE_FILE		1
# else
#  undef IMYP_HAVE_FILE
# endif

# if (defined HAVE_IOCTL) && (defined HAVE_SYS_KD_H)		\
 && (defined HAVE_SYS_IOCTL_H) && (defined HAVE_FCNTL_H) && (defined HAVE_UNISTD_H)	\
 && (defined HAVE_OPEN) && (defined HAVE_CLOSE) && (defined HAVE_SELECT)		\
 && ((defined HAVE_SYS_SELECT_H) || (((defined TIME_WITH_SYS_TIME)			\
	|| (defined HAVE_SYS_TIME_H) || (defined HAVE_TIME_H))				\
 && (defined HAVE_UNISTD_H))) || (defined IMYP_IS_DOS)
#  define IMYP_HAVE_SPKR	1
# else
#  undef IMYP_HAVE_SPKR
# endif

# if (defined HAVE_LIBHIDEIP) && (defined HAVE_LIBHIDEIP_H)
#  define IMYP_HAVE_LIBHIDEIP	1
# else
#  undef IMYP_HAVE_LIBHIDEIP
# endif

# if (defined HAVE_LIBNETBLOCK) && (defined HAVE_LIBNETBLOCK_H)
#  define IMYP_HAVE_LIBNETBLOCK	1
# else
#  undef IMYP_HAVE_LIBNETBLOCK
# endif

#endif /* _IMYPLAYER_H */
