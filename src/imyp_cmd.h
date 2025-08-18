/*
 * IMYplay - A program for playing iMelody ringtones (IMY files).
 *	-- command-line parsing, header file.
 *
 * Copyright (C) 2025 Bogdan Drozdowski, bogdro (at) users.sourceforge.net
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

#ifndef _IMYPLAY_CMD_H
# define _IMYPLAY_CMD_H 1

/* define _GNU_SOURCE before any headers */
# define _GNU_SOURCE	1	/* getopt_long() */

# include "imyp_cfg.h"

# ifdef HAVE_LIBINTL_H
#  include <libintl.h>	/* translation stuff */
# endif

# ifdef HAVE_LOCALE_H
#  include <locale.h>
# endif

# ifdef HAVE_LIBGEN_H
#  include <libgen.h>	/* basename() */
# endif

extern int imyplay_parse_cmdline IMYP_PARAMS ((int argc, char* argv[]));

extern const char * imyp_progname;
extern const char * const ver_msg_compiled;
extern const char * const ver_msg_runtime;

#endif /* _IMYPLAY_CMD_H */
