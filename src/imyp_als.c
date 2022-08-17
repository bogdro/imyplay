/*
 * A program for playing iMelody ringtones (IMY files).
 *	-- ALSA backend.
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
#include "imyp_als.h"
#include "imyp_sig.h"

#include <stdio.h>

#define ALSA_PCM_NEW_HW_PARAMS_API

#if (defined HAVE_LIBASOUND) && ((defined HAVE_ASOUNDLIB_H) || (defined HAVE_ALSA_ASOUNDLIB_H))
# if (defined HAVE_ASOUNDLIB_H)
#  include <asoundlib.h>
# else
#  include <alsa/asoundlib.h>
# endif
#else
# error The ALSA library was not found.
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

static snd_pcm_t * handle;
static snd_pcm_hw_params_t * params;

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
imyp_alsa_play_tune (
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
	int res, dir;
	int samp;	/* better than float */
	int i;
	snd_pcm_format_t format;
	unsigned int quality = 16;
	unsigned int sampfreq = 44100;
	int is_le = 1;
	int is_uns = 0;

	snd_pcm_hw_params_get_format (params, &format);
	if ( (format == SND_PCM_FORMAT_S8) || (format == SND_PCM_FORMAT_U8) )
	{
		if ( format == SND_PCM_FORMAT_U8 )
		{
			is_uns = 1;
		}
		else
		{
			is_uns = 0;
		}
		quality = 8;
	}
	else if ( (format == SND_PCM_FORMAT_S16_LE) || (format == SND_PCM_FORMAT_S16_BE)
		|| (format == SND_PCM_FORMAT_U16_LE) || (format == SND_PCM_FORMAT_U16_BE) )
	{
		if ( (format == SND_PCM_FORMAT_S16_BE) || (format == SND_PCM_FORMAT_U16_BE) )
		{
			is_le = 0;
		}
		else
		{
			is_le = 1;
		}
		if ( (format == SND_PCM_FORMAT_U16_LE) || (format == SND_PCM_FORMAT_U16_BE) )
		{
			is_uns = 1;
		}
		else
		{
			is_uns = 0;
		}
		quality = 16;
	}
	snd_pcm_hw_params_get_rate (params, &sampfreq, &dir);
	bufsize = MIN (bufsize, (duration * (int)sampfreq * ((int)quality/8)) / 1000);

#define NSAMP (sampfreq/freq)
	for ( i=0; i < bufsize; i++ )
	{
		if ( sig_recvd != 0 )
		{
			return -2;
		}
#if (defined HAVE_SIN) || (defined HAVE_LIBM)
		samp = ((1<<(quality-1))-1) /* disable to get rectangular wave */ +
			/* The "/3" is required to have a full sine wave, not
			   trapese-like wave */
			IMYP_ROUND (((1<<(quality-1))-1)
				* sin ((i%((int)IMYP_ROUND(NSAMP)))*(2*M_PI/NSAMP))/3);
#else
		samp = (int) IMYP_ROUND ((i%((int)IMYP_ROUND(NSAMP)))*
			(((1<<(quality-1))-1)/NSAMP));
#endif
		if ( is_uns != 0 )
		{
			samp += (1<<(quality-1));
		}
		if ( quality == 16 )
		{
			if ( is_le != 0 )
			{
				((char *)buf)[i] = ((samp * volume_level) / IMYP_MAX_IMY_VOLUME) & 0x0FF;
				i++;
				((char *)buf)[i] = (((samp * volume_level) / IMYP_MAX_IMY_VOLUME) >> 8) & 0x0FF;
			}
			else
			{
				((char *)buf)[i] = (((samp * volume_level) / IMYP_MAX_IMY_VOLUME) >> 8) & 0x0FF;
				i++;
				((char *)buf)[i] = ((samp * volume_level) / IMYP_MAX_IMY_VOLUME) & 0x0FF;
			}
		}
		else if ( quality == 8 )
		{
			((char *)buf)[i] = (samp * volume_level) / IMYP_MAX_IMY_VOLUME;
		}
	}
	res = snd_pcm_writei (handle, buf, (snd_pcm_uframes_t)bufsize / (quality/8));
	if ( res >= 0 )
	{
		return 0;
	}
	return res;
}

/**
 * Pauses for the specified period of time.
 * \param milliseconds Number of milliseconds to pause.
 */
void
imyp_alsa_pause (
#if defined (__STDC__) || defined (_AIX) \
	|| (defined (__mips) && defined (_SYSTYPE_SVR4)) \
	|| defined(WIN32) || defined(__cplusplus)
	const int milliseconds
# if ! (((defined HAVE_SYS_SELECT_H) || (((defined TIME_WITH_SYS_TIME)	\
	|| (defined HAVE_SYS_TIME_H) || (defined HAVE_TIME_H))		\
 		&& (defined HAVE_UNISTD_H)))				\
	&& (defined HAVE_SELECT))
		IMYP_ATTR ((unused))
# endif
	)
#else
	milliseconds
# if ! (((defined HAVE_SYS_SELECT_H) || (((defined TIME_WITH_SYS_TIME)	\
	|| (defined HAVE_SYS_TIME_H) || (defined HAVE_TIME_H))		\
 		&& (defined HAVE_UNISTD_H)))				\
	&& (defined HAVE_SELECT))
		IMYP_ATTR ((unused))
# endif
	)
	const int milliseconds;
#endif
{
#if (((defined HAVE_SYS_SELECT_H) || (((defined TIME_WITH_SYS_TIME)	\
	|| (defined HAVE_SYS_TIME_H) || (defined HAVE_TIME_H))		\
 		&& (defined HAVE_UNISTD_H)))				\
	&& (defined HAVE_SELECT))
	struct timeval tv;
	tv.tv_sec = milliseconds / 1000;
	tv.tv_usec = ( milliseconds * 1000 ) % 1000000;
	select ( 0, NULL, NULL, NULL, &tv );
#endif
}

/**
 * Outputs the given text.
 * \param text The text to output.
 */
void
imyp_alsa_put_text (
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
 * Initializes the ALSA library for use.
 * \return 0 on success.
 */
int
imyp_alsa_init (
#if defined (__STDC__) || defined (_AIX) \
	|| (defined (__mips) && defined (_SYSTYPE_SVR4)) \
	|| defined(WIN32) || defined(__cplusplus)
	const char * const dev_file)
#else
	dev_file)
	const char * const dev_file;
#endif
{
	unsigned int val;
	int dir;
	int res;

	/* http://www.linuxjournal.com/article/6735 */
	if ( dev_file == NULL )
	{
		res = snd_pcm_open (&handle, "default", SND_PCM_STREAM_PLAYBACK, 0);
		if ( res < 0 ) return -1;
	}
	else
	{
		res = snd_pcm_open (&handle, dev_file, SND_PCM_STREAM_PLAYBACK, 0);
		if ( res < 0 )
		{
			res = snd_pcm_open (&handle, "default", SND_PCM_STREAM_PLAYBACK, 0);
			if ( res < 0 ) return -1;
		}
	}

	res = snd_pcm_hw_params_malloc (&params);
	if ( res < 0 )
	{
		snd_pcm_close (handle);
		return res;
	}

	/* Fill it in with default values. */
	snd_pcm_hw_params_any (handle, params);

	/* Set the desired hardware parameters. */

	/* Interleaved mode */
	snd_pcm_hw_params_set_access (handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);

	/* Signed 16-bit little-endian format */
	snd_pcm_hw_params_set_format (handle, params, SND_PCM_FORMAT_S16_LE);

	/* One channel (mono) */
	snd_pcm_hw_params_set_channels (handle, params, 1);

	/* 44100 bits/second sampling rate (CD quality) */
	val = 44100;
	snd_pcm_hw_params_set_rate_near (handle, params, &val, &dir);

	/* Write the parameters to the driver */
	res = snd_pcm_hw_params (handle, params);
	if ( res < 0 )
	{
		snd_pcm_close (handle);
		return res;
	}
	return 0;
}

/**
 * Closes the ALSA library.
 * \return 0 on success.
 */
int
imyp_alsa_close (
#if defined (__STDC__) || defined (_AIX) \
	|| (defined (__mips) && defined (_SYSTYPE_SVR4)) \
	|| defined(WIN32) || defined(__cplusplus)
	void
#endif
)
{
	snd_pcm_drain (handle);
	snd_pcm_close (handle);
	return 0;
}
