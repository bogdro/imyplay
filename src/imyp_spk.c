/*
 * IMYplay - A program for playing iMelody ringtones (IMY files).
 *	-- PC-speaker backend.
 *
 * Copyright (C) 2009-2025 Bogdan Drozdowski, bogdro (at) users.sourceforge.net
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
#include "imyp_spk.h"
#include "imyp_sig.h"
#include "imyputil.h"

#ifdef IMYP_HAVE_SPKR
# include <stdio.h> /* anything, actually */
#else
# error PC-speaker requested, but components not found.
#endif

#ifdef HAVE_UNISTD_H
# include <unistd.h>	/* close() */
#endif

#ifdef HAVE_STDLIB_H
# include <stdlib.h>
#endif

#ifdef HAVE_MALLOC_H
# include <malloc.h>
#endif

#ifdef IMYP_IS_DOS
# ifdef __cplusplus
extern "C" {
# endif

extern
# ifdef __WATCOMC__
far
# endif
int imyp_spkr_dos_tone IMYP_PARAMS ((const int frequency));

# ifdef __cplusplus
}
# endif

#else /* ! IMYP_IS_DOS */

# ifdef HAVE_SYS_IOCTL_H
#  include <sys/ioctl.h>
# endif

# ifdef HAVE_SYS_KD_H
#  include <sys/kd.h>
# endif

# ifdef HAVE_FCNTL_H
#  include <fcntl.h>	/* open() */
# endif

# ifndef O_WRONLY
#  define O_WRONLY 	1
# endif


struct imyp_spkr_backend_data
{
	int raw_console;
};

# ifndef HAVE_MALLOC
static struct imyp_spkr_backend_data imyp_spkr_backend_data_static;
# endif
#endif

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
imyp_spkr_play_tune (
#ifdef IMYP_ANSIC
	imyp_backend_data_t * const imyp_data,
	const double freq,
	const int volume_level IMYP_ATTR ((unused)),
	const int duration,
	const void * const buf IMYP_ATTR ((unused)),
	int bufsize IMYP_ATTR ((unused)))
#else
	imyp_data, freq, volume_level, duration, buf, bufsize)
	imyp_backend_data_t * const imyp_data;
	const double freq;
	const int volume_level IMYP_ATTR ((unused));
	const int duration;
	const void * const buf IMYP_ATTR ((unused));
	int bufsize IMYP_ATTR ((unused));
#endif
{
	int ioctl_res = 0;
	int ioctl_param;
#ifndef IMYP_IS_DOS
	const struct imyp_spkr_backend_data * const data =
		(struct imyp_spkr_backend_data *)imyp_data;
#endif
	/* The PC speaker can't generate sounds with a frequency
	   less than 0x1234DD / 0x10000 = 18.2 Hz.
	   Remember to "modprobe snd-pcsp" under Linux.
	*/
	if ( freq >= 19 )
	{
		ioctl_param = 0x1234DD / (int)IMYP_ROUND (freq);
#ifdef IMYP_IS_DOS
		/* turn off any sounds being currently generated: */
		imyp_spkr_dos_tone (0);
		/* start generating the tone: */
		ioctl_res = imyp_spkr_dos_tone (ioctl_param);
		/* pause (the sound is still being generated): */
		imyp_spkr_pause (imyp_data, duration);
		/* stop generating the tone: */
		imyp_spkr_dos_tone (0);
#else
		if ( data != NULL )
		{
			/* turn off any sounds being currently generated: */
			ioctl (data->raw_console, KIOCSOUND, 0);
			/* start generating the tone: */
			ioctl_res = ioctl (data->raw_console, KIOCSOUND, ioctl_param);
			/* pause (the sound is still being generated): */
			imyp_spkr_pause (imyp_data, duration);
			/* stop generating the tone: */
			ioctl (data->raw_console, KIOCSOUND, 0);
		}
		else
		{
			imyp_spkr_pause (imyp_data, duration);
		}
#endif
	}
	else
	{
		imyp_spkr_pause (imyp_data, duration);
	}
	return ioctl_res;
}

/**
 * Pauses for the specified period of time.
 * \param imyp_data pointer to the backend's private data.
 * \param milliseconds Number of milliseconds to pause.
 */
void
imyp_spkr_pause (
#ifdef IMYP_ANSIC
	const imyp_backend_data_t * const imyp_data IMYP_ATTR ((unused)),
	const int milliseconds)
#else
	imyp_data, milliseconds)
	const imyp_backend_data_t * const imyp_data IMYP_ATTR ((unused));
	const int milliseconds;
#endif
{
	imyp_pause_select (milliseconds);
}

/**
 * Outputs the given text.
 * \param imyp_data pointer to the backend's private data.
 * \param text The text to output.
 */
void
imyp_spkr_put_text (
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
 * Initializes the PC-speaker backend for use.
 * \param imyp_data pointer to storage for the backend's private data.
 * \param dev The device to open instead of "/dev/console".
 * \return 0 on success.
 */
int
imyp_spkr_init (
#ifdef IMYP_ANSIC
	imyp_backend_data_t ** const imyp_data,
	const char * const dev)
#else
	imyp_data, dev)
	imyp_backend_data_t ** const imyp_data;
	const char * const dev;
#endif
{
	/* DOS doesn't need any initialization */
#ifndef IMYP_IS_DOS
	struct imyp_spkr_backend_data * data;

	if ( imyp_data == NULL )
	{
		return -2;
	}
# ifdef HAVE_MALLOC
	data = (struct imyp_spkr_backend_data *) malloc (sizeof (
		struct imyp_spkr_backend_data));
	if ( data == NULL )
	{
		return -3;
	}
# else
	data = &imyp_spkr_backend_data_static;
# endif
	if ( dev == NULL )
	{
		data->raw_console = open ("/dev/console", O_WRONLY);
		if ( data->raw_console < 0 )
		{
# ifdef HAVE_MALLOC
			free (data);
# endif
			return -4;
		}
	}
	else
	{
		data->raw_console = open (dev, O_WRONLY);
		if ( data->raw_console < 0 )
		{
			data->raw_console = open ("/dev/console", O_WRONLY);
			if ( data->raw_console < 0 )
			{
# ifdef HAVE_MALLOC
				free (data);
# endif
				return -5;
			}
		}
	}
	*imyp_data = (imyp_backend_data_t *)data;
#endif
	return 0;
}

/**
 * Closes the PC-speaker backend.
 * \param imyp_data pointer to the backend's private data.
 * \return 0 on success.
 */
int
imyp_spkr_close (
#ifdef IMYP_ANSIC
	imyp_backend_data_t * const imyp_data)
#else
	imyp_data)
	imyp_backend_data_t * const imyp_data;
#endif
{
#ifdef IMYP_IS_DOS
	/* stop generating the tone: */
	imyp_spkr_dos_tone (0);
	return 0;
#else
	int close_res;
	if ( imyp_data == NULL )
	{
		return -2;
	}
	close_res = close (((struct imyp_spkr_backend_data *)imyp_data)->
		raw_console);
# ifdef HAVE_MALLOC
	free (imyp_data);
# endif
	return close_res;
#endif
}

/**
 * Displays the version of the PC-speaker backend.
 * \param imyp_data pointer to the backend's private data.
 */
void
imyp_spkr_version (
#ifdef IMYP_ANSIC
	const imyp_backend_data_t * const imyp_data IMYP_ATTR ((unused)))
#else
	imyp_data)
	const imyp_backend_data_t * const imyp_data IMYP_ATTR ((unused));
#endif
{
	/* this is an internal backend */
	printf ( "PC-Speaker\n" );
}
