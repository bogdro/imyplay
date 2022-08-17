/*
 * A program for playing iMelody ringtones (IMY files).
 *	-- signal handling.
 *
 * Copyright (C) 2009 Bogdan Drozdowski, bogdandr (at) op.pl
 * License: GNU General Public License, v3+
 *
 * Syntax example: imyplay ringtone.imy
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
 * along with this program; if not, write to the Free Software Foudation:
 *		Free Software Foundation
 *		51 Franklin Street, Fifth Floor
 *		Boston, MA 02110-1301
 *		USA
 */

#include "imyp_cfg.h"

#include "imyplay.h"

#ifdef HAVE_ERRNO_H
# include <errno.h>
#endif

#include <stdio.h>

#ifdef HAVE_STRING_H
# if ((!defined STDC_HEADERS) || (!STDC_HEADERS)) && (defined HAVE_MEMORY_H)
#  include <memory.h>
# endif
# include <string.h>
#endif

#ifdef HAVE_LIBINTL_H
# include <libintl.h>	/* translation stuff */
#endif

#ifdef HAVE_LOCALE_H
# include <locale.h>
#endif

#ifdef HAVE_SIGNAL_H
# include <signal.h>
#endif

#include "imyp_sig.h"

/* This has to be public: */
volatile sig_atomic_t sig_recvd = 0;		/* non-zero after signal received */

#ifdef HAVE_SIGNAL_H
# if (defined HAVE_SIGACTION) && (!defined __STRICT_ANSI__)
static struct sigaction sa/* = { .sa_handler = &term_signal_received }*/;
# endif
/* Handled signals which will cause the program to exit cleanly. */
static const int signals[] =
{
	SIGINT
# ifdef SIGQUIT
	, SIGQUIT
# endif
# ifdef SIGILL
	, SIGILL
# endif
# ifdef SIGABRT
	, SIGABRT
# endif
# ifdef SIGFPE
	, SIGFPE
# endif
# ifdef SIGSEGV
	, SIGSEGV
# endif
# ifdef SIGPIPE
	, SIGPIPE
# endif
# ifdef SIGALRM
	, SIGALRM
# endif
# ifdef SIGTERM
	, SIGTERM
# endif
# ifdef SIGUSR1
	, SIGUSR1
# endif
# ifdef SIGUSR2
	, SIGUSR2
# endif
# ifdef SIGTTIN
	, SIGTTIN
# endif
# ifdef SIGTTOU
	, SIGTTOU
# endif
# ifdef SIGBUS
	, SIGBUS
# endif
# ifdef SIGPROF
	, SIGPROF
# endif
# ifdef SIGSYS
	, SIGSYS
# endif
# ifdef SIGTRAP
	, SIGTRAP
# endif
# ifdef SIGXCPU
	, SIGXCPU
# endif
# ifdef SIGXFSZ
	, SIGXFSZ
# endif
# ifdef SIGVTALRM
	, SIGVTALRM
# endif
# ifdef SIGCHLD
	, SIGCHLD
# endif
# ifdef SIGPOLL
	, SIGPOLL
# endif
# ifdef SIGPWR
	, SIGPWR
# endif
# ifdef SIGUNUSED
	, SIGUNUSED
# endif
# ifdef SIGEMT
	, SIGEMT
# endif
# ifdef SIGLOST
	, SIGLOST
# endif
# ifdef SIGIO
	, SIGIO
# endif
};

# ifndef RETSIGTYPE
#  define RETSIGTYPE void
# endif

/**
 * Signal handler - Sets a flag which will stop further program operations, when a
 * signal which would normally terminate the program is received.
 * \param signum Signal number.
 */
static RETSIGTYPE
term_signal_received (
# if defined (__STDC__) || defined (_AIX) \
	|| (defined (__mips) && defined (_SYSTYPE_SVR4)) \
	|| defined(WIN32) || defined(__cplusplus)
	const int signum)
# else
	signum)
	const int signum;
# endif
{
	sig_recvd = signum;
# define void 1
# define int 2
# if RETSIGTYPE != void
	return 0;
# endif
# undef int
# undef void
}

#endif /* HAVE_SIGNAL_H */

/* =============================================================== */

void
imyp_set_sigh (
#if defined (__STDC__) || defined (_AIX) \
	|| (defined (__mips) && defined (_SYSTYPE_SVR4)) \
	|| defined(WIN32) || defined(__cplusplus)
	error_type * const error)
#else
	error)
	error_type * const error;
#endif
{
#ifdef HAVE_SIGNAL_H
# define 	TMPSIZE	12
	char tmp[TMPSIZE];		/* Place for a signal number in case of error. */
	int res;			/* s(n)printf result */
	size_t s;			/* sizeof(signals) */
# if (defined __STRICT_ANSI__) && (defined HAVE_SIGNAL_H)
	typedef void (*sighandler_t) (int);
	sighandler_t shndlr;
# endif

	/*
	 * Setting signal handlers. We need to catch signals in order to close
	 * the library and the program in a normal way.
	 */

# if (!defined HAVE_SIGACTION) || (defined __STRICT_ANSI__)
	/* ANSI C */
	for ( s=0; s < sizeof (signals) / sizeof (signals[0]); s++ )
	{
#  ifdef HAVE_ERRNO_H
		errno = 0;
#  endif
		shndlr = signal ( signals[s], &term_signal_received );
		if ( (shndlr == SIG_ERR)
#  ifdef HAVE_ERRNO_H
/*			|| (errno != 0)*/
#  endif
		   )
		{
#  ifdef HAVE_ERRNO_H
			if ( error != NULL ) *error = errno;
#  else
			if ( error != NULL ) *error = 1L;
#  endif
#  ifdef HAVE_SNPRINTF
			res = snprintf (tmp, TMPSIZE-1, "%.*d", TMPSIZE-1, signals[s] );
#  else
			res = sprintf (tmp, "%.*d", TMPSIZE-1, signals[s] );
#  endif
			tmp[TMPSIZE-1] = '\0';
			if ( error != NULL )
			{
				if ( *error == 0 ) *error = 1L;
			}
			if ( error != NULL )
			{
				show_error ( *error, err_msg_signal, (res>0)? tmp : _(sig_unk) );
			}
			else
			{
				show_error ( 1, err_msg_signal, (res>0)? tmp : _(sig_unk) );
			}
		}
	}

# else
	/* more than ANSI C */
#  ifdef HAVE_MEMSET
	memset (&sa, 0, sizeof (struct sigaction));
#  else
	for ( s=0; s < sizeof (struct sigaction); s++ )
	{
		((char *)&sa)[s] = '\0';
	}
#  endif
	sa.sa_handler = &term_signal_received;
	for ( s=0; s < sizeof (signals) / sizeof (signals[0]); s++ )
	{
#  ifdef HAVE_ERRNO_H
		errno = 0;
#  endif
		res = sigaction ( signals[s], &sa, NULL);
		if ( (res != 0)
#  ifdef HAVE_ERRNO_H
/*			|| (errno != 0)*/
#  endif
		   )
		{
#  ifdef HAVE_ERRNO_H
			if ( error != NULL ) *error = errno;
#  else
			if ( error != NULL ) *error = 1L;
#  endif

#  ifdef HAVE_SNPRINTF
			res = snprintf (tmp, TMPSIZE-1, "%.*d", TMPSIZE-1, signals[s] );
#  else
			res = sprintf (tmp, "%.*d", TMPSIZE-1, signals[s] );
#  endif
			tmp[TMPSIZE-1] = '\0';
			if ( error != NULL )
			{
				if ( *error == 0 ) *error = 1L;
			}
			if ( error != NULL )
			{
				show_error ( *error, err_msg_signal, (res>0)? tmp : _(sig_unk) );
			}
			else
			{
				show_error ( 1, err_msg_signal, (res>0)? tmp : _(sig_unk) );
			}
		}
	}
# endif		/* ! ANSI C */

#endif /* HAVE_SIGNAL_H */
}
