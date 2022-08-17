/*
 * A program for playing iMelody ringtones (IMY files).
 *	-- Allegro backend.
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
#include "imyp_all.h"
#include "imyp_sig.h"

#include <stdio.h>

#if (defined HAVE_LIBALLEG) && (defined HAVE_ALLEGRO_H)
# include <allegro.h>
#else
# error The Allegro library was not found.
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

/* DOS: manually set all the drivers to use (no gfx, no midi,
   no joystick, ...) to save space */
#if (defined BEGIN_GFX_DRIVER_LIST) && (defined END_GFX_DRIVER_LIST)
BEGIN_GFX_DRIVER_LIST
END_GFX_DRIVER_LIST
#endif

#if (defined BEGIN_COLOR_DEPTH_LIST) && (defined END_COLOR_DEPTH_LIST)
BEGIN_COLOR_DEPTH_LIST
END_COLOR_DEPTH_LIST
#endif

#if (defined BEGIN_JOYSTICK_DRIVER_LIST) && (defined END_JOYSTICK_DRIVER_LIST)
BEGIN_JOYSTICK_DRIVER_LIST
END_JOYSTICK_DRIVER_LIST
#endif

#if (defined BEGIN_MIDI_DRIVER_LIST) && (defined END_MIDI_DRIVER_LIST)
BEGIN_MIDI_DRIVER_LIST
END_MIDI_DRIVER_LIST
#endif

#if (defined BEGIN_DIGI_DRIVER_LIST) && (defined END_DIGI_DRIVER_LIST)
/* required, because we include the midi list */
BEGIN_DIGI_DRIVER_LIST

/* Unix: */
# ifdef DIGI_DRIVER_ALSA
DIGI_DRIVER_ALSA
# endif
# ifdef DIGI_DRIVER_OSS
DIGI_DRIVER_OSS
# endif
# ifdef DIGI_DRIVER_ESD
DIGI_DRIVER_ESD
# endif
# ifdef DIGI_DRIVER_ARTS
DIGI_DRIVER_ARTS
# endif

/* DOS/Windows: */
# ifdef DIGI_DRIVER_SB
DIGI_DRIVER_SB
# endif
# ifdef DIGI_DRIVER_SOUNDSCAPE
DIGI_DRIVER_SOUNDSCAPE
# endif
# ifdef DIGI_DRIVER_AUDIODRIVE
DIGI_DRIVER_AUDIODRIVE
# endif
# ifdef DIGI_DRIVER_WINSOUNDSYS
DIGI_DRIVER_WINSOUNDSYS
# endif

/* BeOS: */
# ifdef DIGI_DRIVER_BEOS
DIGI_DRIVER_BEOS
# endif

END_DIGI_DRIVER_LIST
#endif /* BEGIN_DIGI_DRIVER_LIST && END_DIGI_DRIVER_LIST */

/**
 * Creates an AUDIOSTREAM for playing notes.
 * \param number_of_samples The number of samples for the buffer.
 * \param vol Volume of the tone (from 0 to 15).
 * \param quality Will get the audio stream's quality (16 or 8).
 * \param sampling_frequency Will get the audio stream's sampling frequency (44100, 22050 or 11025).
 * \return an allocated AUDIOSTREAM or NULL in case of error.
 */
static AUDIOSTREAM *
imyp_all_audiostream_init (
#if defined (__STDC__) || defined (_AIX) \
	|| (defined (__mips) && defined (_SYSTYPE_SVR4)) \
	|| defined(WIN32) || defined(__cplusplus)
	const int number_of_samples,
	const int vol,
	unsigned int * const quality,
	unsigned int * const sampling_frequency)
#else
	number_of_samples, vol, quality, sampling_frequency)
	const int number_of_samples;
	const int vol;
	unsigned int * const quality;
	unsigned int * const sampling_frequency;
#endif
{
	AUDIOSTREAM * as;
#define IMYP_ALL_VOL (vol*255/15)

	as = play_audio_stream ((int)number_of_samples, 16, FALSE, 44100, IMYP_ALL_VOL, 128);
	if ( as != NULL )
	{
		*quality = 16;
		*sampling_frequency = 44100;
		return as;
	}
	as = play_audio_stream (number_of_samples, 16, FALSE, 22050, IMYP_ALL_VOL, 128);
	if ( as != NULL )
	{
		*quality = 16;
		*sampling_frequency = 22050;
		return as;
	}
	as = play_audio_stream (number_of_samples, 16, FALSE, 11025, IMYP_ALL_VOL, 128);
	if ( as != NULL )
	{
		*quality = 16;
		*sampling_frequency = 11025;
		return as;
	}
	as = play_audio_stream (number_of_samples, 8, FALSE, 44100, IMYP_ALL_VOL, 128);
	if ( as != NULL )
	{
		*quality = 8;
		*sampling_frequency = 44100;
		return as;
	}
	as = play_audio_stream (number_of_samples, 8, FALSE, 22050, IMYP_ALL_VOL, 128);
	if ( as != NULL )
	{
		*quality = 8;
		*sampling_frequency = 22050;
		return as;
	}
	as = play_audio_stream (number_of_samples, 8, FALSE, 11025, IMYP_ALL_VOL, 128);
	if ( as != NULL )
	{
		*quality = 8;
		*sampling_frequency = 11025;
		return as;
	}
#undef IMYP_ALL_VOL

	*quality = 0;
	*sampling_frequency = 0;
	return NULL;
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
imyp_all_play_tune (
#if defined (__STDC__) || defined (_AIX) \
	|| (defined (__mips) && defined (_SYSTYPE_SVR4)) \
	|| defined(WIN32) || defined(__cplusplus)
	const double freq,
	const int volume_level,
	const int duration,
	void * const buf,
	int bufsize)
#else
	freq, volume_level, duration, buf, bufsize)
	const double freq;
	const int volume_level;
	const int duration;
	void * const buf;
	int bufsize;
#endif
{
	AUDIOSTREAM * as;
	void * new_buf;
	int i;
	unsigned int sampfreq;
	unsigned int quality;
	int samp;	/* better than float */

	if ( buf == NULL ) return -1;

	as = imyp_all_audiostream_init (bufsize, volume_level, &quality, &sampfreq);
	if ( as != NULL )
	{
#define NSAMP (sampfreq/freq)
		for ( i=0; i < bufsize; i++ )
		{
			if ( sig_recvd != 0 )
			{
				stop_audio_stream (as);
				return -2;
			}
#if (defined HAVE_SIN) || (defined HAVE_LIBM)
			samp = ((1<<(quality-1))-1) /* disable to get rectangular wave */ +
				/* The "/3" is required to have a full sine wave, not
				   trapese-like wave */
				IMYP_ROUND (((1<<(quality-1))-1)
					*sin ((i%((int)IMYP_ROUND(NSAMP)))*(2*M_PI/NSAMP))/3);
				/*if ( i < NSAMP ) printf("buf[%d]=%d\n", i, buf[i]);*/;
#else
			samp = (int) IMYP_ROUND ((i%((int)IMYP_ROUND(NSAMP)))*
				(((1<<(quality-1))-1)/NSAMP));
#endif
			if ( quality == 16 )
			{
				((short *)buf)[i] = samp;
			}
			else if ( quality == 8 )
			{
				((char *)buf)[i] = samp;
			}
		}
		new_buf = get_audio_stream_buffer (as);
		if ( new_buf != NULL )
		{
			memcpy (new_buf, buf, bufsize*(quality/8));
			if ( sig_recvd != 0 )
			{
				stop_audio_stream (as);
				return -2;
			}
			free_audio_stream_buffer (as);
		}
		rest (duration);
		stop_audio_stream (as);
		return 0;
	}
	else
	{
		return -3;
	}
#undef NSAMP
}

/**
 * Pauses for the specified period of time.
 * \param milliseconds Number of milliseconds to pause.
 */
void
imyp_all_pause (
#if defined (__STDC__) || defined (_AIX) \
	|| (defined (__mips) && defined (_SYSTYPE_SVR4)) \
	|| defined(WIN32) || defined(__cplusplus)
	const int milliseconds)
#else
	milliseconds)
	const int milliseconds;
#endif
{
	rest (milliseconds);
}

/**
 * Outputs the given text.
 * \param text The text to output.
 */
void
imyp_all_put_text (
#if defined (__STDC__) || defined (_AIX) \
	|| (defined (__mips) && defined (_SYSTYPE_SVR4)) \
	|| defined(WIN32) || defined(__cplusplus)
	const char * const text)
#else
	text)
	const char * const text;
#endif
{
	if ( text != NULL ) printf ("%s", text);
}

/**
 * Initializes the Allegro library for use.
 * \return 0 on success.
 */
int
imyp_all_init (
#if defined (__STDC__) || defined (_AIX) \
	|| (defined (__mips) && defined (_SYSTYPE_SVR4)) \
	|| defined(WIN32) || defined(__cplusplus)
	void
#endif
)
{
	int res;
	res = allegro_init ();
	if ( res != 0 ) return res;

	install_keyboard ();
	/* 2 voices, so they can be interchanged, instead of constantly
	   waiting for the first one */
	reserve_voices (2, 0);
	res = install_sound (DIGI_AUTODETECT, MIDI_NONE, NULL);
	if ( res != 0 )
	{
		reserve_voices (1, 0);
		res = install_sound (DIGI_AUTODETECT, MIDI_NONE, NULL);
		if ( res != 0 )
		{
			allegro_exit ();
			return res;
		}
	}
	return 0;
}

/**
 * Closes the Allegro library.
 * \return 0 on success.
 */
int
imyp_all_close (
#if defined (__STDC__) || defined (_AIX) \
	|| (defined (__mips) && defined (_SYSTYPE_SVR4)) \
	|| defined(WIN32) || defined(__cplusplus)
	void
#endif
)
{
	remove_sound ();
	allegro_exit ();
	return 0;
}
