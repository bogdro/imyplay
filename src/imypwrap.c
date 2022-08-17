/*
 * A program for playing iMelody ringtones (IMY files).
 *	-- wrapper functions between the main program and the backends.
 *
 * Copyright (C) 2009 Bogdan Drozdowski, bogdandr (at) op.pl
 * License: GNU General Public License, v3+
 *
 * Syntax example: imyplayer ringtone.imy
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
#include "imyp_all.h"
#include "imyp_mid.h"

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
#ifdef HAVE_LIBALLEG
		imyp_all_pause (milliseconds);
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
#ifdef HAVE_LIBALLEG
		imyp_all_put_text (text);
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
#ifdef HAVE_LIBALLEG
		return imyp_all_play_tune (freq, volume_level, duration, buf, bufsize);
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
#ifdef HAVE_LIBALLEG
	res = imyp_all_init ();
	if ( res == 0 ) *curr = CURR_ALLEGRO;
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
#ifdef HAVE_LIBALLEG
		return imyp_all_close ();
#endif
	}
	else if ( curr == CURR_MIDI )
	{
		return imyp_midi_close ();
	}
	return -1;
}

