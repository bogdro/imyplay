/*
 * IMYplay - A program for playing iMelody ringtones (IMY files).
 *	-- MIDI writer backend, header file.
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

#ifndef IMYP_MIDIW
# define IMYP_MIDIW 1

# include "imyplay.h"

extern void imyp_midi_pause IMYP_PARAMS ((imyp_backend_data_t * const imyp_data,
	const int milliseconds, const int is_note));
extern void imyp_midi_put_text IMYP_PARAMS ((imyp_backend_data_t * const imyp_data,
	const char * const text));
extern int imyp_midi_play_tune IMYP_PARAMS ((imyp_backend_data_t * const imyp_data,
	const double freq, const int volume_level,
	const int duration, const void * const buf, int bufsize));
extern int imyp_midi_init IMYP_PARAMS ((imyp_backend_data_t ** const imyp_data,
	const char * const filename, const int instrument));
extern int imyp_midi_close IMYP_PARAMS ((imyp_backend_data_t * const imyp_data));
extern void imyp_midi_version IMYP_PARAMS ((const imyp_backend_data_t * const imyp_data));

#endif /* IMYP_MIDIW */
