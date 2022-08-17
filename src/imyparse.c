/*
 * A program for playing iMelody ringtones (IMY files).
 *	-- melody parsing file.
 *
 * Copyright (C) 2009-2021 Bogdan Drozdowski, bogdro (at) users.sourceforge.net
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

# ifdef __GNUC__
#  pragma GCC poison strcpy strcat
# endif

static unsigned short int buf16[IMYP_SAMPBUFSIZE];

static int volume;
static int style;
static int bpm;
static int octave;
static char melody_line[1024+10];	/* the 10 extra to avoid overflows */
static int melody_index;
static int is_sharp;
static int is_flat;

static int is_repeat;
static int repeat_count;
static unsigned int is_repeat_forever;
#if (defined HAVE_FSEEKO) && (defined HAVE_FTELLO)
static off_t repeat_start_pos;
#else
static long int repeat_start_pos;
#endif
static int is_eof;

static const char IMYP_MEL_END[] = "END:IMELODY"; /* a #define produces warnings */

#ifdef TEST_COMPILE
# undef IMYP_ANSIC
# if TEST_COMPILE > 1
#  undef HAVE_MALLOC
# endif
#endif

/* ======================================================================== */

#ifndef IMYP_ANSIC
static void imyp_read_line IMYP_PARAMS ((char * const buffer, int * const buf_index,
	const size_t bufsize, FILE * const imyfile, const unsigned int min_current));
#endif

/**
 * Ensures that the data buffer has data. Reads a new line from the file if necessary.
 * \param buffer The buffer to check/fill.
 * \param buf_index Currently used index to the buffer.
 * \param bufsize The size of the whole buffer.
 * \param imyfile The file to read additional data from if necessary.
 * \param min_current The minimum number of characters required in the buffer.
 */
static void
#ifdef IMYP_ANSIC
IMYP_ATTR((nonnull))
#endif
imyp_read_line (
#ifdef IMYP_ANSIC
	char * const buffer,
	int * const buf_index,
	const size_t bufsize,
	FILE * const imyfile,
	const unsigned int min_current)
#else
	buffer, buf_index, bufsize, imyfile, min_current)
	char * const buffer;
	int * const buf_index;
	const size_t bufsize;
	FILE * const imyfile;
	const unsigned int min_current;
#endif
{
	char * res;
	char tmpbuf[13+1];	/* for "END:IMELODY" */
#if (defined HAVE_FSEEKO) && (defined HAVE_FTELLO)
	off_t curr_pos = 0;
#else
	long int curr_pos = 0;
#endif
	char buf2[2];
	size_t curr_buflen;
	int remainder;

	if ( (buffer == NULL) || (buf_index == NULL) || (imyfile == NULL) || (bufsize == 0) )
	{
		return;
	}
	if ( feof (imyfile) )
	{
		is_eof = 1;
	}
	while ( (imyp_sig_recvd == 0) && (is_eof == 0) )
	{
		while ( (((*buf_index) < 0) || (unsigned int)(*buf_index) + min_current >= strlen (buffer))
			&& (is_eof == 0) && (imyp_sig_recvd == 0) )
		{
			remainder = (int)(strlen (buffer) - (unsigned int)(*buf_index));
			if ( remainder > 0 )
			{
				/* there is something left in the buffer - move it to the beginning. */
				memmove (buffer, &buffer[*buf_index], (unsigned int)remainder);
				buffer[remainder] = '\0';
			}
			if ( bufsize > 1 )
			{
				res = fgets (&buffer[remainder], (int) bufsize - remainder, imyfile);
				buffer[bufsize-1] = '\0';
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
			if ( feof (imyfile) )
			{
				is_eof = 1;
			}
			*buf_index = 0;
			curr_buflen = strlen (buffer);
			if ( (curr_buflen >= 11 /* strlen (IMYP_MEL_END) */)
				&& (strstr (buffer, IMYP_MEL_END) == buffer) )
			{
				printf ("%s", buffer);
				is_eof = 1;
				break;
			}
			if ( strncmp (buffer, IMYP_MEL_END, curr_buflen) == 0 )
			{
				/*
				   read enough bytes from the file to determine
				   if we're at the end of the melody, or we're just
				   starting another 'E'-note, for example
				*/
				strncpy (tmpbuf, buffer, IMYP_MIN (curr_buflen+1, sizeof (tmpbuf)-1) );
#if (defined HAVE_FSEEKO) && (defined HAVE_FTELLO)
				curr_pos = ftello (imyfile)
					/* substract the already-read line */
					- (off_t)curr_buflen
					/* add the offset of the current char: */
					+ (off_t)*buf_index;
#else
				curr_pos = ftell (imyfile)
					/* substract the already-read line */
					- (long int)curr_buflen
					/* add the offset of the current char: */
					+ *buf_index;
#endif
				res = fgets (&tmpbuf[curr_buflen],
					(int)(sizeof (tmpbuf)-1 - curr_buflen),
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
				tmpbuf[sizeof (tmpbuf)-1] = '\0';
				if ( strstr (tmpbuf, IMYP_MEL_END) != NULL )
				{
					printf ("%s", tmpbuf);
					is_eof = 1;
					break;
				}
			}
		}
		if ( is_eof != 0 )
		{
			break;
		}
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

#ifndef IMYP_ANSIC
static int imyp_read_number IMYP_PARAMS ((char * const buffer, int * const buf_index,
	const size_t bufsize, FILE * const imyfile));
#endif

/**
 * Reads a number from a file. Reads a new line from the file if necessary.
 * \param buffer The buffer to fill wit file data, if necessary.
 * \param buf_index Currently used index to the buffer.
 * \param bufsize The size of the whole buffer.
 * \param imyfile The file to read additional data from if necessary.
 * \return the number read.
 */
static int
#ifdef IMYP_ANSIC
IMYP_ATTR((nonnull))
#endif
imyp_read_number (
#ifdef IMYP_ANSIC
	char * const buffer,
	int * const buf_index,
	const size_t bufsize,
	FILE * const imyfile)
#else
	buffer, buf_index, bufsize, imyfile)
	char * const buffer;
	int * const buf_index;
	const size_t bufsize;
	FILE * const imyfile;
#endif
{
	int ret = 0, prev_ret = 0;
	int scanf_res;

	if ( (buffer == NULL) || (buf_index == NULL) || (imyfile == NULL) || (bufsize == 0) )
	{
		return ret;
	}
	imyp_read_line (buffer, buf_index, bufsize, imyfile, 0);
	while ( IMYP_IS_DIGIT (buffer[*buf_index]) )
	{
		scanf_res = sscanf (&buffer[*buf_index], "%d", &ret);
		if ( scanf_res == 1 )
		{
			/* skip the correct number of characters */
			if ( ret >= 1000000000 )
			{
				(*buf_index)++;
			}
			if ( ret >= 100000000 )
			{
				(*buf_index)++;
			}
			if ( ret >= 10000000 )
			{
				(*buf_index)++;
			}
			if ( ret >= 1000000 )
			{
				(*buf_index)++;
			}
			if ( ret >= 100000 )
			{
				(*buf_index)++;
			}
			if ( ret >= 10000 )
			{
				(*buf_index)++;
			}
			if ( ret >= 1000 )
			{
				(*buf_index)++;
			}
			if ( ret >= 100 )
			{
				(*buf_index)++;
			}
			if ( ret >= 10 )
			{
				(*buf_index)++;
			}
			/* always skip at least one */
			(*buf_index)++;
			if ( ret >= 1000000000 )
			{
				ret = /*prev_ret * 10000000000 +*/ ret;
			}
			else if ( ret >= 100000000 )
			{
				ret = prev_ret * 1000000000 + ret;
			}
			else if ( ret >= 10000000 )
			{
				ret = prev_ret * 100000000 + ret;
			}
			else if ( ret >= 1000000 )
			{
				ret = prev_ret * 10000000 + ret;
			}
			else if ( ret >= 100000 )
			{
				ret = prev_ret * 1000000 + ret;
			}
			else if ( ret >= 10000 )
			{
				ret = prev_ret * 100000 + ret;
			}
			else if ( ret >= 1000 )
			{
				ret = prev_ret * 10000 + ret;
			}
			else if ( ret >= 100 )
			{
				ret = prev_ret * 1000 + ret;
			}
			else if ( ret >= 10 )
			{
				ret = prev_ret * 100 + ret;
			}
			else
			{
				ret = prev_ret * 10 + ret;
			}
		}
		prev_ret = ret;
		imyp_read_line (buffer, buf_index, bufsize, imyfile, 0);
		/* read the next part of the number */
	}
	return ret;
}

/* ======================================================================== */

#ifndef IMYP_ANSIC
static int imyp_get_duration IMYP_PARAMS ((char * const note_buffer, int * const how_many_skipped,
	int current_bpm, FILE * const imyfile));
#endif

/**
 * Gets the duration of the note pointed to by note_buffer.
 * \param note_buffer A character buffer with the note's description.
 * \param how_many_skipped Will get the number of characters used from the buffer.
 * \param current_bpm The current BPM value.
 * \param imyfile The file to read additional data from if necessary.
 * \return a duration for the first note in the buffer, in milliseconds.
 */
static int
#ifdef IMYP_ANSIC
IMYP_ATTR((nonnull))
#endif
imyp_get_duration (
#ifdef IMYP_ANSIC
	char * const note_buffer,
	int * const how_many_skipped,
	int current_bpm,
	FILE * const imyfile)
#else
	note_buffer, how_many_skipped, current_bpm, imyfile)
	char * const note_buffer;
	int * const how_many_skipped;
	int current_bpm;
	FILE * const imyfile;
#endif
{
	int imyp_index = 0;
	float duration = 0.0f;
	unsigned int dur_shift = 0;
	size_t note_buf_len;

	if ( (note_buffer == NULL) || (imyfile == NULL) )
	{
		if ( how_many_skipped != NULL )
		{
			*how_many_skipped = 0;
		}
		return 0;
	}
	imyp_read_line (note_buffer, &imyp_index, strlen (note_buffer), imyfile, 0);
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
		&& (imyp_sig_recvd == 0)
	)
	{
		imyp_index++;
		imyp_read_line (note_buffer, &imyp_index, strlen (note_buffer), imyfile, 0);
	}
	if ( (imyp_sig_recvd != 0) || (is_eof != 0) )
	{
		return 0;
	}
	if ( ! IMYP_IS_DIGIT (note_buffer[imyp_index]) )
	{
		if ( how_many_skipped != NULL )
		{
			*how_many_skipped = imyp_index;
		}
		/* not a note */
		return 0;
	}

	sscanf (&note_buffer[imyp_index], "%1u", &dur_shift);
	imyp_index++;
	note_buf_len = strlen (note_buffer);
	imyp_read_line (note_buffer, &imyp_index, note_buf_len, imyfile, 0);
	if ( dur_shift == 0 )
	{
		duration = 1000.0f;
	}
	else
	{
		duration = 1000.0f/(float)(1<<dur_shift);
	}
	if ( note_buffer[imyp_index] == '.' )
	{
		duration *= 1.5f;
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
	imyp_read_line (note_buffer, &imyp_index, note_buf_len, imyfile, 0);
	if ( how_many_skipped != NULL )
	{
		*how_many_skipped = imyp_index;
	}
	if ( current_bpm == 0 )
	{
		current_bpm = IMYP_DEF_BPM;
	}
	return (int)IMYP_ROUND (duration * ((1.0f*IMYP_DEF_BPM)/(1.0f*(double)current_bpm)));
}

/* ======================================================================== */

#ifndef IMYP_ANSIC
static void imyp_play_current_note IMYP_PARAMS ((FILE * const imy, int * const skipped_dur,
	int * const note_duration, const int note_index, imyp_backend_t * const curr,
	int * const play_res));
#endif

/**
 * Play the given note. Also resets the sharp and flat flags.
 * \param[in] imy The file to get any additional data from.
 * \param[out] skipped_dur Will get the number of characters used from the buffer, if any.
 * \param[out] note_duration The duration of the note.
 * \param[in] note_index The index of the note in the array ('C'=0, 'B'=11).
 * \param[in] curr The current output backend.
 * \param[out] play_res The result of playing the note.
 */
static void
#ifdef IMYP_ANSIC
IMYP_ATTR((nonnull))
#endif
imyp_play_current_note (
#ifdef IMYP_ANSIC
	FILE * const imy, int * const skipped_dur, int * const note_duration,
	const int note_index, imyp_backend_t * const curr, int * const play_res)
#else
	imy, skipped_dur, note_duration, note_index, curr, play_res)
	FILE * const imy;
	int * const skipped_dur;
	int * const note_duration;
	const int note_index;
	imyp_backend_t * const curr;
	int * const play_res;
#endif
{
	if ( (imy == NULL) || (skipped_dur == NULL) || (note_duration == NULL)
		|| (curr == NULL) || (play_res == NULL) || (curr == NULL) )
	{
		return;
	}
	if ( curr->imyp_curr_lib == IMYP_CURR_NONE )
	{
		return;
	}

	melody_index++;
	imyp_read_line (melody_line, &melody_index,
		sizeof (melody_line) - 10, imy, 0);
	*skipped_dur = 0;
	*note_duration = imyp_get_duration (
		&melody_line[melody_index],
		skipped_dur,
		bpm, imy);
	if ( (is_flat != 0) && (note_index == 0) && (octave != 0) )
	{
		/* flat 'C' note, but not the first 'C' note */
		*play_res = imyp_play_tune (imyp_notes[octave-1][IMYP_NOTES_PER_OCTAVE-1],
			volume, *note_duration,
			buf16, IMYP_SAMPBUFSIZE, curr);
		if ( (*play_res != 0) && (imyp_sig_recvd == 0) )
		{
			printf ("%s\n",
				_(err_play_tune));
		}
	}
	else if ( (is_flat != 0) && (note_index != 0) )
	{
		/* flat note and not a 'C' note */
		*play_res = imyp_play_tune (imyp_notes[octave][note_index-1],
			volume, *note_duration,
			buf16, IMYP_SAMPBUFSIZE, curr);
		if ( (*play_res != 0) && (imyp_sig_recvd == 0) )
		{
			printf ("%s\n",
				_(err_play_tune));
		}
	}
	else if ( (is_sharp != 0) && (note_index == IMYP_NOTES_PER_OCTAVE-1)
		&& (octave < IMYP_OCTAVES) )
	{
		/* sharp 'B' note and not the last 'B' note */
		*play_res = imyp_play_tune (imyp_notes[octave+1][0],
			volume, *note_duration,
			buf16, IMYP_SAMPBUFSIZE, curr);
		if ( (*play_res != 0) && (imyp_sig_recvd == 0) )
		{
			printf ("%s\n",
				_(err_play_tune));
		}
	}
	else if ( (is_sharp != 0) && (note_index != IMYP_NOTES_PER_OCTAVE-1) )
	{
		/* sharp note and not a 'B' note */
		*play_res = imyp_play_tune (imyp_notes[octave][note_index+1],
			volume, *note_duration,
			buf16, IMYP_SAMPBUFSIZE, curr);
		if ( (*play_res != 0) && (imyp_sig_recvd == 0) )
		{
			printf ("%s\n",
				_(err_play_tune));
		}
	}
	else
	{
		/* Normal note. We also get here on the first 'C', when
		   the flat flag is on or on the last 'B' when the
		   sharp flag is on. */
		*play_res = imyp_play_tune (imyp_notes[octave][note_index],
			volume, *note_duration,
			buf16, IMYP_SAMPBUFSIZE, curr);
		if ( (*play_res != 0) && (imyp_sig_recvd == 0) )
		{
			printf ("%s\n",
				_(err_play_tune));
		}
	}
	imyp_pause (imyp_get_rest_time (*note_duration,
		style), curr, 1, buf16, IMYP_SAMPBUFSIZE);
	melody_index += *skipped_dur;
	imyp_read_line (melody_line, &melody_index,
		sizeof (melody_line) - 10, imy, 0);
	is_sharp = 0;
	is_flat = 0;
}

/* ======================================================================== */

#ifndef IMYP_ANSIC
static int imyp_check_string IMYP_PARAMS ((const char string[]));
#endif

/**
 * Finds and skips the given string.
 * \param[in] string The string to find and skip. It is assumed the whole
 *	string is in the 'melody_line' buffer, starting at 'melody_index'.
 * \return 1 if the full string has been found, 0 otherwise.
 */
static int
#ifdef IMYP_ANSIC
IMYP_ATTR((nonnull))
#endif
imyp_check_string (
#ifdef IMYP_ANSIC
	const char string[])
#else
	string)
	const char string[];
#endif
{
	size_t i, len;
	int j;

	if ( string == NULL )
	{
		return 0;
	}

	len = strlen (string);
	for ( i = 0, j = 0; i < len; i++, j++ )
	{
		if ( IMYP_TOUPPER (melody_line[melody_index+j])
			!= IMYP_TOUPPER (string[i]) )
		{
			return 0;
		}
	}
	return 1;
}

/* ======================================================================== */

#ifndef IMYP_ANSIC
static void imyp_show_token_error IMYP_PARAMS ((
	const char line[], const int pos, const char msg[]));
#endif

/**
 * Shows the "unexpected/unknown token" error.
 * \param[in] line the current 'melody_line'.
 * \param[in] pos the position of the token in the current 'melody_line'.
 */
static void
#ifdef IMYP_ANSIC
IMYP_ATTR((nonnull))
#endif
imyp_show_token_error (
#ifdef IMYP_ANSIC
	const char line[], const int pos, const char msg[])
#else
	line, pos, msg)
	const char line[];
	const int pos;
	const char msg[];
#endif
{
	if ( (line == NULL) || (pos < 0) )
	{
		return;
	}
	if ( (unsigned int)pos < strlen (line) )
	{
		printf ("%s: '%c' (0x%x) %s %d: '%s'\n",
			msg,
			line[pos],
			(unsigned int)line[pos],
			_(err_at_pos),
			pos,
			line);
	}
}

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
	const char * const file_name, imyp_backend_t * const curr)
#else
	file_name, curr)
	const char * const file_name;
	imyp_backend_t * const curr;
#endif
{
	FILE * imy;
	char * res;
	int first_line = 1;
	int scanf_res;
	int skipped_dur;	/* has to be signed */
	int play_res;
	int note_duration = 0;
#ifndef HAVE_MEMSET
	size_t i;
#endif

	if ( (file_name == NULL) || (curr == NULL) )
	{
		return -100;
	}
#ifdef HAVE_MEMSET
	memset (melody_line, 0, sizeof (melody_line));
#else
	for ( i = 0; i < sizeof (melody_line); i++ )
	{
		melody_line[i] = '\0';
	}
#endif
	/* re-initialize the parser for each file: */
	volume = 7;
	style = 0;
	bpm = IMYP_DEF_BPM;
	octave = 4;
	melody_index = 0;
	is_sharp = 0;
	is_flat = 0;

	is_repeat = 0;
	repeat_count = 0;
	is_repeat_forever = 0;
	repeat_start_pos = 0;
	is_eof = 0;

	imy = fopen (file_name, "r");
	if ( imy != NULL )
	{
		do
		{
			res = fgets (melody_line, sizeof (melody_line)-1, imy);
			melody_line[sizeof (melody_line) - 1] = '\0';
			if ( res == NULL )
			{
				is_eof = 1;
				break;
			}
			else if ( melody_line[0] == '\0' /*strlen (melody_line) == 0*/ )
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
				scanf_res = sscanf (&melody_line[5] /* "BEAT:" */, "%d", &bpm);
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
				scanf_res = sscanf (&melody_line[8] /* "VOLUME:" */, "%d", &volume);
				if ( scanf_res != 1 )
				{
					printf ("%s: %s.\n", _(err_parse_vol), melody_line);
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
			}
			else if ( strstr (melody_line, "MELODY:") != NULL )
			{
				melody_index = 7;
				do
				{
					if ( is_eof != 0 )
					{
						break;
					}
					/* get a line from the file and skip the whitespace */
					imyp_read_line (melody_line, &melody_index,
						sizeof (melody_line) - 10, imy, 0);
					if ( imyp_sig_recvd != 0 )
					{
						is_eof = 1;
					}
					if ( is_eof != 0 )
					{
						break;
					}
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
								sizeof (melody_line) - 10, imy, 0);
							if ( IMYP_TOUPPER (melody_line[melody_index]) == 'I' )
							{
								/* put the whole string in the buffer */
								melody_index++;
								imyp_read_line (melody_line, &melody_index,
									sizeof (melody_line) - 10, imy, 5);
								if ( imyp_check_string ("BEON") == 1 )
								{
									melody_index += 4 /*strlen(BEON)*/;
									imyp_read_line (melody_line,
										&melody_index,
										sizeof (melody_line) - 10,
										imy, 0);
								}
								else if ( imyp_check_string ("BEOFF") == 1 )
								{
									melody_index += 5 /*strlen(BEOFF)*/;
									imyp_read_line (melody_line,
										&melody_index,
										sizeof (melody_line) - 10,
										imy, 0);
								}
								else
								{
									/* neither string not found
									   - syntax error. */
									imyp_show_token_error (
										melody_line,
										melody_index,
										_(err_unkn_token));
									melody_index++;
									imyp_read_line (melody_line,
										&melody_index,
										sizeof (melody_line) - 10,
										imy, 0);
								}
								is_sharp = 0;
								is_flat = 0;
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
									sizeof (melody_line) - 10, imy, 0);
								if ( volume < 0 )
								{
									volume = 0;
								}
								else if ( volume > IMYP_MAX_IMY_VOLUME )
								{
									volume = IMYP_MAX_IMY_VOLUME;
								}
							}
							is_sharp = 0;
							is_flat = 0;
							break;
						case 'c':
						case 'C':
							imyp_play_current_note (imy, &skipped_dur,
								&note_duration, 0, curr, &play_res);
							break;
						case 'd':
						case 'D':
							imyp_play_current_note (imy, &skipped_dur,
								&note_duration, 2, curr, &play_res);
							break;
						case 'e':
						case 'E':
							imyp_play_current_note (imy, &skipped_dur,
								&note_duration, 4, curr, &play_res);
							break;
						case 'f':
						case 'F':
							imyp_play_current_note (imy, &skipped_dur,
								&note_duration, 5, curr, &play_res);
							break;
						case 'g':
						case 'G':
							imyp_play_current_note (imy, &skipped_dur,
								&note_duration, 7, curr, &play_res);
							break;
						case 'a':
						case 'A':
							imyp_play_current_note (imy, &skipped_dur,
								&note_duration, 9, curr, &play_res);
							break;
						case 'b':
						case 'B':
							/* skip "backon/off": */
							melody_index++;
							imyp_read_line (melody_line, &melody_index,
								sizeof (melody_line) - 10, imy, 0);
							if ( IMYP_TOUPPER (melody_line[melody_index]) == 'A' )
							{
								melody_index++;
								imyp_read_line (melody_line, &melody_index,
									sizeof (melody_line) - 10, imy, 5);
								if ( imyp_check_string ("CKON") == 1 )
								{
									melody_index += 4 /*strlen(CKON)*/;
									imyp_read_line (melody_line,
										&melody_index,
										sizeof (melody_line) - 10,
										imy, 0);
								}
								else if ( imyp_check_string ("CKOFF") == 1 )
								{
									melody_index += 5 /*strlen(BEOFF)*/;
									imyp_read_line (melody_line,
										&melody_index,
										sizeof (melody_line) - 10,
										imy, 0);
								}
								else
								{
									/* neither string not found
									   - syntax error. */
									imyp_show_token_error (
										melody_line,
										melody_index,
										_(err_unkn_token));
									melody_index++;
									imyp_read_line (melody_line,
										&melody_index,
										sizeof (melody_line) - 10,
										imy, 0);
								}
								is_sharp = 0;
								is_flat = 0;
							}
							/* read note */
							else
							{
								imyp_play_current_note (imy, &skipped_dur,
									&note_duration, 11, curr, &play_res);
							}
							break;
						case '&':
							melody_index++;
							imyp_read_line (melody_line, &melody_index,
								sizeof (melody_line) - 10, imy, 0);
							is_flat = 1;
							is_sharp = 0;
							break;
						case '#':
							melody_index++;
							imyp_read_line (melody_line, &melody_index,
								sizeof (melody_line) - 10, imy, 0);
							is_sharp = 1;
							is_flat = 0;
							break;
						case '*':
							melody_index++;
							imyp_read_line (melody_line, &melody_index,
								sizeof (melody_line) - 10, imy, 0);
							sscanf (&melody_line[melody_index], "%1d", &octave);
							melody_index++;
							imyp_read_line (melody_line, &melody_index,
								sizeof (melody_line) - 10, imy, 0);
							if ( octave < 0 )
							{
								octave = 0;
							}
							else if ( octave > 8 )
							{
								octave = 8;
							}
							is_sharp = 0;
							is_flat = 0;
							break;
						case 'r':
						case 'R':
							melody_index++;
							imyp_read_line (melody_line, &melody_index,
								sizeof (melody_line) - 10, imy, 0);
							imyp_pause (imyp_get_duration (
									&melody_line[melody_index],
									&skipped_dur,
									bpm, imy), curr, 0, buf16,
									IMYP_SAMPBUFSIZE);
							melody_index += skipped_dur;
							imyp_read_line (melody_line, &melody_index,
								sizeof (melody_line) - 10, imy, 0);
							is_sharp = 0;
							is_flat = 0;
							break;
						case 'l':
						case 'L':
							/* skip "ledon/off" */
							melody_index++;
							imyp_read_line (melody_line, &melody_index,
								sizeof (melody_line) - 10, imy, 5);
							if ( imyp_check_string ("EDON") == 1 )
							{
								melody_index += 4 /*strlen(EDON)*/;
								imyp_read_line (melody_line,
									&melody_index,
									sizeof (melody_line) - 10,
									imy, 0);
							}
							else if ( imyp_check_string ("EDOFF") == 1 )
							{
								melody_index += 5 /*strlen(EDOFF)*/;
								imyp_read_line (melody_line,
									&melody_index,
									sizeof (melody_line) - 10,
									imy, 0);
							}
							else
							{
								/* neither string not found
									- syntax error. */
								imyp_show_token_error (
									melody_line,
									melody_index,
									_(err_unkn_token));
								melody_index++;
								imyp_read_line (melody_line,
									&melody_index,
									sizeof (melody_line) - 10,
									imy, 0);
							}
							is_sharp = 0;
							is_flat = 0;
							break;
						case '(':
							if ( is_repeat != 0 )
							{
								imyp_show_token_error (
									melody_line,
			       						melody_index,
									_(err_unex_token));
							}
							else
							{
								is_repeat = 1;
#if (defined HAVE_FSEEKO) && (defined HAVE_FTELLO)
								repeat_start_pos = ftello (imy)
									/* substract the already-read line */
									- (off_t)strlen (melody_line)
									/* add the offset of '(': */
									+ (off_t)melody_index
									/* skip the '(' itself: */
									+ 1;
#else
								repeat_start_pos = ftell (imy)
									/* substract the already-read line */
									- (long int)strlen (melody_line)
									/* add the offset of '(': */
									+ (long int)melody_index
									/* skip the '(' itself: */
									+ 1;
#endif
								/* find the repeat count: */
								while ( (melody_line[melody_index] != '@')
									&& (imyp_sig_recvd == 0) && (is_eof == 0)
								 )
								{
									melody_index++;
									imyp_read_line (melody_line,
										&melody_index,
										sizeof (melody_line) - 10,
										imy, 0);
									if ( strstr (melody_line, IMYP_MEL_END)
										== melody_line )
									{
										printf ("%s", melody_line);
										is_eof = 1;
										break;
									}
								}
								if (imyp_sig_recvd != 0)
								{
									is_eof = 1;
									break;
								}
								if ( is_eof != 0 )
								{
									break;
								}
								melody_index++; /* skip the '@' */
								repeat_count = imyp_read_number (
									melody_line,
									&melody_index,
									(size_t)(sizeof (melody_line)
									- 10), imy);
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
									imy, 0);
								if ( IMYP_TOUPPER
									(melody_line[melody_index])
									== 'V' )
								{
									melody_index++;
									imyp_read_line (melody_line,
										&melody_index,
										sizeof (melody_line) - 10,
										imy, 0);
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
									if ( volume < 0 )
									{
										volume = 0;
									}
									else if ( volume > IMYP_MAX_IMY_VOLUME )
									{
										volume = IMYP_MAX_IMY_VOLUME;
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
									imy, 0);
							}
							is_sharp = 0;
							is_flat = 0;
							break;
						case ')':
							if ( is_repeat == 0 )
							{
								imyp_show_token_error (
									melody_line,
			       						melody_index,
									_(err_unex_token));
								melody_index++;
								imyp_read_line (melody_line,
									&melody_index,
									sizeof (melody_line) - 10,
									imy, 0);
							}
							else if ( repeat_count == 0 )
							{
								is_repeat = 0;
							}
							is_sharp = 0;
							is_flat = 0;
							break;
						case '@':
							if ( is_repeat == 0 )
							{
								imyp_show_token_error (
									melody_line,
			       						melody_index,
									_(err_unex_token));
								melody_index++;
								imyp_read_line (melody_line,
									&melody_index,
									sizeof (melody_line) - 10,
									imy, 0);
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
										&& (imyp_sig_recvd == 0) && (is_eof == 0)
									)
									{
										melody_index++;
										imyp_read_line (melody_line,
											&melody_index,
											sizeof (melody_line) - 10,
											imy, 0);
									}
									if ( imyp_sig_recvd != 0 )
									{
										is_eof = 1;
										break;
									}
									/* skip the ')' itself: */
									melody_index++;
									imyp_read_line (melody_line,
										&melody_index,
										sizeof (melody_line) - 10,
										imy, 0);
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
										imy, 0);
								}
							}
							is_sharp = 0;
							is_flat = 0;
							break;
						default:
							imyp_show_token_error (
								melody_line,
								melody_index,
								_(err_unkn_token));
							melody_index++;
							imyp_read_line (melody_line,
								&melody_index,
								sizeof (melody_line) - 10,
								imy, 0);
							is_sharp = 0;
							is_flat = 0;
							break;
					} /* switch */
				} while ((is_eof == 0) && (imyp_sig_recvd == 0));
			} /* else if ( strstr (melody_line, "MELODY:") != NULL ) */
		} while ((is_eof == 0) && (imyp_sig_recvd == 0));
		fclose (imy);
		return 0;
	}
	else
	{
		printf ("%s %s.\n", _(err_file_open), file_name);
		return -4;
	}
}
