/*
 * A program for playing iMelody ringtones (IMY files).
 *	-- PulseAudio backend.
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
#include "imyp_pul.h"
#include "imyp_sig.h"
#include "imyputil.h"

#include <stdio.h>

#if (defined HAVE_LIBPULSE) && (defined HAVE_LIBPULSE_SIMPLE) && \
	((defined HAVE_SIMPLE_H) || (defined HAVE_PULSE_SIMPLE_H))
# if (defined HAVE_SIMPLE_H)
#  include <simple.h>
#  include <sample.h>
#  include <util.h>
#  include <version.h>
# else
#  include <pulse/simple.h>
#  include <pulse/sample.h>
#  include <pulse/util.h>
#  include <pulse/version.h>
# endif
#else
# error PulseAudio requested, but components not found.
#endif

static pa_simple * stream;
static pa_sample_spec conf;

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
imyp_pulse_play_tune (
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
	int res;
	int is_le = 1;

	if ( (buf == NULL) || (bufsize <= 0) ) return -1;

	if ( conf.format == PA_SAMPLE_U8 )
	{
		quality = 8;
	}
	else if ( (conf.format == PA_SAMPLE_S16LE) || (conf.format == PA_SAMPLE_S16BE) )
	{
		if ( conf.format == PA_SAMPLE_S16BE )
		{
			is_le = 0;
		}
		else
		{
			is_le = 1;
		}
		quality = 16;
	}

	bufsize = imyp_generate_samples (freq, volume_level, duration, buf, bufsize,
		is_le, 1, quality, conf.rate);
	if ( sig_recvd != 0 )
	{
		return -2;
	}
	res = pa_simple_write (stream, buf, (size_t)bufsize, NULL);
	if ( res >= 0 )
	{
		pa_simple_drain (stream, NULL);	/* wait for data to be played */
		return 0;
	}
	return res;
}

/**
 * Pauses for the specified period of time.
 * \param milliseconds Number of milliseconds to pause.
 */
void
imyp_pulse_pause (
#ifdef IMYP_ANSIC
	const int milliseconds)
#else
	milliseconds)
	const int milliseconds;
#endif
{
	if ( milliseconds <= 0 ) return;
	pa_msleep ((unsigned long int)milliseconds);
}

/**
 * Outputs the given text.
 * \param text The text to output.
 */
void
imyp_pulse_put_text (
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
 * Initializes the PulseAudio library for use.
 * \param dev_file The device to open.
 * \return 0 on success.
 */
int
imyp_pulse_init (
#ifdef IMYP_ANSIC
	const char * const dev_file)
#else
	dev_file)
	const char * const dev_file;
#endif
{
	unsigned int i, j;
	const int formats[] = {PA_SAMPLE_S16LE, PA_SAMPLE_S16BE, PA_SAMPLE_U8};
	const uint32_t speeds[] = {44100, 22050, 11025};

	conf.channels = 1;
	for ( i = 0; i < sizeof (formats) / sizeof (formats[0]); i++ )
	{
		conf.format = formats[i];
		for ( j = 0; j < sizeof (speeds) / sizeof (speeds[0]); j++ )
		{
			conf.rate = speeds[j];
			stream = pa_simple_new (dev_file, "IMYplay", PA_STREAM_PLAYBACK,
				NULL, "iMelody", &conf, NULL, NULL, NULL);
			if ( stream != NULL )
			{
				break;
			}
		}
		if ( j < sizeof (speeds) / sizeof (speeds[0]) ) break;
	}
	if ( i == sizeof (formats) / sizeof (formats[0]) ) return -2;
	return 0;
}

/**
 * Closes the PulseAudio library.
 * \return 0 on success.
 */
int
imyp_pulse_close (
#ifdef IMYP_ANSIC
	void
#endif
)
{
	if ( stream != NULL ) pa_simple_free (stream);
	return 0;
}

/**
 * Displays the version of the PulseAudio backend.
 */
void
imyp_pulse_version (
#ifdef IMYP_ANSIC
	void
#endif
)
{
#if (defined PA_MAJOR) && (defined PA_MINOR) && (defined PA_MICRO)
	printf ( "PulseAudio: %d.%d.%d\n", PA_MAJOR, PA_MINOR, PA_MICRO );
#else
# if (defined PA_MAJOR) && (defined PA_MINOR)
	printf ( "PulseAudio: %d.%d\n", PA_MAJOR, PA_MINOR );
# else
#  if (defined PA_MAJOR)
	printf ( "PulseAudio: %d\n", PA_MAJOR );
#  else
	printf ( "PulseAudio: ?\n" );
#  endif
# endif
#endif
}

