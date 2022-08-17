/*
 * A program for playing iMelody ringtones (IMY files).
 *	-- EXEC backend.
 *
 * Copyright (C) 2009-2014 Bogdan Drozdowski, bogdandr (at) op.pl
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
#include "imyputil.h"

#include <stdio.h>

#ifdef IMYP_HAVE_EXEC
# include <stdlib.h>
#else
# error Exec output requested, but components not found.
#endif

#ifdef HAVE_STRING_H
# if ((!defined STDC_HEADERS) || (!STDC_HEADERS)) && (defined HAVE_MEMORY_H)
#  include <memory.h>
# endif
# include <string.h>
#endif

#ifdef HAVE_MALLOC_H
# include <malloc.h>
#endif

#ifdef HAVE_SYS_WAIT_H
# include <sys/wait.h>
#else
# ifdef HAVE_WAIT_H
#  include <wait.h>
# endif
#endif

/* these three should be defined in wait.h or sys/wait.h: */
#ifndef WIFEXITED
# define WIFEXITED(stat_val) (((stat_val) & 255) == 0)
#endif

#ifndef WEXITSTATUS
# define WEXITSTATUS(stat_val) ((unsigned)(stat_val) >> 8)
#endif

#ifndef WIFSIGNALED
# define WIFSIGNALED(status) (((signed char) (((status) & 0x7f) + 1) >> 1) > 0)
#endif

#define IMYP_MAX_PROG_LEN 4097

struct imyp_exec_backend_data
{
	char exename[IMYP_MAX_PROG_LEN];
	char to_execute[IMYP_MAX_PROG_LEN];
};

#ifndef HAVE_MALLOC
static struct imyp_exec_backend_data imyp_exec_backend_data_static;
#endif


#ifndef IMYP_ANSIC
static int launch_program IMYP_PARAMS ((imyp_backend_data_t * const imyp_data,
	const double freq, const int volume_level, const int duration));
#endif

/**
 * Launches the configured program.
 * \param imyp_data pointer to the backend's private data.
 * \param freq The frequency of the tone (in Hz).
 * \param volume_level Volume of the tone (from 0 to 15).
 * \param duration The duration of the tone, in milliseconds.
 * \return 0 on success.
 */
static int launch_program (
#ifdef IMYP_ANSIC
	imyp_backend_data_t * const imyp_data,
	const double freq,
	const int volume_level,
	const int duration)
#else
	imyp_data, freq, volume_level, duration)
	imyp_backend_data_t * const imyp_data;
	const double freq;
	const int volume_level;
	const int duration;
#endif
{
	size_t i, j, exe_index = 0;
	int sys_ret;
	struct imyp_exec_backend_data * data =
		(struct imyp_exec_backend_data *)imyp_data;

	if ( data == NULL )
	{
		return -100;
	}

	i = strlen (data->exename);
	i = IMYP_MIN (i, IMYP_MAX_PROG_LEN-1);

	for ( j = 0; j < IMYP_MAX_PROG_LEN; j++ )
	{
		data->to_execute[j] = '\0';
	}
	for ( j = 0; j < i; j++ )
	{
		if ( data->exename[j] != '%' )
		{
			data->to_execute[exe_index] = data->exename[j];
			exe_index++;
			continue;
		}
		j++;
		switch (data->exename[j])
		{
			case 'i':	/* integer frequency */
#ifdef HAVE_SNPRINTF
				snprintf (&data->to_execute[exe_index],
					sizeof (data->to_execute) - exe_index - 1,
					"%d", (int)IMYP_ROUND (freq));
#else
				if ( sizeof (data->to_execute) - exe_index - 1 >= 10 )
				{
					sprintf (&data->to_execute[exe_index],
						"%d", (int)IMYP_ROUND (freq));
				}
#endif
				data->to_execute[IMYP_MAX_PROG_LEN-1] = '\0';
				exe_index = strlen (data->to_execute);
				break;

			case 'f':	/* float frequency */
#ifdef HAVE_SNPRINTF
				snprintf (&data->to_execute[exe_index],
					sizeof (data->to_execute) - exe_index - 1,
					"%-.6f", IMYP_ROUND (freq));
#else
				if ( sizeof (data->to_execute) - exe_index - 1 >= 12 )
				{
					sprintf (&data->to_execute[exe_index], "%-.6f", freq);
				}
#endif
				data->to_execute[IMYP_MAX_PROG_LEN-1] = '\0';
				exe_index = strlen (data->to_execute);
				break;

			case 'd':	/* integer duration in seconds */
#ifdef HAVE_SNPRINTF
				snprintf (&data->to_execute[exe_index],
					sizeof (data->to_execute) - exe_index - 1,
					"%d", (int)IMYP_ROUND (duration/1000.0));
#else
				if ( sizeof (data->to_execute) - exe_index - 1 >= 10 )
				{
					sprintf (&data->to_execute[exe_index], "%d",
						(int)IMYP_ROUND (duration/1000.0));
				}
#endif
				data->to_execute[IMYP_MAX_PROG_LEN-1] = '\0';
				exe_index = strlen (data->to_execute);
				break;

			case 's':	/* float duration in seconds */
#ifdef HAVE_SNPRINTF
				snprintf (&data->to_execute[exe_index],
					sizeof (data->to_execute) - exe_index - 1,
					"%-.6f", duration/1000.0);
#else
				if ( sizeof (data->to_execute) - exe_index - 1 >= 12 )
				{
					sprintf (&data->to_execute[exe_index],
						"%-.6f", duration/1000.0);
				}
#endif
				data->to_execute[IMYP_MAX_PROG_LEN-1] = '\0';
				exe_index = strlen (data->to_execute);
				break;

			case 'm':	/* integer duration in milliseconds */
#ifdef HAVE_SNPRINTF
				snprintf (&data->to_execute[exe_index],
					sizeof (data->to_execute) - exe_index - 1,
					"%d", duration);
#else
				if ( sizeof (data->to_execute) - exe_index - 1 >= 10 )
				{
					sprintf (&data->to_execute[exe_index],
						"%d", duration);
				}
#endif
				data->to_execute[IMYP_MAX_PROG_LEN-1] = '\0';
				exe_index = strlen (data->to_execute);
				break;

			case 'l':	/* float duration in milliseconds */
#ifdef HAVE_SNPRINTF
				snprintf (&data->to_execute[exe_index],
					sizeof (data->to_execute) - exe_index - 1,
					"%-.6f", duration*1.0);
#else
				if ( sizeof (data->to_execute) - exe_index - 1 >= 12 )
				{
					sprintf (&data->to_execute[exe_index],
						"%-.6f", duration*1.0);
				}
#endif
				data->to_execute[IMYP_MAX_PROG_LEN-1] = '\0';
				exe_index = strlen (data->to_execute);
				break;

			case 'v':	/* volume level 0-15 */
#ifdef HAVE_SNPRINTF
				snprintf (&data->to_execute[exe_index],
					sizeof (data->to_execute) - exe_index - 1,
					"%d", volume_level);
#else
				if ( sizeof (data->to_execute) - exe_index - 1 >= 10 )
				{
					sprintf (&data->to_execute[exe_index],
						"%d", volume_level);
				}
#endif
				data->to_execute[IMYP_MAX_PROG_LEN-1] = '\0';
				exe_index = strlen (data->to_execute);
				break;

			default:
				data->to_execute[exe_index] = '%';	/* copy */
				exe_index++;
				data->to_execute[exe_index] = data->exename[j];
				exe_index++;
				break;
		}
	}
	data->to_execute[IMYP_MAX_PROG_LEN-1] = '\0';
	sys_ret = system (data->to_execute);
	if ( sys_ret >= 0 )
	{
		sys_ret = WEXITSTATUS (sys_ret);
		if ( WIFSIGNALED (sys_ret) )
		{
			sig_recvd = 1; /* anything not zero */
		}
	}
	return sys_ret;
}

/**
 * Play a specified tone.
 * \param imyp_data pointer to the backend's private data.
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
	imyp_backend_data_t * const imyp_data,
	const double freq,
	const int volume_level,
	const int duration,
	void * const buf IMYP_ATTR ((unused)),
	int bufsize IMYP_ATTR ((unused)))
#else
	imyp_data, freq, volume_level, duration, buf, bufsize)
	imyp_backend_data_t * const imyp_data;
	const double freq;
	const int volume_level;
	const int duration;
	void * const buf IMYP_ATTR ((unused));
	int bufsize IMYP_ATTR ((unused));
#endif
{
	return launch_program (imyp_data, freq, volume_level, duration);
}

/**
 * Pauses for the specified period of time.
 * \param imyp_data pointer to the backend's private data.
 * \param milliseconds Number of milliseconds to pause.
 */
void
imyp_exec_pause (
#ifdef IMYP_ANSIC
	imyp_backend_data_t * const imyp_data,
	const int milliseconds)
#else
	imyp_data, milliseconds)
	imyp_backend_data_t * const imyp_data;
	const int milliseconds;
#endif
{
	if ( milliseconds <= 0 )
	{
		return;
	}
	launch_program (imyp_data, 0.0, 0, milliseconds);
}

/**
 * Outputs the given text.
 * \param imyp_data pointer to the backend's private data.
 * \param text The text to output.
 */
void
imyp_exec_put_text (
#ifdef IMYP_ANSIC
	imyp_backend_data_t * const imyp_data IMYP_ATTR ((unused)),
	const char * const text)
#else
	imyp_data, text)
	imyp_backend_data_t * const imyp_data IMYP_ATTR ((unused));
	const char * const text;
#endif
{
	imyp_put_text_stdout (text);
}

/**
 * Initializes the EXEC backend for use.
 * \param imyp_data pointer to storage for the backend's private data.
 * \param program The program to launch on each note/pause.
 * \return 0 on success.
 */
int
imyp_exec_init (
#ifdef IMYP_ANSIC
	imyp_backend_data_t ** const imyp_data,
	const char * const program)
#else
	imyp_data, program)
	imyp_backend_data_t ** const imyp_data;
	const char * const program;
#endif
{
	size_t i, j;
	struct imyp_exec_backend_data * data;

	if ( (imyp_data == NULL) || (program == NULL) )
	{
		return -1;
	}
#ifdef HAVE_MALLOC
	data = (struct imyp_exec_backend_data *) malloc (sizeof (
		struct imyp_exec_backend_data));
	if ( data == NULL )
	{
		return -7;
	}
#else
	data = &imyp_exec_backend_data_static;
#endif

	i = strlen (program);
	if ( (i == 0) || (i >= IMYP_MAX_PROG_LEN) )
	{
#ifdef HAVE_MALLOC
		free (data);
#endif
		return -2;
	}
	for ( j = 0; j < i; j++ )
	{
		if ( program[j] != ' ' )
		{
			break;
		}
	}
	if ( j == i )
	{
#ifdef HAVE_MALLOC
		free (data);
#endif
		return -3; /* all spaces */
	}
	for ( j = 0; j < IMYP_MAX_PROG_LEN; j++ )
	{
		data->exename[j] = '\0';
		data->to_execute[j] = '\0';
	}
	strncpy (data->exename, program, IMYP_MIN (i, IMYP_MAX_PROG_LEN-1));
	data->exename[IMYP_MAX_PROG_LEN-1] = '\0';
	*imyp_data = (imyp_backend_data_t *)data;
	return 0;
}

/**
 * Closes the EXEC backend.
 * \param imyp_data pointer to the backend's private data.
 * \return 0 on success.
 */
int
imyp_exec_close (
#ifdef IMYP_ANSIC
	imyp_backend_data_t * const imyp_data)
#else
	imyp_data)
	imyp_backend_data_t * const imyp_data;
#endif
{
	if ( imyp_data != NULL )
	{
#ifdef HAVE_MALLOC
		free (imyp_data);
#endif
	}
	return 0;
}

/**
 * Displays the version of the EXEC backend.
 * \param imyp_data pointer to the backend's private data.
 */
void
imyp_exec_version (
#ifdef IMYP_ANSIC
	imyp_backend_data_t * const imyp_data IMYP_ATTR ((unused)))
#else
	imyp_data)
	imyp_backend_data_t * const imyp_data IMYP_ATTR ((unused));
#endif
{
	/* this is an internal backend */
	printf ( "EXEC\n" );
}

