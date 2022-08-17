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

#include "imyplay.h"
#include "imypwrap.h"
#include "imyp_all.h"

/**
 * Pause for the specified amount of time.
 * \param milliseconds Number of milliseconds to pause.
 */
void
imyp_pause (
#if defined (__STDC__) || defined (_AIX) \
	|| (defined (__mips) && defined (_SYSTYPE_SVR4)) \
	|| defined(WIN32) || defined(__cplusplus)
	const int milliseconds)
#else
	milliseconds)
	const int milliseconds;
#endif
{
#ifdef HAVE_LIBALLEG
	imyp_all_pause (milliseconds);
#endif
}

/**
 * Play a specified tone.
 * \param freq The frequency of the tone (in Hz).
 * \param volume_level Volume of the tone (from 0 to 15).
 * \param duration The duration of the tone, in milliseconds.
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
	int bufsize)
#else
	freq, volume_level, duration, buf, bufsize)
	const double freq;
	const int volume_level;
	const int duration;
	void * const buf;
	int bufsize;
#endif
{
#ifdef HAVE_LIBALLEG
	return imyp_all_play_tune (freq, volume_level, duration, buf, bufsize);
#else
	return -1;
#endif
}

/**
 * Initializes the library.
 * \return 0 on success.
 */
int
imyp_lib_init (
#if defined (__STDC__) || defined (_AIX) \
	|| (defined (__mips) && defined (_SYSTYPE_SVR4)) \
	|| defined(WIN32) || defined(__cplusplus)
	void
#endif
)
{
#ifdef HAVE_LIBALLEG
	return imyp_all_init ();
#else
	return -1;
#endif
}

/**
 * Closes the library.
 * \return 0 on success.
 */
int
imyp_lib_close (
#if defined (__STDC__) || defined (_AIX) \
	|| (defined (__mips) && defined (_SYSTYPE_SVR4)) \
	|| defined(WIN32) || defined(__cplusplus)
	void
#endif
)
{
#ifdef HAVE_LIBALLEG
	return imyp_all_close ();
#else
	return -1;
#endif
}

