/*
 * A program for playing iMelody ringtones (IMY files).
 *	-- wrapper functions between the main program and the backends.
 *
 * Copyright (C) 2009-2010 Bogdan Drozdowski, bogdandr (at) op.pl
 * License: GNU General Public License, v3+
 *
 * Syntax example: imyplay ringtone.imy
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

#include <stdio.h>	/* NULL */
#ifndef NULL
# define NULL ((void *)0)
#endif

#include "imyplay.h"
#include "imypwrap.h"

#ifdef IMYP_HAVE_ALLEGRO
# include "imyp_all.h"
#endif

#ifdef IMYP_HAVE_SDL
# include "imyp_sdl.h"
#endif

#ifdef IMYP_HAVE_ALSA
# include "imyp_als.h"
#endif

#ifdef IMYP_HAVE_OSS
# include "imyp_oss.h"
#endif

#ifdef IMYP_HAVE_LIBAO
# include "imyp_ao.h"
#endif

#ifdef IMYP_HAVE_PORTAUDIO
# include "imyp_por.h"
#endif

#ifdef IMYP_HAVE_PULSEAUDIO
# include "imyp_pul.h"
#endif

#ifdef IMYP_HAVE_JACK
# include "imyp_jck.h"
#endif

#ifdef IMYP_HAVE_MIDI
# include "imyp_mid.h"
#endif

#ifdef IMYP_HAVE_EXEC
# include "imyp_exe.h"
#endif

/**
 * Pause for the specified amount of time.
 * \param milliseconds Number of milliseconds to pause.
 * \param curr The library to use.
 * \param is_note Whether the pause is for a note (1) or for silence (0).
 */
void
imyp_pause (
#ifdef IMYP_ANSIC
	const int milliseconds, const IMYP_CURR_LIB curr, const int is_note
# ifndef IMYP_HAVE_MIDI
	IMYP_ATTR ((unused))
# endif
	)
#else
	milliseconds, curr, is_note)
	const int milliseconds;
	const IMYP_CURR_LIB curr;
	const int is_note
# ifndef IMYP_HAVE_MIDI
	IMYP_ATTR ((unused))
# endif
	;
#endif
{
	if ( curr == CURR_ALLEGRO )
	{
#ifdef IMYP_HAVE_ALLEGRO
		imyp_all_pause (milliseconds);
#endif
	}
	else if ( curr == CURR_SDL )
	{
#ifdef IMYP_HAVE_SDL
		imyp_sdl_pause (milliseconds);
#endif
	}
	else if ( curr == CURR_ALSA )
	{
#ifdef IMYP_HAVE_ALSA
		imyp_alsa_pause (milliseconds);
#endif
	}
	else if ( curr == CURR_OSS )
	{
#ifdef IMYP_HAVE_OSS
		imyp_oss_pause (milliseconds);
#endif
	}
	else if ( curr == CURR_LIBAO )
	{
#ifdef IMYP_HAVE_LIBAO
		imyp_ao_pause (milliseconds);
#endif
	}
	else if ( curr == CURR_PORTAUDIO )
	{
#ifdef IMYP_HAVE_PORTAUDIO
		imyp_portaudio_pause (milliseconds);
#endif
	}
	else if ( curr == CURR_JACK )
	{
#ifdef IMYP_HAVE_JACK
		imyp_jack_pause (milliseconds);
#endif
	}
	else if ( curr == CURR_PULSEAUDIO )
	{
#ifdef IMYP_HAVE_PULSEAUDIO
		imyp_pulse_pause (milliseconds);
#endif
	}
	else if ( curr == CURR_MIDI )
	{
#ifdef IMYP_HAVE_MIDI
		imyp_midi_pause (milliseconds, is_note);
#endif
	}
	else if ( curr == CURR_EXEC )
	{
#ifdef IMYP_HAVE_EXEC
		imyp_exec_pause (milliseconds);
#endif
	}
}

/**
 * Output the given text.
 * \param text The text to output.
 * \param curr The library to use.
 */
void
imyp_put_text (
#ifdef IMYP_ANSIC
	const char * const text, const IMYP_CURR_LIB curr)
#else
	text, curr)
	const char * const text;
	const IMYP_CURR_LIB curr;
#endif
{
	if ( curr == CURR_ALLEGRO )
	{
#ifdef IMYP_HAVE_ALLEGRO
		imyp_all_put_text (text);
#endif
	}
	else if ( curr == CURR_SDL )
	{
#ifdef IMYP_HAVE_SDL
		imyp_sdl_put_text (text);
#endif
	}
	else if ( curr == CURR_ALSA )
	{
#ifdef IMYP_HAVE_ALSA
		imyp_alsa_put_text (text);
#endif
	}
	else if ( curr == CURR_OSS )
	{
#ifdef IMYP_HAVE_OSS
		imyp_oss_put_text (text);
#endif
	}
	else if ( curr == CURR_LIBAO )
	{
#ifdef IMYP_HAVE_LIBAO
		imyp_ao_put_text (text);
#endif
	}
	else if ( curr == CURR_PORTAUDIO )
	{
#ifdef IMYP_HAVE_PORTAUDIO
		imyp_portaudio_put_text (text);
#endif
	}
	else if ( curr == CURR_JACK )
	{
#ifdef IMYP_HAVE_JACK
		imyp_jack_put_text (text);
#endif
	}
	else if ( curr == CURR_PULSEAUDIO )
	{
#ifdef IMYP_HAVE_PULSEAUDIO
		imyp_pulse_put_text (text);
#endif
	}
	else if ( curr == CURR_MIDI )
	{
#ifdef IMYP_HAVE_MIDI
		imyp_midi_put_text (text);
#endif
	}
	else if ( curr == CURR_EXEC )
	{
#ifdef IMYP_HAVE_EXEC
		imyp_exec_put_text (text);
#endif
	}
}

/**
 * Play a specified tone.
 * \param freq The frequency of the tone (in Hz).
 * \param volume_level Volume of the tone (from 0 to 15).
 * \param duration The duration of the tone, in milliseconds.
 * \param buf The buffer to use.
 * \param bufsize The size of the buffer to use.
 * \param curr The library to use.
 * \return 0 on success.
 */
int
imyp_play_tune (
#ifdef IMYP_ANSIC
	const double freq,
	const int volume_level,
	const int duration,
	void * const buf,
	int bufsize,
	const IMYP_CURR_LIB curr)
#else
	freq, volume_level, duration, buf, bufsize, curr)
	const double freq;
	const int volume_level;
	const int duration;
	void * const buf;
	int bufsize;
	const IMYP_CURR_LIB curr;
#endif
{
	if ( curr == CURR_ALLEGRO )
	{
#ifdef IMYP_HAVE_ALLEGRO
		return imyp_all_play_tune (freq, volume_level, duration, buf, bufsize);
#endif
	}
	else if ( curr == CURR_SDL )
	{
#ifdef IMYP_HAVE_SDL
		return imyp_sdl_play_tune (freq, volume_level, duration, buf, bufsize);
#endif
	}
	else if ( curr == CURR_ALSA )
	{
#ifdef IMYP_HAVE_ALSA
		return imyp_alsa_play_tune (freq, volume_level, duration, buf, bufsize);
#endif
	}
	else if ( curr == CURR_OSS )
	{
#ifdef IMYP_HAVE_OSS
		return imyp_oss_play_tune (freq, volume_level, duration, buf, bufsize);
#endif
	}
	else if ( curr == CURR_LIBAO )
	{
#ifdef IMYP_HAVE_LIBAO
		return imyp_ao_play_tune (freq, volume_level, duration, buf, bufsize);
#endif
	}
	else if ( curr == CURR_PORTAUDIO )
	{
#ifdef IMYP_HAVE_PORTAUDIO
		return imyp_portaudio_play_tune (freq, volume_level, duration, buf, bufsize);
#endif
	}
	else if ( curr == CURR_JACK )
	{
#ifdef IMYP_HAVE_JACK
		return imyp_jack_play_tune (freq, volume_level, duration, buf, bufsize);
#endif
	}
	else if ( curr == CURR_PULSEAUDIO )
	{
#ifdef IMYP_HAVE_PULSEAUDIO
		return imyp_pulse_play_tune (freq, volume_level, duration, buf, bufsize);
#endif
	}
	else if ( curr == CURR_MIDI )
	{
#ifdef IMYP_HAVE_MIDI
		return imyp_midi_play_tune (freq, volume_level, duration, buf, bufsize);
#endif
	}
	else if ( curr == CURR_EXEC )
	{
#ifdef IMYP_HAVE_EXEC
		return imyp_exec_play_tune (freq, volume_level, duration, buf, bufsize);
#endif
	}
	return -1;
}

/**
 * Initializes the library.
 * \param curr Will get the initialized library type.
 * \param want_midi Non-zero if MIDI writing is required.
 * \return 0 on success.
 */
int
imyp_lib_init (
#ifdef IMYP_ANSIC
	IMYP_CURR_LIB * const curr, const int want_midi
# ifndef IMYP_HAVE_MIDI
	IMYP_ATTR ((unused))
# endif
	, const char * const filename, const int want_exec
# ifndef IMYP_HAVE_EXEC
	IMYP_ATTR ((unused))
# endif
	)
#else
	curr, want_midi, filename, want_exec)
	IMYP_CURR_LIB * const curr;
	const int want_midi
# ifndef IMYP_HAVE_MIDI
	IMYP_ATTR ((unused))
# endif
	;
	const char * const filename;
	const int want_exec
# ifndef IMYP_HAVE_EXEC
	IMYP_ATTR ((unused))
# endif
	;
#endif
{
	int res = -1;
	if ( curr == NULL ) return res;
	*curr = CURR_NONE;
#ifdef IMYP_HAVE_MIDI
	if ( want_midi != 0 )
	{
		res = imyp_midi_init (filename);
		if ( res == 0 ) *curr = CURR_MIDI;
		return res;
	}
#endif
#ifdef IMYP_HAVE_EXEC
	if ( want_exec != 0 )
	{
		res = imyp_exec_init (filename);
		if ( res == 0 ) *curr = CURR_EXEC;
		return res;
	}
#endif
	/* first try the general, portable libraries */
#ifdef IMYP_HAVE_ALLEGRO
	res = imyp_all_init ();
	if ( res == 0 )
	{
		*curr = CURR_ALLEGRO;
		return res;
	}
#endif
#ifdef IMYP_HAVE_LIBAO
	res = imyp_ao_init (filename);
	if ( res == 0 )
	{
		*curr = CURR_LIBAO;
		return res;
	}
#endif
#ifdef IMYP_HAVE_PORTAUDIO
	res = imyp_portaudio_init (filename);
	if ( res == 0 )
	{
		*curr = CURR_PORTAUDIO;
		return res;
	}
#endif
#ifdef IMYP_HAVE_SDL
	res = imyp_sdl_init ();
	if ( res == 0 )
	{
		*curr = CURR_SDL;
		return res;
	}
#endif
#ifdef IMYP_HAVE_JACK
	res = imyp_jack_init (filename);
	if ( res == 0 )
	{
		*curr = CURR_JACK;
		return res;
	}
#endif
#ifdef IMYP_HAVE_PULSEAUDIO
	res = imyp_pulse_init (filename);
	if ( res == 0 )
	{
		*curr = CURR_PULSEAUDIO;
		return res;
	}
#endif
	/* now try the less-portable sound systems */
#ifdef IMYP_HAVE_ALSA
	res = imyp_alsa_init (filename);
	if ( res == 0 )
	{
		*curr = CURR_ALSA;
		return res;
	}
#endif
#ifdef IMYP_HAVE_OSS
	res = imyp_oss_init (filename);
	if ( res == 0 )
	{
		*curr = CURR_OSS;
		return res;
	}
#endif
	return res;
}

/**
 * Closes the library.
 * \param curr The library to use.
 * \return 0 on success.
 */
int
imyp_lib_close (
#ifdef IMYP_ANSIC
	const IMYP_CURR_LIB curr)
#else
	curr)
	const IMYP_CURR_LIB curr;
#endif
{
	if ( curr == CURR_ALLEGRO )
	{
#ifdef IMYP_HAVE_ALLEGRO
		return imyp_all_close ();
#endif
	}
	else if ( curr == CURR_SDL )
	{
#ifdef IMYP_HAVE_SDL
		return imyp_sdl_close ();
#endif
	}
	else if ( curr == CURR_ALSA )
	{
#ifdef IMYP_HAVE_ALSA
		return imyp_alsa_close ();
#endif
	}
	else if ( curr == CURR_OSS )
	{
#ifdef IMYP_HAVE_OSS
		return imyp_oss_close ();
#endif
	}
	else if ( curr == CURR_LIBAO )
	{
#ifdef IMYP_HAVE_LIBAO
		return imyp_ao_close ();
#endif
	}
	else if ( curr == CURR_PORTAUDIO )
	{
#ifdef IMYP_HAVE_PORTAUDIO
		return imyp_portaudio_close ();
#endif
	}
	else if ( curr == CURR_JACK )
	{
#ifdef IMYP_HAVE_JACK
		return imyp_jack_close ();
#endif
	}
	else if ( curr == CURR_PULSEAUDIO )
	{
#ifdef IMYP_HAVE_PULSEAUDIO
		return imyp_pulse_close ();
#endif
	}
	else if ( curr == CURR_MIDI )
	{
#ifdef IMYP_HAVE_MIDI
		return imyp_midi_close ();
#endif
	}
	else if ( curr == CURR_EXEC )
	{
#ifdef IMYP_HAVE_EXEC
		return imyp_exec_close ();
#endif
	}
	return -1;
}

