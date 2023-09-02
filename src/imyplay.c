/*
 * IMYplay - A program for playing iMelody ringtones (IMY files).
 *	-- main file.
 *
 * Copyright (C) 2009-2023 Bogdan Drozdowski, bogdro (at) users.sourceforge.net
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

#ifdef IMYP_HAVE_LIBNETBLOCK
# include <libnetblock.h>
#endif

#ifdef IMYP_HAVE_LIBHIDEIP
# include <libhideip.h>
#endif

#include "imypwrap.h"
#include "imyp_sig.h"
#include "imyparse.h"
#include "imyputil.h"

# ifdef __GNUC__
#  pragma GCC poison strcpy strcat
# endif

#ifdef IMYP_HAVE_ALLEGRO
# include <allegro.h>	/* END_OF_MAIN() */
#endif

#define	PROGRAM_NAME	PACKAGE

static const char ver_str[] = N_("version");
static const char author_str[] = "Copyright (C) 2009-2023 Bogdan 'bogdro' Drozdowski, bogdro@users.sourceforge.net\n" \
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
const char * const imyp_err_msg_signal    = N_("Error while trying to set a signal handler for");
static const char * const err_lib_init    = N_("Error during library init.");
#if (defined IMYP_HAVE_MIDI) || (defined IMYP_HAVE_EXEC)
static const char * const err_lib_close   = N_("Error during library closing.");
#endif
/*static const char * const err_snd_init    = N_("Error during sound init.");*/

/* Messages displayed when verbose mode is on */
/*const char * const msg_signal      = N_("Setting signal handlers");*/

/* Signal-related stuff */
#ifdef HAVE_SIGNAL_H
const char * const imyp_sig_unk = N_("unknown");
#endif /* HAVE_SIGNAL_H */

static const char * imyp_progname;	/* The name of the program */

/* Command-line options. */
/* These unconditionally: */
static int imyp_optind= 0;
#ifdef IMYP_HAVE_MIDI
static int opt_tomidi  = 0;
# if (defined HAVE_GETOPT_H) && (defined HAVE_GETOPT_LONG)
static int opt_midiins = 0;
# endif
static int midi_instrument = -1;
#endif
#ifdef IMYP_HAVE_FILE
static int opt_file    = 0;
static char * out_file = NULL;
#endif
static char * device = NULL;
static char * output_system = NULL;
static char prog_name_output_system[25] = {0};
#ifdef IMYP_HAVE_EXEC
# if (defined HAVE_GETOPT_H) && (defined HAVE_GETOPT_LONG)
static int opt_exec    = 0;
# endif
static char * exec_program = NULL;
#endif
#if (defined HAVE_GETOPT_H) && (defined HAVE_GETOPT_LONG)
static int opt_dev     = 0;
static int opt_help    = 0;
static int opt_license = 0;
static int opt_version = 0;
static int opt_output  = 0;

static int opt_char    = 0;

static const struct option opts[] =
{
	{ "device",     required_argument, &opt_dev,     1 },
# ifdef IMYP_HAVE_EXEC
	{ "exec",       required_argument, &opt_exec,    1 },
# endif
# ifdef IMYP_HAVE_FILE
	{ "file",       required_argument, &opt_file,    1 },
# endif
	{ "help",       no_argument,       &opt_help,    1 },
	{ "licence",    no_argument,       &opt_license, 1 },
	{ "license",    no_argument,       &opt_license, 1 },
# ifdef IMYP_HAVE_MIDI
	{ "midi-instr", required_argument, &opt_midiins, 1 },
	{ "to-midi",    no_argument,       &opt_tomidi,  1 },
# endif
	{ "output",     required_argument, &opt_output, 1 },
	{ "version",    no_argument,       &opt_version, 1 },
	{ NULL, 0, NULL, 0 }
};
#endif

static imyp_backend_t current_library =
{
	NULL,
	IMYP_CURR_NONE
};

#ifdef TEST_COMPILE
# undef IMYP_ANSIC
# if TEST_COMPILE > 1
#  undef HAVE_MALLOC
# endif
#endif

/* ======================================================================== */

/**
 * Displays an error message.
 * \param err Error code.
 * \param msg The message.
 * \param extra Last element of the error message (fsname or signal).
 * \param FS The filesystem this message refers to.
 */
void
#ifdef IMYP_ANSIC
IMYP_ATTR ((nonnull))
#endif
imyp_show_error (
#ifdef IMYP_ANSIC
	const imyp_error_type	err,
	const char * const	msg,
	const char * const	extra )
#else
	err, msg, extra )
	const imyp_error_type	err;
	const char * const	msg;
	const char * const	extra;
#endif
{
	if ( msg == NULL )
	{
		return;
	}

	fprintf ( stderr, "%s: %s: %d (%s) %s", imyp_progname,
		_(err_msg),
		err, _(msg),
		(extra != NULL)? extra : "" );
	fflush (stderr);
}

/* ======================================================================== */

#ifndef IMYP_ANSIC
static void print_help IMYP_PARAMS ((const char * const my_name));
#endif

/**
 * Prints the help screen.
 * \param my_name Program identifier, like argv[0], if available.
 */
static void
#ifdef IMYP_ANSIC
IMYP_ATTR ((nonnull))
#endif
print_help (
#ifdef IMYP_ANSIC
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
	}
#ifdef HAVE_STRING_H
	else if ( my_name[0] == '\0' /*strlen (my_name) == 0*/ )
	{
		prog = PROGRAM_NAME;
	}
#endif
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
#ifdef IMYP_HAVE_EXEC
	printf ( "\n-e|--exec <program>\t%s", _("Execute this program instead of sound output") );
#endif
#ifdef IMYP_HAVE_FILE
	printf ( "\n-f|--file <file>\t%s", _("Write raw samples to this file instead of sound output") );
#endif
	printf ( "\n-h|--help\t\t%s", _("Print help") );
	printf ( "\n-l|--license\t\t%s", _("Print license information") );
#ifdef IMYP_HAVE_MIDI
	printf ( "\n--midi-instr <number>\t%s", _("Select MIDI instrument number") );
#endif
	printf ( "\n-o|--output <system>\t%s", _("Use the given output system") );
#ifdef IMYP_HAVE_MIDI
	printf ( "\n--to-midi\t\t%s", _("Convert the given files to MIDI format") );
#endif
	printf ( "\n-V|--version\t\t%s\n", _("Print version number") );
}

/* ======================================================================== */

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
	int ret = 0;
#if ! ((defined HAVE_GETOPT_H) && (defined HAVE_GETOPT_LONG))
	size_t i;
#endif
	imyp_error_type error;
	char * dash;

#ifdef HAVE_LIBINTL_H
# ifdef HAVE_SETLOCALE
	setlocale (LC_ALL, "");
# endif
	bindtextdomain (PACKAGE, LOCALEDIR);
	textdomain (PACKAGE);
#endif
#ifdef IMYP_HAVE_LIBNETBLOCK
	libnetblock_enable ();
#endif
#ifdef IMYP_HAVE_LIBHIDEIP
	libhideip_enable ();
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
		imyp_progname = strrchr (argv[0], (int)'/') + 1;
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

	/* find the output system's name from the program's executable name: */
	dash = strchr (imyp_progname, (int)'-');
	if ( dash != NULL )
	{
		strncpy (prog_name_output_system, dash+1, sizeof (prog_name_output_system)-1);
		prog_name_output_system[sizeof (prog_name_output_system)-1] = '\0';
		/* if dot present, remove it and the rest of the string with it: */
		dash = strchr (prog_name_output_system, (int)'.');
		if ( dash != NULL )
		{
			*dash = '\0';
		}
	}

	/* Parsing the command line */
#if (defined HAVE_GETOPT_H) && (defined HAVE_GETOPT_LONG)
	optind = 0;
	while (1==1)
	{
		opt_char = getopt_long ( argc, argv, "Vh?ld:e:o:f:", opts, NULL );
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
			imyp_report_versions (&current_library);
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
# ifdef IMYP_HAVE_MIDI
		if ( opt_midiins == 1 )
		{
			if ( sscanf (optarg, "%d", &midi_instrument) != 1 )
			{
				print_help (imyp_progname);
				return 1;
			}
			opt_midiins = 0;
		}
# endif
		if ( (opt_char == (int)'o') || (opt_output == 1) )
		{
			output_system = optarg;
			opt_output = 0;
		}
# ifdef IMYP_HAVE_EXEC
		if ( (opt_char == (int)'e') || (opt_exec == 1) )
		{
			exec_program = optarg;
			opt_exec = 0;
		}
# endif
# ifdef IMYP_HAVE_FILE
		if ( (opt_char == (int)'f') || (opt_file == 1) )
		{
			out_file = optarg;
			opt_file = 0;
		}
# endif
	}
	imyp_optind = optind;
#else
	for ( i = 1 ; i < (unsigned int)argc; i++ )	/* argv[0] is the program name */
	{
		if ( argv[i] == NULL )
		{
			continue;
		}
		/* NOTE: these shouldn't be a sequence of else-ifs */
		if ( (strstr (argv[i], "-h") == argv[i]) || (strstr (argv[i], "-?") == argv[i])
			|| (strstr (argv[i], "--help") == argv[i]) )
		{
			print_help (imyp_progname);
			return 1;
		}
		if ( (strstr (argv[i], "-V") == argv[i]) || (strstr (argv[i], "--version") == argv[i]) )
		{
			printf ( "%s %s %s\n", imyp_progname, _(ver_str), VERSION );
			imyp_report_versions (&current_library);
			return 1;
		}
		if ( (strstr (argv[i], "-l") == argv[i]) || (strstr (argv[i], "--license") == argv[i])
			|| (strstr (argv[i], "--licence") == argv[i]) )
		{
			puts ( _(lic_str) );
			puts ( author_str );
			return 1;
		}
# ifdef IMYP_HAVE_MIDI
		if ( strstr (argv[i], "--to-midi") == argv[i] )
		{
			opt_tomidi = 1;
			argv[i] = NULL;
			continue;
		}
		if ( strstr (argv[i], "--midi-instr") == argv[i] )
		{
			if ( i+1 < (unsigned int)argc )
			{
				if ( sscanf (argv[i+1], "%d", &midi_instrument) != 1 )
				{
					print_help (imyp_progname);
					return 1;
				}
				argv[i+1] = NULL;
			}
			else
			{
				print_help (imyp_progname);
				return 1;
			}
			argv[i] = NULL;
			continue;
		}
# endif
		if ( (strstr (argv[i], "--device") == argv[i]) || (strstr (argv[i], "-d") == argv[i]) )
		{
			if ( i+1 < (unsigned int)argc )
			{
				device = argv[i+1];
				argv[i+1] = NULL;
			}
			else
			{
				print_help (imyp_progname);
				return 1;
			}
			argv[i] = NULL;
			continue;
		}
		if ( (strstr (argv[i], "--output") == argv[i]) || (strstr (argv[i], "-o") == argv[i]) )
		{
			if ( i+1 < (unsigned int)argc )
			{
				output_system = argv[i+1];
				argv[i+1] = NULL;
			}
			else
			{
				print_help (imyp_progname);
				return 1;
			}
			argv[i] = NULL;
			continue;
		}
# ifdef IMYP_HAVE_EXEC
		if ( (strstr (argv[i], "--exec") == argv[i]) || (strstr (argv[i], "-e") == argv[i]) )
		{
			if ( i+1 < (unsigned int)argc )
			{
				exec_program = argv[i+1];
				argv[i+1] = NULL;
			}
			else
			{
				print_help (imyp_progname);
				return 1;
			}
			argv[i] = NULL;
			continue;
		}
# endif
# ifdef IMYP_HAVE_FILE
		if ( (strstr (argv[i], "--file") == argv[i]) || (strstr (argv[i], "-f") == argv[i]) )
		{
			if ( i+1 < (unsigned int)argc )
			{
				out_file = argv[i+1];
				argv[i+1] = NULL;
			}
			else
			{
				print_help (imyp_progname);
				return 1;
			}
			argv[i] = NULL;
			continue;
		}
# endif
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

	/* initialize a sound library, if MIDI, FILE and EXEC are not chosen */
#if (defined IMYP_HAVE_MIDI) || (defined IMYP_HAVE_EXEC)
	if (
# ifdef IMYP_HAVE_MIDI
		(opt_tomidi == 0)
#  if (defined IMYP_HAVE_EXEC) || (defined IMYP_HAVE_FILE)
		&&
#  endif
# endif
# ifdef IMYP_HAVE_EXEC
		(exec_program == NULL)
#  if (defined IMYP_HAVE_FILE)
		&&
#  endif
# endif
# ifdef IMYP_HAVE_FILE
		(out_file == NULL)
# endif
		)
#endif
	{
		if ( (output_system == NULL) && (prog_name_output_system[0] != '\0') )
		{
			/* no output on the command line, but in the program's name */
			output_system = prog_name_output_system;
		}
		if ( output_system != NULL )
		{
			enum IMYP_CURR_LIB sel = imyp_parse_system (output_system);
			if ( sel == IMYP_CURR_MIDI )
			{
#ifdef IMYP_HAVE_MIDI
				opt_tomidi = 1;
#endif
			}
			else if ( sel == IMYP_CURR_FILE )
			{
#ifdef IMYP_HAVE_FILE
				/* mark it here and use a default output filename below */
				opt_file = 1;
#endif
			}
#ifdef IMYP_HAVE_EXEC
			else if ( (sel == IMYP_CURR_EXEC) && (exec_program == NULL) )
			{
				/* EXEC backend selected, but no program to run chosen. */
				print_help (imyp_progname);
				return -2;
			}
#endif
			else if ( (sel != IMYP_CURR_MIDI)
				&& (sel != IMYP_CURR_EXEC)
				&& (sel != IMYP_CURR_FILE) )
			{
				/* try to initialize the given output system */
				if ( imyp_init_selected (&current_library,
					output_system,
					device,
#ifdef IMYP_HAVE_MIDI
					midi_instrument,
#else
					0,
#endif
#ifdef IMYP_HAVE_FILE
					out_file
#else
					NULL
#endif
					) != 0 )
				{
					printf ("%s\n", _(err_lib_init));
					return -2;
				}
				/* initialized successfully - set the currently-used library */
				current_library.imyp_curr_lib = sel;
			}
		}
		else if ( imyp_lib_init (&current_library, 0, device, 0,
#ifdef IMYP_HAVE_MIDI
			midi_instrument,
#else
			0,
#endif
			0,
#ifdef IMYP_HAVE_FILE
			out_file
#else
			NULL
#endif
			) != 0 )
		{
			printf ("%s\n", _(err_lib_init));
			return -2;
		}
	}

#ifdef HAVE_SIGNAL_H
	imyp_set_sigh (&error);
#endif	/* HAVE_SIGNAL_H */

	/*
	 * Unrecognised command line options are assumed to be files to play.
	 */
	while ( (imyp_optind < argc) && (imyp_sig_recvd == 0) )
	{
		if ( argv[imyp_optind] == NULL )
		{
			imyp_optind++;
			continue;
		}
		/* initialize special (MIDI/EXEC/FILE) output per each input file */
#ifdef IMYP_HAVE_MIDI
		if ( opt_tomidi == 1 )
		{
			if ( imyp_lib_init (&current_library, 1, argv[imyp_optind],
				0, midi_instrument, 0, NULL) != 0 )
			{
				printf ("%s\n", _(err_lib_init));
				imyp_optind++;
				ret = -2;
				continue;
			}
			else
			{
				/* skip the other special backends from initializing: */
#ifdef IMYP_HAVE_EXEC
				exec_program = NULL;
#endif
#ifdef IMYP_HAVE_FILE
				out_file = NULL;
				opt_file = 0;
#endif
			}
		}
#endif
#ifdef IMYP_HAVE_EXEC
		if ( exec_program != NULL )
		{
			if ( imyp_lib_init (&current_library, 0, exec_program,
				1, 0, 0, NULL) != 0 )
			{
				printf ("%s\n", _(err_lib_init));
				imyp_optind++;
				ret = -2;
				continue;
			}
			else
			{
				/* skip the other special backends from initializing: */
#ifdef IMYP_HAVE_MIDI
				opt_tomidi = 0;
#endif
#ifdef IMYP_HAVE_FILE
				out_file = NULL;
				opt_file = 0;
#endif
			}
		}
#endif
#ifdef IMYP_HAVE_FILE
		if ( (out_file != NULL) || (opt_file != 0) )
		{
			if ( out_file == NULL )
			{
				out_file = argv[imyp_optind];
			}
			if ( imyp_lib_init (&current_library, 0, device,
				0, 0, 1, out_file) != 0 )
			{
				printf ("%s\n", _(err_lib_init));
				imyp_optind++;
				ret = -2;
				continue;
			}
			else
			{
				/* skip the other special backends from initializing: */
#ifdef IMYP_HAVE_EXEC
				exec_program = NULL;
#endif
#ifdef IMYP_HAVE_MIDI
				opt_tomidi = 0;
#endif
			}
			/* restore the empty value, if it was empty before: */
			if ( out_file == argv[imyp_optind] )
			{
				out_file = NULL;
			}
		}
#endif
		ret = imyp_play_file (argv[imyp_optind], &current_library);

		/* close the special (non-sound) output */
#if (defined IMYP_HAVE_MIDI) || (defined IMYP_HAVE_EXEC)
		if (
# ifdef IMYP_HAVE_MIDI
			(opt_tomidi == 1)
#  if (defined IMYP_HAVE_EXEC) || (defined IMYP_HAVE_FILE)
			||
#  endif
# endif
# ifdef IMYP_HAVE_EXEC
			(exec_program != NULL)
#  if (defined IMYP_HAVE_FILE)
			||
#  endif
# endif
# ifdef IMYP_HAVE_FILE
			(out_file != NULL)
			|| (opt_file != 0)
# endif
		)
		{
			/* have to close the MIDI backend, along with its file */
			if ( imyp_lib_close (&current_library) != 0 )
			{
				printf ("%s\n", _(err_lib_close));
				ret = -3;
			}
		}
#endif
		if ( imyp_sig_recvd != 0 )
		{
			break;
		}
		imyp_optind++;
	} /* while optind<argc && !signal */

	/* close the sound output */
#if (defined IMYP_HAVE_MIDI) || (defined IMYP_HAVE_EXEC)
	if (
# ifdef IMYP_HAVE_MIDI
		(opt_tomidi == 0)
#  if (defined IMYP_HAVE_EXEC) || (defined IMYP_HAVE_FILE)
		&&
#  endif
# endif
# ifdef IMYP_HAVE_EXEC
		(exec_program == NULL)
#  if (defined IMYP_HAVE_FILE)
		&&
#  endif
# endif
# ifdef IMYP_HAVE_FILE
		(out_file == NULL)
		&& (opt_file == 0)
# endif
		)
	{
		/* MIDI backend closed, close other backend types: */
		if ( imyp_lib_close (&current_library) != 0 )
		{
			printf ("%s\n", _(err_lib_close));
			ret = -3;
		}
	}
#endif
	if ( imyp_sig_recvd != 0 )
	{
		ret = -100;
	}

	return ret;
}

#ifdef END_OF_MAIN
END_OF_MAIN ()
#endif
