/*
 * A program for playing iMelody ringtones (IMY files).
 *	-- FILE backend.
 *
 * Copyright (C) 2009-2011 Bogdan Drozdowski, bogdandr (at) op.pl
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
#include "imyp_fil.h"
#include "imyp_sig.h"
#include "imyputil.h"

#include <stdio.h>

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

static FILE * raw_file = NULL;
static enum IMYP_SAMPLE_FORMATS format = IMYP_SAMPLE_FORMAT_S16LE;
static int samp_rate = 44100;

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
imyp_file_play_tune (
#ifdef IMYP_ANSIC
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
	unsigned int quality = 16;
	size_t fwrite_res;
	int is_uns = 0;
	int is_le = 1;

	if ( (buf == NULL) || (bufsize <= 0) ) return -1;

	if ( (format == IMYP_SAMPLE_FORMAT_S8LE) || (format == IMYP_SAMPLE_FORMAT_S8BE) ||
		 (format == IMYP_SAMPLE_FORMAT_U8LE) || (format == IMYP_SAMPLE_FORMAT_U8BE) )
	{
		quality = 8;
	}

	if ( (format == IMYP_SAMPLE_FORMAT_U16LE) || (format == IMYP_SAMPLE_FORMAT_U16BE) ||
		 (format == IMYP_SAMPLE_FORMAT_U8LE) || (format == IMYP_SAMPLE_FORMAT_U8BE) )
	{
		is_uns = 1;
	}

	if ( (format == IMYP_SAMPLE_FORMAT_U16BE) || (format == IMYP_SAMPLE_FORMAT_S16BE) ||
		 (format == IMYP_SAMPLE_FORMAT_U8BE) || (format == IMYP_SAMPLE_FORMAT_S8BE) )
	{
		is_le = 0;
	}

	bufsize = imyp_generate_samples (freq, volume_level, duration, buf, bufsize,
		is_le, is_uns, quality, (unsigned int)samp_rate);
	if ( sig_recvd != 0 )
	{
		return -2;
	}
	/* write raw samples to the file */
	fwrite_res = fwrite (buf, 1, (size_t)bufsize, raw_file);
	if ( fwrite_res == (size_t)bufsize )
	{
		return 0;
	}
	return -3;
}

/**
 * Pauses for the specified period of time.
 * \param milliseconds Number of milliseconds to pause.
 */
void
imyp_file_pause (
#ifdef IMYP_ANSIC
	const int milliseconds, void * const buf, int bufsize)
#else
	milliseconds, buf, bufsize)
	const int milliseconds;
	void * const buf;
	int bufsize;
#endif
{
#ifndef HAVE_MEMSET
	int i;
#endif
	int quality = 16;

	if ( (buf == NULL) || (bufsize <= 0) || (milliseconds <= 0) ) return;

	if ( (format == IMYP_SAMPLE_FORMAT_S8LE) || (format == IMYP_SAMPLE_FORMAT_S8BE) ||
		 (format == IMYP_SAMPLE_FORMAT_U8LE) || (format == IMYP_SAMPLE_FORMAT_U8BE) )
	{
		quality = 8;
	}

	bufsize = IMYP_MIN (bufsize, (milliseconds * (int)samp_rate * ((int)quality/8)) / 1000);
#ifdef HAVE_MEMSET
	memset (buf, 0, (size_t)bufsize);
#else
	for (i = 0; i < bufsize; i++ )
	{
		((char *)buf)[i] = '\0';
	}
#endif
	/* write silence to the file */
	fwrite (buf, 1, (size_t)bufsize, raw_file);
}

/**
 * Outputs the given text.
 * \param text The text to output.
 */
void
imyp_file_put_text (
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
 * Initializes the FILE backend for use.
 * \param dev The format of the samples (passed in the
 *	--device option), e.g "44100:s16le".
 * \param file_out The output filename.
 * \return 0 on success.
 */
int
imyp_file_init (
#ifdef IMYP_ANSIC
	const char * const dev, const char * const file_out)
#else
	program, file_out)
	const char * const dev;
	const char * const file_out;
#endif
{
	char * colon;
	int scanf_res;

	if ( file_out == NULL )
	{
		return -1;
	}
	raw_file = fopen (file_out, "wb");
	if ( raw_file == NULL ) return -2;

	samp_rate = 44100;
	format = IMYP_SAMPLE_FORMAT_S16LE;
	if ( dev != NULL )
	{
		colon = strrchr (dev, (int)':');
		if ( colon != NULL )
		{
			format = imyp_get_format (colon+1);
			if ( format == IMYP_SAMPLE_FORMAT_UNKNOWN )
			{
				format = IMYP_SAMPLE_FORMAT_S16LE;
			}
			/* wipe the colon to read the sampling rate */
			*colon = '\0';
		}
		/* get the sampling rate: */
		scanf_res = sscanf (dev, "%d", &samp_rate);
		if ( scanf_res == 1 )
		{
			if ( samp_rate <= 0 )
			{
				samp_rate = 44100;
			}
		}
	}
	return 0;
}

/**
 * Closes the FILE backend.
 * \return 0 on success.
 */
int
imyp_file_close (
#ifdef IMYP_ANSIC
	void
#endif
)
{
	if ( raw_file == NULL )
	{
		return -1;
	}
	fclose (raw_file);
	return 0;
}

/**
 * Displays the version of the FILE backend.
 */
void
imyp_file_version (
#ifdef IMYP_ANSIC
	void
#endif
)
{
	/* this is an internal backend */
	printf ( "FILE\n" );
}

