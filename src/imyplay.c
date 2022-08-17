/*
 * A program for playing iMelody ringtones (IMY files).
 *	-- main file.
 *
 * Copyright (C) 2009 Bogdan Drozdowski, bogdandr (at) op.pl
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

#include "imyp_cfg.h"

#include "imyplay.h"

#if (defined HAVE_GETOPT_H) && (defined HAVE_GETOPT_LONG)
# include <getopt.h>
#endif

#include <stdio.h>

#ifdef HAVE_STRING_H
# if ((!defined STDC_HEADERS) || (!STDC_HEADERS)) && (defined HAVE_MEMORY_H)
#  include <memory.h>
# endif
# include <string.h>
#endif

#ifdef HAVE_LIBINTL_H
# include <libintl.h>	/* translation stuff */
#endif

#ifdef HAVE_LOCALE_H
# include <locale.h>
#endif

#ifdef HAVE_LIBGEN_H
# include <libgen.h>	/* basename() */
#endif

#ifdef HAVE_SYS_TYPES_H
# include <sys/types.h>
#endif

#include "imypwrap.h"
#include "imyp_sig.h"

# ifdef __GNUC__
#  pragma GCC poison strcpy strcat
# endif

#ifdef IMYP_HAVE_ALLEGRO
# include <allegro.h>	/* END_OF_MAIN() */
#endif

#define	PROGRAM_NAME	PACKAGE

static const char ver_str[] = N_("version");
static const char author_str[] = "Copyright (C) 2009 Bogdan 'bogdro' Drozdowski, bogdandr@op.pl\n" \
	"MIDI code: Copyright 1998-2008, Steven Goodwin (StevenGoodwin@gmail.com)";
static const char lic_str[] = N_(							\
	"Program for playing iMelody ringtones (IMY files).\n"				\
	"\nThis program is Free Software; you can redistribute it and/or"		\
	"\nmodify it under the terms of the GNU General Public License"			\
	"\nas published by the Free Software Foundation; either version 3"		\
	"\nof the License, or (at your option) any later version."			\
	"\n\nThis program is distributed in the hope that it will be useful,"		\
	"\nbut WITHOUT ANY WARRANTY; without even the implied warranty of"		\
	"\nMERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n\n");

/* Error messages explaining the stage during which an error occurred. */
const char * const err_msg                = N_("error");
const char * const err_msg_signal         = N_("Error while trying to set a signal handler for");
static const char * const err_lib_init    = N_("Error during library init.");
static const char * const err_lib_close   = N_("Error during library closing.");
/*static const char * const err_snd_init    = N_("Error during sound init.");*/
static const char * const err_unkn_token  = N_("Unknown token");
static const char * const err_at_pos      = N_("at position");
static const char * const err_unex_token  = N_("Unexpected token");
static const char * const err_file_open   = N_("Can't open file");
static const char * const err_parse_beat  = N_("Problem parsing beat");
static const char * const err_parse_style = N_("Problem parsing style");
static const char * const err_parse_vol   = N_("Problem parsing volume");
static const char * const err_bad_file    = N_("Not iMelody file");
static const char * const err_play_tune   = N_("Error playing tune");

/* Messages displayed when verbose mode is on */
/*const char * const msg_signal      = N_("Setting signal handlers");*/

/* Signal-related stuff */
#ifdef HAVE_SIGNAL_H
const char * const sig_unk = N_("unknown");
#endif /* HAVE_SIGNAL_H */

static char *imyp_progname;	/* The name of the program */

/* Command-line options. */
/* These unconditionally: */
static int opt_tomidi  = 0;
static int imyp_optind  = 0;
static int opt_dev = 0;
static char * device = NULL;
#if (defined HAVE_GETOPT_H) && (defined HAVE_GETOPT_LONG)
static int opt_help    = 0;
static int opt_license = 0;
static int opt_version = 0;

static int opt_char    = 0;

static const struct option opts[] =
{
	{ "device",     required_argument, &opt_dev,     1 },
	{ "help",       no_argument,       &opt_help,    1 },
	{ "licence",    no_argument,       &opt_license, 1 },
	{ "license",    no_argument,       &opt_license, 1 },
	{ "to-midi",    no_argument,       &opt_tomidi,  1 },
	{ "version",    no_argument,       &opt_version, 1 },
	{ NULL, 0, NULL, 0 }
};
#endif

/* The frequency multiplication coefficient between each two closest notes: 2^(1/12) */
#define IMYP_C (1.05946309435929531f)

#define IMYP_A0 (55.0f)
#define IMYP_A1 (IMYP_A0*2.0f)
#define IMYP_A2 (IMYP_A1*2.0f)
#define IMYP_A3 (IMYP_A2*2.0f)
#define IMYP_A4 (IMYP_A3*2.0f)
#define IMYP_A5 (IMYP_A4*2.0f)
#define IMYP_A6 (IMYP_A5*2.0f)
#define IMYP_A7 (IMYP_A6*2.0f)
#define IMYP_A8 (IMYP_A7*2.0f)

const float notes[9][12] =
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

#define IMYP_DEF_BPM 120

static unsigned short buf16[SAMPBUFSIZE];

static CURR_LIB current_library = CURR_NONE;

static int volume = 7;
static int style = 0;
static int bpm = IMYP_DEF_BPM;
static int octave = 4;
static char melody_line[1024+10];	/* the 10 extra to avoid overflows */
static unsigned int melody_index = 0;
static int is_sharp = 0;
static int is_flat = 0;

static int is_repeat = 0;
static unsigned int repeat_count = 0;
static unsigned int is_repeat_forever = 0;
#if (defined HAVE_FSEEKO) && (defined HAVE_FTELLO)
static off_t repeat_start_pos = 0;
#else
static long repeat_start_pos = 0;
#endif
static int is_eof = 0;

#define IMYP_TOUPPER(c) ( (c) & 0x5f )
#define IMYP_IS_DIGIT(c) ( ((c) >= '0') && ((c) <= '9') )
#define IMYP_MEL_END "END:IMELODY"

/**
 * Displays an error message.
 * \param err Error code.
 * \param msg The message.
 * \param extra Last element of the error message (fsname or signal).
 * \param FS The filesystem this message refers to.
 */
void IMYP_ATTR ((nonnull))
show_error (
#if defined (__STDC__) || defined (_AIX) \
	|| (defined (__mips) && defined (_SYSTYPE_SVR4)) \
	|| defined(WIN32) || defined(__cplusplus)
	const error_type	err,
	const char * const	msg,
	const char * const	extra )
#else
	err, msg, extra )
	const error_type	err;
	const char * const	msg;
	const char * const	extra;
#endif
{
	if ( msg == NULL ) return;

	fprintf ( stderr, "%s: %s: %d (%s) %s", imyp_progname,
		_(err_msg),
		err, _(msg),
		(extra != NULL)? extra : "" );
	fflush (stderr);
}

/* ======================================================================== */

/**
 * Prints the help screen.
 * \param my_name Program identifier, like argv[0], if available.
 */
static void IMYP_ATTR ((nonnull))
print_help (
#if defined (__STDC__) || defined (_AIX) \
	|| (defined (__mips) && defined (_SYSTYPE_SVR4)) \
	|| defined(WIN32) || defined(__cplusplus)
	const char * const my_name )
#else
	my_name )
	const char * const my_name;
#endif
{
	const char *prog;
	if ( my_name == NULL )
	{
		prog = PROGRAM_NAME;
#ifdef HAVE_STRING_H
	}
	else if ( strlen (my_name) == 0 )
	{
		prog = PROGRAM_NAME;
#endif
	}
	else
	{
		prog = my_name;
	}

	printf ("%s - %s\n\n", prog, _("A program for playing iMelody ringtones (IMY files)") );
	printf ( "%s: ", _("Syntax") );
	printf ( "%s", prog);
	printf ( "%s", _(" [options] ") );
	printf ( "ringtone.imy [...]\n\n" );
	printf ( "%s:", _("Options") );
	printf ( "\n-d|--device <dev>\t%s", _("Try to use this device") );
	printf ( "\n-h|--help\t\t%s", _("Print help") );
	printf ( "\n-l|--license\t\t%s", _("Print license information") );
	printf ( "\n--to-midi\t\t%s", _("Convert the given files to MIDI format") );
	printf ( "\n-V|--version\t\t%s\n", _("Print version number") );
}

/* ======================================================================== */

/**
 * Ensures that the data buffer has data. Reads a new line from the file if necessary.
 * \param buffer The buffer to check/fill.
 * \param buf_index Currently used index to the buffer.
 * \param bufsize The size of the whole buffer.
 * \param imyfile The file to read additional data from if necessary.
 */
static void IMYP_ATTR((nonnull))
imyp_read_line (
#if defined (__STDC__) || defined (_AIX) \
	|| (defined (__mips) && defined (_SYSTYPE_SVR4)) \
	|| defined(WIN32) || defined(__cplusplus)
	char * const buffer,
	unsigned int * const buf_index,
	const size_t bufsize,
	FILE * const imyfile)
#else
	buffer, buf_index, bufsize, imyfile)
	char * const buffer;
	unsigned int * const buf_index;
	const size_t bufsize;
	FILE * const imyfile;
#endif
{
	char * res;
	char tmpbuf[13];	/* for "END:IMELODY" */
#if (defined HAVE_FSEEKO) && (defined HAVE_FTELLO)
	off_t curr_pos = 0;
#else
	long curr_pos = 0;
#endif
	char buf2[2];

	if ( (buffer == NULL) || (buf_index == NULL) || (imyfile == NULL) || (bufsize == 0) ) return;
	if ( feof (imyfile) ) is_eof = 1;
	while ( (sig_recvd == 0) && (is_eof == 0) )
	{
		while ( ((*buf_index) >= strlen (buffer)) && (is_eof == 0) && (sig_recvd == 0) )
		{
			if ( bufsize > 1 )
			{
				res = fgets (buffer, (int) bufsize, imyfile);
			}
			else
			{
				res = fgets (buf2, 2, imyfile);
				buffer[0] = buf2[0];
			}
			if ( res == NULL )
			{
				is_eof = 1;
				break;
			}
			if ( feof (imyfile) ) is_eof = 1;
			*buf_index = 0;
			if ( (strlen (buffer) >= strlen (IMYP_MEL_END))
				&& (strstr (buffer, IMYP_MEL_END) == buffer) )
			{
				printf ("%s", buffer);
				is_eof = 1;
				break;
			}
			if ( strncmp (buffer, IMYP_MEL_END, strlen (buffer)) == 0 )
			{
				/*
				   read enough bytes from the file to determine
				   if we're at the end of the melody, or we're just
				   starting another 'E'-note, for example
				*/
				strncpy (tmpbuf, buffer, bufsize);
#if (defined HAVE_FSEEKO) && (defined HAVE_FTELLO)
				curr_pos = ftello (imyfile)
					/* substract the already-read line */
					- strlen (buffer)
					/* add the offset of the current char: */
					+ *buf_index;
#else
				curr_pos = ftell (imyfile)
					/* substract the already-read line */
					- strlen (buffer)
					/* add the offset of the current char: */
					+ *buf_index;
#endif
				res = fgets (&tmpbuf[strlen (buffer)],
					(int)(sizeof (tmpbuf) - strlen (buffer)),
					imyfile);
#if (defined HAVE_FSEEKO) && (defined HAVE_FTELLO)
				fseeko (imyfile, curr_pos, SEEK_SET);
#else
				fseek (imyfile, curr_pos, SEEK_SET);
#endif
				if ( res == NULL )
				{
					is_eof = 1;
					break;
				}
				if ( strstr (tmpbuf, IMYP_MEL_END) != NULL )
				{
					printf ("%s", tmpbuf);
					is_eof = 1;
					break;
				}
			}
		}
		if ( is_eof != 0 ) break;
		if (
			   (buffer[*buf_index] == ' ' )
			|| (buffer[*buf_index] == '\t')
			|| (buffer[*buf_index] == '\r')
			|| (buffer[*buf_index] == '\n')
			|| (buffer[*buf_index] == '\0')
		)
		{
			(*buf_index)++;
			continue;
		}
		break;
	}
}

/* ======================================================================== */

/**
 * Reads a number from a file. Reads a new line from the file if necessary.
 * \param buffer The buffer to fill wit file data, if necessary.
 * \param buf_index Currently used index to the buffer.
 * \param bufsize The size of the whole buffer.
 * \param imyfile The file to read additional data from if necessary.
 * \return the number read.
 */
static int IMYP_ATTR((nonnull))
imyp_read_number (
#if defined (__STDC__) || defined (_AIX) \
	|| (defined (__mips) && defined (_SYSTYPE_SVR4)) \
	|| defined(WIN32) || defined(__cplusplus)
	char * const buffer,
	unsigned int * const buf_index,
	const size_t bufsize,
	FILE * const imyfile)
#else
	buffer, buf_index, bufsize, imyfile)
	char * const buffer;
	unsigned int * const buf_index;
	const size_t bufsize;
	FILE * const imyfile;
#endif
{
	int ret = 0, prev_ret = 0;
	int scanf_res;

	if ( (buffer == NULL) || (buf_index == NULL) || (imyfile == NULL) || (bufsize == 0) ) return ret;
	imyp_read_line (buffer, buf_index, bufsize, imyfile);
	while ( IMYP_IS_DIGIT (buffer[*buf_index]) )
	{
		scanf_res = sscanf (&buffer[*buf_index], "%d", &ret);
		if ( scanf_res == 1 )
		{
			/* skip the correct number of characters */
			if ( ret >= 1000000000 ) (*buf_index)++;
			if ( ret >= 100000000  ) (*buf_index)++;
			if ( ret >= 10000000   ) (*buf_index)++;
			if ( ret >= 1000000    ) (*buf_index)++;
			if ( ret >= 100000     ) (*buf_index)++;
			if ( ret >= 10000      ) (*buf_index)++;
			if ( ret >= 1000       ) (*buf_index)++;
			if ( ret >= 100        ) (*buf_index)++;
			if ( ret >= 10         ) (*buf_index)++;
			/* always skip at least one */
			(*buf_index)++;
			if      ( ret >= 1000000000 ) ret = /*prev_ret * 10000000000 +*/ ret;
			else if ( ret >= 100000000  ) ret = prev_ret * 1000000000  + ret;
			else if ( ret >= 10000000   ) ret = prev_ret * 100000000   + ret;
			else if ( ret >= 1000000    ) ret = prev_ret * 10000000    + ret;
			else if ( ret >= 100000     ) ret = prev_ret * 1000000     + ret;
			else if ( ret >= 10000      ) ret = prev_ret * 100000      + ret;
			else if ( ret >= 1000       ) ret = prev_ret * 10000       + ret;
			else if ( ret >= 100        ) ret = prev_ret * 1000        + ret;
			else if ( ret >= 10         ) ret = prev_ret * 100         + ret;
			else			      ret = prev_ret * 10          + ret;
		}
		prev_ret = ret;
		imyp_read_line (buffer, buf_index, bufsize, imyfile);
		/* read the next part of the number */
	}
	return ret;
}

/* ======================================================================== */

/**
 * Gets the duration of the note pointed to by buf.
 * \param buf A character buffer with the note's description.
 * \param how_many_skipped Will get the number of characters used from the buffer.
 * \return a duration for the first note in the buffer, in milliseconds.
 */
static int IMYP_ATTR((nonnull))
imyp_get_duration (
#if defined (__STDC__) || defined (_AIX) \
	|| (defined (__mips) && defined (_SYSTYPE_SVR4)) \
	|| defined(WIN32) || defined(__cplusplus)
	char * const note_buffer,
	int * const how_many_skipped,
	int current_bpm,
	FILE * const imyfile)
#else
	note_buffer, how_many_skipped, current_bpm, imyfile)
	const char * note_buffer;
	int * const how_many_skipped;
	int current_bpm;
	FILE * const imyfile;
#endif
{
	unsigned int imyp_index = 0;
	float duration = 0.0f;
	unsigned int dur_shift = 0;

	if ( (note_buffer == NULL) || (imyfile == NULL) )
	{
		if ( how_many_skipped != NULL ) *how_many_skipped = 0;
		return 0;
	}
	imyp_read_line (note_buffer, &imyp_index, strlen (note_buffer), imyfile);
	while (
		   ((note_buffer[imyp_index] == '&')
		|| (note_buffer[imyp_index] == '#')
		|| (IMYP_TOUPPER (note_buffer[imyp_index]) == 'C')
		|| (IMYP_TOUPPER (note_buffer[imyp_index]) == 'D')
		|| (IMYP_TOUPPER (note_buffer[imyp_index]) == 'E')
		|| (IMYP_TOUPPER (note_buffer[imyp_index]) == 'F')
		|| (IMYP_TOUPPER (note_buffer[imyp_index]) == 'G')
		|| (IMYP_TOUPPER (note_buffer[imyp_index]) == 'A')
		|| (IMYP_TOUPPER (note_buffer[imyp_index]) == 'B'))
		&& (sig_recvd == 0)
	)
	{
		imyp_index++;
		imyp_read_line (note_buffer, &imyp_index, strlen (note_buffer), imyfile);
	}
	if ( (sig_recvd != 0) || (is_eof != 0) ) return 0;
	if ( ! IMYP_IS_DIGIT (note_buffer[imyp_index]) )
	{
		if ( how_many_skipped != NULL ) *how_many_skipped = imyp_index;
		/* not a note */
		return 0;
	}

	sscanf (&note_buffer[imyp_index], "%1u", &dur_shift);
	imyp_index++;
	imyp_read_line (note_buffer, &imyp_index, strlen (note_buffer), imyfile);
	if ( dur_shift == 0 ) duration = 1000.0f;
	else duration = 1000.0f/(1<<dur_shift);
	if ( note_buffer[imyp_index] == '.' )
	{
		duration *= 1.5;
		imyp_index++;
	}
	else if ( note_buffer[imyp_index] == ':' )
	{
		duration *= 2;
		imyp_index++;
	}
	else if ( note_buffer[imyp_index] == ';' )
	{
		duration = (duration*2.0f)/3.0f;
		imyp_index++;
	}
	imyp_read_line (note_buffer, &imyp_index, strlen (note_buffer), imyfile);
	if ( how_many_skipped != NULL ) *how_many_skipped = imyp_index;
	if ( current_bpm == 0 ) current_bpm = IMYP_DEF_BPM;
	return IMYP_ROUND (duration * ((IMYP_DEF_BPM*1.0f)/current_bpm));
}

/**
 * Gets the pause time after the last note.
 * \param last_note_duration The duration of the last note played, in milliseconds.
 * \param current_style Playing style.
 * \return a duration for the pause between notes, in milliseconds.
 */
static int
imyp_get_rest_time (
#if defined (__STDC__) || defined (_AIX) \
	|| (defined (__mips) && defined (_SYSTYPE_SVR4)) \
	|| defined(WIN32) || defined(__cplusplus)
	const int last_note_duration,
	const int current_style)
#else
	last_note_duration, current_style)
	const int last_note_duration;
	const int current_style;
#endif
{
	if      ( current_style == 0 ) return last_note_duration/20;
	else if ( current_style == 1 ) return 0;
	else if ( current_style == 2 ) return last_note_duration;
	else return 0;
}

/**
 * Plays the given IMY file.
 * \param file_name The name of the file to play.
 * \return 0 on success
 */
static int IMYP_ATTR((nonnull))
imyp_play_file (
#if defined (__STDC__) || defined (_AIX) \
	|| (defined (__mips) && defined (_SYSTYPE_SVR4)) \
	|| defined(WIN32) || defined(__cplusplus)
	const char * const file_name, const CURR_LIB curr)
#else
	file_name, curr)
	const char * const file_name;
	const CURR_LIB curr;
#endif
{
	FILE * imy;
	char * res;
	int first_line = 1;
	int scanf_res;
	int skipped_dur;	/* has to be signed */
	int play_res;
	int note_duration = 0;

	imy = fopen (file_name, "r");
	if ( imy != NULL )
	{
		do
		{
			res = fgets (melody_line, sizeof (melody_line), imy);
			if ( res == NULL )
			{
				is_eof = 1;
				break;
			}
			else if ( strlen (melody_line) == 0 )
			{
				is_eof = 1;
				break;
			}
			else if ( first_line != 0 )
			{
				/* check for header */
				if ( strstr (melody_line, "BEGIN:IMELODY") == NULL )
				{
					printf ("%s: %s.\n", _(err_bad_file), file_name);
					is_eof = 1;
					break;
				}
				first_line = 0;
			}
			else if ( strstr (melody_line, "VERSION:") != NULL )
			{
				imyp_put_text (melody_line, curr);
			}
			else if ( strstr (melody_line, "FORMAT:") != NULL )
			{
				imyp_put_text (melody_line, curr);
			}
			else if ( strstr (melody_line, "NAME:") != NULL )
			{
				imyp_put_text (melody_line, curr);
			}
			else if ( strstr (melody_line, "BEAT:") != NULL )
			{
				imyp_put_text (melody_line, curr);
				/* parse beat */
				scanf_res = sscanf (&melody_line[5], "%d", &bpm);
				if ( scanf_res != 1 )
				{
					printf ("%s: %s.\n", _(err_parse_beat), melody_line);
					bpm = 120;
				}
				if ( (bpm < 25) || (bpm > 900) )
				{
					bpm = 120;
				}
			}
			else if ( strstr (melody_line, "STYLE:") != NULL )
			{
				imyp_put_text (melody_line, curr);
				/* parse style */
				scanf_res = sscanf (&melody_line[7] /* "STYLE:S" */, "%d", &style);
				if ( scanf_res != 1 )
				{
					printf ("%s: %s.\n", _(err_parse_style), melody_line);
					style = 0;
				}
				if ( (style < 0) || (style > 2) )
				{
					style = 0;
				}
			}
			else if ( strstr (melody_line, "VOLUME:") != NULL )
			{
				imyp_put_text (melody_line, curr);
				/* parse volume */
				scanf_res = sscanf (&melody_line[8], "%d", &volume);
				if ( scanf_res != 1 )
				{
					printf ("%s: %s.\n", _(err_parse_vol), melody_line);
					volume = 7;
				}
			}
			else if ( strstr (melody_line, "MELODY:") != NULL )
			{
				melody_index = 7;
				do
				{
					if (is_eof != 0) break;
					/* get a line from the file and skip the whitespace */
					imyp_read_line (melody_line, &melody_index,
						sizeof (melody_line) - 10, imy);
					if ( sig_recvd != 0 ) is_eof = 1;
					if ( is_eof != 0 ) break;
					if ( strstr (melody_line, IMYP_MEL_END) == melody_line )
					{
						imyp_put_text (melody_line, curr);
						is_eof = 1;
						break;
					}
					switch (melody_line[melody_index])
					{
						/* parse melody: */
						case 'v':
						case 'V':
							/* skip "vibeon/off" */
							melody_index++;
							imyp_read_line (melody_line, &melody_index,
								sizeof (melody_line) - 10, imy);
							if ( IMYP_TOUPPER (melody_line[melody_index]) == 'I' )
							{
								melody_index++;
								imyp_read_line (melody_line, &melody_index,
									sizeof (melody_line) - 10, imy);
								if ( ! (IMYP_TOUPPER
									(melody_line[melody_index]) == 'B') )
								{
									is_sharp = 0;
									is_flat = 0;
									break;
								}
								melody_index++;
								imyp_read_line (melody_line, &melody_index,
									sizeof (melody_line) - 10, imy);
								if ( ! (IMYP_TOUPPER
									(melody_line[melody_index]) == 'E') )
								{
									is_sharp = 0;
									is_flat = 0;
									break;
								}
								melody_index++;
								imyp_read_line (melody_line, &melody_index,
									sizeof (melody_line) - 10, imy);
								if ( ! (IMYP_TOUPPER
									(melody_line[melody_index]) == 'O') )
								{
									is_sharp = 0;
									is_flat = 0;
									break;
								}
								melody_index++;
								imyp_read_line (melody_line, &melody_index,
									sizeof (melody_line) - 10, imy);
								if ( (IMYP_TOUPPER
									(melody_line[melody_index]) == 'N') )
								{
									melody_index++;
									imyp_read_line (melody_line,
										&melody_index,
										sizeof (melody_line) - 10,
										imy);
									is_sharp = 0;
									is_flat = 0;
									break;
								}
								if ( ! (IMYP_TOUPPER
									(melody_line[melody_index]) == 'F') )
								{
									is_sharp = 0;
									is_flat = 0;
									break;
								}
								melody_index++;
								imyp_read_line (melody_line, &melody_index,
									sizeof (melody_line) - 10, imy);
								if ( ! (IMYP_TOUPPER
									(melody_line[melody_index]) == 'F') )
								{
									is_sharp = 0;
									is_flat = 0;
									break;
								}
								melody_index++;
								imyp_read_line (melody_line, &melody_index,
									sizeof (melody_line) - 10, imy);
							}
							/* read volume */
							else
							{
								if ( melody_line[melody_index] == '-' )
								{
									volume--;
									melody_index++;
								}
								else if ( melody_line[melody_index] == '+' )
								{
									volume++;
									melody_index++;
								}
								else
								{
									volume = imyp_read_number (
										melody_line,
										&melody_index,
										sizeof (melody_line)
										- 10, imy);
								}
								imyp_read_line (melody_line, &melody_index,
									sizeof (melody_line) - 10, imy);
								if ( volume < 0 ) volume = 0;
								else if ( volume > IMYP_MAX_IMY_VOLUME )
									volume = IMYP_MAX_IMY_VOLUME;
							}
							is_sharp = 0;
							is_flat = 0;
							break;
						case 'c':
						case 'C':
							melody_index++;
							imyp_read_line (melody_line, &melody_index,
								sizeof (melody_line) - 10, imy);
							skipped_dur = 0;
							note_duration = imyp_get_duration (
								&melody_line[melody_index],
								&skipped_dur,
								bpm, imy);
							if ( (is_flat != 0) && (octave != 0) )
							{
								play_res = imyp_play_tune (notes[octave-1][11],
									volume, note_duration,
									buf16, SAMPBUFSIZE, curr);
								if ( (play_res != 0) && (sig_recvd == 0) )
								{
									printf ("%s\n",
										_(err_play_tune));
								}
							}
							else if ( is_sharp != 0 )
							{
								play_res = imyp_play_tune (notes[octave][1],
									volume, note_duration,
									buf16, SAMPBUFSIZE, curr);
								if ( (play_res != 0) && (sig_recvd == 0) )
								{
									printf ("%s\n",
										_(err_play_tune));
								}
							}
							else
							{
								play_res = imyp_play_tune (notes[octave][0],
									volume, note_duration,
									buf16, SAMPBUFSIZE, curr);
								if ( (play_res != 0) && (sig_recvd == 0) )
								{
									printf ("%s\n",
										_(err_play_tune));
								}
							}
							imyp_pause (imyp_get_rest_time (note_duration,
								style), curr, 1);
							melody_index += skipped_dur;
							imyp_read_line (melody_line, &melody_index,
								sizeof (melody_line) - 10, imy);
							is_sharp = 0;
							is_flat = 0;
							break;
						case 'd':
						case 'D':
							melody_index++;
							imyp_read_line (melody_line, &melody_index,
								sizeof (melody_line) - 10, imy);
							skipped_dur = 0;
							note_duration = imyp_get_duration (
								&melody_line[melody_index],
								&skipped_dur,
								bpm, imy);
							if ( is_flat != 0 )
							{
								play_res = imyp_play_tune (notes[octave][1],
									volume, note_duration,
									buf16, SAMPBUFSIZE, curr);
								if ( (play_res != 0) && (sig_recvd == 0) )
								{
									printf ("%s\n",
										_(err_play_tune));
								}
							}
							else if ( is_sharp != 0 )
							{
								play_res = imyp_play_tune (notes[octave][3],
									volume, note_duration,
									buf16, SAMPBUFSIZE, curr);
								if ( (play_res != 0) && (sig_recvd == 0) )
								{
									printf ("%s\n",
										_(err_play_tune));
								}
							}
							else
							{
								play_res = imyp_play_tune (notes[octave][2],
									volume, note_duration,
									buf16, SAMPBUFSIZE, curr);
								if ( (play_res != 0) && (sig_recvd == 0) )
								{
									printf ("%s\n",
										_(err_play_tune));
								}
							}
							imyp_pause (imyp_get_rest_time (note_duration,
								style), curr, 1);
							melody_index += skipped_dur;
							imyp_read_line (melody_line, &melody_index,
								sizeof (melody_line) - 10, imy);
							is_sharp = 0;
							is_flat = 0;
							break;
						case 'e':
						case 'E':
							melody_index++;
							imyp_read_line (melody_line, &melody_index,
								sizeof (melody_line) - 10, imy);
							skipped_dur = 0;
							note_duration = imyp_get_duration (
								&melody_line[melody_index],
								&skipped_dur,
								bpm, imy);
							if ( is_flat != 0 )
							{
								play_res = imyp_play_tune (notes[octave][3],
									volume, note_duration,
									buf16, SAMPBUFSIZE, curr);
								if ( (play_res != 0) && (sig_recvd == 0) )
								{
									printf ("%s\n",
										_(err_play_tune));
								}
							}
							else if ( is_sharp != 0 )
							{
								play_res = imyp_play_tune (notes[octave][5],
									volume, note_duration,
									buf16, SAMPBUFSIZE, curr);
								if ( (play_res != 0) && (sig_recvd == 0) )
								{
									printf ("%s\n",
										_(err_play_tune));
								}
							}
							else
							{
								play_res = imyp_play_tune (notes[octave][4],
									volume, note_duration,
									buf16, SAMPBUFSIZE, curr);
								if ( (play_res != 0) && (sig_recvd == 0) )
								{
									printf ("%s\n",
										_(err_play_tune));
								}
							}
							imyp_pause (imyp_get_rest_time (note_duration,
								style), curr, 1);
							melody_index += skipped_dur;
							imyp_read_line (melody_line, &melody_index,
								sizeof (melody_line) - 10, imy);
							is_sharp = 0;
							is_flat = 0;
							break;
						case 'f':
						case 'F':
							melody_index++;
							imyp_read_line (melody_line, &melody_index,
								sizeof (melody_line) - 10, imy);
							skipped_dur = 0;
							note_duration = imyp_get_duration (
								&melody_line[melody_index],
								&skipped_dur,
								bpm, imy);
							if ( is_flat != 0 )
							{
								play_res = imyp_play_tune (notes[octave][4],
									volume, note_duration,
									buf16, SAMPBUFSIZE, curr);
								if ( (play_res != 0) && (sig_recvd == 0) )
								{
									printf ("%s\n",
										_(err_play_tune));
								}
							}
							else if ( is_sharp != 0 )
							{
								play_res = imyp_play_tune (notes[octave][6],
									volume, note_duration,
									buf16, SAMPBUFSIZE, curr);
								if ( (play_res != 0) && (sig_recvd == 0) )
								{
									printf ("%s\n",
										_(err_play_tune));
								}
							}
							else
							{
								play_res = imyp_play_tune (notes[octave][5],
									volume, note_duration,
									buf16, SAMPBUFSIZE, curr);
								if ( (play_res != 0) && (sig_recvd == 0) )
								{
									printf ("%s\n",
										_(err_play_tune));
								}
							}
							imyp_pause (imyp_get_rest_time (note_duration,
								style), curr, 1);
							melody_index += skipped_dur;
							imyp_read_line (melody_line, &melody_index,
								sizeof (melody_line) - 10, imy);
							is_sharp = 0;
							is_flat = 0;
							break;
						case 'g':
						case 'G':
							melody_index++;
							imyp_read_line (melody_line, &melody_index,
								sizeof (melody_line) - 10, imy);
							skipped_dur = 0;
							note_duration = imyp_get_duration (
								&melody_line[melody_index],
								&skipped_dur,
								bpm, imy);
							if ( is_flat != 0 )
							{
								play_res = imyp_play_tune (notes[octave][6],
									volume, note_duration,
									buf16, SAMPBUFSIZE, curr);
								if ( (play_res != 0) && (sig_recvd == 0) )
								{
									printf ("%s\n",
										_(err_play_tune));
								}
							}
							else if ( is_sharp != 0 )
							{
								play_res = imyp_play_tune (notes[octave][8],
									volume, note_duration,
									buf16, SAMPBUFSIZE, curr);
								if ( (play_res != 0) && (sig_recvd == 0) )
								{
									printf ("%s\n",
										_(err_play_tune));
								}
							}
							else
							{
								play_res = imyp_play_tune (notes[octave][7],
									volume, note_duration,
									buf16, SAMPBUFSIZE, curr);
								if ( (play_res != 0) && (sig_recvd == 0) )
								{
									printf ("%s\n",
										_(err_play_tune));
								}
							}
							imyp_pause (imyp_get_rest_time (note_duration,
								style), curr, 1);
							melody_index += skipped_dur;
							imyp_read_line (melody_line, &melody_index,
								sizeof (melody_line) - 10, imy);
							is_sharp = 0;
							is_flat = 0;
							break;
						case 'a':
						case 'A':
							melody_index++;
							imyp_read_line (melody_line, &melody_index,
								sizeof (melody_line) - 10, imy);
							skipped_dur = 0;
							note_duration = imyp_get_duration (
								&melody_line[melody_index],
								&skipped_dur,
								bpm, imy);
							if ( is_flat != 0 )
							{
								play_res = imyp_play_tune (notes[octave][8],
									volume, note_duration,
									buf16, SAMPBUFSIZE, curr);
								if ( (play_res != 0) && (sig_recvd == 0) )
								{
									printf ("%s\n",
										_(err_play_tune));
								}
							}
							else if ( is_sharp != 0 )
							{
								play_res = imyp_play_tune (notes[octave][10],
									volume, note_duration,
									buf16, SAMPBUFSIZE, curr);
								if ( (play_res != 0) && (sig_recvd == 0) )
								{
									printf ("%s\n",
										_(err_play_tune));
								}
							}
							else
							{
								play_res = imyp_play_tune (notes[octave][9],
									volume, note_duration,
									buf16, SAMPBUFSIZE, curr);
								if ( (play_res != 0) && (sig_recvd == 0) )
								{
									printf ("%s\n",
										_(err_play_tune));
								}
							}
							imyp_pause (imyp_get_rest_time (note_duration,
								style), curr, 1);
							melody_index += skipped_dur;
							imyp_read_line (melody_line, &melody_index,
								sizeof (melody_line) - 10, imy);
							is_sharp = 0;
							is_flat = 0;
							break;
						case 'b':
						case 'B':
							/* skip "backon/off": */
							melody_index++;
							imyp_read_line (melody_line, &melody_index,
								sizeof (melody_line) - 10, imy);
							if ( IMYP_TOUPPER (melody_line[melody_index]) == 'A' )
							{
								melody_index++;
								imyp_read_line (melody_line, &melody_index,
									sizeof (melody_line) - 10, imy);
								if ( ! (IMYP_TOUPPER
									(melody_line[melody_index]) == 'C') )
								{
									is_sharp = 0;
									is_flat = 0;
									break;
								}
								melody_index++;
								imyp_read_line (melody_line, &melody_index,
									sizeof (melody_line) - 10, imy);
								if ( ! (IMYP_TOUPPER
									(melody_line[melody_index]) == 'K') )
								{
									is_sharp = 0;
									is_flat = 0;
									break;
								}
								melody_index++;
								imyp_read_line (melody_line, &melody_index,
									sizeof (melody_line) - 10, imy);
								if ( ! (IMYP_TOUPPER
									(melody_line[melody_index]) == 'O') )
								{
									is_sharp = 0;
									is_flat = 0;
									break;
								}
								melody_index++;
								imyp_read_line (melody_line, &melody_index,
									sizeof (melody_line) - 10, imy);
								if ( (IMYP_TOUPPER
									(melody_line[melody_index]) == 'N') )
								{
									melody_index++;
									imyp_read_line (melody_line,
										&melody_index,
										sizeof (melody_line) - 10,
										imy);
									is_sharp = 0;
									is_flat = 0;
									break;
								}
								if ( ! (IMYP_TOUPPER
									(melody_line[melody_index]) == 'F') )
								{
									is_sharp = 0;
									is_flat = 0;
									break;
								}
								melody_index++;
								imyp_read_line (melody_line, &melody_index,
									sizeof (melody_line) - 10, imy);
								if ( ! (IMYP_TOUPPER
									(melody_line[melody_index]) == 'F') )
								{
									is_sharp = 0;
									is_flat = 0;
									break;
								}
								melody_index++;
								imyp_read_line (melody_line, &melody_index,
									sizeof (melody_line) - 10, imy);
							}
							/* read note */
							else
							{
								skipped_dur = 0;
								note_duration = imyp_get_duration (
									&melody_line[melody_index],
									&skipped_dur,
									bpm, imy);
								if ( is_flat != 0 )
								{
									play_res = imyp_play_tune (notes[octave][10],
										volume, note_duration,
										buf16, SAMPBUFSIZE, curr);
									if ( (play_res != 0) && (sig_recvd == 0) )
									{
										printf ("%s\n",
											_(err_play_tune));
									}
								}
								else if ( (is_sharp != 0) && (octave != 8) )
								{
									play_res = imyp_play_tune (notes[octave+1][0],
										volume, note_duration,
										buf16, SAMPBUFSIZE, curr);
									if ( (play_res != 0) && (sig_recvd == 0) )
									{
										printf ("%s\n",
											_(err_play_tune));
									}
								}
								else
								{
									play_res = imyp_play_tune (notes[octave][11],
										volume, note_duration,
										buf16, SAMPBUFSIZE, curr);
									if ( (play_res != 0) && (sig_recvd == 0) )
									{
										printf ("%s\n",
											_(err_play_tune));
									}
								}
								imyp_pause (imyp_get_rest_time (
									note_duration, style), curr, 1);
								melody_index += skipped_dur;
								imyp_read_line (melody_line, &melody_index,
									sizeof (melody_line) - 10, imy);
							}
							is_sharp = 0;
							is_flat = 0;
							break;
						case '&':
							melody_index++;
							imyp_read_line (melody_line, &melody_index,
								sizeof (melody_line) - 10, imy);
							is_flat = 1;
							is_sharp = 0;
							break;
						case '#':
							melody_index++;
							imyp_read_line (melody_line, &melody_index,
								sizeof (melody_line) - 10, imy);
							is_sharp = 1;
							is_flat = 0;
							break;
						case '*':
							melody_index++;
							imyp_read_line (melody_line, &melody_index,
								sizeof (melody_line) - 10, imy);
							sscanf (&melody_line[melody_index], "%1d", &octave);
							melody_index++;
							imyp_read_line (melody_line, &melody_index,
								sizeof (melody_line) - 10, imy);
							if ( octave < 0 ) octave = 0;
							else if ( octave > 8 ) octave = 8;
							is_sharp = 0;
							is_flat = 0;
							break;
						case 'r':
						case 'R':
							melody_index++;
							imyp_read_line (melody_line, &melody_index,
								sizeof (melody_line) - 10, imy);
							imyp_pause (imyp_get_duration (
									&melody_line[melody_index],
									&skipped_dur,
									bpm, imy), curr, 0);
							melody_index += skipped_dur;
							imyp_read_line (melody_line, &melody_index,
								sizeof (melody_line) - 10, imy);
							is_sharp = 0;
							is_flat = 0;
							break;
						case 'l':
						case 'L':
							/* skip "ledon/off" */
							melody_index++;
							imyp_read_line (melody_line, &melody_index,
								sizeof (melody_line) - 10, imy);
							if ( IMYP_TOUPPER (melody_line[melody_index]) == 'E' )
							{
								melody_index++;
								imyp_read_line (melody_line, &melody_index,
									sizeof (melody_line) - 10, imy);
								if ( ! (IMYP_TOUPPER
									(melody_line[melody_index]) == 'D') )
								{
									is_sharp = 0;
									is_flat = 0;
									break;
								}
								melody_index++;
								imyp_read_line (melody_line, &melody_index,
									sizeof (melody_line) - 10, imy);
								if ( ! (IMYP_TOUPPER
									(melody_line[melody_index]) == 'O') )
								{
									is_sharp = 0;
									is_flat = 0;
									break;
								}
								melody_index++;
								imyp_read_line (melody_line, &melody_index,
									sizeof (melody_line) - 10, imy);
								if ( (IMYP_TOUPPER
									(melody_line[melody_index]) == 'N') )
								{
									melody_index++;
									imyp_read_line (melody_line,
										&melody_index,
										sizeof (melody_line) - 10,
										imy);
									is_sharp = 0;
									is_flat = 0;
									break;
								}
								if ( ! (IMYP_TOUPPER
									(melody_line[melody_index]) == 'F') )
								{
									is_sharp = 0;
									is_flat = 0;
									break;
								}
								melody_index++;
								imyp_read_line (melody_line, &melody_index,
									sizeof (melody_line) - 10, imy);
								if ( ! (IMYP_TOUPPER
									(melody_line[melody_index]) == 'F') )
								{
									is_sharp = 0;
									is_flat = 0;
									break;
								}
								melody_index++;
								imyp_read_line (melody_line, &melody_index,
									sizeof (melody_line) - 10, imy);
							}
							is_sharp = 0;
							is_flat = 0;
							break;
						case '(':
							if ( is_repeat != 0 )
							{
								printf ("%s: '%c' (0x%x) %s %d: '%s'\n",
									_(err_unex_token),
									melody_line[melody_index],
									melody_line[melody_index],
									_(err_at_pos),
									melody_index,
									melody_line);
							}
							else
							{
								is_repeat = 1;
#if (defined HAVE_FSEEKO) && (defined HAVE_FTELLO)
								repeat_start_pos = ftello (imy)
									/* substract the already-read line */
									- strlen (melody_line)
									/* add the offset of '(': */
									+ melody_index
									/* skip the '(' itself: */
									+ 1;
#else
								repeat_start_pos = ftell (imy)
									/* substract the already-read line */
									- strlen (melody_line)
									/* add the offset of '(': */
									+ melody_index
									/* skip the '(' itself: */
									+ 1;
#endif
								/* find the repeat count: */
								while ( (melody_line[melody_index] != '@')
									&& (sig_recvd == 0) && (is_eof == 0)
								 )
								{
									melody_index++;
									imyp_read_line (melody_line,
										&melody_index,
										sizeof (melody_line) - 10,
										imy);
									if ( strstr (melody_line, IMYP_MEL_END)
										== melody_line )
									{
										printf ("%s", melody_line);
										is_eof = 1;
										break;
									}
								}
								if (sig_recvd != 0)
								{
									is_eof = 1;
									break;
								}
								if ( is_eof != 0 ) break;
								melody_index++; /* skip the '@' */
								repeat_count = imyp_read_number (
									melody_line,
									&melody_index,
									sizeof (melody_line)
									- 10, imy);
								if ( repeat_count == 0 )
								{
									repeat_count = 1;
									is_repeat_forever = 1;
								}
								else
								{
									is_repeat_forever = 0;
								}
								/* get the volume modifier */
								imyp_read_line (melody_line,
									&melody_index,
									sizeof (melody_line) - 10,
									imy);
								if ( IMYP_TOUPPER
									(melody_line[melody_index])
									== 'V' )
								{
									melody_index++;
									imyp_read_line (melody_line,
										&melody_index,
										sizeof (melody_line) - 10,
										imy);
									if ( melody_line[melody_index]
										== '-' )
									{
										volume--;
										melody_index++;
									}
									else if ( melody_line[melody_index]
										== '+' )
									{
										volume++;
										melody_index++;
									}
								}
								/* start repeating */
#if (defined HAVE_FSEEKO) && (defined HAVE_FTELLO)
								fseeko (imy, repeat_start_pos,
									SEEK_SET);
#else
								fseek (imy, repeat_start_pos,
									SEEK_SET);
#endif
								/* make sure a new line will be read */
								melody_index = sizeof (melody_line)+1;
								imyp_read_line (melody_line,
									&melody_index,
									sizeof (melody_line) - 10,
									imy);
							}
							is_sharp = 0;
							is_flat = 0;
							break;
						case ')':
							if ( is_repeat == 0 )
							{
								printf ("%s: '%c' (0x%x) %s %d: '%s'\n",
									_(err_unex_token),
									melody_line[melody_index],
									melody_line[melody_index],
									_(err_at_pos),
									melody_index,
									melody_line);
								melody_index++;
								imyp_read_line (melody_line,
									&melody_index,
									sizeof (melody_line) - 10,
									imy);
							}
							else
							{
								if ( repeat_count == 0 ) is_repeat = 0;
							}
							is_sharp = 0;
							is_flat = 0;
							break;
						case '@':
							if ( is_repeat == 0 )
							{
								printf ("%s: '%c' (0x%x) %s %d: '%s'\n",
									_(err_unex_token),
									melody_line[melody_index],
									melody_line[melody_index],
									_(err_at_pos),
									melody_index,
									melody_line);
								melody_index++;
								imyp_read_line (melody_line,
									&melody_index,
									sizeof (melody_line) - 10,
									imy);
							}
							else
							{
								/* repeat count means end of
								   repeat block */
								if ( is_repeat_forever == 0 )
								{
									repeat_count--;
								}
								if ( repeat_count == 0 )
								{
									is_repeat = 0;
									/* skip the repeat block */
									while ( (melody_line[melody_index] != ')')
										&& (sig_recvd == 0) && (is_eof == 0)
									)
									{
										melody_index++;
										imyp_read_line (melody_line,
											&melody_index,
											sizeof (melody_line) - 10,
											imy);
									}
									if ( sig_recvd != 0 )
									{
										is_eof = 1;
										break;
									}
									/* skip the ')' itself: */
									melody_index++;
									imyp_read_line (melody_line,
										&melody_index,
										sizeof (melody_line) - 10,
										imy);
								}
								else
								{
									/* start a new iteration */
#if (defined HAVE_FSEEKO) && (defined HAVE_FTELLO)
									fseeko (imy, repeat_start_pos,
										SEEK_SET);
#else
									fseek (imy, repeat_start_pos,
										SEEK_SET);
#endif
									/* make sure a new line
									   will be read */
									melody_index =
										sizeof (melody_line)+1;
									imyp_read_line (melody_line,
										&melody_index,
										sizeof (melody_line) - 10,
										imy);
								}
							}
							is_sharp = 0;
							is_flat = 0;
							break;
						default:
							if ( melody_index < strlen (melody_line) )
							{
								printf ("%s: '%c' (0x%x) %s %d: '%s'\n",
									_(err_unkn_token),
									melody_line[melody_index],
									melody_line[melody_index],
									_(err_at_pos),
									melody_index,
									melody_line);
							}
							melody_index++;
							imyp_read_line (melody_line,
								&melody_index,
								sizeof (melody_line) - 10,
								imy);
							is_sharp = 0;
							is_flat = 0;
							break;
					} /* switch */
				} while ((is_eof == 0) && (sig_recvd == 0));
			} /* else if ( strstr (melody_line, "MELODY:") != NULL ) */
		} while ((is_eof == 0) && (sig_recvd == 0));
		fclose (imy);
		return 0;
	}
	else
	{
		printf ("%s %s.\n", _(err_file_open), file_name);
		return -4;
	}
}

/* ======================================================================== */

#ifdef END_OF_MAIN
int _mangled_main (int argc, char * argv[]);
#endif

/* SDL: */
#ifdef __cplusplus
extern "C"
#endif
int
main (
#if defined (__STDC__) || defined (_AIX) \
	|| (defined (__mips) && defined (_SYSTYPE_SVR4)) \
	|| defined(WIN32) || defined(__cplusplus)
	int argc, char* argv[] )
#else
	argc, argv )
	int argc;
	char* argv[];
#endif
{
	int ret = 0;
	size_t i;
	error_type error;

#ifdef HAVE_LIBINTL_H
# ifdef HAVE_SETLOCALE
	setlocale (LC_ALL, "");
# endif
	bindtextdomain (PACKAGE, LOCALEDIR);
	textdomain (PACKAGE);
#endif

	if ( (argc <= 1) || (argv == NULL) )
	{
		print_help ("");
		return -1;
	}

	if ( argv[0] != NULL )
	{
#ifdef HAVE_LIBGEN_H
		imyp_progname = basename (argv[0]);
#else
# if (defined HAVE_STRING_H)
		imyp_progname = strrchr (argv[0], (int)'/');
# else
		imyp_progname = argv[0];
# endif
#endif
		if ( imyp_progname == NULL )
		{
			imyp_progname = PROGRAM_NAME;
		}
	}
	else
	{
		imyp_progname = PROGRAM_NAME;
	}

	/* Parsing the command line */
#if (defined HAVE_GETOPT_H) && (defined HAVE_GETOPT_LONG)
	optind = 0;
	while (1==1)
	{
		opt_char = getopt_long ( argc, argv, "Vhld:", opts, NULL );
		if ( opt_char == -1 )
		{
			break;
		}

		/* NOTE: these shouldn't be a sequence of else-ifs */
		if ( (opt_char == (int)'?') || (opt_char == (int)'h') || (opt_help == 1) )
		{
			print_help (imyp_progname);
			return 1;
		}

		if ( (opt_char == (int)'V') || (opt_version == 1) )
		{
			printf ( "%s %s %s\n", imyp_progname, _(ver_str), VERSION );
			return 1;
		}

		if ( (opt_char == (int)'l') || (opt_license == 1) )
		{
			puts ( _(lic_str) );
			puts ( author_str );
			return 1;
		}
		if ( (opt_char == (int)'d') || (opt_dev == 1) )
		{
			device = optarg;
			opt_dev = 0;
		}
	}
	imyp_optind = optind;
#else
	for ( i = 1 ; i < (unsigned)argc; i++ )	/* argv[0] is the program name */
	{
		if ( argv[i] == NULL ) continue;
		/* NOTE: these shouldn't be a sequence of else-ifs */
		if ( (strstr (argv[i], "-h") == argv[i]) || (strstr (argv[i], "-?") == argv[i]) )
		{
			print_help (imyp_progname);
			return 1;
		}
		if ( strstr (argv[i], "-V") == argv[i] )
		{
			printf ( "%s %s %s\n", imyp_progname, _(ver_str), VERSION );
			return 1;
		}
		if ( strstr (argv[i], "-l") == argv[i] )
		{
			puts ( _(lic_str) );
			puts ( author_str );
			return 1;
		}
		if ( strstr (argv[i], "--to-midi") == argv[i] )
		{
			opt_tomidi = 1;
			argv[i] = NULL;
		}
		if ( (strstr (argv[i], "--device") == argv[i]) || (strstr (argv[i], "-d") == argv[i]) )
		{
			if ( i+1 < (unsigned)argc )
			{
				device = argv[i+1];
				argv[i+1] = NULL;
			}
			argv[i] = NULL;
		}
	}
	imyp_optind = 1;
#endif
#ifdef __GNUC__
# pragma GCC poison optind
#endif
	if ( imyp_optind >= argc )
	{
		print_help (imyp_progname);
		return -1;
	}

	if ( opt_tomidi == 0 )
	{
		if ( imyp_lib_init (&current_library, 0, device) != 0 )
		{
			printf ("%s\n", _(err_lib_init));
			return -2;
		}
	}

#ifdef HAVE_SIGNAL_H
	imyp_set_sigh (&error);
#endif		/* HAVE_SIGNAL_H */

	for ( i = 0 ; i < sizeof (melody_line); i++ )
	{
		melody_line[i] = '\0';
	}

	/*
	 * Unrecognised command line options are assumed to be files to play.
	 */
	while ( (imyp_optind < argc) && (sig_recvd == 0) )
	{
		if ( argv[imyp_optind] == NULL )
		{
			imyp_optind++;
			continue;
		}
		if ( opt_tomidi == 1 )
		{
			if ( imyp_lib_init (&current_library, 1, argv[imyp_optind]) != 0 )
			{
				printf ("%s\n", _(err_lib_init));
				imyp_optind++;
				continue;
			}
		}
		ret = imyp_play_file (argv[imyp_optind], current_library);
		if ( sig_recvd != 0 ) break;
		imyp_optind++;
	} /* while optind<argc && !signal */

	if ( imyp_lib_close (current_library) != 0 )
	{
		printf ("%s\n", _(err_lib_close));
		ret = -3;
	}
	if ( sig_recvd != 0 ) ret = -100;

	return ret;
}

#ifdef END_OF_MAIN
END_OF_MAIN ()
#endif
