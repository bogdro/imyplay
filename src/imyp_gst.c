/*
 * IMYplay - A program for playing iMelody ringtones (IMY files).
 *	-- GStreamer backend.
 *
 * Copyright (C) 2012-2023 Bogdan Drozdowski, bogdro (at) users.sourceforge.net
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
#include "imyp_gst.h"
#include "imyp_sig.h"
#include "imyputil.h"

#include <stdio.h>

#ifdef IMYP_HAVE_GST
# include <gst/gst.h>
# include <gst/controller/gstinterpolationcontrolsource.h>
# include <gst/gstversion.h>
#else
# error GStreamer requested, but components not found.
#endif

#if GST_VERSION_MAJOR >= 1
# define IMYP_HAVE_GST1
#endif

#ifdef IMYP_HAVE_GST1
# include <gst/controller/controller.h>
#else
# include <gst/controller/gstcontroller.h>
#endif

#ifdef HAVE_LIBINTL_H
# include <libintl.h>	/* translation stuff */
#endif

#ifdef HAVE_STDLIB_H
# include <stdlib.h>
#endif

#ifdef HAVE_MALLOC_H
# include <malloc.h>
#endif

#ifdef HAVE_STRING_H
# if ((!defined STDC_HEADERS) || (!STDC_HEADERS)) && (defined HAVE_MEMORY_H)
#  include <memory.h>
# endif
# include <string.h>
#endif

struct imyp_gst_backend_data
{
	GstElement *src;
	GstElement *sink;
	GstElement *binpipe;
#ifndef IMYP_HAVE_GST1
	GstController *ctl;
#endif
	GstInterpolationControlSource *volctl;
	GstInterpolationControlSource *freqctl;
	GstClock *gclock;
	GstClockID gclock_ID;
	GstClockReturn gclock_ret;
	GValue g_vol;
	GValue g_freq;
};

#ifdef TEST_COMPILE
# undef IMYP_ANSIC
# if TEST_COMPILE > 1
#  undef HAVE_MALLOC
# endif
#endif

#ifndef HAVE_MALLOC
static struct imyp_gst_backend_data imyp_gst_backend_data_static;
#endif

#define prop_name_volume "volume"
#define prop_name_freq "freq"
#define prop_name_mode "mode"

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
imyp_gst_play_tune (
#ifdef IMYP_ANSIC
	imyp_backend_data_t * const imyp_data,
	const double freq,
	const int volume_level,
	const int duration,
	void * const buf IMYP_ATTR ((unused)),
	int bufsize IMYP_ATTR ((unused)))
#else
	imyp_data, freq, volume_level, duration, buf, bufsize)
	imyp_backend_data_t * const imyp_data;
	const double freq;
	const int volume_level;
	const int duration;
	void * const buf IMYP_ATTR ((unused));
	int bufsize IMYP_ATTR ((unused));
#endif
{
	int res = 0;
	struct imyp_gst_backend_data * data =
		(struct imyp_gst_backend_data *)imyp_data;

	if ( (duration <= 0) || (data == NULL) )
	{
		return -1;
	}

	if ( (volume_level >= 0) && (volume_level <= IMYP_MAX_IMY_VOLUME) )
	{
		g_value_set_double (&(data->g_vol), (volume_level * 1.0)/IMYP_MAX_IMY_VOLUME);
	}
	else
	{
		g_value_set_double (&(data->g_vol), 1.0);
	}

	/* maybe not the simplest method, but works. */
#ifdef IMYP_HAVE_GST1
	gst_timed_value_control_source_set ((GstTimedValueControlSource *) data->volctl,
		0 * GST_SECOND, g_value_get_double (&(data->g_vol)));
	gst_timed_value_control_source_set ((GstTimedValueControlSource *) data->volctl,
		1 * GST_SECOND, g_value_get_double (&(data->g_vol)));
#else
	gst_interpolation_control_source_set (data->volctl,
		0 * GST_SECOND, &(data->g_vol));
	gst_interpolation_control_source_set (data->volctl,
		1 * GST_SECOND, &(data->g_vol));
#endif
	if ( freq < 0.0 )
	{
		g_value_set_double (&(data->g_freq), 0.0);
	}
	else
	{
		g_value_set_double (&(data->g_freq), freq);
	}
#ifdef IMYP_HAVE_GST1
	/* This doesn't work, despite the examples:
	gst_timed_value_control_source_set ((GstTimedValueControlSource *) data->freqctl,
		0 * GST_SECOND, g_value_get_double (&(data->g_freq)));
	gst_timed_value_control_source_set ((GstTimedValueControlSource *) data->freqctl,
		(unsigned int)duration * GST_MSECOND, g_value_get_double (&(data->g_freq)));
	*/
	g_object_set (data->src, prop_name_freq, g_value_get_double (&(data->g_freq)), NULL);
#else
	gst_interpolation_control_source_set (data->freqctl,
		0 * GST_SECOND, &(data->g_freq));
	gst_interpolation_control_source_set (data->freqctl,
		1 * GST_SECOND, &(data->g_freq));
#endif

	data->gclock_ID = gst_clock_new_single_shot_id (data->gclock,
		gst_clock_get_time (data->gclock)
		+ (unsigned int)duration * GST_MSECOND /* warning inside GST_MSECOND */);

	if ( gst_element_set_state (data->binpipe, GST_STATE_PLAYING) != 0 )
	{
		data->gclock_ret = gst_clock_id_wait (data->gclock_ID, NULL);
		/* This is disabled, because somtimes (especially for short periods)
		   the clock doesn't return GST_CLOCK_OK, but the note still is played,
		   so everything is OK anyway. */
		/*if ( data->gclock_ret != GST_CLOCK_OK ) res = -2;*/
		gst_element_set_state (data->binpipe, GST_STATE_NULL);
	}
	else
	{
		res = -3;
	}

	return res;
}

/**
 * Pauses for the specified period of time.
 * \param imyp_data pointer to the backend's private data.
 * \param milliseconds Number of milliseconds to pause.
 */
void
imyp_gst_pause (
#ifdef IMYP_ANSIC
	imyp_backend_data_t * const imyp_data,
	const int milliseconds)
#else
	imyp_data, milliseconds)
	imyp_backend_data_t * const imyp_data;
	const int milliseconds;
#endif
{
	struct imyp_gst_backend_data * data =
		(struct imyp_gst_backend_data *)imyp_data;
	GstClockID pause_clock_ID;

	if ( (milliseconds <= 0) || (data == NULL) )
	{
		return;
	}

	pause_clock_ID = gst_clock_new_single_shot_id (data->gclock,
		gst_clock_get_time (data->gclock)
		+ (unsigned int)milliseconds * GST_MSECOND /* warning inside GST_MSECOND */);
	gst_clock_id_wait (pause_clock_ID, NULL);
}

/**
 * Outputs the given text.
 * \param imyp_data pointer to the backend's private data.
 * \param text The text to output.
 */
void
imyp_gst_put_text (
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
 * Initializes the GStreamer library for use.
 * \param imyp_data pointer to storage for the backend's private data.
 * \param dev_file The output device to open, if any.
 * \return 0 on success.
 */
int
imyp_gst_init (
#ifdef IMYP_ANSIC
	imyp_backend_data_t ** const imyp_data,
	const char * const dev_file)
#else
	imyp_data, dev_file)
	imyp_backend_data_t ** const imyp_data;
	const char * const dev_file;
#endif
{
	struct imyp_gst_backend_data * data;

	if ( imyp_data == NULL )
	{
		return -100;
	}
#ifdef HAVE_MALLOC
	data = (struct imyp_gst_backend_data *) malloc (sizeof (
		struct imyp_gst_backend_data));
	if ( data == NULL )
	{
		return -13;
	}
#else
	data = &imyp_gst_backend_data_static;
#endif

	gst_init (NULL, NULL);
#ifndef IMYP_HAVE_GST1
	gst_controller_init (NULL, NULL);
#endif

	data->binpipe = gst_pipeline_new ("imyplay");
	if ( data->binpipe == NULL )
	{
		gst_deinit ();
#ifdef HAVE_MALLOC
		free (data);
#endif
		return -1;
	}

	data->gclock = gst_pipeline_get_clock (GST_PIPELINE (data->binpipe));
	if ( data->gclock == NULL )
	{
		g_object_unref (G_OBJECT (data->binpipe));
		gst_deinit ();
#ifdef HAVE_MALLOC
		free (data);
#endif
		return -2;
	}

	data->src = gst_element_factory_make ("audiotestsrc", "audiogen");
	if ( data->src == NULL )
	{
		g_object_unref (G_OBJECT (data->gclock));
		g_object_unref (G_OBJECT (data->binpipe));
		gst_deinit ();
#ifdef HAVE_MALLOC
		free (data);
#endif
		return -3;
	}

#define IMYP_GST_OUTPUT_NAME "audioout"
	if ( dev_file == NULL )
	{
		data->sink = gst_element_factory_make ("autoaudiosink", IMYP_GST_OUTPUT_NAME);
		if ( data->sink == NULL )
		{
			data->sink = gst_element_factory_make ("alsasink", IMYP_GST_OUTPUT_NAME);
		}
	}
	else
	{
		data->sink = gst_element_factory_make (dev_file, IMYP_GST_OUTPUT_NAME);
	}


	if ( data->sink == NULL )
	{
		/* look in imyp_gst_close() for why this is disabled: */
		/*g_object_unref (G_OBJECT (src));*/
		g_object_unref (G_OBJECT (data->gclock));
		g_object_unref (G_OBJECT (data->binpipe));
		gst_deinit ();
#ifdef HAVE_MALLOC
		free (data);
#endif
		return -4;
	}

	gst_bin_add_many (GST_BIN (data->binpipe), data->src, data->sink, NULL);
	if ( gst_element_link (data->src, data->sink) == 0 )
	{
		/* look in imyp_gst_close() for why this is disabled: */
		/*g_object_unref (G_OBJECT (sink));
		g_object_unref (G_OBJECT (src));*/
		g_object_unref (G_OBJECT (data->gclock));
		g_object_unref (G_OBJECT (data->binpipe));
		gst_deinit ();
#ifdef HAVE_MALLOC
		free (data);
#endif
		return -5;
	}

#ifndef IMYP_HAVE_GST1
	data->ctl = gst_controller_new (G_OBJECT (data->src), prop_name_freq, prop_name_volume, NULL);
	if ( data->ctl == NULL )
	{
		/* look in imyp_gst_close() for why this is disabled: */
		/*g_object_unref (G_OBJECT (sink));
		g_object_unref (G_OBJECT (src));*/
		g_object_unref (G_OBJECT (data->gclock));
		g_object_unref (G_OBJECT (data->binpipe));
		gst_deinit ();
# ifdef HAVE_MALLOC
		free (data);
# endif
		return -6;
	}
#endif /* ! IMYP_HAVE_GST1 */

	/* maybe not the simplest method, but works. */
	data->volctl = GST_INTERPOLATION_CONTROL_SOURCE (gst_interpolation_control_source_new());
	if ( data->volctl == NULL )
	{
#ifndef IMYP_HAVE_GST1
		g_object_unref (G_OBJECT (data->ctl));
#endif
		/* look in imyp_gst_close() for why this is disabled: */
		/*g_object_unref (G_OBJECT (sink));
		g_object_unref (G_OBJECT (src));*/
		g_object_unref (G_OBJECT (data->gclock));
		g_object_unref (G_OBJECT (data->binpipe));
		gst_deinit ();
#ifdef HAVE_MALLOC
		free (data);
#endif
		return -7;
	}

#ifdef IMYP_HAVE_GST1
	if ( gst_object_add_control_binding (GST_OBJECT_CAST (data->src),
		gst_direct_control_binding_new (GST_OBJECT_CAST (data->src),
			prop_name_volume, GST_CONTROL_SOURCE (data->volctl))) == FALSE )
#else
	if ( gst_controller_set_control_source (data->ctl, prop_name_volume,
		GST_CONTROL_SOURCE (data->volctl)) == FALSE )
#endif
	{
		g_object_unref (data->volctl);
#ifndef IMYP_HAVE_GST1
		g_object_unref (G_OBJECT (data->ctl));
#endif
		/* look in imyp_gst_close() for why this is disabled: */
		/*g_object_unref (G_OBJECT (sink));
		g_object_unref (G_OBJECT (src));*/
		g_object_unref (G_OBJECT (data->gclock));
		g_object_unref (G_OBJECT (data->binpipe));
		gst_deinit ();
#ifdef HAVE_MALLOC
		free (data);
#endif
		return -8;
	}

#ifdef IMYP_HAVE_GST1
	g_object_set (data->volctl, prop_name_mode, GST_INTERPOLATION_MODE_LINEAR, NULL);
#else
	if ( gst_interpolation_control_source_set_interpolation_mode (
		data->volctl, GST_INTERPOLATE_LINEAR) == FALSE )
	{
		g_object_unref (data->volctl);
		g_object_unref (G_OBJECT (data->ctl));
		/* look in imyp_gst_close() for why this is disabled: */
		/*g_object_unref (G_OBJECT (sink));
		g_object_unref (G_OBJECT (src));*/
		g_object_unref (G_OBJECT (data->gclock));
		g_object_unref (G_OBJECT (data->binpipe));
		gst_deinit ();
# ifdef HAVE_MALLOC
		free (data);
# endif
		return -9;
	}
#endif /* IMYP_HAVE_GST1 */

	data->freqctl = GST_INTERPOLATION_CONTROL_SOURCE (gst_interpolation_control_source_new());
	if ( data->freqctl == NULL )
	{
		g_object_unref (data->volctl);
#ifndef IMYP_HAVE_GST1
		g_object_unref (G_OBJECT (data->ctl));
#endif
		/* look in imyp_gst_close() for why this is disabled: */
		/*g_object_unref (G_OBJECT (sink));
		g_object_unref (G_OBJECT (src));*/
		g_object_unref (G_OBJECT (data->gclock));
		g_object_unref (G_OBJECT (data->binpipe));
		gst_deinit ();
#ifdef HAVE_MALLOC
		free (data);
#endif
		return -10;
	}

#ifdef IMYP_HAVE_GST1
	if ( gst_object_add_control_binding (GST_OBJECT_CAST (data->src),
		gst_direct_control_binding_new (GST_OBJECT_CAST (data->src),
			prop_name_freq, GST_CONTROL_SOURCE (data->freqctl))) == FALSE )
#else
	if ( gst_controller_set_control_source (data->ctl, prop_name_freq,
		GST_CONTROL_SOURCE (data->freqctl)) == FALSE )
#endif
	{
		g_object_unref (data->freqctl);
		g_object_unref (data->volctl);
#ifndef IMYP_HAVE_GST1
		g_object_unref (G_OBJECT (data->ctl));
#endif
		/* look in imyp_gst_close() for why this is disabled: */
		/*g_object_unref (G_OBJECT (sink));
		g_object_unref (G_OBJECT (src));*/
		g_object_unref (G_OBJECT (data->gclock));
		g_object_unref (G_OBJECT (data->binpipe));
		gst_deinit ();
#ifdef HAVE_MALLOC
		free (data);
#endif
		return -11;
	}

#ifdef IMYP_HAVE_GST1
	g_object_set (data->freqctl, prop_name_mode, GST_INTERPOLATION_MODE_LINEAR, NULL);
#else
	if ( gst_interpolation_control_source_set_interpolation_mode (
		data->freqctl, GST_INTERPOLATE_LINEAR) == FALSE )
	{
		g_object_unref (data->freqctl);
		g_object_unref (data->volctl);
		g_object_unref (G_OBJECT (data->ctl));
		/* look in imyp_gst_close() for why this is disabled: */
		/*g_object_unref (G_OBJECT (sink));
		g_object_unref (G_OBJECT (src));*/
		g_object_unref (G_OBJECT (data->gclock));
		g_object_unref (G_OBJECT (data->binpipe));
		gst_deinit ();
# ifdef HAVE_MALLOC
		free (data);
# endif
		return -12;
	}
#endif /* IMYP_HAVE_GST1 */

	IMYP_MEMSET (&(data->g_vol), 0, sizeof (GValue));
	IMYP_MEMSET (&(data->g_freq), 0, sizeof (GValue));
	g_value_init (&(data->g_vol), G_TYPE_DOUBLE);
	g_value_init (&(data->g_freq), G_TYPE_DOUBLE);
	*imyp_data = (imyp_backend_data_t *)data;

	return 0;
}

/**
 * Closes the GStreamer library.
 * \param imyp_data pointer to the backend's private data.
 * \return 0 on success.
 */
int
imyp_gst_close (
#ifdef IMYP_ANSIC
	imyp_backend_data_t * const imyp_data)
#else
	imyp_data)
	imyp_backend_data_t * const imyp_data;
#endif
{
	struct imyp_gst_backend_data * data =
		(struct imyp_gst_backend_data *)imyp_data;

	if ( data != NULL )
	{
		if ( data->freqctl != NULL )
		{
			g_object_unref (data->freqctl);
		}
		if ( data->volctl != NULL )
		{
			g_object_unref (data->volctl);
		}
#ifndef IMYP_HAVE_GST1
		if ( data->ctl != NULL )
		{
			g_object_unref (G_OBJECT (data->ctl));
		}
#endif
		if ( data->gclock != NULL )
		{
			g_object_unref (G_OBJECT (data->gclock));
		}
		/* binpipe must be released before sink and src, otherwise:
			(<unknown>:26285): GStreamer-CRITICAL **:
			Trying to dispose object "audioout", but it still has a parent "imyplay".
			You need to let the parent manage the object instead of unreffing the object directly.
		*/
		if ( data->binpipe != NULL )
		{
			g_object_unref (G_OBJECT (data->binpipe));
		}

		/* If binpipe is released before sink and src, it takes care of them.
		If we release sink and src here, we get:
		(<unknown>:26329): GLib-GObject-WARNING **: invalid uninstantiatable type `<invalid>' in cast to `GObject'
		(<unknown>:26329): GLib-GObject-CRITICAL **: g_object_unref: assertion `G_IS_OBJECT (object)' failed
		*/
		/*if ( data->sink != NULL ) g_object_unref (G_OBJECT (data->sink));
		if ( data->src != NULL ) g_object_unref (G_OBJECT (data->src));*/
		gst_deinit ();
#ifdef HAVE_MALLOC
		free (data);
#endif
	}
	return 0;
}

/**
 * Displays the version of the GStreamer library IMYplay was compiled with.
 * \param imyp_data pointer to the backend's private data.
 */
void
imyp_gst_version (
#ifdef IMYP_ANSIC
	imyp_backend_data_t * const imyp_data IMYP_ATTR ((unused)))
#else
	imyp_data)
	imyp_backend_data_t * const imyp_data IMYP_ATTR ((unused));
#endif
{
	guint major;
	guint minor;
	guint micro;
	guint nano;

#if (defined GST_VERSION_MAJOR) && (defined GST_VERSION_MINOR) \
	&& (defined GST_VERSION_MICRO) && (defined GST_VERSION_NANO)
	printf ( "GStreamer (%s): %d.%d.%d.%d\n", _(ver_msg_compiled),
		GST_VERSION_MAJOR, GST_VERSION_MINOR,
		GST_VERSION_MICRO, GST_VERSION_NANO );
#else
# if (defined GST_VERSION_MAJOR) && (defined GST_VERSION_MINOR) \
	&& (defined GST_VERSION_MICRO)
	printf ( "GStreamer (%s): %d.%d.%d\n", _(ver_msg_compiled),
		GST_VERSION_MAJOR, GST_VERSION_MINOR,
		GST_VERSION_MICRO );
# else
#  if (defined GST_VERSION_MAJOR) && (defined GST_VERSION_MINOR)
	printf ( "GStreamer (%s): %d.%d\n", _(ver_msg_compiled),
		GST_VERSION_MAJOR, GST_VERSION_MINOR );
#  else
#   if (defined GST_VERSION_MAJOR)
	printf ( "GStreamer (%s): %d\n", _(ver_msg_compiled), GST_VERSION_MAJOR );
#   else
	printf ( "GStreamer (%s): ?\n", _(ver_msg_compiled) );
#   endif
#  endif
# endif
#endif
	gst_version (&major, &minor, &micro, &nano);
	printf ( "GStreamer (%s): %d.%d.%d.%d\n", _(ver_msg_runtime),
		major, minor, micro, nano );
}
