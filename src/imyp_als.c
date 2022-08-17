/*
 * A program for playing iMelody ringtones (IMY files).
 *	-- ALSA backend.
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
#include "imyp_als.h"
#include "imyp_sig.h"
#include "imyputil.h"

#include <stdio.h>

#define ALSA_PCM_NEW_HW_PARAMS_API

#ifdef IMYP_HAVE_ALSA
# if (defined HAVE_ASOUNDLIB_H)
#  include <asoundlib.h>
#  include <version.h>
# else
#  include <alsa/asoundlib.h>
#  include <alsa/version.h>
# endif
#else
# error ALSA requested, but components not found.
#endif

#ifdef HAVE_STDLIB_H
# include <stdlib.h>
#endif

#ifdef HAVE_MALLOC_H
# include <malloc.h>
#endif

struct imyp_alsa_backend_data
{
	snd_pcm_t * handle;
	snd_pcm_hw_params_t * params;
};

#ifndef HAVE_MALLOC
static struct imyp_alsa_backend_data imyp_alsa_backend_data_static;
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
imyp_alsa_play_tune (
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
	int res, dir;
	snd_pcm_format_t format;
	unsigned int quality = 16;
	unsigned int sampfreq = 44100;
	int is_le = 1;
	int is_uns = 0;
	struct imyp_alsa_backend_data * data =
		(struct imyp_alsa_backend_data *)imyp_data;

	if ( (data == NULL) || (buf == NULL) || (bufsize <= 0) )
	{
		return -1;
	}

	snd_pcm_hw_params_get_format (data->params, &format);
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
	snd_pcm_hw_params_get_rate (data->params, &sampfreq, &dir);
	bufsize = imyp_generate_samples (freq, volume_level, duration, buf, bufsize,
		is_le, is_uns, quality, sampfreq, NULL);
	if ( sig_recvd != 0 )
	{
		return -2;
	}
	res = snd_pcm_writei (data->handle, buf, (snd_pcm_uframes_t)bufsize / (quality/8));
	if ( res >= 0 )
	{
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
imyp_alsa_pause (
#ifdef IMYP_ANSIC
	imyp_backend_data_t * const imyp_data IMYP_ATTR ((unused)),
	const int milliseconds)
#else
	imyp_data, milliseconds)
	imyp_backend_data_t * const imyp_data IMYP_ATTR ((unused));
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
imyp_alsa_put_text (
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
 * Initializes the ALSA library for use.
 * \param imyp_data pointer to storage for the backend's private data.
 * \param dev_file The device to open.
 * \return 0 on success.
 */
int
imyp_alsa_init (
#ifdef IMYP_ANSIC
	imyp_backend_data_t ** const imyp_data,
	const char * const dev_file)
#else
	imyp_data, dev_file)
	imyp_backend_data_t ** const imyp_data;
	const char * const dev_file;
#endif
{
	unsigned int val;
	int dir = 0;
	int res;
	struct imyp_alsa_backend_data * data;

	if ( imyp_data == NULL )
	{
		return -6;
	}
#ifdef HAVE_MALLOC
	data = (struct imyp_alsa_backend_data *) malloc (sizeof (
		struct imyp_alsa_backend_data));
	if ( data == NULL )
	{
		return -7;
	}
#else
	data = &imyp_alsa_backend_data_static;
#endif

	/* http://www.linuxjournal.com/article/6735 */
	if ( dev_file == NULL )
	{
		res = snd_pcm_open (&(data->handle), "default",
			SND_PCM_STREAM_PLAYBACK, 0);
		if ( res < 0 )
		{
#ifdef HAVE_MALLOC
			free (data);
#endif
			return -1;
		}
	}
	else
	{
		res = snd_pcm_open (&(data->handle), dev_file,
			SND_PCM_STREAM_PLAYBACK, 0);
		if ( res < 0 )
		{
			res = snd_pcm_open (&(data->handle),
				"default", SND_PCM_STREAM_PLAYBACK, 0);
			if ( res < 0 )
			{
#ifdef HAVE_MALLOC
				free (data);
#endif
				return -1;
			}
		}
	}

	res = snd_pcm_hw_params_malloc (&(data->params));
	if ( res < 0 )
	{
		snd_pcm_close (data->handle);
#ifdef HAVE_MALLOC
		free (data);
#endif
		return res;
	}

	/* Fill it in with default values. */
	snd_pcm_hw_params_any (data->handle, data->params);

	/* Set the desired hardware parameters. */

	/* Interleaved mode */
	snd_pcm_hw_params_set_access (data->handle, data->params, SND_PCM_ACCESS_RW_INTERLEAVED);

	/* Signed 16-bit little-endian format */
	snd_pcm_hw_params_set_format (data->handle, data->params, SND_PCM_FORMAT_S16_LE);

	/* One channel (mono) */
	snd_pcm_hw_params_set_channels (data->handle, data->params, 1);

	/* 44100 bits/second sampling rate (CD quality) */
	val = 44100;
	snd_pcm_hw_params_set_rate_near (data->handle, data->params, &val, &dir);

	/* Write the parameters to the driver */
	res = snd_pcm_hw_params (data->handle, data->params);
	if ( res < 0 )
	{
		snd_pcm_close (data->handle);
		snd_pcm_hw_params_free (data->params);
#ifdef HAVE_MALLOC
		free (data);
#endif
		return res;
	}
	*imyp_data = (imyp_backend_data_t *)data;
	return 0;
}

/**
 * Closes the ALSA library.
 * \param imyp_data pointer to the backend's private data.
 * \return 0 on success.
 */
int
imyp_alsa_close (
#ifdef IMYP_ANSIC
	imyp_backend_data_t * const imyp_data)
#else
	imyp_data)
	imyp_backend_data_t * const imyp_data;
#endif
{
	struct imyp_alsa_backend_data * data =
		(struct imyp_alsa_backend_data *)imyp_data;

	if ( data != NULL )
	{
		snd_pcm_drain (data->handle);
		snd_pcm_close (data->handle);
		snd_pcm_hw_params_free (data->params);
#ifdef HAVE_MALLOC
		free (data);
#endif
	}
	return 0;
}

/**
 * Displays the version of the ALSA library IMYplay was compiled with.
 * \param imyp_data pointer to the backend's private data.
 */
void
imyp_alsa_version (
#ifdef IMYP_ANSIC
	imyp_backend_data_t * const imyp_data IMYP_ATTR ((unused)))
#else
	imyp_data)
	imyp_backend_data_t * const imyp_data IMYP_ATTR ((unused));
#endif
{
#ifdef SND_LIB_VERSION_STR
	printf ( "ALSA: %s\n", SND_LIB_VERSION_STR );
#else
	printf ( "ALSA: ?\n" );
#endif
}

