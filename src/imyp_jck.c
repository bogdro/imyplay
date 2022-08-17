/*
 * A program for playing iMelody ringtones (IMY files).
 *	-- JACK backend.
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
#include "imyp_jck.h"
#include "imyp_sig.h"

#include <stdio.h>

#if (defined HAVE_LIBJACK) && ((defined HAVE_JACK_H) || (defined HAVE_JACK_JACK_H))
# if (defined HAVE_JACK_H)
#  include <jack.h>
# else
#  include <jack/jack.h>
# endif
#else
# error The JACK library was not found.
#endif

#ifndef HAVE_JACK_FREE
 /* use standard free() */
# ifdef HAVE_STDLIB_H
#  include <stdlib.h>
# endif
# ifdef HAVE_MALLOC_H
#  include <malloc.h>
# endif
#endif

#ifdef HAVE_MATH_H
# include <math.h>	/* sin() */
#endif

#ifndef M_PI
# define M_PI 3.14159265358979323846
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

struct imyp_jack_sound_data
{
	double tone_freq;
	int volume_level;
	int duration;
	unsigned long int last_index;
	volatile int inside_callback;
};

static struct imyp_jack_sound_data sound_data = {0.0, 0, 0, 0, 0};
static jack_port_t * joutput;
static jack_client_t * jclient;

#ifndef IMYP_ANSIC
static int imyp_jack_fill_buffer PARAMS((jack_nframes_t nframes, void * arg));
#endif

/**
 * The stream callback function.
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

	output = jack_port_get_buffer (joutput, nframes);

#define data ((struct imyp_jack_sound_data *) arg)

	if ( (data == NULL) || (output == NULL) ) return 0;

	sampfreq = jack_get_sample_rate (jclient);
	if ( data->tone_freq > 0.0 )
	{
		data->inside_callback = 1;
#define NSAMP ((sampfreq)/(data->tone_freq))
		for ( i=data->last_index; i < data->last_index+nframes; i++ )
		{
			if ( sig_recvd != 0 )
			{
				data->inside_callback = 0;
				return -2;
			}
#if (defined HAVE_SIN) || (defined HAVE_LIBM)
			samp = (jack_default_audio_sample_t)
				sin ((i%((jack_nframes_t)IMYP_ROUND(NSAMP)))*(2*M_PI/NSAMP));
#else
			samp = (jack_default_audio_sample_t)
				(i%((jack_nframes_t)IMYP_ROUND(NSAMP)))/NSAMP;
#endif
			output[i-data->last_index] = (jack_default_audio_sample_t)
				((samp * (jack_default_audio_sample_t)data->volume_level) / IMYP_MAX_IMY_VOLUME);
		}
		data->last_index += i - data->last_index;
		/* just slows things down
		while ( (data->last_index > (unsigned long int)NSAMP) && (sig_recvd == 0) )
		{
			data->last_index -= (unsigned long int)NSAMP;
		}*/
		data->inside_callback = 0;
	}
	else
	{
		data->inside_callback = 1;
		data->last_index = 0;
		for ( i=data->last_index; i < data->last_index+nframes; i++ )
		{
			if ( sig_recvd != 0 )
			{
				data->inside_callback = 0;
				return -2;
			}
			output[i-data->last_index] = 0;
		}
		data->inside_callback = 0;
	}
	return 0;
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
imyp_jack_play_tune (
#ifdef IMYP_ANSIC
	const double freq,
	const int volume_level,
	const int duration,
	void * const buf IMYP_ATTR((unused)),
	int bufsize IMYP_ATTR((unused)))
#else
	freq, volume_level, duration, buf, bufsize)
	const double freq;
	const int volume_level;
	const int duration;
	void * const buf IMYP_ATTR((unused));
	int bufsize IMYP_ATTR((unused));
#endif
{
	sound_data.tone_freq = freq;
	sound_data.volume_level = volume_level;
	sound_data.duration = duration;
	sound_data.last_index = 0;

	imyp_jack_pause (duration);

	while ( sound_data.inside_callback != 0 ) {};
	sound_data.tone_freq = 0.0;
	sound_data.volume_level = 0;
	sound_data.duration = 0;
	sound_data.last_index = 0;

	return 0;
}

/**
 * Pauses for the specified period of time.
 * \param milliseconds Number of milliseconds to pause.
 */
void
imyp_jack_pause (
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
imyp_jack_put_text (
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
 * Initializes the JACK library for use.
 * \param dev_file The server to connect to.
 * \return 0 on success.
 */
int
imyp_jack_init (
#ifdef IMYP_ANSIC
	const char * const dev_file)
#else
	dev_file)
	const char * const dev_file;
#endif
{
	int res;
	jack_status_t jstatus;
	const char **ports;

	sound_data.tone_freq = 0.0;
	sound_data.volume_level = 0;
	sound_data.duration = 0;
	sound_data.last_index = 0;
	sound_data.inside_callback = 0;

	jclient = jack_client_open ("IMYplay", JackNullOption, &jstatus, dev_file);
	if ( jclient == NULL )
	{
		return -1;
	}

	jack_set_process_callback (jclient, imyp_jack_fill_buffer, &sound_data);

	joutput = jack_port_register (jclient, "output", JACK_DEFAULT_AUDIO_TYPE,
		JackPortIsOutput, 0);

	if ( joutput == NULL )
	{
		jack_client_close (jclient);
		return -2;
	}

	res = jack_activate (jclient);
	if ( res != 0 )
	{
		jack_port_unregister (jclient, joutput);
		jack_client_close (jclient);
		return -3;
	}

	ports = jack_get_ports (jclient, NULL, NULL, JackPortIsPhysical|JackPortIsInput);
	if ( ports == NULL )
	{
		/* no ports available */
		jack_deactivate (jclient);
		jack_port_unregister (jclient, joutput);
		jack_client_close (jclient);
		return -4;
	}

	res = jack_connect (jclient, jack_port_name (joutput), ports[0]);
	if ( res != 0 )
	{
#ifdef HAVE_JACK_FREE
		jack_free (ports);
#else
		free (ports);
#endif
		jack_deactivate (jclient);
		jack_port_unregister (jclient, joutput);
		jack_client_close (jclient);
		return -5;
	}

#ifdef HAVE_JACK_FREE
	jack_free (ports);
#else
	free (ports);
#endif

	return 0;
}

/**
 * Closes the JACK library.
 * \return 0 on success.
 */
int
imyp_jack_close (
#ifdef IMYP_ANSIC
	void
#endif
)
{
	int res = 0, tempres;

	tempres = jack_deactivate (jclient);
	if ( res == 0 ) res = tempres;
	tempres = jack_port_unregister (jclient, joutput);
	if ( res == 0 ) res = tempres;
	tempres = jack_client_close (jclient);
	if ( res == 0 ) res = tempres;

	return res;
}
