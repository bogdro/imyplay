/*
 * A program for playing iMelody ringtones (IMY files).
 *	-- wrapper functions between the main program and the backends.
 *
 * Copyright (C) 2009 Bogdan Drozdowski, bogdandr (at) op.pl
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
#include "imyp_mid.h"

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

#ifdef IMYP_HAVE_JACK1
# include "imyp_jck.h"
#endif

/**
 * Pause for the specified amount of time.
 * \param milliseconds Number of milliseconds to pause.
 * \param curr The library to use.
 * \param is_note Whether the pause is for a note (1) or for silence (0).
 */
void
imyp_pause (
#if defined (__STDC__) || defined (_AIX) \
	|| (defined (__mips) && defined (_SYSTYPE_SVR4)) \
	|| defined(WIN32) || defined(__cplusplus)
	const int milliseconds, const CURR_LIB curr, const int is_note)
#else
	milliseconds, curr, is_note)
	const int milliseconds;
	const CURR_LIB curr;
	const int is_note;
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
	else if ( curr == CURR_JACK1 )
	{
#ifdef IMYP_HAVE_JACK1
		imyp_jack1_pause (milliseconds);
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
		imyp_midi_pause (milliseconds, is_note);
	}
}

/**
 * Output the given text.
 * \param text The text to output.
 * \param curr The library to use.
 */
void
imyp_put_text (
#if defined (__STDC__) || defined (_AIX) \
	|| (defined (__mips) && defined (_SYSTYPE_SVR4)) \
	|| defined(WIN32) || defined(__cplusplus)
	const char * const text, const CURR_LIB curr)
#else
	text, curr)
	const char * const text;
	const CURR_LIB curr;
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
	else if ( curr == CURR_JACK1 )
	{
#ifdef IMYP_HAVE_JACK1
		imyp_jack1_put_text (text);
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
		imyp_midi_put_text (text);
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
#if defined (__STDC__) || defined (_AIX) \
	|| (defined (__mips) && defined (_SYSTYPE_SVR4)) \
	|| defined(WIN32) || defined(__cplusplus)
	const double freq,
	const int volume_level,
	const int duration,
	void * const buf,
	int bufsize,
	const CURR_LIB curr)
#else
	freq, volume_level, duration, buf, bufsize, curr)
	const double freq;
	const int volume_level;
	const int duration;
	void * const buf;
	int bufsize;
	const CURR_LIB curr;
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
	else if ( curr == CURR_JACK1 )
	{
#ifdef IMYP_HAVE_JACK1
		return imyp_jack1_play_tune (freq, volume_level, duration, buf, bufsize);
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
		return imyp_midi_play_tune (freq, volume_level, duration, buf, bufsize);
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
#if defined (__STDC__) || defined (_AIX) \
	|| (defined (__mips) && defined (_SYSTYPE_SVR4)) \
	|| defined(WIN32) || defined(__cplusplus)
	CURR_LIB * const curr, const int want_midi, const char * const filename)
#else
	curr, want_midi, filename)
	CURR_LIB * const curr;
	const int want_midi;
	const char * const filename;
#endif
{
	int res = -1;
	if ( curr == NULL ) return res;
	*curr = CURR_NONE;
	if ( want_midi != 0 )
	{
		res = imyp_midi_init (filename);
		if ( res == 0 ) *curr = CURR_MIDI;
		return res;
	}
	/* first try the general, portable libraries */
#ifdef IMYP_HAVE_ALLEGRO
	res = imyp_all_init ();
	if ( res == 0 )
	{
		*curr = CURR_ALLEGRO;
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
#ifdef IMYP_HAVE_JACK1
	res = imyp_jack1_init (filename);
	if ( res == 0 )
	{
		*curr = CURR_JACK1;
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
#if defined (__STDC__) || defined (_AIX) \
	|| (defined (__mips) && defined (_SYSTYPE_SVR4)) \
	|| defined(WIN32) || defined(__cplusplus)
	const CURR_LIB curr)
#else
	curr)
	const CURR_LIB curr;
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
	else if ( curr == CURR_JACK1 )
	{
#ifdef IMYP_HAVE_JACK1
		return imyp_jack1_close ();
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
		return imyp_midi_close ();
	}
	return -1;
}

