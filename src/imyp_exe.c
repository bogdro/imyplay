/*
 * IMYplay - A program for playing iMelody ringtones (IMY files).
 *	-- EXEC backend.
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

struct imyp_exec_backend_data
{
	char exename[IMYP_MAX_PROG_LEN];
	char to_execute[IMYP_MAX_PROG_LEN];
};

#ifdef TEST_COMPILE
# undef IMYP_ANSIC
# if TEST_COMPILE > 1
#  undef HAVE_MALLOC
# endif
#endif

#ifndef HAVE_MALLOC
static struct imyp_exec_backend_data imyp_exec_backend_data_static;
#endif


#ifndef IMYP_ANSIC
static void imyp_put_value_as_int IMYP_PARAMS ((
	char buffer[], size_t buf_index,
	size_t total_buflen, double value));
#endif

/**
 * Puts the given value in the buffer as an integer (rounded).
 * \param buffer buffer to put the value in.
 * \param buf_index current location to put the value at.
 * \param total_buflen total size of the buffer.
 * \param value The value to put in the buffer.
 */
static void imyp_put_value_as_int (
#ifdef IMYP_ANSIC
	char buffer[],
	size_t buf_index,
	size_t total_buflen,
	double value)
#else
	buffer, buf_index, total_buflen, value)
	char buffer[];
	size_t buf_index;
	size_t total_buflen;
	double value;
#endif
{
	if ( (buffer == NULL) || (total_buflen == 0) )
	{
		return;
	}
#ifdef HAVE_SNPRINTF
	snprintf (&(buffer[buf_index]),
		total_buflen - buf_index - 1,
		"%d", (int)IMYP_ROUND (value));
#else
	if ( total_buflen - buf_index - 1 >= 10 )
	{
		sprintf (&(buffer[buf_index]),
			"%d", (int)IMYP_ROUND (value));
	}
#endif
	buffer[total_buflen-1] = '\0';
}



#ifndef IMYP_ANSIC
static void imyp_put_value_as_float IMYP_PARAMS ((
	char buffer[], size_t buf_index,
	size_t total_buflen, double value));
#endif

/**
 * Puts the given value in the buffer as a flotin-point value.
 * \param buffer buffer to put the value in.
 * \param buf_index current location to put the value at.
 * \param total_buflen total size of the buffer.
 * \param value The value to put in the buffer.
 */
static void imyp_put_value_as_float (
#ifdef IMYP_ANSIC
	char buffer[],
	size_t buf_index,
	size_t total_buflen,
	double value)
#else
	buffer, buf_index, total_buflen, value)
	char buffer[];
	size_t buf_index;
	size_t total_buflen;
	double value;
#endif
{
	if ( (buffer == NULL) || (total_buflen == 0) )
	{
		return;
	}
#ifdef HAVE_SNPRINTF
	snprintf (&(buffer[buf_index]),
		total_buflen - buf_index - 1,
		"%-.6f", value);
#else
	if ( total_buflen - buf_index - 1 >= 12 )
	{
		sprintf (&(buffer[buf_index]),
			"%-.6f", value);
	}
#endif
	buffer[total_buflen-1] = '\0';
}


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
	size_t i;
	size_t j;
	size_t exe_index = 0;
	int sys_ret;
	struct imyp_exec_backend_data * data =
		(struct imyp_exec_backend_data *)imyp_data;

	if ( data == NULL )
	{
		return -100;
	}

	i = strlen (data->exename);
	i = IMYP_MIN (i, IMYP_MAX_PROG_LEN-1);

	IMYP_MEMSET (data->to_execute, '\0', IMYP_MAX_PROG_LEN);
	for ( j = 0; j < i; j++ )
	{
		exe_index = strlen (data->to_execute);
		if ( exe_index >= sizeof (data->to_execute) - 1 )
		{
			break;
		}
		if ( data->exename[j] != '%' )
		{
			data->to_execute[exe_index] = data->exename[j];
			continue;
		}
		j++;
		switch (data->exename[j])
		{
			case 'i':	/* integer frequency */
				imyp_put_value_as_int (data->to_execute,
					exe_index, sizeof (data->to_execute),
					freq);
				break;

			case 'f':	/* float frequency */
				imyp_put_value_as_float (data->to_execute,
					exe_index, sizeof (data->to_execute),
					freq);
				break;

			case 'd':	/* integer duration in seconds */
				imyp_put_value_as_int (data->to_execute,
					exe_index, sizeof (data->to_execute),
					duration/1000.0);
				break;

			case 's':	/* float duration in seconds */
				imyp_put_value_as_float (data->to_execute,
					exe_index, sizeof (data->to_execute),
					duration/1000.0);
				break;

			case 'm':	/* integer duration in milliseconds */
				imyp_put_value_as_int (data->to_execute,
					exe_index, sizeof (data->to_execute),
					1.0 * duration);
				break;

			case 'l':	/* float duration in milliseconds */
				imyp_put_value_as_float (data->to_execute,
					exe_index, sizeof (data->to_execute),
					1.0 * duration);
				break;

			case 'v':	/* volume level 0-15 */
				imyp_put_value_as_int (data->to_execute,
					exe_index, sizeof (data->to_execute),
					1.0 * volume_level);
				break;

			default:
				if ( exe_index < sizeof (data->to_execute) )
				{
					data->to_execute[exe_index] = '%';	/* copy */
					exe_index++;
					if ( exe_index < sizeof (data->to_execute) )
					{
						data->to_execute[exe_index] = data->exename[j];
						exe_index++;
					}
				}
				data->to_execute[IMYP_MAX_PROG_LEN-1] = '\0';
				break;
		}
	}
	data->to_execute[IMYP_MAX_PROG_LEN-1] = '\0';
	sys_ret = system (data->to_execute);
	if ( sys_ret >= 0 )
	{
#ifdef WEXITSTATUS
		sys_ret = WEXITSTATUS (sys_ret);
# ifdef WIFSIGNALED
		if ( WIFSIGNALED (sys_ret) )
		{
			imyp_sig_recvd = 1; /* anything not zero */
		}
# endif /* WIFSIGNALED */
#else /* ! WEXITSTATUS */
		/* can't check, assuming success */
		sys_ret = 0;
#endif /* WEXITSTATUS */
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
	const void * const buf IMYP_ATTR ((unused)),
	int bufsize IMYP_ATTR ((unused)))
#else
	imyp_data, freq, volume_level, duration, buf, bufsize)
	imyp_backend_data_t * const imyp_data;
	const double freq;
	const int volume_level;
	const int duration;
	const void * const buf IMYP_ATTR ((unused));
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
	const imyp_backend_data_t * const imyp_data IMYP_ATTR ((unused)),
	const char * const text)
#else
	imyp_data, text)
	const imyp_backend_data_t * const imyp_data IMYP_ATTR ((unused));
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
	size_t i;
	size_t j;
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
	IMYP_MEMSET (data->exename, '\0', IMYP_MAX_PROG_LEN);
	IMYP_MEMSET (data->to_execute, '\0', IMYP_MAX_PROG_LEN);
	strncpy (data->exename, program, IMYP_MIN (i+1, IMYP_MAX_PROG_LEN-1));
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
	const imyp_backend_data_t * const imyp_data IMYP_ATTR ((unused)))
#else
	imyp_data)
	const imyp_backend_data_t * const imyp_data IMYP_ATTR ((unused));
#endif
{
	/* this is an internal backend */
	printf ( "EXEC\n" );
}
