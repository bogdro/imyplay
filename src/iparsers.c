/*
 * A program for playing iMelody ringtones (IMY files).
 *	-- parsers' utility functions.
 *
 * Copyright (C) 2014-2016 Bogdan Drozdowski, bogdandr (at) op.pl
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
		(float) IMYP_A0/(IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		(float) IMYP_A0/(IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		(float) IMYP_A0/(IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		(float) IMYP_A0/(IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		(float) IMYP_A0/(IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		(float) IMYP_A0/(IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		(float) IMYP_A0/(IMYP_C*IMYP_C*IMYP_C),
		(float) IMYP_A0/(IMYP_C*IMYP_C),
		(float) IMYP_A0/IMYP_C,
		(float) IMYP_A0,
		(float) IMYP_A0*IMYP_C,
		(float) IMYP_A0*IMYP_C*IMYP_C
	},
	{
		(float) IMYP_A1/(IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		(float) IMYP_A1/(IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		(float) IMYP_A1/(IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		(float) IMYP_A1/(IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		(float) IMYP_A1/(IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		(float) IMYP_A1/(IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		(float) IMYP_A1/(IMYP_C*IMYP_C*IMYP_C),
		(float) IMYP_A1/(IMYP_C*IMYP_C),
		(float) IMYP_A1/IMYP_C,
		(float) IMYP_A1,
		(float) IMYP_A1*IMYP_C,
		(float) IMYP_A1*IMYP_C*IMYP_C
	},
	{
		(float) IMYP_A2/(IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		(float) IMYP_A2/(IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		(float) IMYP_A2/(IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		(float) IMYP_A2/(IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		(float) IMYP_A2/(IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		(float) IMYP_A2/(IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		(float) IMYP_A2/(IMYP_C*IMYP_C*IMYP_C),
		(float) IMYP_A2/(IMYP_C*IMYP_C),
		(float) IMYP_A2/IMYP_C,
		(float) IMYP_A2,
		(float) IMYP_A2*IMYP_C,
		(float) IMYP_A2*IMYP_C*IMYP_C
	},
	{
		(float) IMYP_A3/(IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		(float) IMYP_A3/(IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		(float) IMYP_A3/(IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		(float) IMYP_A3/(IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		(float) IMYP_A3/(IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		(float) IMYP_A3/(IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		(float) IMYP_A3/(IMYP_C*IMYP_C*IMYP_C),
		(float) IMYP_A3/(IMYP_C*IMYP_C),
		(float) IMYP_A3/IMYP_C,
		(float) IMYP_A3,
		(float) IMYP_A3*IMYP_C,
		(float) IMYP_A3*IMYP_C*IMYP_C
	},
	{
		(float) IMYP_A4/(IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		(float) IMYP_A4/(IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		(float) IMYP_A4/(IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		(float) IMYP_A4/(IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		(float) IMYP_A4/(IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		(float) IMYP_A4/(IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		(float) IMYP_A4/(IMYP_C*IMYP_C*IMYP_C),
		(float) IMYP_A4/(IMYP_C*IMYP_C),
		(float) IMYP_A4/IMYP_C,
		(float) IMYP_A4,
		(float) IMYP_A4*IMYP_C,
		(float) IMYP_A4*IMYP_C*IMYP_C
	},
	{
		(float) IMYP_A5/(IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		(float) IMYP_A5/(IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		(float) IMYP_A5/(IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		(float) IMYP_A5/(IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		(float) IMYP_A5/(IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		(float) IMYP_A5/(IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		(float) IMYP_A5/(IMYP_C*IMYP_C*IMYP_C),
		(float) IMYP_A5/(IMYP_C*IMYP_C),
		(float) IMYP_A5/IMYP_C,
		(float) IMYP_A5,
		(float) IMYP_A5*IMYP_C,
		(float) IMYP_A5*IMYP_C*IMYP_C
	},
	{
		(float) IMYP_A6/(IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		(float) IMYP_A6/(IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		(float) IMYP_A6/(IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		(float) IMYP_A6/(IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		(float) IMYP_A6/(IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		(float) IMYP_A6/(IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		(float) IMYP_A6/(IMYP_C*IMYP_C*IMYP_C),
		(float) IMYP_A6/(IMYP_C*IMYP_C),
		(float) IMYP_A6/IMYP_C,
		(float) IMYP_A6,
		(float) IMYP_A6*IMYP_C,
		(float) IMYP_A6*IMYP_C*IMYP_C
	},
	{
		(float) IMYP_A7/(IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		(float) IMYP_A7/(IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		(float) IMYP_A7/(IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		(float) IMYP_A7/(IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		(float) IMYP_A7/(IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		(float) IMYP_A7/(IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		(float) IMYP_A7/(IMYP_C*IMYP_C*IMYP_C),
		(float) IMYP_A7/(IMYP_C*IMYP_C),
		(float) IMYP_A7/IMYP_C,
		(float) IMYP_A7,
		(float) IMYP_A7*IMYP_C,
		(float) IMYP_A7*IMYP_C*IMYP_C
	},
	{
		(float) IMYP_A8/(IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		(float) IMYP_A8/(IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		(float) IMYP_A8/(IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		(float) IMYP_A8/(IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		(float) IMYP_A8/(IMYP_C*IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		(float) IMYP_A8/(IMYP_C*IMYP_C*IMYP_C*IMYP_C),
		(float) IMYP_A8/(IMYP_C*IMYP_C*IMYP_C),
		(float) IMYP_A8/(IMYP_C*IMYP_C),
		(float) IMYP_A8/IMYP_C,
		(float) IMYP_A8,
		(float) IMYP_A8*IMYP_C,
		(float) IMYP_A8*IMYP_C*IMYP_C
	}
};

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

