/*
 * A program for playing iMelody ringtones (IMY files).
 *	-- SDL backend.
 *
 * Copyright (C) 2009 Bogdan Drozdowski, bogdandr (at) op.pl
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

#include <stdio.h>

#if (defined HAVE_LIBSDL) && ((defined HAVE_SDL_H) || (defined HAVE_SDL_SDL_H))
# if (defined HAVE_SDL_H)
#  include <SDL.h>
# else
#  include <SDL/SDL.h>
# endif
#else
# error The SDL library was not found.
#endif

#ifdef HAVE_MATH_H
# include <math.h>	/* sin() */
#endif

#ifndef M_PI
# define M_PI 3.14159265358979323846
#endif

#ifdef HAVE_STRING_H
# if ((!defined STDC_HEADERS) || (!STDC_HEADERS)) && (defined HAVE_MEMORY_H)
#  include <memory.h>
# endif
# include <string.h>
#endif

struct imyp_sdl_sound_data
{
	double tone_freq;
	int volume_level;
	int duration;
	int last_index;
	SDL_AudioSpec received;
};

static struct imyp_sdl_sound_data sound_data;

/**
 * Fill the given buffer with new data for playing.
 * \param userdata User data (a pointer to a imyp_sdl_sound_data structure).
 * \param stream The buffer to fill.
 * \param len The length of the buffer to fill.
 */
static void SDLCALL imyp_sdl_fill_buffer (
#if defined (__STDC__) || defined (_AIX) \
	|| (defined (__mips) && defined (_SYSTYPE_SVR4)) \
	|| defined(WIN32) || defined(__cplusplus)
	void * userdata, Uint8 * stream, int len)
#else
	userdata, stream, len)
	void * userdata;
	Uint8 * stream;
	int len;
#endif
{
	int i;
	int samp;	/* better than float */
	unsigned int quality = 16;
	int is_le = 1;
	int is_uns = 0;

	if ( (stream == NULL) || (len <= 0) || (userdata == NULL) ) return;

#define data ((struct imyp_sdl_sound_data *)userdata)

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

#define NSAMP ((data->received.freq)/(data->tone_freq))

	for ( i=data->last_index; i < data->last_index+len; i++ )
	{
		if ( sig_recvd != 0 )
		{
			return;
		}
#if (defined HAVE_SIN) || (defined HAVE_LIBM)
		samp = ((1<<(quality-1))-1) /* disable to get rectangular wave */ +
			/* The "/3" is required to have a full sine wave, not
			   trapese-like wave */
			IMYP_ROUND (((1<<(quality-1))-1)
				* sin ((i%((int)IMYP_ROUND(NSAMP)))*(2*M_PI/NSAMP))/3);
			/*if ( i < NSAMP ) printf("buf[%d]=%d\n", i, buf[i]);*/;
#else
		samp = (int) IMYP_ROUND ((i%((int)IMYP_ROUND(NSAMP)))*
			(((1<<(quality-1))-1)/NSAMP));
#endif
		if ( quality == 16 )
		{
			if ( is_le != 0 )
			{
				((char *)stream)[i-data->last_index] = ((samp * data->volume_level) / IMYP_MAX_IMY_VOLUME) & 0x0FF;
				i++;
				((char *)stream)[i-data->last_index] = (((samp * data->volume_level) / IMYP_MAX_IMY_VOLUME) >> 8) & 0x0FF;
			}
			else
			{
				((char *)stream)[i-data->last_index] = (((samp * data->volume_level) / IMYP_MAX_IMY_VOLUME) >> 8) & 0x0FF;
				i++;
				((char *)stream)[i-data->last_index] = ((samp * data->volume_level) / IMYP_MAX_IMY_VOLUME) & 0x0FF;
			}
		}
		else if ( quality == 8 )
		{
			((char *)stream)[i-data->last_index] = (samp*data->volume_level)/IMYP_MAX_IMY_VOLUME;
		}
	}
	data->last_index += i-data->last_index;
	while ( data->last_index > (int)NSAMP )
	{
		data->last_index -= (int)NSAMP;
	}
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
imyp_sdl_play_tune (
#if defined (__STDC__) || defined (_AIX) \
	|| (defined (__mips) && defined (_SYSTYPE_SVR4)) \
	|| defined(WIN32) || defined(__cplusplus)
	const double freq,
	const int volume_level,
	const int duration,
	void * const buf IMYP_ATTR ((unused)),
	int bufsize IMYP_ATTR ((unused)))
#else
	freq, volume_level, duration, buf IMYP_ATTR ((unused)), bufsize IMYP_ATTR ((unused)))
	const double freq;
	const int volume_level;
	const int duration;
	void * const buf;
	int bufsize;
#endif
{
	int res;
	SDL_AudioSpec desired = {44100, AUDIO_S16LSB, 1, 0, 32768, 0, 0,
		&imyp_sdl_fill_buffer, &sound_data};

	sound_data.tone_freq = freq;
	sound_data.volume_level = volume_level;
	sound_data.duration = duration;
	sound_data.last_index = 0;

	desired.freq = 44100;
	desired.format = AUDIO_S16LSB;
	desired.channels = 1;
	desired.samples = 1024;
	desired.callback = &imyp_sdl_fill_buffer;
	desired.userdata = &sound_data;

	res = SDL_OpenAudio (&desired, &(sound_data.received));
	if ( res != 0 )
	{
		return res;
	}
	SDL_PauseAudio (0);	/* start playing */
	SDL_Delay ((Uint32)duration);
	SDL_PauseAudio (1);
	SDL_CloseAudio ();

	return 0;
}

/**
 * Pauses for the specified period of time.
 * \param milliseconds Number of milliseconds to pause.
 */
void
imyp_sdl_pause (
#if defined (__STDC__) || defined (_AIX) \
	|| (defined (__mips) && defined (_SYSTYPE_SVR4)) \
	|| defined(WIN32) || defined(__cplusplus)
	const int milliseconds)
#else
	milliseconds)
	const int milliseconds;
#endif
{
	SDL_Delay ((Uint32)milliseconds);
}

/**
 * Outputs the given text.
 * \param text The text to output.
 */
void
imyp_sdl_put_text (
#if defined (__STDC__) || defined (_AIX) \
	|| (defined (__mips) && defined (_SYSTYPE_SVR4)) \
	|| defined(WIN32) || defined(__cplusplus)
	const char * const text)
#else
	text)
	const char * const text;
#endif
{
	if ( (text != NULL) && (stdout != NULL) ) printf ("%s", text);
}

/**
 * Initializes the SDL library for use.
 * \return 0 on success.
 */
int
imyp_sdl_init (
#if defined (__STDC__) || defined (_AIX) \
	|| (defined (__mips) && defined (_SYSTYPE_SVR4)) \
	|| defined(WIN32) || defined(__cplusplus)
	void
#endif
)
{
	int res;
	res = SDL_Init (SDL_INIT_AUDIO);
	if ( res != 0 ) return res;
	return 0;
}

/**
 * Closes the SDL library.
 * \return 0 on success.
 */
int
imyp_sdl_close (
#if defined (__STDC__) || defined (_AIX) \
	|| (defined (__mips) && defined (_SYSTYPE_SVR4)) \
	|| defined(WIN32) || defined(__cplusplus)
	void
#endif
)
{
	SDL_CloseAudio ();
	SDL_Quit ();
	return 0;
}
