%{
/*
 * IMYplay - A program for playing iMelody ringtones (IMY files).
 *	-- melody parsing file using (f)lex.
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

#include "imyp_cfg.h"

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

#include "imyplay.h"
#include "imyp_sig.h"
#include "imypwrap.h"
#include "imyparse.h"
#include "iparsers.h"

static unsigned short int buf16[IMYP_SAMPBUFSIZE];

static int volume;
static int style;
static int bpm;
static int octave;

static int pos;
static int repeat_count;
static int repeat_pos;

static int scanf_res;
static int play_result;
static int note_duration;
static imyp_backend_t * curr;

#define ECHO do { imyp_put_text (yytext, curr); } while (0)

#ifdef TEST_COMPILE
# undef IMYP_ANSIC
# if TEST_COMPILE > 1
#  undef HAVE_MALLOC
# endif
#endif

/* ======================================================================== */

#ifndef IMYP_ANSIC
static int imyp_get_duration IMYP_PARAMS ((const char * const note_buffer, int current_bpm));
#endif

/**
 * Gets the duration of the note pointed to by buf.
 * \param note_buffer A character buffer with the note's description.
 * \param current_bpm The current BPM value.
 * \return a duration for the first note in the buffer, in milliseconds.
 */
static int
#ifdef IMYP_ANSIC
IMYP_ATTR((nonnull))
#endif
imyp_get_duration (
#ifdef IMYP_ANSIC
	const char * const note_buffer,
	int current_bpm)
#else
	note_buffer, current_bpm)
	const char * const note_buffer;
	int current_bpm;
#endif
{
	float duration = 0.0f;
	unsigned int dur_shift = 0;

	if ( note_buffer == NULL )
	{
		return 0;
	}
	scanf_res = sscanf (note_buffer+1, "%u", &dur_shift);
	if ( scanf_res != 1 )
	{
		return 0;
	}
	if ( dur_shift == 0 )
	{
		duration = 1000.0f;
	}
	else
	{
		duration = 1000.0f/(float)(1<<dur_shift);
	}
	if ( note_buffer[2] == '.' )
	{
		duration *= 1.5f;
	}
	else if ( note_buffer[2] == ':' )
	{
		duration *= 2;
	}
	else if ( note_buffer[2] == ';' )
	{
		duration = (duration*2.0f)/3.0f;
	}
	if ( current_bpm == 0 )
	{
		current_bpm = IMYP_DEF_BPM;
	}
	return (int)IMYP_ROUND (duration * ((1.0f*IMYP_DEF_BPM)/(1.0f*(double)current_bpm)));
}

/* ======================================================================== */

#ifndef IMYP_ANSIC
static void imyp_play_current_note IMYP_PARAMS ((const char melody[],
	int * const duration, imyp_backend_t * const curr_lib,
	int * const play_res, const int is_sharp,
	const int is_flat, const int current_bpm));
#endif

/**
 * Play the given note. Also resets the sharp and flat flags.
 * \param[in] melody The note string to play.
 * \param[out] duration The duration of the note.
 * \param[in] curr_lib The current output backend.
 * \param[out] play_res The result of playing the note.
 * \param[in] is_sharp Non-zero if the note is sharp.
 * \param[in] is_flat Non-zero if the note is flat.
 * \param[in] current_bpm The current BPM value.
 */
static void
#ifdef IMYP_ANSIC
IMYP_ATTR((nonnull))
#endif
imyp_play_current_note (
#ifdef IMYP_ANSIC
	const char melody[], int * const duration,
	imyp_backend_t * const curr_lib, int * const play_res,
	const int is_sharp, const int is_flat, const int current_bpm)
#else
	melody, duration, curr_lib, play_res, is_sharp, is_flat, current_bpm)
	const char melody[];
	int * const duration;
	imyp_backend_t * const curr_lib;
	int * const play_res;
	const int is_sharp;
	const int is_flat;
	const int current_bpm;
#endif
{
	int note_index = 0;

	if ( (duration == NULL) || (curr_lib == NULL)
		|| (play_res == NULL) || (duration == NULL) )
	{
		return;
	}
	if ( curr_lib->imyp_curr_lib == IMYP_CURR_NONE )
	{
		return;
	}

	if ( IMYP_TOUPPER (melody[0]) == 'C' )
	{
		note_index = 0;
	}
	else if ( IMYP_TOUPPER (melody[0]) == 'D' )
	{
		note_index = 2;
	}
	else if ( IMYP_TOUPPER (melody[0]) == 'E' )
	{
		note_index = 4;
	}
	else if ( IMYP_TOUPPER (melody[0]) == 'F' )
	{
		note_index = 5;
	}
	else if ( IMYP_TOUPPER (melody[0]) == 'G' )
	{
		note_index = 7;
	}
	else if ( IMYP_TOUPPER (melody[0]) == 'A' )
	{
		note_index = 9;
	}
	else if ( IMYP_TOUPPER (melody[0]) == 'B' )
	{
		note_index = 11;
	}

	*duration = imyp_get_duration (melody, current_bpm);
	if ( (is_flat != 0) && (note_index == 0) && (octave != 0) )
	{
		/* flat 'C' note, but not the first 'C' note */
		*play_res = imyp_play_tune (imyp_notes[octave-1][IMYP_NOTES_PER_OCTAVE-1],
			volume, *duration,
			buf16, IMYP_SAMPBUFSIZE, curr_lib);
		if ( (*play_res != 0) && (imyp_sig_recvd == 0) )
		{
			printf ("%s\n", _(err_play_tune));
		}
	}
	else if ( (is_flat != 0) && (note_index != 0) )
	{
		/* flat note and not a 'C' note */
		*play_res = imyp_play_tune (imyp_notes[octave][note_index-1],
			volume, *duration,
			buf16, IMYP_SAMPBUFSIZE, curr_lib);
		if ( (*play_res != 0) && (imyp_sig_recvd == 0) )
		{
			printf ("%s\n", _(err_play_tune));
		}
	}
	else if ( (is_sharp != 0) && (note_index == IMYP_NOTES_PER_OCTAVE-1)
		&& (octave < IMYP_OCTAVES) )
	{
		/* sharp 'B' note and not the last 'B' note */
		*play_res = imyp_play_tune (imyp_notes[octave+1][0],
			volume, *duration,
			buf16, IMYP_SAMPBUFSIZE, curr_lib);
		if ( (*play_res != 0) && (imyp_sig_recvd == 0) )
		{
			printf ("%s\n", _(err_play_tune));
		}
	}
	else if ( (is_sharp != 0) && (note_index != IMYP_NOTES_PER_OCTAVE-1) )
	{
		/* sharp note and not a 'B' note */
		*play_res = imyp_play_tune (imyp_notes[octave][note_index+1],
			volume, *duration,
			buf16, IMYP_SAMPBUFSIZE, curr_lib);
		if ( (*play_res != 0) && (imyp_sig_recvd == 0) )
		{
			printf ("%s\n", _(err_play_tune));
		}
	}
	else
	{
		/* Normal note. We also get here on the first 'C', when
		   the flat flag is on or on the last 'B' when the
		   sharp flag is on. */
		*play_res = imyp_play_tune (imyp_notes[octave][note_index],
			volume, *duration,
			buf16, IMYP_SAMPBUFSIZE, curr_lib);
		if ( (*play_res != 0) && (imyp_sig_recvd == 0) )
		{
			printf ("%s\n", _(err_play_tune));
		}
	}
	imyp_pause (imyp_get_rest_time (*duration, style), curr_lib, 1, buf16, IMYP_SAMPBUFSIZE);
}

/* ======================================================================== */

#ifndef IMYP_ANSIC
static void imyp_make_pause IMYP_PARAMS ((const char melody[],
	imyp_backend_t * const curr_lib, const int current_bpm));
#endif

/**
 * Make a pause in the melody.
 * \param[in] melody The pause string.
 * \param[in] curr_lib The current output backend.
 * \param[in] current_bpm The current BPM value.
 */
static void
#ifdef IMYP_ANSIC
IMYP_ATTR((nonnull))
#endif
imyp_make_pause (
#ifdef IMYP_ANSIC
	const char melody[], imyp_backend_t * const curr_lib, const int current_bpm)
#else
	melody, curr_lib, current_bpm)
	const char melody[];
	imyp_backend_t * const curr_lib;
	const int current_bpm;
#endif
{
	if ( (melody == NULL) || (curr_lib == NULL) || (imyp_sig_recvd != 0) )
	{
		return;
	}
	if ( curr_lib->imyp_curr_lib == IMYP_CURR_NONE )
	{
		return;
	}
	imyp_pause (imyp_get_duration (melody, current_bpm), curr_lib, 0, buf16, IMYP_SAMPBUFSIZE);
}

%}

/* ======================================================================== */

%option batch
%option case-insensitive
/*%option fast - disables 8-bit, not needed */
%option never-interactive
 /*%option outfile="imyparsf.c" -- is taken care of by autoconf/automake */
%option nounput
%option warn nodefault
%option yylineno
 /* portability, recommended by autoconf: */
%option noyywrap
%option nomain

%x IN_IMELODY
%x IN_MELODY_LINE
%x IN_REPEAT_BLOCK
%x IN_REPEAT_BLOCK_PASS
%x REPEAT_BLOCK_VOL
%x REPEAT_BLOCK_VOL_PASS
%x REPEAT_BLOCK_END

IGNORED_STATEMENTS	(vibeon|vibeoff|backon|backoff|ledon|ledoff)
DURATION		[1-5]
OCTAVE_MODIFIER		(\*[0-8])

NOTE			([a-g]{DURATION}[.:;]?)
FLAT_NOTE		&{NOTE}
SHARP_NOTE		#{NOTE}
SILENCE			(r{DURATION}[.:;]?)
NOTES			{NOTE}|{FLAT_NOTE}|{SHARP_NOTE}|{SILENCE}

VOLUME_MODIFIER		(V[+-])
VOLUME_VALUE		(V([0-9])+)
VOLUME_STMTS		{VOLUME_MODIFIER}|{VOLUME_VALUE}

REPEAT_ELEMS		{IGNORED_STATEMENTS}|{NOTES}|{OCTAVE_MODIFIER}|{VOLUME_STMTS}
REPEAT_BLOCK		("("({REPEAT_ELEMS})+@[0-9]+({VOLUME_MODIFIER})?")")
FULL_MELODY		(({REPEAT_ELEMS}|{REPEAT_BLOCK})+)

%%

^BEGIN:IMELODY[\r\n]*			{

	BEGIN (IN_IMELODY);
}

<IN_IMELODY>^VERSION:.*			ECHO;

<IN_IMELODY>^FORMAT:.*			ECHO;

<IN_IMELODY>^NAME:.*			ECHO;

<IN_IMELODY>^BEAT:.*			{

	ECHO;
	/* parse beat */
	scanf_res = sscanf (yytext+5 /* "BEAT:" */, "%d", &bpm);
	if ( scanf_res != 1 )
	{
		printf ("%s: %s.\n", _(err_parse_beat), yytext+5);
		bpm = 120;
	}
	if ( (bpm < 25) || (bpm > 900) )
	{
		bpm = 120;
	}
	if ( imyp_sig_recvd != 0 )
	{
		return -1;
	}
}

<IN_IMELODY>^STYLE:S.*			{

	ECHO;
	/* parse style */
	scanf_res = sscanf (yytext+7 /* "STYLE:S" */, "%d", &style);
	if ( scanf_res != 1 )
	{
		printf ("%s: %s.\n", _(err_parse_style), yytext+7);
		style = 0;
	}
	if ( (style < 0) || (style > 2) )
	{
		style = 0;
	}
	if ( imyp_sig_recvd != 0 )
	{
		return -1;
	}
}

<IN_IMELODY>^VOLUME:V.*			{

	ECHO;
	/* parse volume */
	scanf_res = sscanf (yytext+8 /* VOLUME:V */, "%d", &volume);
	if ( scanf_res != 1 )
	{
		printf ("%s: %s.\n", _(err_parse_vol), yytext+8);
		volume = 7;
	}
	if ( volume < 0 )
	{
		volume = 0;
	}
	else if ( volume > IMYP_MAX_IMY_VOLUME )
	{
		volume = IMYP_MAX_IMY_VOLUME;
	}
	if ( imyp_sig_recvd != 0 )
	{
		return -1;
	}
}

 /* don't use "^MELODY:{FULL_MELODY} here, because we want to catch unknown tokens
	ourselves and play the rest of the melody anyway. */
<IN_IMELODY>^MELODY:			{

	pos = 0;
	BEGIN (IN_MELODY_LINE);
	if ( imyp_sig_recvd != 0 )
	{
		return -1;
	}
}

<IN_MELODY_LINE>{NOTE}			{

	imyp_play_current_note (yytext, &note_duration, curr, &play_result, 0, 0, bpm);
	/* update the line position to display the position of unknown tokens */
	pos += yyleng;
	if ( imyp_sig_recvd != 0 )
	{
		return -1;
	}
}

<IN_MELODY_LINE>{FLAT_NOTE}		{

	imyp_play_current_note (yytext+1 /* skip the '&' */, &note_duration,
		curr, &play_result, 0, 1, bpm);
	/* update the line position to display the position of unknown tokens */
	pos += yyleng;
	if ( imyp_sig_recvd != 0 )
	{
		return -1;
	}
}

<IN_MELODY_LINE>{SHARP_NOTE}		{

	imyp_play_current_note (yytext+1 /* skip the '#' */, &note_duration,
		curr, &play_result, 1, 0, bpm);
	/* update the line position to display the position of unknown tokens */
	pos += yyleng;
	if ( imyp_sig_recvd != 0 )
	{
		return -1;
	}
}

<IN_MELODY_LINE>{SILENCE}		{

	imyp_make_pause (yytext, curr, bpm);
	/* update the line position to display the position of unknown tokens */
	pos += yyleng;
	if ( imyp_sig_recvd != 0 )
	{
		return -1;
	}
}

<IN_MELODY_LINE>{IGNORED_STATEMENTS}	{

	/* update the line position to display the position of unknown tokens */
	pos += yyleng;
	if ( imyp_sig_recvd != 0 )
	{
		return -1;
	}
}

<IN_MELODY_LINE>{OCTAVE_MODIFIER}	{

	scanf_res = sscanf (yytext+1, "%d", &octave);
	if ( scanf_res == 1 )
	{
		if ( octave < 0 )
		{
			octave = 0;
		}
		else if ( octave > 8 )
		{
			octave = 8;
		}
	}
	else
	{
		printf ("%s: '%c' (0x%x) %s %d:%d\n",
			_(err_unkn_token),
			*yytext,
			(unsigned int)*yytext,
			_(err_at_pos),
			yylineno,
			pos+1);
	}
	/* update the line position to display the position of unknown tokens */
	pos += yyleng;
	if ( imyp_sig_recvd != 0 )
	{
		return -1;
	}
}

<IN_MELODY_LINE>{VOLUME_MODIFIER}	{

	if ( *(yytext+1) == '-' )
	{
		volume--;
	}
	else if ( *(yytext+1) == '+' )
	{
		volume++;
	}
	if ( volume < 0 )
	{
		volume = 0;
	}
	else if ( volume > IMYP_MAX_IMY_VOLUME )
	{
		volume = IMYP_MAX_IMY_VOLUME;
	}
	/* update the line position to display the position of unknown tokens */
	pos += yyleng;
	if ( imyp_sig_recvd != 0 )
	{
		return -1;
	}
}

<IN_MELODY_LINE>{VOLUME_VALUE}		{

	scanf_res = sscanf (yytext+1, "%d", &volume);
	if ( scanf_res != 1 )
	{
		printf ("%s: %s.\n", _(err_parse_vol), yytext+8);
	}
	if ( volume < 0 )
	{
		volume = 0;
	}
	else if ( volume > IMYP_MAX_IMY_VOLUME )
	{
		volume = IMYP_MAX_IMY_VOLUME;
	}
	/* update the line position to display the position of unknown tokens */
	pos += yyleng;
	if ( imyp_sig_recvd != 0 )
	{
		return -1;
	}
}

<IN_MELODY_LINE>"("			{

	pos += yyleng;
	BEGIN (IN_REPEAT_BLOCK);
	/* initialize repeat block variables: */
	repeat_count = -2;
	repeat_pos = pos;
	if ( imyp_sig_recvd != 0 )
	{
		return -1;
	}
}

<IN_MELODY_LINE>[\r\n]+[^[:space:]]	{

	/* a newline NOT followed by whitespace means the end of the melody line. */
	BEGIN (IN_IMELODY);
	/* return everything to the stream, so we can catch the line again. */
	yyless (0);
	if ( imyp_sig_recvd != 0 )
	{
		return -1;
	}
}

 /* eat up newlines inside the melody and reset the line position: */
<IN_MELODY_LINE>[\n\r]			{

	pos = 0;
	if ( imyp_sig_recvd != 0 )
	{
		return -1;
	}
}

 /* eat up whitespace inside the melody, but update the position: */
<IN_MELODY_LINE>[[:space:]]		{

	/* update the line position to display the position of unknown tokens */
	pos += yyleng;
	if ( imyp_sig_recvd != 0 )
	{
		return -1;
	}
}

<IN_REPEAT_BLOCK,IN_REPEAT_BLOCK_PASS>{NOTE}			{

	/* update the line position to display the position of unknown tokens */
	repeat_pos = pos + yyleng;
	/* in a repeat block, collect all the elements to repeat and return to them
	   after looking throuhg the whole repeat block. */
	yymore ();
	if ( YY_START == IN_REPEAT_BLOCK_PASS /*repeat_count != -2*/ )
	{
		/* perform any action here, where we know that the block has been fully parsed */
		imyp_play_current_note (yytext + yyleng-2, &note_duration, curr,
			&play_result, 0, 0, bpm);
	}
	if ( imyp_sig_recvd != 0 )
	{
		return -1;
	}
}

<IN_REPEAT_BLOCK,IN_REPEAT_BLOCK_PASS>{FLAT_NOTE}		{

	/* update the line position to display the position of unknown tokens */
	repeat_pos = pos + yyleng;
	/* in a repeat block, collect all the elements to repeat and return to them
	   after looking throuhg the whole repeat block. */
	yymore ();
	if ( YY_START == IN_REPEAT_BLOCK_PASS /*repeat_count != -2*/ )
	{
		/* perform any action here, where we know that the block has been fully parsed */
		imyp_play_current_note (yytext + yyleng-3 + 1 /* skip the '&' */,
			&note_duration, curr, &play_result, 0, 0, bpm);
	}
	if ( imyp_sig_recvd != 0 )
	{
		return -1;
	}
}

<IN_REPEAT_BLOCK,IN_REPEAT_BLOCK_PASS>{SHARP_NOTE}		{

	/* update the line position to display the position of unknown tokens */
	repeat_pos = pos + yyleng;
	/* in a repeat block, collect all the elements to repeat and return to them
	   after looking throuhg the whole repeat block. */
	yymore ();
	if ( YY_START == IN_REPEAT_BLOCK_PASS /*repeat_count != -2*/ )
	{
		/* perform any action here, where we know that the block has been fully parsed */
		imyp_play_current_note (yytext + yyleng-3 + 1 /* skip the '#' */,
			&note_duration, curr, &play_result, 0, 0, bpm);
	}
	if ( imyp_sig_recvd != 0 )
	{
		return -1;
	}
}

<IN_REPEAT_BLOCK,IN_REPEAT_BLOCK_PASS>{SILENCE}		{

	/* update the line position to display the position of unknown tokens */
	repeat_pos = pos + yyleng;
	/* in a repeat block, collect all the elements to repeat and return to them
	   after looking throuhg the whole repeat block. */
	yymore ();
	if ( YY_START == IN_REPEAT_BLOCK_PASS /*repeat_count != -2*/ )
	{
		/* perform any action here, where we know that the block has been fully parsed */
		imyp_make_pause (yytext, curr, bpm);
	}
	if ( imyp_sig_recvd != 0 )
	{
		return -1;
	}
}

<IN_REPEAT_BLOCK,IN_REPEAT_BLOCK_PASS>{IGNORED_STATEMENTS}	{

	/* update the line position to display the position of unknown tokens */
	repeat_pos = pos + yyleng;
	/* in a repeat block, collect all the elements to repeat and return to them
	   after looking throuhg the whole repeat block. */
	yymore ();
	if ( YY_START == IN_REPEAT_BLOCK_PASS /*repeat_count != -2*/ )
	{
		/* perform any action here, where we know that the block has been fully parsed */
	}
	if ( imyp_sig_recvd != 0 )
	{
		return -1;
	}
}

<IN_REPEAT_BLOCK,IN_REPEAT_BLOCK_PASS>{OCTAVE_MODIFIER}	{

	/* update the line position to display the position of unknown tokens */
	repeat_pos = pos + yyleng;
	/* in a repeat block, collect all the elements to repeat and return to them
	   after looking throuhg the whole repeat block. */
	yymore ();
	if ( YY_START == IN_REPEAT_BLOCK_PASS /*repeat_count != -2*/ )
	{
		/* perform any action here, where we know that the block has been fully parsed */
		scanf_res = sscanf (yytext + yyleng-2+1, "%d", &octave);
		if ( scanf_res == 1 )
		{
			if ( octave < 0 )
			{
				octave = 0;
			}
			else if ( octave > 8 )
			{
				octave = 8;
			}
		}
		else
		{
			printf ("%s: '%c' (0x%x) %s %d:%d\n",
				_(err_unkn_token),
				*yytext,
				(unsigned int)*yytext,
				_(err_at_pos),
				yylineno,
				repeat_pos+1);
		}
	}
	if ( imyp_sig_recvd != 0 )
	{
		return -1;
	}
}

<IN_REPEAT_BLOCK,IN_REPEAT_BLOCK_PASS>{VOLUME_MODIFIER}	{

	/* update the line position to display the position of unknown tokens */
	repeat_pos = pos + yyleng;
	/* in a repeat block, collect all the elements to repeat and return to them
	   after looking throuhg the whole repeat block. */
	yymore ();
	if ( YY_START == IN_REPEAT_BLOCK_PASS /*repeat_count != -2*/ )
	{
		/* perform any action here, where we know that the block has been fully parsed */
		if ( *(yytext+yyleng-2+1) == '-' )
		{
			volume--;
		}
		else if ( *(yytext+yyleng-2+1) == '+' )
		{
			volume++;
		}
		if ( volume < 0 )
		{
			volume = 0;
		}
		else if ( volume > IMYP_MAX_IMY_VOLUME )
		{
			volume = IMYP_MAX_IMY_VOLUME;
		}
	}
	if ( imyp_sig_recvd != 0 )
	{
		return -1;
	}
}

<IN_REPEAT_BLOCK,IN_REPEAT_BLOCK_PASS>{VOLUME_VALUE}		{

	char * vol_ptr = NULL;
	char * last_v = strrchr (yytext, 'v');
	char * last_V = strrchr (yytext, 'V');
	if ( (last_v != NULL) && (last_V != NULL) )
	{
		vol_ptr = (last_v > last_V)? last_v : last_V;
	}
	else if ( last_v != NULL )
	{
		vol_ptr = last_v;
	}
	else if ( last_V != NULL )
	{
		vol_ptr = last_V;
	}
	/*printf ("Got new volume (value) in repeat block: '%s'\n", yytext);*/
	/* update the line position to display the position of unknown tokens */
	repeat_pos = pos + yyleng;
	/* in a repeat block, collect all the elements to repeat and return to them
	   after looking throuhg the whole repeat block. */
	yymore ();
	if ( YY_START == IN_REPEAT_BLOCK_PASS /*repeat_count != -2*/ )
	{
		/* perform any action here, where we know that the block has been fully parsed */
		if ( vol_ptr != NULL )
		{
			scanf_res = sscanf (vol_ptr+1, "%d", &volume);
			if ( scanf_res != 1 )
			{
				printf ("%s: %s.\n", _(err_parse_vol), yytext+8);
			}
			if ( volume < 0 )
			{
				volume = 0;
			}
			else if ( volume > IMYP_MAX_IMY_VOLUME )
			{
				volume = IMYP_MAX_IMY_VOLUME;
			}
		}
		else
		{
			printf ("%s: '%c' (0x%x) %s %d:%d\n",
				_(err_unkn_token),
				*yytext,
				(unsigned int)*yytext,
				_(err_at_pos),
				yylineno,
				repeat_pos+1);
		}
	}
	if ( imyp_sig_recvd != 0 )
	{
		return -1;
	}
}

<IN_REPEAT_BLOCK,IN_REPEAT_BLOCK_PASS>@[0-9]+		{

	if ( YY_START == IN_REPEAT_BLOCK /*repeat_count == -2*/ )
	{
		if ( sscanf (strchr (yytext, '@')+1, "%d", &repeat_count) == 1 )
		{
			if ( repeat_count == 0 ) repeat_count = -1;
		}
		else
		{
			printf ("%s: '%c' (0x%x) %s %d:%d\n",
				_(err_unkn_token),
				*yytext,
				(unsigned int)*yytext,
				_(err_at_pos),
				yylineno,
				repeat_pos+1);
		}
		/* perform initial volume setting */
		BEGIN (REPEAT_BLOCK_VOL);
	}
	else
	{
		/* do NOT perform any volume setting for the whole block */
		BEGIN (REPEAT_BLOCK_VOL_PASS);
	}
	/* in a repeat block, collect all the elements to repeat and return to them
	   after looking throuhg the whole repeat block. */
	yymore ();
	if ( imyp_sig_recvd != 0 )
	{
		return -1;
	}
}

<REPEAT_BLOCK_VOL,REPEAT_BLOCK_VOL_PASS>{VOLUME_MODIFIER}		{

	/* update the line position to display the position of unknown tokens */
	repeat_pos = pos + yyleng;
	/* in a repeat block, collect all the elements to repeat and return to them
	   after looking throuhg the whole repeat block. */
	yymore ();
	if ( YY_START == REPEAT_BLOCK_VOL /*repeat_count != -2*/ )
	{
		/* perform any action here, where we know that the block has been fully parsed */
		if ( *(yytext+yyleng-2+1) == '-' )
		{
			volume--;
		}
		else if ( *(yytext+yyleng-2+1) == '+' )
		{
			volume++;
		}
		if ( volume < 0 )
		{
			volume = 0;
		}
		else if ( volume > IMYP_MAX_IMY_VOLUME )
		{
			volume = IMYP_MAX_IMY_VOLUME;
		}
	}
	BEGIN (REPEAT_BLOCK_END);
	if ( imyp_sig_recvd != 0 )
	{
		return -1;
	}
}

<IN_REPEAT_BLOCK,REPEAT_BLOCK_VOL,REPEAT_BLOCK_VOL_PASS,REPEAT_BLOCK_END>")"	{

	if ( repeat_count == -1 )
	{
		/* infinite repeat: return everything to the stream, so we can catch the line again. */
		yyless (0);
		BEGIN (IN_REPEAT_BLOCK_PASS);
	}
	else
	{
		repeat_count--;
		if ( repeat_count > 0 )
		{
			/* return everything to the stream, so we can catch the line again. */
			yyless (0);
			BEGIN (IN_REPEAT_BLOCK_PASS);
		}
		else
		{
			repeat_count = -2;
			/* back to "normal" melody mode. */
			BEGIN (IN_MELODY_LINE);
			repeat_pos = 0;
		}
	}
	if ( imyp_sig_recvd != 0 )
	{
		return -1;
	}
}

<IN_REPEAT_BLOCK,REPEAT_BLOCK_VOL,REPEAT_BLOCK_END,IN_REPEAT_BLOCK_PASS,REPEAT_BLOCK_VOL_PASS>\n[^[:space:]]	{

	/* a newline NOT followed by whitespace means the end of the melody line. */
	BEGIN (IN_IMELODY);
	/* return everything to the stream, so we can catch the line again. */
	yyless (0);
	if ( imyp_sig_recvd != 0 )
	{
		return -1;
	}
}

 /* eat up newlines inside the melody and reset the line position: */
<IN_REPEAT_BLOCK,REPEAT_BLOCK_VOL,REPEAT_BLOCK_END,IN_REPEAT_BLOCK_PASS,REPEAT_BLOCK_VOL_PASS>[\n\r]		{

	pos = 0;
	/* in a repeat block, collect all the elements to repeat and return to them
	   after looking throuhg the whole repeat block. */
	yymore ();
	if ( imyp_sig_recvd != 0 )
	{
		return -1;
	}
}

 /* eat up whitespace inside the melody, but update the position: */
<IN_REPEAT_BLOCK,REPEAT_BLOCK_VOL,REPEAT_BLOCK_END,IN_REPEAT_BLOCK_PASS,REPEAT_BLOCK_VOL_PASS>[[:space:]]	{

	/* update the line position to display the position of unknown tokens */
	repeat_pos = pos + yyleng;
	/* in a repeat block, collect all the elements to repeat and return to them
	   after looking throuhg the whole repeat block. */
	yymore ();
	if ( imyp_sig_recvd != 0 )
	{
		return -1;
	}
}

 /* anything not processed up to here is an unknown token */
<IN_MELODY_LINE>.			{

	printf ("%s: '%c' (0x%x) %s %d:%d\n",
		_(err_unkn_token),
		*yytext,
		(unsigned int)*yytext,
		_(err_at_pos),
		yylineno,
		pos+1);
	/* update the line position to display the position of unknown tokens */
	pos += yyleng;
	if ( imyp_sig_recvd != 0 )
	{
		return -1;
	}
}

 /* anything not processed up to here is an unknown token */
<IN_REPEAT_BLOCK,REPEAT_BLOCK_VOL,REPEAT_BLOCK_END,IN_REPEAT_BLOCK_PASS,REPEAT_BLOCK_VOL_PASS>.		{

	printf ("%s: '%c' (0x%x) %s %d:%d\n",
		_(err_unkn_token),
		*yytext,
		(unsigned int)*yytext,
		_(err_at_pos),
		yylineno,
		repeat_pos+1);
	/* update the line position to display the position of unknown tokens */
	repeat_pos = pos + yyleng;
	/* in a repeat block, collect all the elements to repeat and return to them
	   after looking throuhg the whole repeat block. */
	yymore ();
	if ( imyp_sig_recvd != 0 )
	{
		return -1;
	}
}

<IN_IMELODY>^END:IMELODY		{

	BEGIN (INITIAL);
	ECHO;
	/* print a newline no matter if it was present in the file, so we can use just one pattern. */
	printf ("\n");
	if ( imyp_sig_recvd != 0 )
	{
		return -1;
	}
}

<IN_IMELODY>.|\n			{

	ECHO; /* echo unknown lines inside the melody block */
	if ( imyp_sig_recvd != 0 )
	{
		return -1;
	}
}

.|[\x00-\x1F]|[\x80-\xFF]		{
/* NOTE: using just ".|\x00" is NOT enough */

	fflush(stdout);
	/* eat up everything before "BEGIN:IMELODY" or after "END:IMELODY" */
	if ( imyp_sig_recvd != 0 )
	{
		return -1;
	}
}

%%

/* ======================================================================== */

/**
 * Plays the given IMY file.
 * \param file_name The name of the file to play.
 * \return 0 on success
 */
int
#ifdef IMYP_ANSIC
IMYP_ATTR((nonnull))
#endif
imyp_play_file (
#ifdef IMYP_ANSIC
	const char * const file_name, imyp_backend_t * const curr_lib)
#else
	file_name, curr_lib)
	const char * const file_name;
	imyp_backend_t * const curr_lib;
#endif
{
	FILE * imy;
	if ( (file_name == NULL) || (curr_lib == NULL) )
	{
		return -100;
	}

	/* re-initialize the parser for each file: */
	volume = 7;
	style = 0;
	bpm = IMYP_DEF_BPM;
	octave = 4;

	pos = 0;
	repeat_count = 0;
	repeat_pos = 0;

	note_duration = 0;

	if ( curr_lib->imyp_curr_lib != IMYP_CURR_NONE )
	{
		imy = fopen (file_name, "r");
		if ( imy != NULL )
		{
			curr = curr_lib;
			yyin = imy;
			yylex ();
			/* file parsing done, free all memory: */
			yylex_destroy ();
			fclose (imy);
			return 0;
		}
		else
		{
			printf ("%s %s.\n", _(err_file_open), file_name);
			return -2;
		}
	}
	return -1;
}
