/*
 * A program for playing iMelody ringtones (IMY files).
 *	-- SDL backend.
 *
 * Copyright (C) 2009-2019 Bogdan Drozdowski, bogdandr (at) op.pl
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
#include "imyp_sdl.h"
#include "imyp_sig.h"
#include "imyputil.h"

#include <stdio.h>

#ifdef IMYP_HAVE_SDL
# if (defined HAVE_SDL_H)
#  include <SDL.h>
#  include <SDL_version.h>
# else
#  include <SDL/SDL.h>
#  include <SDL/SDL_version.h>
# endif
#else
# error SDL requested, but components not found.
#endif

#ifdef HAVE_STDLIB_H
# include <stdlib.h>
#endif

#ifdef HAVE_MALLOC_H
# include <malloc.h>
#endif

#ifdef HAVE_MATH_H
# include <math.h>	/* sin() */
#endif

#ifndef M_PI
# define M_PI 3.14159265358979323846
#endif

struct imyp_sdl_backend_data
{
	double tone_freq;
	int volume_level;
	int duration;
	SDL_AudioSpec received;
	/* modified in the callback: */
	volatile unsigned long int last_index;
	volatile long int samples_remain;
};

#ifndef HAVE_MALLOC
static struct imyp_sdl_backend_data imyp_sdl_backend_data_static;
#endif


#ifndef IMYP_ANSIC
static void SDLCALL imyp_sdl_fill_buffer IMYP_PARAMS ((void * userdata, Uint8 * stream, int len));
#endif

/**
 * Fill the given buffer with new data for playing.
 * \param userdata User data (a pointer to a imyp_sdl_backend_data structure).
 * \param stream The buffer to fill.
 * \param len The length of the buffer to fill.
 */
static void SDLCALL imyp_sdl_fill_buffer (
#ifdef IMYP_ANSIC
	void * userdata, Uint8 * stream, int len)
#else
	userdata, stream, len)
	void * userdata;
	Uint8 * stream;
	int len;
#endif
{
	unsigned long int i;
	unsigned int quality = 16;
	int is_le = 1;
	int is_uns = 0;
	int nsamp;
	struct imyp_sdl_backend_data * data =
		(struct imyp_sdl_backend_data *)userdata;

	if ( (stream == NULL) || (len <= 0) || (userdata == NULL) )
	{
		return;
	}

	if ( (data->received.format == AUDIO_U8) || (data->received.format == AUDIO_S8) )
	{
		if ( data->received.format == AUDIO_U8 )
		{
			is_uns = 1;
		}
		else
		{
			is_uns = 0;
		}
		quality = 8;
	}
	else if ( (data->received.format == AUDIO_S16LSB) || (data->received.format == AUDIO_S16MSB)
		|| (data->received.format == AUDIO_U16LSB) || (data->received.format == AUDIO_U16MSB) )
	{
		if ( (data->received.format == AUDIO_S16MSB) || (data->received.format == AUDIO_U16MSB) )
		{
			is_le = 0;
		}
		else
		{
			is_le = 1;
		}
		if ( (data->received.format == AUDIO_U16LSB) || (data->received.format == AUDIO_U16MSB) )
		{
			is_uns = 1;
		}
		else
		{
			is_uns = 0;
		}
		quality = 16;
	}
	for ( i = data->last_index; i < data->last_index + (unsigned long int)len; i++ )
	{
		((char *)stream)[i - data->last_index] = 0;
	}

	if ( data->samples_remain > 0 )
	{
		/* the number of samples to put in the buffer: */
		nsamp = (int)IMYP_MIN(len / ((int)quality / 8), data->samples_remain);
		i = data->last_index;
		imyp_generate_samples (data->tone_freq, data->volume_level,
			data->duration, stream, nsamp,
			is_le, is_uns, quality, (unsigned int)data->received.freq,
			&i);
		data->last_index = i;
		data->samples_remain -= nsamp;
		if ( data->samples_remain <= 0 )
		{
			data->samples_remain = 0;
		}
	}
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
imyp_sdl_play_tune (
#ifdef IMYP_ANSIC
	imyp_backend_data_t * const imyp_data,
	const double freq,
	const int volume_level,
	const int duration,
	void * const buf IMYP_ATTR ((unused)),
	int bufsize)
#else
	imyp_data, freq, volume_level, duration, buf, bufsize)
	imyp_backend_data_t * const imyp_data;
	const double freq;
	const int volume_level;
	const int duration;
	void * const buf IMYP_ATTR ((unused));
	int bufsize;
#endif
{
	int qual_bits = 16;
	int qual_bytes;
	struct imyp_sdl_backend_data * data =
		(struct imyp_sdl_backend_data *)imyp_data;

	if ( data == NULL )
	{
		return -100;
	}

	data->tone_freq = freq;
	data->volume_level = volume_level;
	data->duration = duration;
	data->last_index = 0;

	/* set the number of remaining samples to the initial duration of the tone */
	if ( (data->received.format == AUDIO_U8) || (data->received.format == AUDIO_S8) )
	{
		qual_bits = 8;
	}
	qual_bytes = qual_bits/8;
	data->samples_remain = (long int)(duration * (long int)data->received.freq * qual_bytes) / 1000; /* bytes */
	data->samples_remain = IMYP_MIN (bufsize, data->samples_remain); /* bytes */
	data->samples_remain /= qual_bytes; /* samples */

	SDL_PauseAudio (0);	/* start playing */
	SDL_UnlockAudio ();

	while ( (data->samples_remain > 0) && (imyp_sig_recvd == 0) ) {}

	/*imyp_sdl_pause (imyp_data, duration); */
	SDL_PauseAudio (1);
	SDL_LockAudio ();

	data->tone_freq = 0.0;
	data->volume_level = 0;
	data->duration = 0;
	data->last_index = 0;
	data->samples_remain = 0;

	return 0;
}

/**
 * Pauses for the specified period of time.
 * \param imyp_data pointer to the backend's private data.
 * \param milliseconds Number of milliseconds to pause.
 */
void
imyp_sdl_pause (
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

	SDL_Delay ((Uint32)milliseconds);
/*
#ifdef IMYP_HAVE_SELECT
	imyp_pause_select (milliseconds);
#else
	SDL_Delay ((Uint32)milliseconds);
#endif
*/
}

/**
 * Outputs the given text.
 * \param imyp_data pointer to the backend's private data.
 * \param text The text to output.
 */
void
imyp_sdl_put_text (
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
 * Initializes the SDL library for use.
 * \param imyp_data pointer to storage for the backend's private data.
 * \return 0 on success.
 */
int
imyp_sdl_init (
#ifdef IMYP_ANSIC
	imyp_backend_data_t ** const imyp_data,
	const char * const dev_file)
#else
	imyp_data, dev_file)
	imyp_backend_data_t ** const imyp_data;
	const char * const dev_file;
#endif
{
	int res;
	struct imyp_sdl_backend_data * data;
	SDL_AudioSpec desired = {44100, AUDIO_S16LSB, 1, 0, 44100 /* do NOT set to less than 32768 */,
		0, 0, &imyp_sdl_fill_buffer, NULL};
	enum IMYP_SAMPLE_FORMATS format;
	char * colon;
	int scanf_res;

	if ( imyp_data == NULL )
	{
		return -100;
	}

	res = SDL_Init (SDL_INIT_AUDIO | SDL_INIT_TIMER);
	if ( res != 0 )
	{
		return res;
	}

#ifdef HAVE_MALLOC
	data = (struct imyp_sdl_backend_data *) malloc (sizeof (
		struct imyp_sdl_backend_data));
	if ( data == NULL )
	{
		return -1;
	}
#else
	data = &imyp_sdl_backend_data_static;
#endif

	data->tone_freq = 0.0;
	data->volume_level = 0;
	data->duration = 0;
	data->last_index = 0;

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

			if ( format == IMYP_SAMPLE_FORMAT_S8LE )
			{
				desired.format = AUDIO_S8;
			}
			else if ( format == IMYP_SAMPLE_FORMAT_S8BE )
			{
				desired.format = AUDIO_S8;
			}
			else if ( format == IMYP_SAMPLE_FORMAT_U8LE )
			{
				desired.format = AUDIO_U8;
			}
			else if ( format == IMYP_SAMPLE_FORMAT_U8BE )
			{
				desired.format = AUDIO_U8;
			}
			else if ( format == IMYP_SAMPLE_FORMAT_S16LE )
			{
				desired.format = AUDIO_S16LSB;
			}
			else if ( format == IMYP_SAMPLE_FORMAT_S16BE )
			{
				desired.format = AUDIO_S16MSB;
			}
			else if ( format == IMYP_SAMPLE_FORMAT_U16LE )
			{
				desired.format = AUDIO_U16LSB;
			}
			else if ( format == IMYP_SAMPLE_FORMAT_U16BE )
			{
				desired.format = AUDIO_U16MSB;
			}
			/* wipe the colon to read the sampling rate */
			*colon = '\0';
		}
		/* get the sampling rate: */
		scanf_res = sscanf (dev_file, "%d", &(desired.freq));
		if ( scanf_res == 1 )
		{
			if ( desired.freq <= 0 )
			{
				desired.freq = 44100;
			}
		}
	}

	desired.userdata = data;
	res = SDL_OpenAudio (&desired, &(data->received));
	if ( res != 0 )
	{
#ifdef HAVE_MALLOC
		free (data);
#endif
		SDL_Quit ();
		return res;
	}
	SDL_LockAudio ();

	*imyp_data = (imyp_backend_data_t *)data;

	return 0;
}

/**
 * Closes the SDL library.
 * \param imyp_data pointer to the backend's private data.
 * \return 0 on success.
 */
int
imyp_sdl_close (
#ifdef IMYP_ANSIC
	imyp_backend_data_t * const imyp_data)
#else
	imyp_data)
	imyp_backend_data_t * const imyp_data;
#endif
{
	SDL_UnlockAudio ();
	SDL_CloseAudio ();
	SDL_Quit ();
	if ( imyp_data != NULL )
	{
#ifdef HAVE_MALLOC
		free (imyp_data);
#endif
	}
	return 0;
}

/**
 * Displays the version of the SDL backend.
 * \param imyp_data pointer to the backend's private data.
 */
void
imyp_sdl_version (
#ifdef IMYP_ANSIC
	imyp_backend_data_t * const imyp_data IMYP_ATTR ((unused)))
#else
	imyp_data)
	imyp_backend_data_t * const imyp_data IMYP_ATTR ((unused));
#endif
{
#if (defined SDL_MAJOR_VERSION) && (defined SDL_MINOR_VERSION) && (defined SDL_PATCHLEVEL)
	printf ( "SDL: %d.%d.%d\n", SDL_MAJOR_VERSION, SDL_MINOR_VERSION,
		SDL_PATCHLEVEL );
#else
# if (defined SDL_MAJOR_VERSION) && (defined SDL_MINOR_VERSION)
	printf ( "SDL: %d.%d\n", SDL_MAJOR_VERSION, SDL_MINOR_VERSION );
# else
#  if (defined SDL_MAJOR_VERSION)
	printf ( "SDL: %d\n", SDL_MAJOR_VERSION );
#  else
	printf ( "SDL: ?\n" );
#  endif
# endif
#endif
}
