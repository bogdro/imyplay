/*
 * IMYplay - A program for playing iMelody ringtones (IMY files).
 *	-- main file.
 *
 * Copyright (C) 2009-2025 Bogdan Drozdowski, bogdro (at) users.sourceforge.net
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
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "imyp_cfg.h"

#include "imyplay.h"
#include "imypwrap.h"
#include "imyp_sig.h"
#include "imyparse.h"
#include "imyputil.h"
#include "imyp_cmd.h"

#ifdef IMYP_HAVE_ALLEGRO
# include <allegro.h>	/* END_OF_MAIN() */
#endif

# ifdef __GNUC__
#  pragma GCC poison strcpy strcat
# endif

/* Allegro: */
#ifdef END_OF_MAIN
int _mangled_main (int argc, char * argv[]);
#endif

#ifndef IMYP_ANSIC
int main IMYP_PARAMS ((int argc, char* argv[]));
#endif

/* SDL: */
#ifdef __cplusplus
extern "C"
#endif
int
main (
#ifdef IMYP_ANSIC
	int argc, char* argv[] )
#else
	argc, argv )
	int argc;
	char* argv[];
#endif
{
	return imyplay_parse_cmdline(argc, argv);
}

#ifdef END_OF_MAIN
END_OF_MAIN ()
#endif
