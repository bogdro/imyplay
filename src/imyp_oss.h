/*
 * IMYplay - A program for playing iMelody ringtones (IMY files).
 *	-- OSS backend, header file.
 *
 * Copyright (C) 2009-2025 Bogdan Drozdowski, bogdro (at) users.sourceforge.net
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

#ifndef IMYP_OSS
# define IMYP_OSS 1

# include "imyplay.h"

extern void imyp_oss_pause IMYP_PARAMS ((const imyp_backend_data_t * const imyp_data,
	const int milliseconds));
extern void imyp_oss_put_text IMYP_PARAMS ((const imyp_backend_data_t * const imyp_data,
	const char * const text));
extern int imyp_oss_play_tune IMYP_PARAMS ((imyp_backend_data_t * const imyp_data,
	const double freq, const int volume_level,
	const int duration, void * const buf, int bufsize));
extern int imyp_oss_init IMYP_PARAMS ((imyp_backend_data_t ** const imyp_data,
	const char * const dev_file));
extern int imyp_oss_close IMYP_PARAMS ((imyp_backend_data_t * const imyp_data));
extern void imyp_oss_version IMYP_PARAMS ((const imyp_backend_data_t * const imyp_data));

#endif /* IMYP_OSS */
