/*
 * A program for playing iMelody ringtones (IMY files).
 *	-- wrapper functions between the main program and the backends,
 *		header file.
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

#ifndef IMYP_WRAPPERS
# define IMYP_WRAPPERS 1

# include "imyplay.h"

extern void imyp_pause PARAMS((const int milliseconds, const CURR_LIB curr, const int is_note));
extern void imyp_put_text PARAMS((const char * const text, const CURR_LIB curr));
extern int imyp_play_tune PARAMS((const double freq, const int volume_level,
	const int duration, void * const buf, int bufsize, const CURR_LIB curr));
extern int imyp_lib_init PARAMS((CURR_LIB * const curr, const int want_midi,
	const char * const filename));
extern int imyp_lib_close PARAMS((const CURR_LIB curr));

#endif /* IMYP_WRAPPERS */
