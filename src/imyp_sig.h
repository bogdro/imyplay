/*
 * IMYplay - A program for playing iMelody ringtones (IMY files).
 *	-- signal handling, header file.
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

#ifndef IMYP_SIGNAL
# define IMYP_SIGNAL 1

# include "imyplay.h"

extern volatile sig_atomic_t imyp_sig_recvd;	/* non-zero after signal received */

extern void imyp_set_sigh IMYP_PARAMS ((imyp_error_type * const error));

#endif /* IMYP_SIGNAL */
