/*
 * IMYplay - A program for playing iMelody ringtones (IMY files).
 *	-- PulseAudio backend.
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
#include "imyp_pul.h"
#include "imyp_sig.h"
#include "imyputil.h"

#include <stdio.h>

#ifdef IMYP_HAVE_PULSEAUDIO
# if (defined HAVE_PULSE_SIMPLE_H)
#  include <pulse/simple.h>
#  include <pulse/sample.h>
#  include <pulse/util.h>
#  include <pulse/version.h>
# else
#  include <simple.h>
#  include <sample.h>
#  include <util.h>
#  include <version.h>
# endif
#else
# error PulseAudio requested, but components not found.
#endif

#ifdef HAVE_STDLIB_H
# include <stdlib.h>
#endif

#ifdef HAVE_MALLOC_H
# include <malloc.h>
#endif

struct imyp_pulse_backend_data
{
	pa_simple * stream;
	pa_sample_spec conf;
};

#ifdef TEST_COMPILE
# undef IMYP_ANSIC
# if TEST_COMPILE > 1
#  undef HAVE_MALLOC
# endif
#endif

#ifndef HAVE_MALLOC
static struct imyp_pulse_backend_data imyp_pulse_backend_data_static;
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
imyp_pulse_play_tune (
#ifdef IMYP_ANSIC
	imyp_backend_data_t * const imyp_data,
	const double freq,
	const int volume_level,
	const int duration,
	void * const buf,
	int bufsize)
#else
	imyp_data, freq, volume_level, duration, buf, bufsize)
	imyp_backend_data_t * const imyp_data;
	const double freq;
	const int volume_level;
	const int duration;
	void * const buf;
	int bufsize;
#endif
{
	unsigned int quality = 16;
	int res;
	int is_le = 1;
	int is_unsigned = 0;
	struct imyp_pulse_backend_data * data =
		(struct imyp_pulse_backend_data *)imyp_data;

	if ( (data == NULL) || (buf == NULL) || (bufsize <= 0) )
	{
		return -1;
	}

	if ( data->conf.format == PA_SAMPLE_U8 )
	{
		quality = 8;
		is_unsigned = 1;
	}
	else if ( (data->conf.format == PA_SAMPLE_S16LE) || (data->conf.format == PA_SAMPLE_S16BE) )
	{
		if ( data->conf.format == PA_SAMPLE_S16BE )
		{
			is_le = 0;
		}
		else
		{
			is_le = 1;
		}
		quality = 16;
	}

	bufsize = imyp_generate_samples (freq, volume_level, duration, buf, bufsize,
		is_le, is_unsigned, quality, data->conf.rate, NULL);
	if ( imyp_sig_recvd != 0 )
	{
		return -2;
	}
	res = pa_simple_write (data->stream, buf, (size_t)bufsize, NULL);
	if ( res >= 0 )
	{
		pa_simple_drain (data->stream, NULL);	/* wait for data to be played */
		return 0;
	}
	return res;
}

/**
 * Pauses for the specified period of time.
 * \param imyp_data pointer to the backend's private data.
 * \param milliseconds Number of milliseconds to pause.
 */
void
imyp_pulse_pause (
#ifdef IMYP_ANSIC
	imyp_backend_data_t * const imyp_data IMYP_ATTR ((unused)),
	const int milliseconds)
#else
	imyp_data, milliseconds)
	imyp_backend_data_t * const imyp_data IMYP_ATTR ((unused));
	const int milliseconds;
#endif
{
	if ( milliseconds <= 0 )
	{
		return;
	}
	pa_msleep ((unsigned long int)milliseconds);
}

/**
 * Outputs the given text.
 * \param imyp_data pointer to the backend's private data.
 * \param text The text to output.
 */
void
imyp_pulse_put_text (
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
 * Initializes the PulseAudio library for use.
 * \param imyp_data pointer to storage for the backend's private data.
 * \param dev_file The device to open.
 * \return 0 on success.
 */
int
imyp_pulse_init (
#ifdef IMYP_ANSIC
	imyp_backend_data_t ** const imyp_data,
	const char * const dev_file)
#else
	imyp_data, dev_file)
	imyp_backend_data_t ** const imyp_data;
	const char * const dev_file;
#endif
{
	unsigned int i, j;
	const int formats[] = {PA_SAMPLE_S16LE, PA_SAMPLE_S16BE, PA_SAMPLE_U8};
	const uint32_t samp_freqs[] = {44100, 22050, 11025};
	struct imyp_pulse_backend_data * data;

	if ( imyp_data == NULL )
	{
		return -100;
	}
#ifdef HAVE_MALLOC
	data = (struct imyp_pulse_backend_data *) malloc (sizeof (
		struct imyp_pulse_backend_data));
	if ( data == NULL )
	{
		return -1;
	}
#else
	data = &imyp_pulse_backend_data_static;
#endif

	data->conf.channels = 1;
	for ( i = 0; i < sizeof (formats) / sizeof (formats[0]); i++ )
	{
		data->conf.format = formats[i];
		for ( j = 0; j < sizeof (samp_freqs) / sizeof (samp_freqs[0]); j++ )
		{
			data->conf.rate = samp_freqs[j];
			data->stream = pa_simple_new (dev_file, "IMYplay", PA_STREAM_PLAYBACK,
				NULL, "iMelody", &(data->conf), NULL, NULL, NULL);
			if ( data->stream != NULL )
			{
				break;
			}
		}
		if ( j < sizeof (samp_freqs) / sizeof (samp_freqs[0]) )
		{
			break;
		}
	}
	if ( i == sizeof (formats) / sizeof (formats[0]) )
	{
#ifdef HAVE_MALLOC
		free (data);
#endif
		return -2;
	}
	*imyp_data = (imyp_backend_data_t *)data;

	return 0;
}

/**
 * Closes the PulseAudio library.
 * \param imyp_data pointer to the backend's private data.
 * \return 0 on success.
 */
int
imyp_pulse_close (
#ifdef IMYP_ANSIC
	imyp_backend_data_t * const imyp_data)
#else
	imyp_data)
	imyp_backend_data_t * const imyp_data;
#endif
{
	struct imyp_pulse_backend_data * data =
		(struct imyp_pulse_backend_data *)imyp_data;

	if ( data != NULL )
	{
		if ( data->stream != NULL )
		{
			pa_simple_free (data->stream);
		}
#ifdef HAVE_MALLOC
		free (data);
#endif
	}
	return 0;
}

/**
 * Displays the version of the PulseAudio backend.
 * \param imyp_data pointer to the backend's private data.
 */
void
imyp_pulse_version (
#ifdef IMYP_ANSIC
	imyp_backend_data_t * const imyp_data IMYP_ATTR ((unused)))
#else
	imyp_data)
	imyp_backend_data_t * const imyp_data IMYP_ATTR ((unused));
#endif
{
#if (defined PA_MAJOR) && (defined PA_MINOR) && (defined PA_MICRO)
	printf ( "PulseAudio: %d.%d.%d\n", PA_MAJOR, PA_MINOR, PA_MICRO );
#else
# if (defined PA_MAJOR) && (defined PA_MINOR)
	printf ( "PulseAudio: %d.%d\n", PA_MAJOR, PA_MINOR );
# else
#  if (defined PA_MAJOR)
	printf ( "PulseAudio: %d\n", PA_MAJOR );
#  else
	printf ( "PulseAudio: ?\n" );
#  endif
# endif
#endif
}
