/*
 * A program for playing iMelody ringtones (IMY files).
 *	-- ALSA backend, header file.
 *
 * Copyright (C) 2009-2013 Bogdan Drozdowski, bogdandr (at) op.pl
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

#ifndef IMYP_ALSA
# define IMYP_ALSA 1

# include "imyplay.h"

extern void imyp_alsa_pause PARAMS ((imyp_backend_data_t * const imyp_data,
	const int milliseconds));
extern void imyp_alsa_put_text PARAMS ((imyp_backend_data_t * const imyp_data,
	const char * const text));
extern int imyp_alsa_play_tune PARAMS ((imyp_backend_data_t * const imyp_data,
	const double freq, const int volume_level,
	const int duration, void * const buf, int bufsize));
extern int imyp_alsa_init PARAMS ((imyp_backend_data_t ** const imyp_data,
	const char * const dev_file));
extern int imyp_alsa_close PARAMS ((imyp_backend_data_t * const imyp_data));
extern void imyp_alsa_version PARAMS ((imyp_backend_data_t * const imyp_data));

#endif /* IMYP_ALSA */
