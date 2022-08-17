/*
 * A program for playing iMelody ringtones (IMY files).
 *	-- wrapper functions between the main program and the backends,
 *		header file.
 *
 * Copyright (C) 2009-2019 Bogdan Drozdowski, bogdandr (at) op.pl
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

#ifndef IMYP_WRAPPERS
# define IMYP_WRAPPERS 1

# include "imyplay.h"

extern void imyp_pause IMYP_PARAMS ((const int milliseconds, imyp_backend_t * const curr,
	const int is_note, void * const buf, int bufsize));
extern void imyp_put_text IMYP_PARAMS ((const char * const text, imyp_backend_t * const curr));
extern int imyp_play_tune IMYP_PARAMS ((const double freq, const int volume_level,
	const int duration, void * const buf, int bufsize, imyp_backend_t * const curr));
extern int imyp_lib_init IMYP_PARAMS ((imyp_backend_t * const curr, const int want_midi,
	const char * const filename, const int want_exec, const int midi_instrument,
	const int want_file, const char * const out_file));
extern int imyp_init_selected IMYP_PARAMS ((imyp_backend_t * const curr,
	const char output_system[], const char * const filename,
	const int midi_instrument, const char * const out_file));
extern int imyp_lib_close IMYP_PARAMS ((imyp_backend_t * const curr));
extern void imyp_report_versions IMYP_PARAMS ((imyp_backend_t * const curr));

#endif /* IMYP_WRAPPERS */
