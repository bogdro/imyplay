/*
 * A program for playing iMelody ringtones (IMY files).
 *	-- utility functions, header file.
 *
 * Copyright (C) 2011 Bogdan Drozdowski, bogdandr (at) op.pl
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

#ifndef IMYP_UTILS
# define IMYP_UTILS 1

# include "imyplay.h"

enum IMYP_SAMPLE_FORMATS
{
	IMYP_SAMPLE_FORMAT_UNKNOWN,
	IMYP_SAMPLE_FORMAT_S16LE,
	IMYP_SAMPLE_FORMAT_S16BE,
	IMYP_SAMPLE_FORMAT_U16LE,
	IMYP_SAMPLE_FORMAT_U16BE,
	IMYP_SAMPLE_FORMAT_S8LE,
	IMYP_SAMPLE_FORMAT_S8BE,
	IMYP_SAMPLE_FORMAT_U8LE,
	IMYP_SAMPLE_FORMAT_U8BE
};

extern int imyp_compare PARAMS ((const char string1[], const char string2[]));
extern IMYP_CURR_LIB imyp_parse_system PARAMS ((const char system_name[]));
extern enum IMYP_SAMPLE_FORMATS imyp_get_format PARAMS ((const char string[]));
extern int imyp_generate_samples PARAMS ((const double freq, const int volume_level,
	const int duration, void * const buf, int bufsize, const int is_le,
	const int is_uns, unsigned int quality, const unsigned int samp_rate));

#endif /* IMYP_UTILS */
