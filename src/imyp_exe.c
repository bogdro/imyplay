/*
 * A program for playing iMelody ringtones (IMY files).
 *	-- EXEC backend.
 *
 * Copyright (C) 2009-2011 Bogdan Drozdowski, bogdandr (at) op.pl
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
 * along with this program; if not, write to the Free Software Foudation:
 *		Free Software Foundation
 *		51 Franklin Street, Fifth Floor
 *		Boston, MA 02110-1301
 *		USA
 */

#include "imyp_cfg.h"

#include "imyplay.h"
#include "imyp_exe.h"
#include "imyp_sig.h"

#include <stdio.h>

#if (defined HAVE_STDLIB_H) && (defined HAVE_SYSTEM)
# include <stdlib.h>
#else
# error Exec otput selected, but stdlib.h or sysytem() not found.
#endif

#ifdef HAVE_STRING_H
# if ((!defined STDC_HEADERS) || (!STDC_HEADERS)) && (defined HAVE_MEMORY_H)
#  include <memory.h>
# endif
# include <string.h>
#endif

#ifdef HAVE_SYS_WAIT_H
# include <sys/wait.h>
#else
# ifdef HAVE_WAIT_H
#  include <wait.h>
# endif
#endif
# ifndef WIFEXITED
#  define WIFEXITED(stat_val) (((stat_val) & 255) == 0)
# endif
#ifndef WEXITSTATUS
# define WEXITSTATUS(stat_val) ((unsigned)(stat_val) >> 8)
#endif
#ifndef WIFSIGNALED
# define WIFSIGNALED(status) (((signed char) (((status) & 0x7f) + 1) >> 1) > 0)
#endif

#define IMYP_MAX_PROG_LEN 4097
static char exename[IMYP_MAX_PROG_LEN] = {0};
static char to_execute[IMYP_MAX_PROG_LEN] = {0};



#ifndef IMYP_ANSIC
static int launch_program PARAMS((const double freq, const int volume_level, const int duration));
#endif

static int launch_program (
#ifdef IMYP_ANSIC
	const double freq,
	const int volume_level,
	const int duration)
#else
	freq, volume_level, duration)
	const double freq;
	const int volume_level;
	const int duration;
#endif
{
	size_t i, j, exe_index = 0;
	int sys_ret;

	i = strlen (exename);
	for ( j = 0; j < IMYP_MAX_PROG_LEN; j++ )
	{
		to_execute[j] = '\0';
	}
	for ( j = 0; j < IMYP_MIN (i, IMYP_MAX_PROG_LEN-1); j++ )
	{
		if ( exename[j] != '%' )
		{
			to_execute[exe_index] = exename[j];
			exe_index++;
			continue;
		}
		j++;
		switch (exename[j])
		{
			case 'i':	/* integer frequency */
#ifdef HAVE_SNPRINTF
				snprintf (&to_execute[exe_index], sizeof (to_execute) - exe_index - 1,
					  "%d", (int)IMYP_ROUND (freq));
#else
				if ( sizeof (to_execute) - exe_index - 1 >= 10 )
				{
					sprintf (&to_execute[exe_index], "%d", (int)IMYP_ROUND (freq));
				}
#endif
				to_execute[IMYP_MAX_PROG_LEN-1] = '\0';
				exe_index = strlen (to_execute);
				break;
			case 'f':	/* float frequency */
#ifdef HAVE_SNPRINTF
				snprintf (&to_execute[exe_index], sizeof (to_execute) - exe_index - 1,
					  "%-.6f", IMYP_ROUND (freq));
#else
				if ( sizeof (to_execute) - exe_index - 1 >= 12 )
				{
					sprintf (&to_execute[exe_index], "%-.6f", freq);
				}
#endif
				to_execute[IMYP_MAX_PROG_LEN-1] = '\0';
				exe_index = strlen (to_execute);
				break;
			case 'd':	/* integer duration in seconds */
#ifdef HAVE_SNPRINTF
				snprintf (&to_execute[exe_index], sizeof (to_execute) - exe_index - 1,
					  "%d", (int)IMYP_ROUND (duration/1000.0));
#else
				if ( sizeof (to_execute) - exe_index - 1 >= 10 )
				{
					sprintf (&to_execute[exe_index], "%d",
						 (int)IMYP_ROUND (duration/1000.0));
				}
#endif
				to_execute[IMYP_MAX_PROG_LEN-1] = '\0';
				exe_index = strlen (to_execute);
				break;
			case 's':	/* float duration in seconds */
#ifdef HAVE_SNPRINTF
				snprintf (&to_execute[exe_index], sizeof (to_execute) - exe_index - 1,
					  "%-.6f", duration/1000.0);
#else
				if ( sizeof (to_execute) - exe_index - 1 >= 12 )
				{
					sprintf (&to_execute[exe_index], "%-.6f", duration/1000.0);
				}
#endif
				to_execute[IMYP_MAX_PROG_LEN-1] = '\0';
				exe_index = strlen (to_execute);
				break;
			case 'm':	/* integer duration in milliseconds */
#ifdef HAVE_SNPRINTF
				snprintf (&to_execute[exe_index], sizeof (to_execute) - exe_index - 1,
					  "%d", duration);
#else
				if ( sizeof (to_execute) - exe_index - 1 >= 10 )
				{
					sprintf (&to_execute[exe_index], "%d", duration);
				}
#endif
				to_execute[IMYP_MAX_PROG_LEN-1] = '\0';
				exe_index = strlen (to_execute);
				break;
			case 'l':	/* float duration in milliseconds */
#ifdef HAVE_SNPRINTF
				snprintf (&to_execute[exe_index], sizeof (to_execute) - exe_index - 1,
					  "%-.6f", duration*1.0);
#else
				if ( sizeof (to_execute) - exe_index - 1 >= 12 )
				{
					sprintf (&to_execute[exe_index], "%-.6f", duration*1.0);
				}
#endif
				to_execute[IMYP_MAX_PROG_LEN-1] = '\0';
				exe_index = strlen (to_execute);
				break;
			case 'v':	/* volume level 0-15 */
#ifdef HAVE_SNPRINTF
				snprintf (&to_execute[exe_index], sizeof (to_execute) - exe_index - 1,
					  "%d", volume_level);
#else
				if ( sizeof (to_execute) - exe_index - 1 >= 10 )
				{
					sprintf (&to_execute[exe_index], "%d", volume_level);
				}
#endif
				to_execute[IMYP_MAX_PROG_LEN-1] = '\0';
				exe_index = strlen (to_execute);
				break;
			default:
				to_execute[exe_index] = '%';	/* copy */
				exe_index++;
				to_execute[exe_index] = exename[j];
				exe_index++;
				break;
		}
	}
	to_execute[IMYP_MAX_PROG_LEN-1] = '\0';
	sys_ret = system (to_execute);
	if ( sys_ret >= 0 )
	{
		sys_ret = WEXITSTATUS (sys_ret);
		if ( WIFSIGNALED (sys_ret) ) sig_recvd = 1; /* anything not zero */
	}
	return sys_ret;
}

/**
 * Play a specified tone.
 * \param freq The frequency of the tone (in Hz).
 * \param volume_level Volume of the tone (from 0 to 15).
 * \param duration The duration of the tone, in milliseconds.
 * \param buf The buffer for samples.
 * \param bufsize The buffer size, in samples.
 * \return 0 on success.
 */
int
imyp_exec_play_tune (
#ifdef IMYP_ANSIC
	const double freq,
	const int volume_level,
	const int duration,
	void * const buf IMYP_ATTR ((unused)),
	int bufsize IMYP_ATTR ((unused)))
#else
	freq, volume_level, duration, buf, bufsize)
	const double freq;
	const int volume_level;
	const int duration;
	void * const buf IMYP_ATTR ((unused));
	int bufsize IMYP_ATTR ((unused));
#endif
{
	return launch_program (freq, volume_level, duration);
}

/**
 * Pauses for the specified period of time.
 * \param milliseconds Number of milliseconds to pause.
 */
void
imyp_exec_pause (
#ifdef IMYP_ANSIC
	const int milliseconds)
#else
	milliseconds)
	const int milliseconds;
#endif
{
	if ( milliseconds <= 0 ) return;
	launch_program (0.0, 0, milliseconds);
}

/**
 * Outputs the given text.
 * \param text The text to output.
 */
void
imyp_exec_put_text (
#ifdef IMYP_ANSIC
	const char * const text)
#else
	text)
	const char * const text;
#endif
{
	if ( (text != NULL) && (stdout != NULL) ) printf ("%s", text);
}

/**
 * Initializes the EXEC backend for use.
 * \param program The program to launch on each note/pause.
 * \return 0 on success.
 */
int
imyp_exec_init (
#ifdef IMYP_ANSIC
	const char * const program)
#else
	program)
	const char * const program;
#endif
{
	size_t i, j;

	if ( program == NULL ) return -1;
	i = strlen (program);
	if ( (i == 0) || (i >= IMYP_MAX_PROG_LEN) ) return -2;
	for ( j = 0; j < i; j++ )
	{
		if ( program[j] != ' ' ) break;
	}
	if ( j == i ) return -3; /* all spaces */
	for ( j = 0; j < IMYP_MAX_PROG_LEN; j++ )
	{
		exename[j] = '\0';
	}
	strncpy (exename, program, IMYP_MIN (i, IMYP_MAX_PROG_LEN-1));
	exename[IMYP_MAX_PROG_LEN-1] = '\0';
	return 0;
}

/**
 * Closes the EXEC backend.
 * \return 0 on success.
 */
int
imyp_exec_close (
#ifdef IMYP_ANSIC
	void
#endif
)
{
	size_t j;

	for ( j = 0; j < sizeof (exename); j++ )
	{
		exename[j] = '\0';
		to_execute[j] = '\0';
	}
	return 0;
}
