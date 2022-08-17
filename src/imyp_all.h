/*
 * A program for playing iMelody ringtones (IMY files).
 *	-- Allegro backend, header file.
 *
 * Copyright (C) 2009 Bogdan Drozdowski, bogdandr (at) op.pl
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

#ifndef IMYP_ALLEGRO
# define IMYP_ALLEGRO 1

# include "imyplay.h"

extern void imyp_all_pause PARAMS((const int milliseconds));
extern void imyp_all_put_text PARAMS((const char * const text));
extern int imyp_all_play_tune PARAMS((const double freq, const int volume_level,
	const int duration, void * const buf, int bufsize));
extern int imyp_all_init PARAMS((void));
extern int imyp_all_close PARAMS((void));

#endif /* IMYP_ALLEGRO */
