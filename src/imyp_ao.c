/*
 * A program for playing iMelody ringtones (IMY files).
 *	-- LIBAO backend.
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
#include "imyp_ao.h"
#include "imyp_sig.h"
#include "imyputil.h"

#include <stdio.h>

#if (defined HAVE_LIBAO) && ((defined HAVE_AO_H) || (defined HAVE_AO_AO_H))
# if (defined HAVE_AO_H)
#  include <ao.h>
# else
#  include <ao/ao.h>
# endif
#else
# error AO requested, but components not found.
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

static ao_device *device;
static ao_sample_format format;

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
imyp_ao_play_tune (
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
	int res;

	if ( (buf == NULL) || (bufsize <= 0) )
	{
		return -1;
	}

	bufsize = imyp_generate_samples (freq, volume_level, duration, buf, bufsize,
		(format.byte_format == AO_FMT_LITTLE)? 1 : 0, 1,
		(unsigned int)format.bits, (unsigned int)format.rate);
	if ( sig_recvd != 0 )
	{
		return -2;
	}
	res = ao_play (device, buf, (uint_32) bufsize);
	if ( res > 0 )
	{
		return 0;
	}
	return -1;
}

/**
 * Pauses for the specified period of time.
 * \param milliseconds Number of milliseconds to pause.
 */
void
imyp_ao_pause (
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
#endif
}

/**
 * Outputs the given text.
 * \param text The text to output.
 */
void
imyp_ao_put_text (
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
 * Initializes the AO library for use.
 * \param dev_file The device to open.
 * \return 0 on success.
 */
int
imyp_ao_init (
#ifdef IMYP_ANSIC
	const char * const dev_file)
#else
	dev_file)
	const char * const dev_file;
#endif
{
	int driver;
	int res;
	const int speeds[] = {44100, 22050, 11025};
	const int endians[] = {AO_FMT_LITTLE, AO_FMT_BIG};
	size_t i, j;

	ao_initialize ();
	if ( dev_file != NULL )
	{
		res = sscanf (dev_file, "%d", &driver);
		if ( res == 1 ) driver = ao_default_driver_id ();
	}
	else driver = ao_default_driver_id ();

	format.channels = 1;

	/* file:///usr/share/doc/libao-devel-X.Y.Z/ao_example.c */
	for ( i = 0; i < sizeof (speeds) / sizeof (speeds[0]); i++ )
	{
		format.rate = speeds[i];
		for ( j = 0; j < sizeof (endians) / sizeof (endians[0]); j++ )
		{
			format.byte_format = endians[j];
			format.bits = 16;
			device = ao_open_live (driver, &format, NULL /* no options */);
			if (device != NULL)
			{
				break;
			}
			format.bits = 8;
			device = ao_open_live (driver, &format, NULL /* no options */);
			if (device != NULL)
			{
				break;
			}
		}
		if ( j < sizeof (endians) / sizeof (endians[0]) ) break;
	}
	if ( i == sizeof (speeds) / sizeof (speeds[0]) ) return -1;

	return 0;
}

/**
 * Closes the AO library.
 * \return 0 on success.
 */
int
imyp_ao_close (
#ifdef IMYP_ANSIC
	void
#endif
)
{
	if ( device != NULL ) ao_close (device);
	ao_shutdown ();
	return 0;
}

/**
 * Displays the version of the AO library IMYplay was compiled with.
 */
void
imyp_ao_version (
#ifdef IMYP_ANSIC
	void
#endif
)
{
	/* no version information currently available */
	printf ( "AO: ?\n" );
}

