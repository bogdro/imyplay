/*
 * IMYplay - A program for playing iMelody ringtones (IMY files).
 *	-- Allegro backend.
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
#include "imyp_all.h"
#include "imyp_sig.h"
#include "imyputil.h"

#include <stdio.h>

#if (defined __GNUC__) && ( \
	(defined __MINGW32__) || \
	(defined __MINGW64__) || \
	(defined ___MSVCRT__) || \
	(defined _WIN32) || \
	(defined WIN32) || \
	(defined __WIN32__) || \
	(defined __WINNT) || \
	(defined __WINNT__) || \
	(defined WINNT) \
  )

/* Compatibility fix for DLL imports in MinGW - define function once. */
# define AL_INLINE(type, name, args, code)	\
	static __inline__ type name args;	\
        static __inline__ type name args code

#endif

#ifdef IMYP_HAVE_ALLEGRO
# include <allegro.h>
#else
# error Allegro requested, but components not found.
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

/* DOS: manually set all the drivers to use (no gfx, no midi,
   no joystick, ...) to save space */
#if (defined BEGIN_GFX_DRIVER_LIST) && (defined END_GFX_DRIVER_LIST)
BEGIN_GFX_DRIVER_LIST
END_GFX_DRIVER_LIST
#endif

#if (defined BEGIN_COLOR_DEPTH_LIST) && (defined END_COLOR_DEPTH_LIST)
BEGIN_COLOR_DEPTH_LIST
END_COLOR_DEPTH_LIST
#endif

/* DLL-export-related errors on MinGW */
#if (defined BEGIN_JOYSTICK_DRIVER_LIST) && (defined END_JOYSTICK_DRIVER_LIST) && (defined ALLEGRO_UNIX)
BEGIN_JOYSTICK_DRIVER_LIST
END_JOYSTICK_DRIVER_LIST
#endif

#if (defined BEGIN_MIDI_DRIVER_LIST) && (defined END_MIDI_DRIVER_LIST)
BEGIN_MIDI_DRIVER_LIST
END_MIDI_DRIVER_LIST
#endif

#if (defined BEGIN_DIGI_DRIVER_LIST) && (defined END_DIGI_DRIVER_LIST)
/* required, because we include the midi list */
BEGIN_DIGI_DRIVER_LIST

/* Unix: */
# ifdef DIGI_DRIVER_ALSA
DIGI_DRIVER_ALSA
# endif
# ifdef DIGI_DRIVER_OSS
DIGI_DRIVER_OSS
# endif
# ifdef DIGI_DRIVER_ESD
DIGI_DRIVER_ESD
# endif
# ifdef DIGI_DRIVER_ARTS
DIGI_DRIVER_ARTS
# endif
# ifdef DIGI_DRIVER_SGIAL
DIGI_DRIVER_SGIAL
# endif
# ifdef DIGI_DRIVER_JACK
DIGI_DRIVER_JACK
# endif

/* DOS/Windows: */
# ifdef DIGI_DRIVER_SB
DIGI_DRIVER_SB
# endif
# ifdef DIGI_DRIVER_SOUNDSCAPE
DIGI_DRIVER_SOUNDSCAPE
# endif
# ifdef DIGI_DRIVER_AUDIODRIVE
DIGI_DRIVER_AUDIODRIVE
# endif
# ifdef DIGI_DRIVER_WINSOUNDSYS
DIGI_DRIVER_WINSOUNDSYS
# endif

/* BeOS: */
# ifdef DIGI_DRIVER_BEOS
DIGI_DRIVER_BEOS
# endif

END_DIGI_DRIVER_LIST
#endif /* BEGIN_DIGI_DRIVER_LIST && END_DIGI_DRIVER_LIST */

struct imyp_allegro_backend_data
{
	int quality;
	int sampfreq;
};

#ifdef TEST_COMPILE
# undef IMYP_ANSIC
# if TEST_COMPILE > 1
#  undef HAVE_MALLOC
# endif
#endif

#ifndef HAVE_MALLOC
static struct imyp_allegro_backend_data imyp_allegro_backend_data_static;
#endif

#ifndef IMYP_ANSIC
static AUDIOSTREAM *
imyp_all_audiostream_init IMYP_PARAMS ((const int duration, const int vol,
	imyp_backend_data_t * const imyp_data));
#endif

/**
 * Creates an AUDIOSTREAM for playing notes.
 * \param duration The duration of the sound to generate, in milliseconds (defines the number of samples).
 * \param vol Volume of the tone (from 0 to 15).
 * \param imyp_data The driver's data structure.
 * \return an allocated AUDIOSTREAM or NULL in case of error.
 */
static AUDIOSTREAM *
imyp_all_audiostream_init (
#ifdef IMYP_ANSIC
	const int duration,
	const int vol,
	imyp_backend_data_t * const imyp_data)
#else
	duration, vol, imyp_data)
	const int duration;
	const int vol;
	imyp_backend_data_t * const imyp_data;
#endif
{
#define IMYP_ALL_VOL ((vol * 255)/IMYP_MAX_IMY_VOLUME)
#define IMYP_ALL_MONO 0
#define IMYP_ALL_PAN_MIDDLE 128

	AUDIOSTREAM * as;
	struct imyp_allegro_backend_data * data =
		(struct imyp_allegro_backend_data *)imyp_data;

	if ( (data->quality == 8 || data->quality == 16) && (data->sampfreq > 0) )
	{
		as = play_audio_stream ((duration * data->sampfreq) / 1000,
			data->quality, IMYP_ALL_MONO, data->sampfreq,
			IMYP_ALL_VOL, IMYP_ALL_PAN_MIDDLE);
		if ( as != NULL )
		{
			return as;
		}
	}

	as = play_audio_stream ((duration * 44100) / 1000, 16,
		IMYP_ALL_MONO, 44100, IMYP_ALL_VOL, IMYP_ALL_PAN_MIDDLE);
	if ( as != NULL )
	{
		data->quality = 16;
		data->sampfreq = 44100;
		return as;
	}
	as = play_audio_stream ((duration * 22050) / 1000, 16,
		IMYP_ALL_MONO, 22050, IMYP_ALL_VOL, IMYP_ALL_PAN_MIDDLE);
	if ( as != NULL )
	{
		data->quality = 16;
		data->sampfreq = 22050;
		return as;
	}
	as = play_audio_stream ((duration * 11025) / 1000, 16,
		IMYP_ALL_MONO, 11025, IMYP_ALL_VOL, IMYP_ALL_PAN_MIDDLE);
	if ( as != NULL )
	{
		data->quality = 16;
		data->sampfreq = 11025;
		return as;
	}

	as = play_audio_stream ((duration * 44100) / 1000, 8,
		IMYP_ALL_MONO, 44100, IMYP_ALL_VOL, IMYP_ALL_PAN_MIDDLE);
	if ( as != NULL )
	{
		data->quality = 8;
		data->sampfreq = 44100;
		return as;
	}
	as = play_audio_stream ((duration * 22050) / 1000, 8,
		IMYP_ALL_MONO, 22050, IMYP_ALL_VOL, IMYP_ALL_PAN_MIDDLE);
	if ( as != NULL )
	{
		data->quality = 8;
		data->sampfreq = 22050;
		return as;
	}
	as = play_audio_stream ((duration * 11025) / 1000, 8,
		IMYP_ALL_MONO, 11025, IMYP_ALL_VOL, IMYP_ALL_PAN_MIDDLE);
	if ( as != NULL )
	{
		data->quality = 8;
		data->sampfreq = 11025;
		return as;
	}
#undef IMYP_ALL_VOL
#undef IMYP_ALL_MONO
#undef IMYP_ALL_PAN_MIDDLE

	return NULL;
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
imyp_all_play_tune (
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
	AUDIOSTREAM * as;
	void * new_buf;
	struct imyp_allegro_backend_data * data =
		(struct imyp_allegro_backend_data *)imyp_data;

	if ( (buf == NULL) || (bufsize <= 0) || (imyp_data == NULL) || (duration <= 0) )
	{
		return -1;
	}
	/*as = imyp_all_audiostream_init (bufsize, volume_level, imyp_data);*/
	as = imyp_all_audiostream_init (duration, volume_level, imyp_data);

	if ( as != NULL )
	{
		bufsize = imyp_generate_samples (freq, volume_level, duration, buf, bufsize,
			1, 1, (unsigned int)data->quality, (unsigned int)data->sampfreq, NULL);
		if ( imyp_sig_recvd != 0 )
		{
			stop_audio_stream (as);
			return -2;
		}
		new_buf = get_audio_stream_buffer (as);
		while ( new_buf != NULL )
		{
			IMYP_MEMCOPY (new_buf, buf, (size_t)bufsize);
			if ( imyp_sig_recvd != 0 )
			{
				stop_audio_stream (as);
				return -2;
			}
			free_audio_stream_buffer (as);
			new_buf = get_audio_stream_buffer (as);
		}
		/* wait for the sound to play for the required period of time: */
		imyp_all_pause (imyp_data, duration);
		stop_audio_stream (as);
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
imyp_all_pause (
#ifdef IMYP_ANSIC
	imyp_backend_data_t * const imyp_data IMYP_ATTR ((unused)),
	const int milliseconds)
#else
	imyp_data, milliseconds)
	imyp_backend_data_t * const imyp_data IMYP_ATTR ((unused));
	const int milliseconds;
#endif
{
	if ( milliseconds <= 0 )
	{
		return;
	}

	/* for Allegro, a default wait function must be used, because
	   rest() seems not to work as expected */
#ifdef IMYP_HAVE_SELECT
	imyp_pause_select (milliseconds);
#else
	rest ((unsigned int)milliseconds);
#endif

}

/**
 * Outputs the given text.
 * \param imyp_data pointer to the backend's private data.
 * \param text The text to output.
 */
void
imyp_all_put_text (
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
 * Initializes the Allegro library for use.
 * \param imyp_data pointer to storage for the backend's private data.
 * \return 0 on success.
 */
int
imyp_all_init (
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
	struct imyp_allegro_backend_data * data;
	enum IMYP_SAMPLE_FORMATS format;
	char * dev_copy;
	char * colon;
	int scanf_res;

	res = allegro_init ();
	if ( res != 0 )
	{
		return res;
	}

	if ( imyp_data == NULL )
	{
		return -100;
	}
#ifdef HAVE_MALLOC
	data = (struct imyp_allegro_backend_data *) malloc (sizeof (
		struct imyp_allegro_backend_data));
	if ( data == NULL )
	{
		return -6;
	}
#else
	data = &imyp_allegro_backend_data_static;
#endif

	install_keyboard ();
	/* 2 voices, so they can be interchanged, instead of constantly
	   waiting for the first one */
	reserve_voices (2, 0);
	res = install_sound (DIGI_AUTODETECT, MIDI_NONE, NULL);
	if ( res != 0 )
	{
		reserve_voices (1, 0);
		res = install_sound (DIGI_AUTODETECT, MIDI_NONE, NULL);
		if ( res != 0 )
		{
			allegro_exit ();
#ifdef HAVE_MALLOC
			free (data);
#endif
			return res;
		}
	}

	set_volume(255, -1);
	data->quality = 0;
	data->sampfreq = 0;
	if ( dev_file != NULL )
	{
		dev_copy = IMYP_STRDUP (dev_file);
		if ( dev_copy != NULL )
		{
			colon = strrchr (dev_copy, (int)':');
			if ( colon != NULL )
			{
				format = imyp_get_format (colon+1);
				if ( format == IMYP_SAMPLE_FORMAT_UNKNOWN )
				{
					format = IMYP_SAMPLE_FORMAT_S16LE;
				}

				if ( (format == IMYP_SAMPLE_FORMAT_S8LE)
					|| (format == IMYP_SAMPLE_FORMAT_S8BE)
					|| (format == IMYP_SAMPLE_FORMAT_U8LE)
					|| (format == IMYP_SAMPLE_FORMAT_U8BE))
				{
					data->quality = 8;
				}
				else
				{
					data->quality = 16;
				}
				/* wipe the colon to read the sampling rate */
				*colon = '\0';
			}
			/* get the sampling rate: */
			scanf_res = sscanf (dev_copy, "%d", &(data->sampfreq));
			if ( scanf_res == 1 )
			{
				if ( data->sampfreq <= 0 )
				{
					data->sampfreq = 44100;
				}
			}
			free (dev_copy);
		}
	}

	*imyp_data = (imyp_backend_data_t *)data;
	return 0;
}

/**
 * Closes the Allegro library.
 * \param imyp_data pointer to the backend's private data.
 * \return 0 on success.
 */
int
imyp_all_close (
#ifdef IMYP_ANSIC
	imyp_backend_data_t * const imyp_data)
#else
	imyp_data)
	imyp_backend_data_t * const imyp_data;
#endif
{
	remove_sound ();
	allegro_exit ();
	if ( imyp_data != NULL )
	{
		free (imyp_data);
	}
	return 0;
}

/**
 * Displays the version of the Allegro library IMYplay was compiled with.
 * \param imyp_data pointer to the backend's private data.
 */
void
imyp_all_version (
#ifdef IMYP_ANSIC
	imyp_backend_data_t * const imyp_data IMYP_ATTR ((unused)))
#else
	imyp_data)
	imyp_backend_data_t * const imyp_data IMYP_ATTR ((unused));
#endif
{
#ifdef ALLEGRO_VERSION_STR
	printf ( "Allegro: %s\n", ALLEGRO_VERSION_STR );
#else
	printf ( "Allegro: ?\n" );
#endif
}
