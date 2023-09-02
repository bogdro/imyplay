/*
 * A program for playing iMelody ringtones (IMY files).
 *	-- parsers' utility functions.
 *
 * Copyright (C) 2014-2023 Bogdan Drozdowski, bogdro (at) users.sourceforge.net
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

#ifndef IMYP_PARSERS_COMMON
# define IMYP_PARSERS_COMMON 1

# include "imyplay.h"

/* internationalized error messages: */
extern const char * const err_unkn_token;
extern const char * const err_at_pos;
extern const char * const err_unex_token;
extern const char * const err_file_open;
extern const char * const err_parse_beat;
extern const char * const err_parse_style;
extern const char * const err_parse_vol;
extern const char * const err_bad_file;
extern const char * const err_play_tune;

/* The frequency multiplication coefficient between each two closest notes: 2^(1/12) */
# define IMYP_C (1.05946309435929531f)

# define IMYP_A0 (55.0f)
# define IMYP_A1 (IMYP_A0 * 2.0f)
# define IMYP_A2 (IMYP_A1 * 2.0f)
# define IMYP_A3 (IMYP_A2 * 2.0f)
# define IMYP_A4 (IMYP_A3 * 2.0f)
# define IMYP_A5 (IMYP_A4 * 2.0f)
# define IMYP_A6 (IMYP_A5 * 2.0f)
# define IMYP_A7 (IMYP_A6 * 2.0f)
# define IMYP_A8 (IMYP_A7 * 2.0f)

# define IMYP_OCTAVES 9
# define IMYP_NOTES_PER_OCTAVE 12
extern const float imyp_notes[IMYP_OCTAVES][IMYP_NOTES_PER_OCTAVE];

extern int imyp_get_rest_time IMYP_PARAMS ((
	const int last_note_duration,
	const int current_style));

#endif /* IMYP_PARSERS_COMMON */
