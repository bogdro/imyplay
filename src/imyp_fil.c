/*
 * A program for playing iMelody ringtones (IMY files).
 *	-- FILE backend.
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
#include "imyp_fil.h"
#include "imyp_sig.h"
#include "imyputil.h"

#ifndef IMYP_HAVE_FILE
# error FILE output requested, but components not found.
#endif

#include <stdio.h>

#ifdef HAVE_STRING_H
# if ((!defined STDC_HEADERS) || (!STDC_HEADERS)) && (defined HAVE_MEMORY_H)
#  include <memory.h>
# endif
# include <string.h>
#endif

#ifdef HAVE_STDLIB_H
# include <stdlib.h>
#endif

#ifdef HAVE_MALLOC_H
# include <malloc.h>
#endif

struct imyp_file_backend_data
{
	FILE * raw_file;
	enum IMYP_SAMPLE_FORMATS format;
	int samp_rate;
};

#ifdef TEST_COMPILE
# undef IMYP_ANSIC
# if TEST_COMPILE > 1
#  undef HAVE_MALLOC
# endif
#endif

#ifndef HAVE_MALLOC
static struct imyp_file_backend_data imyp_file_backend_data_static;
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
imyp_file_play_tune (
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
	size_t fwrite_res;
	int is_uns = 0;
	int is_le = 1;
	struct imyp_file_backend_data * data =
		(struct imyp_file_backend_data *)imyp_data;

	if ( (data == NULL) || (buf == NULL) || (bufsize <= 0) )
	{
		return -1;
	}

	if (       (data->format == IMYP_SAMPLE_FORMAT_S8LE)
		|| (data->format == IMYP_SAMPLE_FORMAT_S8BE)
		|| (data->format == IMYP_SAMPLE_FORMAT_U8LE)
		|| (data->format == IMYP_SAMPLE_FORMAT_U8BE) )
	{
		quality = 8;
	}

	if (       (data->format == IMYP_SAMPLE_FORMAT_U16LE)
		|| (data->format == IMYP_SAMPLE_FORMAT_U16BE)
		|| (data->format == IMYP_SAMPLE_FORMAT_U8LE)
		|| (data->format == IMYP_SAMPLE_FORMAT_U8BE) )
	{
		is_uns = 1;
	}

	if (       (data->format == IMYP_SAMPLE_FORMAT_U16BE)
		|| (data->format == IMYP_SAMPLE_FORMAT_S16BE)
		|| (data->format == IMYP_SAMPLE_FORMAT_U8BE)
		|| (data->format == IMYP_SAMPLE_FORMAT_S8BE) )
	{
		is_le = 0;
	}

	bufsize = imyp_generate_samples (freq, volume_level, duration, buf, bufsize,
		is_le, is_uns, quality, (unsigned int)data->samp_rate, NULL);
	if ( imyp_sig_recvd != 0 )
	{
		return -2;
	}
	/* write raw samples to the file */
	fwrite_res = fwrite (buf, 1, (size_t)bufsize, data->raw_file);
	if ( fwrite_res == (size_t)bufsize )
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
imyp_file_pause (
#ifdef IMYP_ANSIC
	imyp_backend_data_t * const imyp_data,
	const int milliseconds, void * const buf, int bufsize)
#else
	imyp_data, milliseconds, buf, bufsize)
	imyp_backend_data_t * const imyp_data;
	const int milliseconds;
	void * const buf;
	int bufsize;
#endif
{
#ifndef HAVE_MEMSET
	int i;
#endif
	int quality = 16;
	struct imyp_file_backend_data * data =
		(struct imyp_file_backend_data *)imyp_data;

	if ( (data == NULL) || (buf == NULL) || (bufsize <= 0) || (milliseconds <= 0) )
	{
		return;
	}

	if (       (data->format == IMYP_SAMPLE_FORMAT_S8LE)
		|| (data->format == IMYP_SAMPLE_FORMAT_S8BE)
		|| (data->format == IMYP_SAMPLE_FORMAT_U8LE)
		|| (data->format == IMYP_SAMPLE_FORMAT_U8BE) )
	{
		quality = 8;
	}

	bufsize = IMYP_MIN (bufsize, (milliseconds * (int)data->samp_rate * ((int)quality/8)) / 1000);
#ifdef HAVE_MEMSET
	memset (buf, 0, (size_t)bufsize);
#else
	for (i = 0; i < bufsize; i++ )
	{
		((char *)buf)[i] = '\0';
	}
#endif
	/* write silence to the file */
	fwrite (buf, 1, (size_t)bufsize, data->raw_file);
}

/**
 * Outputs the given text.
 * \param imyp_data pointer to the backend's private data.
 * \param text The text to output.
 */
void
imyp_file_put_text (
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
 * Initializes the FILE backend for use.
 * \param imyp_data pointer to storage for the backend's private data.
 * \param dev The format of the samples (passed in the
 *	--device option), e.g "44100:s16le".
 * \param file_out The output filename.
 * \return 0 on success.
 */
int
imyp_file_init (
#ifdef IMYP_ANSIC
	imyp_backend_data_t ** const imyp_data,
	const char * const dev, const char * const file_out)
#else
	imyp_data, dev, file_out)
	imyp_backend_data_t ** const imyp_data;
	const char * const dev;
	const char * const file_out;
#endif
{
	char * colon;
	int scanf_res;
	char * filename;
	struct imyp_file_backend_data * data;

	if ( (imyp_data == NULL) || (file_out == NULL) )
	{
		return -1;
	}
#ifdef HAVE_MALLOC
	data = (struct imyp_file_backend_data *) malloc (sizeof (
		struct imyp_file_backend_data));
	if ( data == NULL )
	{
		return -2;
	}
#else
	data = &imyp_file_backend_data_static;
#endif
	filename = imyp_generate_filename (file_out, ".raw");

	if ( filename == NULL )
	{
#ifdef HAVE_MALLOC
		free (data);
#endif
		return -3;
	}

	data->raw_file = fopen (filename, "wb");
	free (filename);
	if ( data->raw_file == NULL )
	{
#ifdef HAVE_MALLOC
		free (data);
#endif
		return -4;
	}

	data->samp_rate = 44100;
	data->format = IMYP_SAMPLE_FORMAT_S16LE;
	if ( dev != NULL )
	{
		colon = strrchr (dev, (int)':');
		if ( colon != NULL )
		{
			data->format = imyp_get_format (colon+1);
			if ( data->format == IMYP_SAMPLE_FORMAT_UNKNOWN )
			{
				data->format = IMYP_SAMPLE_FORMAT_S16LE;
			}
			/* wipe the colon to read the sampling rate */
			*colon = '\0';
		}
		/* get the sampling rate: */
		scanf_res = sscanf (dev, "%d", &(data->samp_rate));
		if ( scanf_res == 1 )
		{
			if ( data->samp_rate <= 0 )
			{
				data->samp_rate = 44100;
			}
		}
	}
	*imyp_data = (imyp_backend_data_t *)data;
	return 0;
}

/**
 * Closes the FILE backend.
 * \param imyp_data pointer to the backend's private data.
 * \return 0 on success.
 */
int
imyp_file_close (
#ifdef IMYP_ANSIC
	imyp_backend_data_t * const imyp_data)
#else
	imyp_data)
	imyp_backend_data_t * const imyp_data;
#endif
{
	int res = 0;
	struct imyp_file_backend_data * data =
		(struct imyp_file_backend_data *)imyp_data;

	if ( data != NULL )
	{
		if ( data->raw_file == NULL )
		{
			res = -1;
		}
		else
		{
			fclose (data->raw_file);
		}
#ifdef HAVE_MALLOC
		free (data);
#endif
	}
	return res;
}

/**
 * Displays the version of the FILE backend.
 * \param imyp_data pointer to the backend's private data.
 */
void
imyp_file_version (
#ifdef IMYP_ANSIC
	imyp_backend_data_t * const imyp_data IMYP_ATTR ((unused)))
#else
	imyp_data)
	imyp_backend_data_t * const imyp_data IMYP_ATTR ((unused));
#endif
{
	/* this is an internal backend */
	printf ( "FILE\n" );
}
