/*
 * A program for playing iMelody ringtones (IMY files).
 *	-- melody parsing, header file.
 *
 * Copyright (C) 2009-2011 Bogdan Drozdowski, bogdandr (at) op.pl
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

#ifndef IMYP_PARSE_H
# define IMYP_PARSE_H 1

# include "imyplay.h"	/* PARAMS() */

extern int imyp_play_file PARAMS ((const char * const file_name, const IMYP_CURR_LIB curr));

#endif /* IMYP_PARSE_H */