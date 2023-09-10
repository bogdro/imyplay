/*
 * IMYplay - A program for playing iMelody ringtones (IMY files).
 *	-- utility functions, header file.
 *
 * Copyright (C) 2012-2023 Bogdan Drozdowski, bogdro (at) users.sourceforge.net
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

#ifndef IMYP_UTILS
# define IMYP_UTILS 1

# include "imyplay.h"

# ifdef HAVE_STRCASECMP
#  define IMYP_STRCASECMP strcasecmp
#  ifdef HAVE_STRINGS_H
#   include <strings.h>
#  endif
#  ifdef AX_STRCASECMP_HEADER
#   include AX_STRCASECMP_HEADER
#  endif
# else
#  define IMYP_STRCASECMP imyp_compare
extern int imyp_compare IMYP_PARAMS ((const char string1[], const char string2[]));
# endif

# ifdef HAVE_MEMCPY
#  define IMYP_MEMCOPY memcpy
# else
extern void imyp_memcopy IMYP_PARAMS ((void * const dest,
	const void * const src, const size_t len));
#  define IMYP_MEMCOPY imyp_memcopy
# endif

# ifdef HAVE_MEMSET
#  define IMYP_MEMSET memset
# else
extern void imyp_mem_set IMYP_PARAMS ((void * const dest,
	const char value, const size_t len));
#  define IMYP_MEMSET imyp_mem_set
# endif

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

extern enum IMYP_CURR_LIB imyp_parse_system IMYP_PARAMS ((const char system_name[]));
extern enum IMYP_SAMPLE_FORMATS imyp_get_format IMYP_PARAMS ((const char string[]));
extern int imyp_generate_samples IMYP_PARAMS ((
	const double freq,
	const int volume_level,
	const int duration,
	void * const buf,
	const int bufsize,
	const int is_le,
	const int is_uns,
	const unsigned int samp_quality,
	const unsigned int samp_rate,
	unsigned long int * const start_index));
extern void imyp_pause_select IMYP_PARAMS ((const int milliseconds));
extern void imyp_put_text_stdout IMYP_PARAMS ((const char * const text));
extern char * imyp_generate_filename IMYP_PARAMS ((
	const char * const filename,
	const char * const ext));

#endif /* IMYP_UTILS */
