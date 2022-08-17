/*
 * A program for playing iMelody ringtones (IMY files).
 *	-- LIBAO backend.
 *
 * Copyright (C) 2009-2013 Bogdan Drozdowski, bogdandr (at) op.pl
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

#ifdef IMYP_HAVE_LIBAO
# if (defined HAVE_AO_H)
#  include <ao.h>
# else
#  include <ao/ao.h>
# endif
#else
# error AO requested, but components not found.
#endif

#ifdef HAVE_STDLIB_H
# include <stdlib.h>
#endif

#ifdef HAVE_MALLOC_H
# include <malloc.h>
#endif

struct imyp_ao_backend_data
{
	ao_device *device;
	ao_sample_format format;
};

#ifndef HAVE_MALLOC
struct imyp_ao_backend_data imyp_ao_backend_data_static;
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
imyp_ao_play_tune (
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
	int res;
	struct imyp_ao_backend_data * data =
		(struct imyp_ao_backend_data *)imyp_data;

	if ( (data == NULL) || (buf == NULL) || (bufsize <= 0) )
	{
		return -1;
	}

	bufsize = imyp_generate_samples (freq, volume_level, duration, buf, bufsize,
		(data->format.byte_format == AO_FMT_LITTLE)? 1 : 0, 1,
		(unsigned int)data->format.bits, (unsigned int)data->format.rate, NULL);
	if ( sig_recvd != 0 )
	{
		return -2;
	}
	res = ao_play (data->device, buf, (uint_32) bufsize);
	if ( res > 0 )
	{
		return 0;
	}
	return -1;
}

/**
 * Pauses for the specified period of time.
 * \param imyp_data pointer to the backend's private data.
 * \param milliseconds Number of milliseconds to pause.
 */
void
imyp_ao_pause (
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
imyp_ao_put_text (
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
 * Initializes the AO library for use.
 * \param imyp_data pointer to storage for the backend's private data.
 * \param dev_file The device to open.
 * \return 0 on success.
 */
int
imyp_ao_init (
#ifdef IMYP_ANSIC
	imyp_backend_data_t ** const imyp_data,
	const char * const dev_file)
#else
	imyp_data, dev_file)
	imyp_backend_data_t ** const imyp_data;
	const char * const dev_file;
#endif
{
	int driver;
	int res;
	const int speeds[] = {44100, 22050, 11025};
	const int endians[] = {AO_FMT_LITTLE, AO_FMT_BIG};
	size_t i, j;
	struct imyp_ao_backend_data * data;

	if ( imyp_data == NULL )
	{
		return -6;
	}
#ifdef HAVE_MALLOC
	data = (struct imyp_ao_backend_data *) malloc (sizeof (
		struct imyp_ao_backend_data));
	if ( data == NULL )
	{
		return -7;
	}
#else
	data = &imyp_ao_backend_data_static;
#endif

	ao_initialize ();
	if ( dev_file != NULL )
	{
		res = sscanf (dev_file, "%d", &driver);
		if ( res == 1 )
		{
			driver = ao_default_driver_id ();
		}
	}
	else
	{
		driver = ao_default_driver_id ();
	}

	data->format.channels = 1;

	/* file:///usr/share/doc/libao-devel-X.Y.Z/ao_example.c */
	for ( i = 0; i < sizeof (speeds) / sizeof (speeds[0]); i++ )
	{
		data->format.rate = speeds[i];
		for ( j = 0; j < sizeof (endians) / sizeof (endians[0]); j++ )
		{
			data->format.byte_format = endians[j];
			data->format.bits = 16;
			data->device = ao_open_live (driver, &(data->format), NULL /* no options */);
			if ( data->device != NULL )
			{
				break;
			}
			data->format.bits = 8;
			data->device = ao_open_live (driver, &(data->format), NULL /* no options */);
			if ( data->device != NULL )
			{
				break;
			}
		}
		if ( j < sizeof (endians) / sizeof (endians[0]) )
		{
			break;
		}
	}
	if ( i == sizeof (speeds) / sizeof (speeds[0]) )
	{
#ifdef HAVE_MALLOC
		free (data);
#endif
		ao_shutdown ();
		return -1;
	}
	*imyp_data = (imyp_backend_data_t *)data;

	return 0;
}

/**
 * Closes the AO library.
 * \param imyp_data pointer to the backend's private data.
 * \return 0 on success.
 */
int
imyp_ao_close (
#ifdef IMYP_ANSIC
	imyp_backend_data_t * const imyp_data IMYP_ATTR ((unused)))
#else
	imyp_data)
	imyp_backend_data_t * const imyp_data IMYP_ATTR ((unused));
#endif
{
	struct imyp_ao_backend_data * data =
		(struct imyp_ao_backend_data *)imyp_data;

	if ( data != NULL )
	{
		if ( data->device != NULL )
		{
			ao_close (data->device);
		}
#ifdef HAVE_MALLOC
		free (data);
#endif
	}
	ao_shutdown ();
	return 0;
}

/**
 * Displays the version of the AO library IMYplay was compiled with.
 * \param imyp_data pointer to the backend's private data.
 */
void
imyp_ao_version (
#ifdef IMYP_ANSIC
	imyp_backend_data_t * const imyp_data IMYP_ATTR ((unused)))
#else
	imyp_data)
	imyp_backend_data_t * const imyp_data IMYP_ATTR ((unused));
#endif
{
	/* no version information currently available */
	printf ( "AO: ?\n" );
}

