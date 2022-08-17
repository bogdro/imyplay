/*
 * A program for playing iMelody ringtones (IMY files).
 *	-- GStreamer backend.
 *
 * Copyright (C) 2011 Bogdan Drozdowski, bogdandr (at) op.pl
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
#include "imyp_gst.h"
#include "imyp_sig.h"

#include <stdio.h>

#ifdef HAVE_GST_GST_H
# include <gst/gst.h>
# include <gst/controller/gstcontroller.h>
# include <gst/controller/gstinterpolationcontrolsource.h>
# include <gst/gstversion.h>
#else
# error The GStreamer library was not found.
#endif

static GstElement *src = NULL;
static GstElement *sink = NULL;
static GstElement *binpipe = NULL;
static GstController *ctl = NULL;
static GstInterpolationControlSource *volctl = NULL;
static GstInterpolationControlSource *freqctl = NULL;
static GstClock *gclock = NULL;
static GstClockID gclock_ID;
static GstClockReturn gclock_ret;
static GValue g_vol;
static GValue g_freq;

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
imyp_gst_play_tune (
#ifdef IMYP_ANSIC
	const double freq,
	const int volume_level,
	const int duration,
	void * const buf IMYP_ATTR ((unused)),
	int bufsize IMYP_ATTR ((unused)))
#else
	freq, volume_level, duration, buf, bufsize)
	const double freq;
	const int volume_level;
	const int duration;
	void * const buf IMYP_ATTR ((unused));
	int bufsize IMYP_ATTR ((unused));
#endif
{
	int res = 0;

	if ( duration <= 0 ) return -1;

	if ( (volume_level >= 0) && (volume_level <= IMYP_MAX_IMY_VOLUME) )
	{
		g_value_set_double (&g_vol, (volume_level * 1.0)/IMYP_MAX_IMY_VOLUME);
	}
	else
	{
		g_value_set_double (&g_vol, 1.0);
	}

	/* maybe not the simplest method, but works. */
	gst_interpolation_control_source_set (volctl, 0 * GST_SECOND, &g_vol);
	gst_interpolation_control_source_set (volctl, 1 * GST_SECOND, &g_vol);

	if ( freq < 0.0 ) g_value_set_double (&g_freq, 0.0);
	else g_value_set_double (&g_freq, freq);

	gst_interpolation_control_source_set (freqctl, 0 * GST_SECOND, &g_freq);
	gst_interpolation_control_source_set (freqctl, 1 * GST_SECOND, &g_freq);

	gclock_ID = gst_clock_new_single_shot_id (gclock,
		gst_clock_get_time (gclock) + duration * GST_MSECOND /* warning inside GST_MSECOND */);

	if ( gst_element_set_state (binpipe, GST_STATE_PLAYING) != 0 )
	{
		gclock_ret = gst_clock_id_wait (gclock_ID, NULL);
		/* This is disabled, because somtimes (especially for short periods)
		   the clock doesn't return GST_CLOCK_OK, but the note still is played,
		   so everything is OK anyway. */
		/*if ( gclock_ret != GST_CLOCK_OK ) res = -2;*/
		gst_element_set_state (binpipe, GST_STATE_NULL);
	}
	else res = -3;

	return res;
}

/**
 * Pauses for the specified period of time.
 * \param milliseconds Number of milliseconds to pause.
 */
void
imyp_gst_pause (
#ifdef IMYP_ANSIC
	const int milliseconds)
#else
	milliseconds)
	const int milliseconds;
#endif
{
	GstClockID pause_clock_ID;
	if ( milliseconds <= 0 ) return;

	pause_clock_ID = gst_clock_new_single_shot_id (gclock,
		gst_clock_get_time (gclock)
		+ (unsigned int)milliseconds * GST_MSECOND /* warning inside GST_MSECOND */);
	gst_clock_id_wait (pause_clock_ID, NULL);
}

/**
 * Outputs the given text.
 * \param text The text to output.
 */
void
imyp_gst_put_text (
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
 * Initializes the GStreamer library for use.
 * \param dev_file The output device to open, if any.
 * \return 0 on success.
 */
int
imyp_gst_init (
#ifdef IMYP_ANSIC
	const char * const dev_file)
#else
	dev_file)
	const char * const dev_file;
#endif
{
#ifndef HAVE_MEMSET
	size_t i;
#endif

	gst_init (NULL, NULL);
	gst_controller_init (NULL, NULL);

	binpipe = gst_pipeline_new ("imyplay");
	if ( binpipe == NULL )
	{
		return -1;
	}

	gclock = gst_pipeline_get_clock (GST_PIPELINE (binpipe));
	if ( gclock == NULL )
	{
		g_object_unref (G_OBJECT (binpipe));
		return -2;
	}

	src = gst_element_factory_make ("audiotestsrc", "audiogen");
	if ( src == NULL )
	{
		g_object_unref (G_OBJECT (gclock));
		g_object_unref (G_OBJECT (binpipe));
		return -3;
	}

	if ( dev_file == NULL )
	{
		sink = gst_element_factory_make ("autoaudiosink", "audioout");
	}
	else
	{
		sink = gst_element_factory_make (dev_file, "audioout");
	}

	if ( sink == NULL )
	{
		/* look in imyp_gst_close() for why this is disabled: */
		/*g_object_unref (G_OBJECT (src));*/
		g_object_unref (G_OBJECT (gclock));
		g_object_unref (G_OBJECT (binpipe));
		return -4;
	}

	gst_bin_add_many (GST_BIN (binpipe), src, sink, NULL);
	if ( gst_element_link (src, sink) == 0 )
	{
		/* look in imyp_gst_close() for why this is disabled: */
		/*g_object_unref (G_OBJECT (sink));
		g_object_unref (G_OBJECT (src));*/
		g_object_unref (G_OBJECT (gclock));
		g_object_unref (G_OBJECT (binpipe));
		return -5;
	}

	ctl = gst_controller_new (G_OBJECT (src), "freq", "volume", NULL);
	if ( ctl == NULL )
	{
		/* look in imyp_gst_close() for why this is disabled: */
		/*g_object_unref (G_OBJECT (sink));
		g_object_unref (G_OBJECT (src));*/
		g_object_unref (G_OBJECT (gclock));
		g_object_unref (G_OBJECT (binpipe));
		return -6;
	}

	/* maybe not the simplest method, but works. */
	volctl = gst_interpolation_control_source_new ();
	if ( volctl == NULL )
	{
		g_object_unref (G_OBJECT (ctl));
		/* look in imyp_gst_close() for why this is disabled: */
		/*g_object_unref (G_OBJECT (sink));
		g_object_unref (G_OBJECT (src));*/
		g_object_unref (G_OBJECT (gclock));
		g_object_unref (G_OBJECT (binpipe));
		return -7;
	}
	if ( gst_controller_set_control_source (ctl, "volume", GST_CONTROL_SOURCE (volctl)) == FALSE )
	{
		g_object_unref (volctl);
		g_object_unref (G_OBJECT (ctl));
		/* look in imyp_gst_close() for why this is disabled: */
		/*g_object_unref (G_OBJECT (sink));
		g_object_unref (G_OBJECT (src));*/
		g_object_unref (G_OBJECT (gclock));
		g_object_unref (G_OBJECT (binpipe));
		return -8;
	}
	if ( gst_interpolation_control_source_set_interpolation_mode (volctl, GST_INTERPOLATE_LINEAR)
		== FALSE )
	{
		g_object_unref (volctl);
		g_object_unref (G_OBJECT (ctl));
		/* look in imyp_gst_close() for why this is disabled: */
		/*g_object_unref (G_OBJECT (sink));
		g_object_unref (G_OBJECT (src));*/
		g_object_unref (G_OBJECT (gclock));
		g_object_unref (G_OBJECT (binpipe));
		return -9;
	}

	freqctl = gst_interpolation_control_source_new ();
	if ( freqctl == NULL )
	{
		g_object_unref (volctl);
		g_object_unref (G_OBJECT (ctl));
		/* look in imyp_gst_close() for why this is disabled: */
		/*g_object_unref (G_OBJECT (sink));
		g_object_unref (G_OBJECT (src));*/
		g_object_unref (G_OBJECT (gclock));
		g_object_unref (G_OBJECT (binpipe));
		return -10;
	}

	if ( gst_controller_set_control_source (ctl, "freq", GST_CONTROL_SOURCE (freqctl)) == FALSE )
	{
		g_object_unref (freqctl);
		g_object_unref (volctl);
		g_object_unref (G_OBJECT (ctl));
		/* look in imyp_gst_close() for why this is disabled: */
		/*g_object_unref (G_OBJECT (sink));
		g_object_unref (G_OBJECT (src));*/
		g_object_unref (G_OBJECT (gclock));
		g_object_unref (G_OBJECT (binpipe));
		return -11;
	}
	if ( gst_interpolation_control_source_set_interpolation_mode (freqctl, GST_INTERPOLATE_LINEAR)
		== FALSE )
	{
		g_object_unref (freqctl);
		g_object_unref (volctl);
		g_object_unref (G_OBJECT (ctl));
		/* look in imyp_gst_close() for why this is disabled: */
		/*g_object_unref (G_OBJECT (sink));
		g_object_unref (G_OBJECT (src));*/
		g_object_unref (G_OBJECT (gclock));
		g_object_unref (G_OBJECT (binpipe));
		return -12;
	}

#ifdef HAVE_MEMSET
	memset (&g_vol, 0, sizeof (GValue));
	memset (&g_freq, 0, sizeof (GValue));
#else
	for ( i = 0; i < sizeof (GValue); i++ )
	{
		((char *)&g_vol)[i] = '\0';
	}
	for ( i = 0; i < sizeof (GValue); i++ )
	{
		((char *)&g_freq)[i] = '\0';
	}
#endif
	g_value_init (&g_vol, G_TYPE_DOUBLE);
	g_value_init (&g_freq, G_TYPE_DOUBLE);

	return 0;
}

/**
 * Closes the GStreamer library.
 * \return 0 on success.
 */
int
imyp_gst_close (
#ifdef IMYP_ANSIC
	void
#endif
)
{
	if ( freqctl != NULL ) g_object_unref (freqctl);
	if ( volctl  != NULL ) g_object_unref (volctl);
	if ( ctl     != NULL ) g_object_unref (G_OBJECT (ctl));
	if ( gclock  != NULL ) g_object_unref (G_OBJECT (gclock));
	/* binpipe must be released before sink and src, otherwise:
		(<unknown>:26285): GStreamer-CRITICAL **:
		Trying to dispose object "audioout", but it still has a parent "imyplay".
		You need to let the parent manage the object instead of unreffing the object directly.
	*/
	if ( binpipe != NULL ) g_object_unref (G_OBJECT (binpipe));

	/* If binpipe is released before sink and src, it takes care of them.
	  If we release sink and src here, we get:
	(<unknown>:26329): GLib-GObject-WARNING **: invalid uninstantiatable type `<invalid>' in cast to `GObject'
	(<unknown>:26329): GLib-GObject-CRITICAL **: g_object_unref: assertion `G_IS_OBJECT (object)' failed
	*/
	/*if ( sink    != NULL ) g_object_unref (G_OBJECT (sink));
	if ( src     != NULL ) g_object_unref (G_OBJECT (src));*/

	return 0;
}

/**
 * Displays the version of the GStreamer library IMYplay was compiled with.
 */
void
imyp_gst_version (
#ifdef IMYP_ANSIC
	void
#endif
)
{
#if (defined GST_VERSION_MAJOR) && (defined GST_VERSION_MINOR) \
	&& (defined GST_VERSION_MICRO) && (defined GST_VERSION_NANO)
	printf ( "GStreamer: %d.%d.%d.%d\n", GST_VERSION_MAJOR, GST_VERSION_MINOR,
		GST_VERSION_MICRO, GST_VERSION_NANO );
#else
# if (defined GST_VERSION_MAJOR) && (defined GST_VERSION_MINOR) \
	&& (defined GST_VERSION_MICRO)
	printf ( "GStreamer: %d.%d.%d\n", GST_VERSION_MAJOR, GST_VERSION_MINOR,
		GST_VERSION_MICRO );
# else
#  if (defined GST_VERSION_MAJOR) && (defined GST_VERSION_MINOR)
	printf ( "GStreamer: %d.%d\n", GST_VERSION_MAJOR, GST_VERSION_MINOR );
#  else
#   if (defined GST_VERSION_MAJOR)
	printf ( "GStreamer: %d\n", GST_VERSION_MAJOR );
#   else
	printf ( "GStreamer: ?\n" );
#   endif
#  endif
# endif
#endif
}

