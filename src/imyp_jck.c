/*
 * IMYplay - A program for playing iMelody ringtones (IMY files).
 *	-- JACK backend.
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
#include "imyp_jck.h"
#include "imyp_sig.h"
#include "imyputil.h"

#include <stdio.h>

#ifdef IMYP_HAVE_JACK
# if (defined HAVE_JACK_JACK_H)
#  include <jack/jack.h>
# else
#  include <jack.h>
# endif
#else
# error JACK requested, but components not found.
#endif

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

#ifdef HAVE_MATH_H
# include <math.h>	/* sin() */
#endif

#ifndef M_PI
# define M_PI 3.14159265358979323846
#endif

struct imyp_jack_backend_data
{
	jack_nframes_t last_index;
	jack_port_t * joutput1;
	jack_port_t * joutput2;
	jack_client_t * jclient;
	const char **ports;
	volatile jack_nframes_t samples_remain;
	void * buf;
};

#ifdef TEST_COMPILE
# undef IMYP_ANSIC
# if TEST_COMPILE > 1
#  undef HAVE_MALLOC
# endif
#endif

#ifndef HAVE_MALLOC
static struct imyp_jack_backend_data imyp_jack_backend_data_static;
#endif

#ifndef IMYP_ANSIC
static int imyp_jack_fill_single_buffer
	IMYP_PARAMS ((struct imyp_jack_backend_data * const data,
		jack_default_audio_sample_t * const output,
		const jack_nframes_t nframes,
		jack_nframes_t * const frames_to_play));
#endif

/**
 * The fill buffer function.
 * \param data pointer to the backend's private data.
 * \param nframes the number of frames to fill.
 * \return 0 in case of success.
 */
static int imyp_jack_fill_single_buffer (
#ifdef IMYP_ANSIC
	struct imyp_jack_backend_data * const data,
	jack_default_audio_sample_t * const output,
	const jack_nframes_t nframes,
	jack_nframes_t * const frames_to_play)
#else
	data, output, nframes, frames_to_play)
	struct imyp_jack_backend_data * const data;
	jack_default_audio_sample_t * const output;
	const jack_nframes_t nframes;
	jack_nframes_t * const frames_to_play;
#endif
{
	jack_nframes_t i;

	if ( (data == NULL) || (output == NULL) || (frames_to_play == NULL) )
	{
		return -1;
	}
	if ( data->buf == NULL )
	{
		return -1;
	}
	/* check for the marker, or if we simply played the whole buffer already: */
	if ( data->samples_remain <= 1 )
	{
		IMYP_MEMSET (output, 0, nframes * sizeof (jack_default_audio_sample_t));
		data->samples_remain = 0;
		return 0;
	}
	*frames_to_play = IMYP_MIN(nframes, data->samples_remain);
	for ( i = 0; i < *frames_to_play; i++ )
	{
		/* data is little-endian - low byte is first,
		* data is signed - subtract the high value,
		* data is 16-bit - divide by the maximum value to get a signal between -1.0 and +1.0
		*/
		output[i] = (float)(((((char *)data->buf)[(data->last_index + i) * 2] & 0x0FF)
			| ((((char *)data->buf)[(data->last_index + i) * 2 + 1] & 0x0FF) << 8)) - (1 << 15)) * 1.0f / (1 << 15);
	}
	/* fill the remaining part of the buffer, if any: */
	IMYP_MEMSET (&output[*frames_to_play], 0, (nframes - *frames_to_play) * sizeof (jack_default_audio_sample_t));
	return 0; /* no error */
}

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
	jack_nframes_t frames_to_play = 0;
	int ret1;
	int ret2;

	struct imyp_jack_backend_data * data =
		(struct imyp_jack_backend_data *)arg;

	if ( data == NULL )
	{
		return -100;
	}
	if ( data->buf == NULL )
	{
		return -1;
	}

	ret1 = imyp_jack_fill_single_buffer (data,
		jack_port_get_buffer (data->joutput1, nframes),
		nframes, &frames_to_play);
	ret2 = imyp_jack_fill_single_buffer (data,
		jack_port_get_buffer (data->joutput2, nframes),
		nframes, &frames_to_play);
	data->last_index += frames_to_play;
	data->samples_remain -= frames_to_play;
	if ( data->samples_remain <= 0 )
	{
		/* just a marker, because we need another call to the callback, otherwise the note is lost: */
		data->samples_remain = 1;
	}
	return (ret1 != 0)? ret1 : ret2;
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
	struct imyp_jack_backend_data * data =
		(struct imyp_jack_backend_data *)imyp_data;

	if ( (data == NULL) || (buf == NULL) )
	{
		return -100;
	}

	if ( (duration > 0) && (bufsize > 0) )
	{
		/* zero-out the old values first */
		IMYP_MEMSET (buf, 0, (size_t)bufsize);
		/* now generate the new samples: */
		bufsize = imyp_generate_samples (freq, volume_level, duration,
			buf, bufsize,
			1 /* little endian */, 1 /* unsigned */, 16 /* quality */,
			jack_get_sample_rate (data->jclient) /*44100*/, NULL);
		data->buf = buf;
		data->last_index = 0;

		/* set the number of remaining samples to the initial duration of the tone, in one operation: */
		data->samples_remain = (jack_nframes_t)bufsize; /*IMYP_MIN ((jack_nframes_t)bufsize, (jack_nframes_t)duration * jack_get_sample_rate (data->jclient) / 1000); */ /* in samples */

		/* imyp_jack_pause (imyp_data, duration); */

		while ( (data->samples_remain > 0) && (imyp_sig_recvd == 0) ) {}
	}
	else
	{
		return -2;
	}

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
	data->last_index = 0;
	data->samples_remain = 0;
	data->buf = NULL;

	/* sample server start: "jackd -d alsa -r 44100 -p 8192 &" */
	data->jclient = jack_client_open ("IMYplay", JackNullOption, &jstatus, dev_file);
	if ( data->jclient == NULL )
	{
#ifdef HAVE_MALLOC
		free (data);
#endif
		return -1;
	}
	for ( data->last_index = (1 << 16); data->last_index >= (1 << 10); data->last_index /= 2 )
	{
		if ( jack_set_buffer_size (data->jclient, data->last_index) == 0 )
		{
			break;
		}
	}
	data->last_index = 0;

	jack_set_process_callback (data->jclient, &imyp_jack_fill_buffer, data);

	data->joutput1 = jack_port_register (data->jclient, "output1",
		JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);

	if ( data->joutput1 == NULL )
	{
		jack_client_close (data->jclient);
#ifdef HAVE_MALLOC
		free (data);
#endif
		return -2;
	}

	data->joutput2 = jack_port_register (data->jclient, "output2",
		JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);

	if ( data->joutput2 == NULL )
	{
		jack_port_unregister (data->jclient, data->joutput1);
		jack_client_close (data->jclient);
#ifdef HAVE_MALLOC
		free (data);
#endif
		return -2;
	}

	res = jack_activate (data->jclient);
	if ( res != 0 )
	{
		jack_port_unregister (data->jclient, data->joutput2);
		jack_port_unregister (data->jclient, data->joutput1);
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
		jack_port_unregister (data->jclient, data->joutput2);
		jack_port_unregister (data->jclient, data->joutput1);
		jack_client_close (data->jclient);
#ifdef HAVE_MALLOC
		free (data);
#endif
		return -4;
	}

	res = jack_connect (data->jclient, jack_port_name (data->joutput1), data->ports[0]);
	if ( res != 0 )
	{
#ifdef HAVE_JACK_FREE
		jack_free (data->ports);
#else
		free (data->ports);
#endif
		jack_deactivate (data->jclient);
		jack_port_unregister (data->jclient, data->joutput2);
		jack_port_unregister (data->jclient, data->joutput1);
		jack_client_close (data->jclient);
#ifdef HAVE_MALLOC
		free (data);
#endif
		return -5;
	}

	res = jack_connect (data->jclient, jack_port_name (data->joutput2), data->ports[1]);
	if ( res != 0 )
	{
#ifdef HAVE_JACK_FREE
		jack_free (data->ports);
#else
		free (data->ports);
#endif
		jack_deactivate (data->jclient);
		jack_port_unregister (data->jclient, data->joutput2);
		jack_port_unregister (data->jclient, data->joutput1);
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
		jack_disconnect (data->jclient, jack_port_name (data->joutput1), data->ports[0]);
		jack_disconnect (data->jclient, jack_port_name (data->joutput2), data->ports[1]);
		tempres = jack_deactivate (data->jclient);
		if ( res == 0 )
		{
			res = tempres;
		}
		tempres = jack_port_unregister (data->jclient, data->joutput2);
		if ( res == 0 )
		{
			res = tempres;
		}
		tempres = jack_port_unregister (data->jclient, data->joutput1);
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
#ifdef HAVE_JACK_GET_VERSION_STRING
	printf ( "JACK: %s\n", jack_get_version_string () );
#else
	/* no version information currently available */
	printf ( "JACK: ?\n" );
#endif
}
