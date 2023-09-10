/*
 * IMYplay - A program for playing iMelody ringtones (IMY files).
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

#include "imyp_cfg.h"

#ifdef HAVE_LIBINTL_H
# include <libintl.h>	/* translation stuff */
#endif

#ifdef HAVE_LOCALE_H
# include <locale.h>
#endif

#include "imyplay.h"
#include "iparsers.h"

/* internationalized error messages: */
const char * const err_unkn_token  = N_("Unknown token");
const char * const err_at_pos      = N_("at position");
const char * const err_unex_token  = N_("Unexpected token");
const char * const err_file_open   = N_("Can't open file");
const char * const err_parse_beat  = N_("Problem parsing beat");
const char * const err_parse_style = N_("Problem parsing style");
const char * const err_parse_vol   = N_("Problem parsing volume");
const char * const err_bad_file    = N_("Not iMelody file");
const char * const err_play_tune   = N_("Error playing tune");

const float imyp_notes[IMYP_OCTAVES][IMYP_NOTES_PER_OCTAVE] =
{
	{
		IMYP_A0/(IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		IMYP_A0/(IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		IMYP_A0/(IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		IMYP_A0/(IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		IMYP_A0/(IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		IMYP_A0/(IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		IMYP_A0/(IMYP_C*IMYP_C*IMYP_C),
		IMYP_A0/(IMYP_C*IMYP_C),
		IMYP_A0/IMYP_C,
		IMYP_A0,
		IMYP_A0*IMYP_C,
		IMYP_A0*IMYP_C*IMYP_C
	},
	{
		IMYP_A1/(IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		IMYP_A1/(IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		IMYP_A1/(IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		IMYP_A1/(IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		IMYP_A1/(IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		IMYP_A1/(IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		IMYP_A1/(IMYP_C*IMYP_C*IMYP_C),
		IMYP_A1/(IMYP_C*IMYP_C),
		IMYP_A1/IMYP_C,
		IMYP_A1,
		IMYP_A1*IMYP_C,
		IMYP_A1*IMYP_C*IMYP_C
	},
	{
		IMYP_A2/(IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		IMYP_A2/(IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		IMYP_A2/(IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		IMYP_A2/(IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		IMYP_A2/(IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		IMYP_A2/(IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		IMYP_A2/(IMYP_C*IMYP_C*IMYP_C),
		IMYP_A2/(IMYP_C*IMYP_C),
		IMYP_A2/IMYP_C,
		IMYP_A2,
		IMYP_A2*IMYP_C,
		IMYP_A2*IMYP_C*IMYP_C
	},
	{
		IMYP_A3/(IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		IMYP_A3/(IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		IMYP_A3/(IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		IMYP_A3/(IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		IMYP_A3/(IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		IMYP_A3/(IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		IMYP_A3/(IMYP_C*IMYP_C*IMYP_C),
		IMYP_A3/(IMYP_C*IMYP_C),
		IMYP_A3/IMYP_C,
		IMYP_A3,
		IMYP_A3*IMYP_C,
		IMYP_A3*IMYP_C*IMYP_C
	},
	{
		IMYP_A4/(IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		IMYP_A4/(IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		IMYP_A4/(IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		IMYP_A4/(IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		IMYP_A4/(IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		IMYP_A4/(IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		IMYP_A4/(IMYP_C*IMYP_C*IMYP_C),
		IMYP_A4/(IMYP_C*IMYP_C),
		IMYP_A4/IMYP_C,
		IMYP_A4,
		IMYP_A4*IMYP_C,
		IMYP_A4*IMYP_C*IMYP_C
	},
	{
		IMYP_A5/(IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		IMYP_A5/(IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		IMYP_A5/(IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		IMYP_A5/(IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		IMYP_A5/(IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		IMYP_A5/(IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		IMYP_A5/(IMYP_C*IMYP_C*IMYP_C),
		IMYP_A5/(IMYP_C*IMYP_C),
		IMYP_A5/IMYP_C,
		IMYP_A5,
		IMYP_A5*IMYP_C,
		IMYP_A5*IMYP_C*IMYP_C
	},
	{
		IMYP_A6/(IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		IMYP_A6/(IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		IMYP_A6/(IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		IMYP_A6/(IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		IMYP_A6/(IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		IMYP_A6/(IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		IMYP_A6/(IMYP_C*IMYP_C*IMYP_C),
		IMYP_A6/(IMYP_C*IMYP_C),
		IMYP_A6/IMYP_C,
		IMYP_A6,
		IMYP_A6*IMYP_C,
		IMYP_A6*IMYP_C*IMYP_C
	},
	{
		IMYP_A7/(IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		IMYP_A7/(IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		IMYP_A7/(IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		IMYP_A7/(IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		IMYP_A7/(IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		IMYP_A7/(IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		IMYP_A7/(IMYP_C*IMYP_C*IMYP_C),
		IMYP_A7/(IMYP_C*IMYP_C),
		IMYP_A7/IMYP_C,
		IMYP_A7,
		IMYP_A7*IMYP_C,
		IMYP_A7*IMYP_C*IMYP_C
	},
	{
		IMYP_A8/(IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		IMYP_A8/(IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		IMYP_A8/(IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		IMYP_A8/(IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		IMYP_A8/(IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		IMYP_A8/(IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		IMYP_A8/(IMYP_C*IMYP_C*IMYP_C),
		IMYP_A8/(IMYP_C*IMYP_C),
		IMYP_A8/IMYP_C,
		IMYP_A8,
		IMYP_A8*IMYP_C,
		IMYP_A8*IMYP_C*IMYP_C
	}
};

#ifdef TEST_COMPILE
# undef IMYP_ANSIC
# if TEST_COMPILE > 1
#  undef HAVE_MALLOC
# endif
#endif

/* ======================================================================== */

/**
 * Gets the pause time after the last note.
 * \param last_note_duration The duration of the last note played, in milliseconds.
 * \param current_style Playing style.
 * \return a duration for the pause between imyp_notes, in milliseconds.
 */
int
imyp_get_rest_time (
#ifdef IMYP_ANSIC
	const int last_note_duration,
	const int current_style)
#else
	last_note_duration, current_style)
	const int last_note_duration;
	const int current_style;
#endif
{
	if ( current_style == 0 )
	{
		return last_note_duration/20;
	}
	else if ( current_style == 1 )
	{
		return 0;
	}
	else if ( current_style == 2 )
	{
		return last_note_duration;
	}
	else
	{
		return 0;
	}
}
