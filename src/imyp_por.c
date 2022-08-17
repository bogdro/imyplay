/*
 * A program for playing iMelody ringtones (IMY files).
 *	-- PortAudio backend.
 *
 * Copyright (C) 2009-2013 Bogdan Drozdowski, bogdandr (at) op.pl
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
#include "imyp_por.h"
#include "imyp_sig.h"
#include "imyputil.h"

#include <stdio.h>

#ifdef IMYP_HAVE_PORTAUDIO
# if (defined HAVE_PORTAUDIO_H)
#  include <portaudio.h>
# else
#  include <portaudio/portaudio.h>
# endif
#else
# error PortAudio requested, but components not found.
#endif

#ifdef HAVE_STRING_H
# if ((!defined STDC_HEADERS) || (!STDC_HEADERS)) && (defined HAVE_MEMORY_H)
#  include <memory.h>
# endif
# include <string.h>
#endif

#ifdef HAVE_MATH_H
# include <math.h>	/* sin() */
#endif

#ifndef M_PI
# define M_PI 3.14159265358979323846
#endif

#ifdef HAVE_STDLIB_H
# include <stdlib.h>
#endif

#ifdef HAVE_MALLOC_H
# include <malloc.h>
#endif

struct imyp_portaudio_backend_data
{
	double tone_freq;
	int volume_level;
	int duration;
	unsigned long int last_index;
	PaSampleFormat format;
	double sampfreq;
	volatile int inside_callback;
	PaStream * stream;
};

#ifndef HAVE_MALLOC
struct imyp_portaudio_backend_data imyp_portaudio_backend_data_static;
#endif


#ifndef IMYP_ANSIC
static int imyp_portaudio_fill_buffer PARAMS((const void * input IMYP_ATTR((unused)), void * output,
	unsigned long int frameCount,
	const PaStreamCallbackTimeInfo * timeInfo IMYP_ATTR((unused)),
	PaStreamCallbackFlags statusFlags IMYP_ATTR((unused)),
	void * userData));
#endif

/**
 * The stream callback function.
 */
static int imyp_portaudio_fill_buffer (
#ifdef IMYP_ANSIC
	const void * input IMYP_ATTR((unused)), void * output,
	unsigned long int frameCount,
	const PaStreamCallbackTimeInfo * timeInfo IMYP_ATTR((unused)),
	PaStreamCallbackFlags statusFlags IMYP_ATTR((unused)),
	void * userData)
#else
	input, output, frameCount, timeInfo, statusFlags, userData)
	const void * input IMYP_ATTR((unused));
	void *output;
	unsigned long int frameCount;
	const PaStreamCallbackTimeInfo * timeInfo IMYP_ATTR((unused));
	PaStreamCallbackFlags statusFlags IMYP_ATTR((unused));
	void * userData;
#endif
{
	unsigned int quality = 16;
	int is_uns = 0;
	struct imyp_portaudio_backend_data * data =
		(struct imyp_portaudio_backend_data *)userData;

	if ( (userData == NULL) || (output == NULL) || (frameCount <= 0) )
	{
		return paContinue;
	}

	if ( (data->format == paInt8) || (data->format == paUInt8) )
	{
		if ( data->format == paUInt8 )
		{
			is_uns = 1;
		}
		else
		{
			is_uns = 0;
		}
		quality = 8;
	}
	else if ( data->format == paInt16 )
	{
		quality = 16;
	}

	data->inside_callback = 1;
	imyp_generate_samples (data->tone_freq, data->volume_level, data->duration,
		output, (int)frameCount*2, 1 /* little-endian */, is_uns, quality,
		(unsigned int)data->sampfreq, &(data->last_index));
	data->inside_callback = 0;
	if ( sig_recvd != 0 )
	{
		return paAbort;
	}

	return paContinue;
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
imyp_portaudio_play_tune (
#ifdef IMYP_ANSIC
	imyp_backend_data_t * const imyp_data,
	const double freq,
	const int volume_level,
	const int duration,
	void * const buf IMYP_ATTR((unused)),
	int bufsize IMYP_ATTR((unused)))
#else
	imyp_data, freq, volume_level, duration, buf, bufsize)
	imyp_backend_data_t * const imyp_data;
	const double freq;
	const int volume_level;
	const int duration;
	void * const buf IMYP_ATTR((unused));
	int bufsize IMYP_ATTR((unused));
#endif
{
	PaError error;
	size_t i, j;
	const PaSampleFormat formats[] = {paInt16, paInt8, paUInt8};
	/* the lowest sampling frequency seems to work best. */
	const double speeds[] = {/*44100.0, 22050.0,*/ 11025.0};
	int was_set = 0;
	struct imyp_portaudio_backend_data * data =
		(struct imyp_portaudio_backend_data *)imyp_data;

	if ( data->sampfreq > 0 )
	{
		error = Pa_OpenDefaultStream (
				&(data->stream),
				0, 1,	/* no input, mono output */
				data->format, data->sampfreq,
				128,
				/*(duration * speeds[j] * ((formats[i] == paInt16)? 2 : 1)) / 1000,*/
				/*16384,*/
				/*paFramesPerBufferUnspecified,*/
				/*, 0*/
				&imyp_portaudio_fill_buffer, data);
		if ( error == paNoError )
		{
			was_set = 1;
		}
	}
	if ( was_set == 0 )
	{
		for ( i = 0; i < sizeof (formats) / sizeof (formats[0]); i++ )
		{
			for ( j = 0; j < sizeof (speeds) / sizeof (speeds[0]); j++ )
			{
				error = Pa_OpenDefaultStream (
						&(data->stream),
						0, 1,	/* no input, mono output */
						formats[i], speeds[j],
						128,
						/*(duration * speeds[j] * ((formats[i] == paInt16)? 2 : 1)) / 1000,*/
						/*16384,*/
						/*paFramesPerBufferUnspecified,*/
						/*, 0*/
						&imyp_portaudio_fill_buffer, data);
				if ( error == paNoError )
				{
					data->format = formats[i];
					data->sampfreq = speeds[j];
					break;
				}
			}
			if ( j < sizeof (speeds) / sizeof (speeds[0]) )
			{
				break;
			}
		}
		if ( i == sizeof (formats) / sizeof (formats[0]) )
		{
			return -2;
		}
	}

	data->tone_freq = freq;
	data->volume_level = volume_level;
	data->duration = duration;
	data->last_index = 0;

	error = Pa_StartStream (data->stream);
	if ( error != paNoError )
	{
		return -1;
	}
	imyp_portaudio_pause (imyp_data, duration);
	Pa_StopStream (data->stream);
	Pa_CloseStream (data->stream);

	while ( data->inside_callback != 0 ) {};

	data->tone_freq = 0.0;
	data->volume_level = 0;
	data->duration = 0;
	data->last_index = 0;

	return 0;
}

/**
 * Pauses for the specified period of time.
 * \param imyp_data pointer to the backend's private data.
 * \param milliseconds Number of milliseconds to pause.
 */
void
imyp_portaudio_pause (
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
#if (((defined HAVE_SYS_SELECT_H) || (((defined TIME_WITH_SYS_TIME)	\
	|| (defined HAVE_SYS_TIME_H) || (defined HAVE_TIME_H))		\
 		&& (defined HAVE_UNISTD_H)))				\
	&& (defined HAVE_SELECT))

	imyp_pause_select (milliseconds);

#else
	Pa_Sleep (milliseconds);
#endif
}

/**
 * Outputs the given text.
 * \param imyp_data pointer to the backend's private data.
 * \param text The text to output.
 */
void
imyp_portaudio_put_text (
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
 * Initializes the PortAudio library for use.
 * \param imyp_data pointer to storage for the backend's private data.
 * \param dev_file The device to open.
 * \return 0 on success.
 */
int
imyp_portaudio_init (
#ifdef IMYP_ANSIC
	imyp_backend_data_t ** const imyp_data,
	const char * const dev_file)
#else
	imyp_data, dev_file)
	imyp_backend_data_t ** const imyp_data;
	const char * const dev_file;
#endif
{
	struct imyp_portaudio_backend_data * data;
	enum IMYP_SAMPLE_FORMATS format;
	char * colon;
	int scanf_res;

	if ( Pa_Initialize () != paNoError )
	{
		return -1;
	}

	if ( imyp_data == NULL )
	{
		Pa_Terminate ();
		return -100;
	}
#ifdef HAVE_MALLOC
	data = (struct imyp_portaudio_backend_data *) malloc (sizeof (
		struct imyp_portaudio_backend_data));
	if ( data == NULL )
	{
		Pa_Terminate ();
		return -2;
	}
#else
	data = &imyp_portaudio_backend_data_static;
#endif

	data->tone_freq = 0.0;
	data->volume_level = 0;
	data->duration = 0;
	data->last_index = 0;
	data->inside_callback = 0;
	data->sampfreq = 0;

	if ( dev_file != NULL )
	{
		colon = strrchr (dev_file, (int)':');
		if ( colon != NULL )
		{
			format = imyp_get_format (colon+1);
			if ( format == IMYP_SAMPLE_FORMAT_UNKNOWN )
			{
				format = IMYP_SAMPLE_FORMAT_S16LE;
			}

			if ( (format == IMYP_SAMPLE_FORMAT_S8LE)
				|| (format == IMYP_SAMPLE_FORMAT_S8BE) )
			{
				data->format = paInt8;
			}
			else if ( (format == IMYP_SAMPLE_FORMAT_U8LE)
				|| (format == IMYP_SAMPLE_FORMAT_U8BE) )
			{
				data->format = paUInt8;
			}
			else
			{
				data->format = paInt16;
			}
			/* wipe the colon to read the sampling rate */
			*colon = '\0';
		}
		/* get the sampling rate: */
		scanf_res = sscanf (dev_file, "%lf", &(data->sampfreq));
		if ( scanf_res == 1 )
		{
			if ( data->sampfreq <= 0 )
			{
				data->sampfreq = 44100;
			}
		}
	}
	*imyp_data = (imyp_backend_data_t *)data;

	return 0;
}

/**
 * Closes the PortAudio library.
 * \param imyp_data pointer to the backend's private data.
 * \return 0 on success.
 */
int
imyp_portaudio_close (
#ifdef IMYP_ANSIC
	imyp_backend_data_t * const imyp_data)
#else
	imyp_data)
	imyp_backend_data_t * const imyp_data;
#endif
{
	Pa_Terminate ();
	if ( imyp_data != NULL )
	{
#ifdef HAVE_MALLOC
		free (imyp_data);
#endif
	}
	return 0;
}

/**
 * Displays the version of the PortAudio backend.
 * \param imyp_data pointer to the backend's private data.
 */
void
imyp_portaudio_version (
#ifdef IMYP_ANSIC
	imyp_backend_data_t * const imyp_data IMYP_ATTR ((unused)))
#else
	imyp_data)
	imyp_backend_data_t * const imyp_data IMYP_ATTR ((unused));
#endif
{
	printf ( "PortAudio: %s\n", Pa_GetVersionText () );
}

