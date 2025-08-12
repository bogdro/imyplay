/*
 * IMYplay - A program for playing iMelody ringtones (IMY files).
 *	-- OSS backend.
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
#include "imyp_oss.h"
#include "imyp_sig.h"
#include "imyputil.h"

#include <stdio.h>

#ifdef IMYP_HAVE_OSS
# include <sys/soundcard.h>
#else
# error OSS requested, but components not found.
#endif

#ifdef HAVE_UNISTD_H
# include <unistd.h>	/* write(), close() */
#endif

#ifdef HAVE_FCNTL_H
# include <fcntl.h>	/* open() */
#endif

#ifdef HAVE_SYS_IOCTL_H
# include <sys/ioctl.h>
#endif

#ifdef HAVE_STDLIB_H
# include <stdlib.h>
#endif

#ifdef HAVE_MALLOC_H
# include <malloc.h>
#endif

#ifndef O_WRONLY
# define O_WRONLY 	1
#endif

struct imyp_oss_backend_data
{
	int pcm_fd;
	int glob_format;
	int glob_speed;
};

#ifdef TEST_COMPILE
# undef IMYP_ANSIC
# if TEST_COMPILE > 1
#  undef HAVE_MALLOC
# endif
#endif

#ifndef HAVE_MALLOC
static struct imyp_oss_backend_data imyp_oss_backend_data_static;
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
imyp_oss_play_tune (
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
	ssize_t res;
	int is_le = 1;
	int is_uns = 0;
	const struct imyp_oss_backend_data * const data =
		(struct imyp_oss_backend_data *)imyp_data;

	if ( (data == NULL) || (buf == NULL) || (bufsize <= 0) )
	{
		return -1;
	}

	if ( (data->glob_format == AFMT_S8) || (data->glob_format == AFMT_U8) )
	{
		if ( data->glob_format == AFMT_U8 )
		{
			is_uns = 1;
		}
		else
		{
			is_uns = 0;
		}
		quality = 8;
	}
	else if ( (data->glob_format == AFMT_S16_LE) || (data->glob_format == AFMT_S16_BE)
		|| (data->glob_format == AFMT_U16_LE) || (data->glob_format == AFMT_U16_BE) )
	{
		if ( (data->glob_format == AFMT_S16_BE) || (data->glob_format == AFMT_U16_BE) )
		{
			is_le = 0;
		}
		else
		{
			is_le = 1;
		}
		if ( (data->glob_format == AFMT_U16_LE) || (data->glob_format == AFMT_U16_BE) )
		{
			is_uns = 1;
		}
		else
		{
			is_uns = 0;
		}
		quality = 16;
	}

	bufsize = imyp_generate_samples (freq, volume_level, duration, buf, bufsize,
		is_le, is_uns, quality, (unsigned int)data->glob_speed, NULL);
	if ( imyp_sig_recvd != 0 )
	{
		return -2;
	}
	res = write (data->pcm_fd, buf, (size_t)bufsize);
	if ( res == bufsize )
	{
		return 0;
	}
	return -3;
}

/**
 * Pauses for the specified period of time.
 * \param imyp_data pointer to the backend's private data.
 * \param milliseconds Number of milliseconds to pause.
 */
void
imyp_oss_pause (
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
imyp_oss_put_text (
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
 * Initializes the OSS subsystem for use.
 * \param imyp_data pointer to storage for the backend's private data.
 * \param dev_file The device to open.
 * \return 0 on success.
 */
int
imyp_oss_init (
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
	unsigned int i;
	unsigned int j;
	const int formats[] = {AFMT_S16_LE, AFMT_U16_LE, AFMT_S16_BE, AFMT_U16_LE,
		AFMT_S8, AFMT_U8};
	const int samp_freqs[] = {44100, 22050, 11025};
	int format;
	struct imyp_oss_backend_data * data;

	if ( imyp_data == NULL )
	{
		return -100;
	}
#ifdef HAVE_MALLOC
	data = (struct imyp_oss_backend_data *) malloc (sizeof (
		struct imyp_oss_backend_data));
	if ( data == NULL )
	{
		return -6;
	}
#else
	data = &imyp_oss_backend_data_static;
#endif

	if ( dev_file == NULL )
	{
		data->pcm_fd = open ("/dev/dsp", O_WRONLY);
		if ( data->pcm_fd < 0 )
		{
#ifdef HAVE_MALLOC
			free (data);
#endif
			return -1;
		}
	}
	else
	{
		data->pcm_fd = open (dev_file, O_WRONLY);
		if ( data->pcm_fd < 0 )
		{
			data->pcm_fd = open ("/dev/dsp", O_WRONLY);
			if ( data->pcm_fd < 0 )
			{
#ifdef HAVE_MALLOC
				free (data);
#endif
				return -1;
			}
		}
	}

	for ( i = 0; i < sizeof (formats) / sizeof (formats[0]); i++ )
	{
		format = formats[i];
		res = ioctl (data->pcm_fd, SNDCTL_DSP_SETFMT, &format);
		if ( res < 0 )
		{
			continue;
		}

		if ( format == formats[i] )
		{
			data->glob_format = format;
		}
		else
		{
			continue;
		}

		format = 1;
		res = ioctl (data->pcm_fd, SNDCTL_DSP_CHANNELS, &format);
		if ( res < 0 )
		{
			continue;
		}

		if ( format != 1 )
		{
			continue;
		}

		for ( j = 0; j < sizeof (samp_freqs) / sizeof (samp_freqs[0]); j++ )
		{
			format = samp_freqs[j];
			res = ioctl (data->pcm_fd, SNDCTL_DSP_SPEED, &format);
			if ( res < 0 )
			{
				continue;
			}

			if ( format == samp_freqs[j] )
			{
				data->glob_speed = format;
				break;
			}
		}
		if ( j == sizeof (samp_freqs) / sizeof (samp_freqs[0]) )
		{
			/* if none of the sampling frequencies can be set, try next */
			continue;
		}
		else
		{
			/* all parameters set, backend initialized */
			break;
		}
	}
	if ( i == sizeof (formats) / sizeof (formats[0]) )
	{
		close (data->pcm_fd);
#ifdef HAVE_MALLOC
		free (data);
#endif
		return -2;
	}

	*imyp_data = (imyp_backend_data_t *)data;

	return 0;
}

/**
 * Closes the OSS subsystem.
 * \param imyp_data pointer to the backend's private data.
 * \return 0 on success.
 */
int
imyp_oss_close (
#ifdef IMYP_ANSIC
	imyp_backend_data_t * const imyp_data)
#else
	imyp_data)
	imyp_backend_data_t * const imyp_data;
#endif
{
	struct imyp_oss_backend_data * data =
		(struct imyp_oss_backend_data *)imyp_data;
	int res = 0;

	if ( data != NULL )
	{
		if ( data->pcm_fd >= 0 )
		{
			res = close (data->pcm_fd);
		}
#ifdef HAVE_MALLOC
		free (data);
#endif
	}
	return res;
}

/**
 * Displays the version of the OSS backend.
 * \param imyp_data pointer to the backend's private data.
 */
void
imyp_oss_version (
#ifdef IMYP_ANSIC
	const imyp_backend_data_t * const imyp_data IMYP_ATTR ((unused)))
#else
	imyp_data)
	const imyp_backend_data_t * const imyp_data IMYP_ATTR ((unused));
#endif
{
#ifdef SOUND_VERSION
# if SOUND_VERSION < 361
	/* old style version */
	printf ( "OSS: %d.%d.%d\n", SOUND_VERSION/100, (SOUND_VERSION/10) % 10,
		SOUND_VERSION%10 );
# else
	/* new style version */
	printf ( "OSS: %d.%d.%d\n", (SOUND_VERSION >> 16) & 0x0FF,
		(SOUND_VERSION >> 8) & 0x0FF , SOUND_VERSION & 0x0FF );
# endif
#else
	printf ( "OSS: ?\n" );
#endif
}
