/*
 * A program for playing iMelody ringtones (IMY files).
 *	-- PortAudio backend.
 *
 * Copyright (C) 2009-2012 Bogdan Drozdowski, bogdandr (at) op.pl
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

#include <stdio.h>

#if (defined HAVE_LIBPORTAUDIO) && ((defined HAVE_PORTAUDIO_H) || (defined HAVE_PORTAUDIO_PORTAUDIO_H))
# if (defined HAVE_PORTAUDIO_H)
#  include <portaudio.h>
# else
#  include <portaudio/portaudio.h>
# endif
#else
# error PortAudio requested, but components not found.
#endif

#ifdef HAVE_MATH_H
# include <math.h>	/* sin() */
#endif

#ifndef M_PI
# define M_PI 3.14159265358979323846
#endif

/* select() the old way */
#if TIME_WITH_SYS_TIME
# include <sys/time.h>
# include <time.h>
#else
# if HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  ifdef HAVE_TIME_H
#   include <time.h>
#  endif
# endif
#endif

#ifdef HAVE_UNISTD_H
# include <unistd.h>	/* select() (the old way) */
#endif

/* select () - the new way */
#ifdef HAVE_SYS_SELECT_H
# include <sys/select.h>
#endif

struct imyp_portaudio_sound_data
{
	double tone_freq;
	int volume_level;
	int duration;
	unsigned long int last_index;
	PaSampleFormat format;
	double sampfreq;
	volatile int inside_callback;
};

static struct imyp_portaudio_sound_data sound_data;
static PaStream * stream;

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
	int samp;	/* better than float */
	unsigned long int i;
	unsigned int quality = 16;
	int is_uns = 0;
	double nperiods;

	if ( (userData == NULL) || (output == NULL) || (frameCount <= 0) ) return paContinue;

#define data ((struct imyp_portaudio_sound_data *) userData)

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

	if ( data->tone_freq > 0.0 )
	{
		data->inside_callback = 1;
		nperiods = data->sampfreq/data->tone_freq;
		for ( i=data->last_index; i < data->last_index+frameCount; i++ )
		{
			if ( sig_recvd != 0 )
			{
				data->inside_callback = 0;
				return paAbort;
			}
			if ( (int)IMYP_ROUND(nperiods) == 0 )
			{
				/* not a single full period fits in the buffer */
#if (defined HAVE_SIN) || (defined HAVE_LIBM)
				samp = (int)(((1<<(quality-1))-1) /* disable to get rectangular wave */ +
					/* The "/3" is required to have a full sine wave, not
					trapese-like wave */
					IMYP_ROUND (((1<<(quality-1))-1)
						* sin (i*(2*M_PI/nperiods))/3));
#else
				samp = (int) IMYP_ROUND (i*
					(((1<<(quality-1))-1)/nperiods));
#endif
			}
			else
			{
#if (defined HAVE_SIN) || (defined HAVE_LIBM)
				samp = (int)(((1<<(quality-1))-1) /* disable to get rectangular wave */ +
					/* The "/3" is required to have a full sine wave, not
					trapese-like wave */
					IMYP_ROUND (((1<<(quality-1))-1)
						* sin ((i%((unsigned long int)IMYP_ROUND(nperiods)))*(2*M_PI/nperiods))/3));
#else
				samp = (int) IMYP_ROUND ((i%((unsigned long int)IMYP_ROUND(nperiods)))*
					(((1<<(quality-1))-1)/nperiods));
#endif
			}
			if ( is_uns == 0 )
			{
				samp -= (1<<(quality-1))-1;
			}
			if ( quality == 16 )
			{
				if ( (i-data->last_index)*2 >= frameCount ) break;
				((char *)output)[(i-data->last_index)*2] =
					(char)(((samp * data->volume_level) / IMYP_MAX_IMY_VOLUME) & 0x0FF);
				((char *)output)[(i-data->last_index)*2+1] =
					(char)((((samp * data->volume_level) / IMYP_MAX_IMY_VOLUME) >> 8) & 0x0FF);
			}
			else if ( quality == 8 )
			{
				((char *)output)[i-data->last_index] =
					(char)(((samp * data->volume_level) / IMYP_MAX_IMY_VOLUME) & 0x0FF);
			}
		}
		data->last_index += frameCount;
		/* just slows things down
		while ( (data->last_index > (unsigned long int)NSAMP) && (sig_recvd == 0) )
		{
			data->last_index -= (unsigned long int)NSAMP;
		}*/
		data->inside_callback = 0;
	}
	else
	{
		data->inside_callback = 1;
		data->last_index = 0;
		for ( i=data->last_index; i < data->last_index+frameCount; i++ )
		{
			if ( sig_recvd != 0 )
			{
				data->inside_callback = 0;
				return paAbort;
			}
			((char *)output)[i-data->last_index] = 0;
		}
		data->inside_callback = 0;
	}
	return paContinue;
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
imyp_portaudio_play_tune (
#ifdef IMYP_ANSIC
	const double freq,
	const int volume_level,
	const int duration,
	void * const buf IMYP_ATTR((unused)),
	int bufsize IMYP_ATTR((unused)))
#else
	freq, volume_level, duration, buf, bufsize)
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
	const double speeds[] = {44100.0, 22050.0, 11025.0};

	for ( i = 0; i < sizeof (formats) / sizeof (formats[0]); i++ )
	{
		for ( j = 0; j < sizeof (speeds) / sizeof (speeds[0]); j++ )
		{
			error = Pa_OpenDefaultStream (
					&stream,
					0, 1,	/* no input, mono output */
					formats[i], speeds[j],
					4096,
					/*(duration * speeds[j] * ((formats[i] == paInt16)? 2 : 1)) / 1000,*/
					/*16384,*/
					/*paFramesPerBufferUnspecified,*/
					/*, 0*/
					&imyp_portaudio_fill_buffer, &sound_data);
			if ( error == paNoError )
			{
				sound_data.format = formats[i];
				sound_data.sampfreq = speeds[j];
				break;
			}
		}
		if ( j < sizeof (speeds) / sizeof (speeds[0]) ) break;
	}
	if ( i == sizeof (formats) / sizeof (formats[0]) ) return -2;

	sound_data.tone_freq = freq;
	sound_data.volume_level = volume_level;
	sound_data.duration = duration;
	sound_data.last_index = 0;

	error = Pa_StartStream (stream);
	if ( error != paNoError )
	{
		return -1;
	}
	imyp_portaudio_pause (duration);
	Pa_StopStream (stream);
	Pa_CloseStream (stream);

	while ( sound_data.inside_callback != 0 ) {};
	sound_data.tone_freq = 0.0;
	sound_data.volume_level = 0;
	sound_data.duration = 0;
	sound_data.last_index = 0;

	return 0;
}

/**
 * Pauses for the specified period of time.
 * \param milliseconds Number of milliseconds to pause.
 */
void
imyp_portaudio_pause (
#ifdef IMYP_ANSIC
	const int milliseconds)
#else
	milliseconds)
	const int milliseconds;
#endif
{
	if ( milliseconds <= 0 ) return;
#if (((defined HAVE_SYS_SELECT_H) || (((defined TIME_WITH_SYS_TIME)	\
	|| (defined HAVE_SYS_TIME_H) || (defined HAVE_TIME_H))		\
 		&& (defined HAVE_UNISTD_H)))				\
	&& (defined HAVE_SELECT))
	{
		struct timeval tv;
		tv.tv_sec = milliseconds / 1000;
		tv.tv_usec = ( milliseconds * 1000 ) % 1000000;
		select ( 0, NULL, NULL, NULL, &tv );
	}
#else
	Pa_Sleep (milliseconds);
#endif
}

/**
 * Outputs the given text.
 * \param text The text to output.
 */
void
imyp_portaudio_put_text (
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
 * Initializes the PortAudio library for use.
 * \param dev_file The device to open.
 * \return 0 on success.
 */
int
imyp_portaudio_init (
#ifdef IMYP_ANSIC
	const char * const dev_file IMYP_ATTR((unused)))
#else
	dev_file)
	const char * const dev_file IMYP_ATTR((unused));
#endif
{
	if ( Pa_Initialize () != paNoError ) return -1;
	sound_data.tone_freq = 0.0;
	sound_data.volume_level = 0;
	sound_data.duration = 0;
	sound_data.last_index = 0;
	sound_data.inside_callback = 0;

	return 0;
}

/**
 * Closes the PortAudio library.
 * \return 0 on success.
 */
int
imyp_portaudio_close (
#ifdef IMYP_ANSIC
	void
#endif
)
{
	Pa_Terminate ();
	return 0;
}

/**
 * Displays the version of the PortAudio backend.
 */
void
imyp_portaudio_version (
#ifdef IMYP_ANSIC
	void
#endif
)
{
	printf ( "PortAudio: %s\n", Pa_GetVersionText () );
}

