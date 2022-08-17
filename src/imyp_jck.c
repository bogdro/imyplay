/*
 * A program for playing iMelody ringtones (IMY files).
 *	-- JACK backend.
 *
 * Copyright (C) 2009-2016 Bogdan Drozdowski, bogdandr (at) op.pl
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
#include "imyp_jck.h"
#include "imyp_sig.h"
#include "imyputil.h"

#include <stdio.h>

#ifdef IMYP_HAVE_JACK
# if (defined HAVE_JACK_H)
#  include <jack.h>
# else
#  include <jack/jack.h>
# endif
#else
# error JACK requested, but components not found.
#endif

#ifdef HAVE_STDLIB_H
# include <stdlib.h>
#endif
#ifdef HAVE_MALLOC_H
# include <malloc.h>
#endif

#ifdef HAVE_MATH_H
# include <math.h>	/* sin() */
#endif

#ifndef M_PI
# define M_PI 3.14159265358979323846
#endif

struct imyp_jack_backend_data
{
	double tone_freq;
	int volume_level;
	int duration;
	unsigned long int last_index;
	volatile int inside_callback;
	jack_port_t * joutput;
	jack_client_t * jclient;
	const char **ports;
};

#ifndef HAVE_MALLOC
static struct imyp_jack_backend_data imyp_jack_backend_data_static;
#endif

#ifndef IMYP_ANSIC
static int imyp_jack_fill_buffer IMYP_PARAMS ((jack_nframes_t nframes, void * arg));
#endif

/**
 * The stream callback function.
 * \param nframes the number of frames to fill.
 * \param imyp_data pointer to the backend's private data.
 * \return 0 in case of success.
 */
static int imyp_jack_fill_buffer (
#ifdef IMYP_ANSIC
	jack_nframes_t nframes, void * arg)
#else
	nframes, arg)
	jack_nframes_t nframes;
	void * arg;
#endif
{
	jack_nframes_t i;
	jack_default_audio_sample_t samp;
	jack_default_audio_sample_t *output;
	jack_nframes_t sampfreq;
	double nperiods;
	unsigned long int nperiods_rounded;
	double periods_in_full;

	struct imyp_jack_backend_data * data =
		(struct imyp_jack_backend_data *)arg;

	if ( data == NULL )
	{
		return -100;
	}

	output = jack_port_get_buffer (data->joutput, nframes);

	if ( output == NULL )
	{
		return 0;
	}

	data->inside_callback = 1;
	if ( data->tone_freq > 0.0 )
	{
		sampfreq = jack_get_sample_rate (data->jclient);
		nperiods = sampfreq / data->tone_freq;
		/* round down, else we may think that a period fits while it doesn't: */
		/* nperiods_rounded = (unsigned long int)IMYP_ROUND(nperiods); */
		nperiods_rounded = (unsigned long int)nperiods;
		periods_in_full = 2 * M_PI / nperiods;
		for ( i = data->last_index; i < data->last_index + nframes; i++ )
		{
			if ( sig_recvd != 0 )
			{
				data->inside_callback = 0;
				return -2;
			}
			if ( nperiods_rounded == 0 )
			{
				/* not a single full period fits in the buffer */
#if (defined HAVE_SIN) || (defined HAVE_LIBM)
				samp = (jack_default_audio_sample_t)
					sin (i * periods_in_full);
#else
				samp = (jack_default_audio_sample_t) i / nperiods;
#endif
			}
			else
			{
#if (defined HAVE_SIN) || (defined HAVE_LIBM)
				samp = (jack_default_audio_sample_t)
					sin (i * periods_in_full);
#else
				samp = (jack_default_audio_sample_t)
					(i % nperiods_rounded) / nperiods;
#endif
			}
			output[i - data->last_index] = (jack_default_audio_sample_t)
				((samp * (jack_default_audio_sample_t)data->volume_level) / IMYP_MAX_IMY_VOLUME);
		}
		data->last_index += i - data->last_index;
		/* just slows things down
		while ( (data->last_index > (unsigned long int)NSAMP) && (sig_recvd == 0) )
		{
			data->last_index -= (unsigned long int)NSAMP;
		}*/
		/*data->last_index %= (unsigned long int)nperiods;*/
	}
	else
	{
		/*data->last_index = 0;*/
		for ( i = data->last_index; i < data->last_index + nframes; i++ )
		{
			if ( sig_recvd != 0 )
			{
				data->inside_callback = 0;
				return -2;
			}
			output[i - data->last_index] = 0;
		}
	}
	data->inside_callback = 0;
	return 0;
}

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
imyp_jack_play_tune (
#ifdef IMYP_ANSIC
	imyp_backend_data_t * const imyp_data,
	const double freq,
	const int volume_level,
	const int duration,
	void * const buf IMYP_ATTR((unused)),
	int bufsize IMYP_ATTR((unused)))
#else
	imyp_data, freq, volume_level, duration, buf, bufsize)
	imyp_backend_data_t * const imyp_data;
	const double freq;
	const int volume_level;
	const int duration;
	void * const buf IMYP_ATTR((unused));
	int bufsize IMYP_ATTR((unused));
#endif
{
	struct imyp_jack_backend_data * data =
		(struct imyp_jack_backend_data *)imyp_data;

	if ( data == NULL )
	{
		return -100;
	}

	data->tone_freq = freq;
	data->volume_level = volume_level;
	data->duration = duration;
	data->last_index = 0;

	imyp_jack_pause (imyp_data, duration);

	while ( data->inside_callback != 0 ) {}

	data->tone_freq = 0.0;
	data->volume_level = 0;
	data->duration = 0;
	data->last_index = 0;

	return 0;
}

/**
 * Pauses for the specified period of time.
 * \param imyp_data pointer to the backend's private data.
 * \param milliseconds Number of milliseconds to pause.
 */
void
imyp_jack_pause (
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
imyp_jack_put_text (
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
 * Initializes the JACK library for use.
 * \param imyp_data pointer to storage for the backend's private data.
 * \param dev_file The server to connect to.
 * \return 0 on success.
 */
int
imyp_jack_init (
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
	jack_status_t jstatus;
	struct imyp_jack_backend_data * data;

	if ( imyp_data == NULL )
	{
		return -100;
	}
#ifdef HAVE_MALLOC
	data = (struct imyp_jack_backend_data *) malloc (sizeof (
		struct imyp_jack_backend_data));
	if ( data == NULL )
	{
		return -6;
	}
#else
	data = &imyp_jack_backend_data_static;
#endif

	data->tone_freq = 0.0;
	data->volume_level = 0;
	data->duration = 0;
	data->last_index = 0;
	data->inside_callback = 0;

	data->jclient = jack_client_open ("IMYplay", JackNullOption, &jstatus, dev_file);
	if ( data->jclient == NULL )
	{
#ifdef HAVE_MALLOC
		free (data);
#endif
		return -1;
	}

	jack_set_process_callback (data->jclient, imyp_jack_fill_buffer, data);

	data->joutput = jack_port_register (data->jclient, "output", JACK_DEFAULT_AUDIO_TYPE,
		JackPortIsOutput, 0);

	if ( data->joutput == NULL )
	{
		jack_client_close (data->jclient);
#ifdef HAVE_MALLOC
		free (data);
#endif
		return -2;
	}

	res = jack_activate (data->jclient);
	if ( res != 0 )
	{
		jack_port_unregister (data->jclient, data->joutput);
		jack_client_close (data->jclient);
#ifdef HAVE_MALLOC
		free (data);
#endif
		return -3;
	}

	data->ports = jack_get_ports (data->jclient, NULL, NULL, JackPortIsPhysical|JackPortIsInput);
	if ( data->ports == NULL )
	{
		/* no ports available */
		jack_deactivate (data->jclient);
		jack_port_unregister (data->jclient, data->joutput);
		jack_client_close (data->jclient);
#ifdef HAVE_MALLOC
		free (data);
#endif
		return -4;
	}

	res = jack_connect (data->jclient, jack_port_name (data->joutput), data->ports[0]);
	if ( res != 0 )
	{
#ifdef HAVE_JACK_FREE
		jack_free (data->ports);
#else
		free (data->ports);
#endif
		jack_deactivate (data->jclient);
		jack_port_unregister (data->jclient, data->joutput);
		jack_client_close (data->jclient);
#ifdef HAVE_MALLOC
		free (data);
#endif
		return -5;
	}

	*imyp_data = (imyp_backend_data_t *)data;

	return 0;
}

/**
 * Closes the JACK library.
 * \param imyp_data pointer to the backend's private data.
 * \return 0 on success.
 */
int
imyp_jack_close (
#ifdef IMYP_ANSIC
	imyp_backend_data_t * const imyp_data)
#else
	imyp_data)
	imyp_backend_data_t * const imyp_data;
#endif
{
	int res = 0, tempres;
	struct imyp_jack_backend_data * data =
		(struct imyp_jack_backend_data *)imyp_data;

	if ( data != NULL )
	{
		jack_disconnect (data->jclient, jack_port_name (data->joutput), data->ports[0]);
		tempres = jack_deactivate (data->jclient);
		if ( res == 0 )
		{
			res = tempres;
		}
		tempres = jack_port_unregister (data->jclient, data->joutput);
		if ( res == 0 )
		{
			res = tempres;
		}
		tempres = jack_client_close (data->jclient);
		if ( res == 0 )
		{
			res = tempres;
		}
#ifdef HAVE_JACK_FREE
		jack_free (data->ports);
#else
		free (data->ports);
#endif
#ifdef HAVE_MALLOC
		free (data);
#endif
	}

	return res;
}

/**
 * Displays the version of the JACK library IMYplay was compiled with.
 * \param imyp_data pointer to the backend's private data.
 */
void
imyp_jack_version (
#ifdef IMYP_ANSIC
	imyp_backend_data_t * const imyp_data IMYP_ATTR ((unused)))
#else
	imyp_data)
	imyp_backend_data_t * const imyp_data IMYP_ATTR ((unused));
#endif
{
	/* no version information currently available */
	printf ( "JACK: ?\n" );
}

