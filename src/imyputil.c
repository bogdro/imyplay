/*
 * A program for playing iMelody ringtones (IMY files).
 *	-- utility functions.
 *
 * Copyright (C) 2012-2018 Bogdan Drozdowski, bogdandr (at) op.pl
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

#include <stdio.h>	/* NULL */
#ifndef NULL
# define NULL ((void *)0)
#endif

#ifdef HAVE_STRING_H
# if ((!defined STDC_HEADERS) || (!STDC_HEADERS)) && (defined HAVE_MEMORY_H)
#  include <memory.h>
# endif
# include <string.h>
#endif

#ifdef HAVE_MALLOC_H
# include <malloc.h>
#endif

#ifdef HAVE_MATH_H
# include <math.h>	/* sin() */
#endif

#ifndef M_PI
# define M_PI 3.14159265358979323846
#endif

/* select() the old way */
#if TIME_WITH_SYS_TIME
# include <sys/time.h>
# include <time.h>
#else
# if HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  ifdef HAVE_TIME_H
#   include <time.h>
#  endif
# endif
#endif

#ifdef HAVE_UNISTD_H
# include <unistd.h>	/* select() (the old way) */
#endif

/* select () - the new way */
#ifdef HAVE_SYS_SELECT_H
# include <sys/select.h>
#endif

#include "imyplay.h"
#include "imyputil.h"
#include "imyp_sig.h"

/**
 * Comapres the give strings case-insensitively.
 * \param string1 The first string.
 * \param string2 The second string.
 * \return 0 if the strings are equal, -1 is string1 is "less" than string2 and 1 otherwise.
 */
int
imyp_compare (
#ifdef IMYP_ANSIC
	const char string1[], const char string2[])
#else
	string1, string2)
	const char string1[];
	const char string2[];
#endif
{
	size_t i, len1, len2;
	char c1, c2;

	if ( (string1 == NULL) && (string2 == NULL) )
	{
		return 0;
	}
	else if ( string1 == NULL )
	{
		return -1;
	}
	else if ( string2 == NULL )
	{
		return 1;
	}
	else
	{
		/* both strings not-null */
		len1 = strlen (string1);
		len2 = strlen (string2);
		if ( len1 < len2 )
		{
			return -1;
		}
		else if ( len1 > len2 )
		{
			return 1;
		}
		else
		{
			/* both lengths equal */
			for ( i = 0; i < len1; i++ )
			{
				c1 = IMYP_TOUPPER (string1[i]);
				c2 = IMYP_TOUPPER (string2[i]);
				if ( c1 < c2 )
				{
					return -1;
				}
				else if ( c1 > c2 )
				{
					return 1;
				}
			}
		}
	}
	return 0;
}

/**
 * Checks the given string for an output system name and returns the enum value
 *	that corresponds to the given name.
 * \param system_name The name to check.
 * \return the enum value that corresponds to the given name.
 */
enum IMYP_CURR_LIB
imyp_parse_system (
#ifdef IMYP_ANSIC
	const char system_name[])
#else
	system_name)
	const char system_name[];
#endif
{
	if ( system_name == NULL )
	{
		return IMYP_CURR_NONE;
	}

	if ( (imyp_compare (system_name, "allegro") == 0)
		|| (imyp_compare (system_name, "all") == 0) )
	{
		return IMYP_CURR_ALLEGRO;
	}
	else if ( (imyp_compare (system_name, "midi") == 0)
		|| (imyp_compare (system_name, "mid") == 0) )
	{
		return IMYP_CURR_MIDI;
	}
	else if ( imyp_compare (system_name, "sdl") == 0 )
	{
		return IMYP_CURR_SDL;
	}
	else if ( imyp_compare (system_name, "alsa") == 0 )
	{
		return IMYP_CURR_ALSA;
	}
	else if ( imyp_compare (system_name, "oss") == 0 )
	{
		return IMYP_CURR_OSS;
	}
	else if ( (imyp_compare (system_name, "libao") == 0)
		|| (imyp_compare (system_name, "ao") == 0) )
	{
		return IMYP_CURR_LIBAO;
	}
	else if ( (imyp_compare (system_name, "portaudio") == 0)
		|| (imyp_compare (system_name, "port") == 0) )
	{
		return IMYP_CURR_PORTAUDIO;
	}
	else if ( imyp_compare (system_name, "jack") == 0 )
	{
		return IMYP_CURR_JACK;
	}
	else if ( (imyp_compare (system_name, "pulseaudio") == 0)
		|| (imyp_compare (system_name, "pulse") == 0) )
	{
		return IMYP_CURR_PULSEAUDIO;
	}
	else if ( (imyp_compare (system_name, "exec") == 0)
		|| (imyp_compare (system_name, "exe") == 0) )
	{
		return IMYP_CURR_EXEC;
	}
	else if ( (imyp_compare (system_name, "gstreamer") == 0)
		|| (imyp_compare (system_name, "gst") == 0) )
	{
		return IMYP_CURR_GSTREAMER;
	}
	else if ( imyp_compare (system_name, "file") == 0 )
	{
		return IMYP_CURR_FILE;
	}
	else if ( (imyp_compare (system_name, "speaker") == 0)
		|| (imyp_compare (system_name, "pcspeaker") == 0)
		|| (imyp_compare (system_name, "spkr") == 0)
		|| (imyp_compare (system_name, "pcspkr") == 0) )
	{
		return IMYP_CURR_SPKR;
	}
	return IMYP_CURR_NONE;
}

/**
 * Returns the sample format described in the given string.
 * @param string the string with the format name, case-insensitive (e.g. "s16le").
 * @return the sample format described in the given string.
 */
enum IMYP_SAMPLE_FORMATS
imyp_get_format (
#ifdef IMYP_ANSIC
	const char string[])
#else
	string)
	const char string[];
#endif
{
	if ( string == NULL )
	{
		return IMYP_SAMPLE_FORMAT_UNKNOWN;
	}
	if ( imyp_compare (string, "s16le") == 0 )
	{
		return IMYP_SAMPLE_FORMAT_S16LE;
	}
	if ( imyp_compare (string, "s16be") == 0 )
	{
		return IMYP_SAMPLE_FORMAT_S16BE;
	}
	if ( imyp_compare (string, "u16le") == 0 )
	{
		return IMYP_SAMPLE_FORMAT_U16LE;
	}
	if ( imyp_compare (string, "u16be") == 0 )
	{
		return IMYP_SAMPLE_FORMAT_U16BE;
	}
	if ( imyp_compare (string, "s8le") == 0 )
	{
		return IMYP_SAMPLE_FORMAT_S8LE;
	}
	if ( imyp_compare (string, "s8be") == 0 )
	{
		return IMYP_SAMPLE_FORMAT_S8BE;
	}
	if ( imyp_compare (string, "u8le") == 0 )
	{
		return IMYP_SAMPLE_FORMAT_U8LE;
	}
	if ( imyp_compare (string, "u8be") == 0 )
	{
		return IMYP_SAMPLE_FORMAT_U8BE;
	}

	return IMYP_SAMPLE_FORMAT_UNKNOWN;
}

/**
 * Generates the samples and puts them in the given buffer.
 * \param freq The frequency of the tone (in Hz).
 * \param volume_level Volume of the tone (from 0 to 15).
 * \param duration The duration of the tone, in milliseconds.
 * \param buf The buffer for samples.
 * \param bufsize The buffer size, in samples.
 * \param is_le if not zero, multibyte samples will be in little-endian format.
 * \param is_uns if not zero, samples will be in unsigned format.
 * \param quality the numer of bits per sample (8 or 16).
 * \param samp_rate sampling rate of the samples.
 * \param start_index pointer to the starting index (last used index) if continuing filling a buffer.
 * \return the buffer size used, in bytes
 */
int
imyp_generate_samples (
#ifdef IMYP_ANSIC
	const double freq,
	const int volume_level,
	const int duration,
	void * const buf,
	const int bufsize,
	const int is_le,
	const int is_uns,
	const unsigned int samp_quality,
	const unsigned int samp_rate,
	unsigned long int * const start_index)
#else
	freq, volume_level, duration, buf, bufsize, is_le,
	is_uns, samp_quality, samp_rate, start_index)
	const double freq;
	const int volume_level;
	const int duration;
	void * const buf;
	const int bufsize;
	const int is_le;
	const int is_uns;
	const unsigned int samp_quality;
	const unsigned int samp_rate;
	unsigned long int * const start_index;
#endif
{
	int samp;	/* better than float */
	double nperiods;
	unsigned long int i;
	unsigned long int last_index = 0;
	unsigned int imyp_bufsize;
	int max_val;
	int samp_with_volume;
	unsigned long int nperiods_rounded;
	double periods_in_full;
	unsigned int qual_bits = samp_quality;
	unsigned int qual_bytes;

	if ( (buf == NULL) || (bufsize <= 0) || (samp_rate <= 0) || (duration <= 0) )
	{
		return 0;
	}

	if ( (qual_bits != 8) && (qual_bits != 16) )
	{
		qual_bits = 16;
	}
	if ( start_index != NULL )
	{
		last_index = (*start_index) + 1;
	}

	qual_bytes = qual_bits/8;
	imyp_bufsize = (unsigned int)(duration * (int)samp_rate * ((int)qual_bytes)) / 1000; /* bytes */
	imyp_bufsize = (unsigned int)IMYP_MIN ((unsigned int)bufsize, imyp_bufsize); /* bytes */
	imyp_bufsize /= qual_bytes; /* samples */
	max_val = (1 << (qual_bits - 1)) - 1;
	if ( freq > 0.0 )
	{
		nperiods = samp_rate/freq;
		/* round down, else we may think that a period fits while it doesn't: */
		/* nperiods_rounded = (unsigned long int)IMYP_ROUND(nperiods); */
		nperiods_rounded = (unsigned long int)nperiods;

		periods_in_full = 2 * M_PI / nperiods;
		for ( i = last_index; i < last_index + imyp_bufsize; i++ )
		{
			if ( imyp_sig_recvd != 0 )
			{
				if ( qual_bits == 16 )
				{
					return (int)(i - last_index) * 2;
				}
				else
				{
					return (int)(i - last_index);
				}
			}
			if ( nperiods_rounded == 0 )
			{
				/* not a single full period fits in the buffer */
#if (defined HAVE_SIN) || (defined HAVE_LIBM)
				samp = (int)(max_val /* disable to get rectangular wave */ +
					IMYP_ROUND (max_val
						/* * sin (i * periods_in_full)/3)); */
						* sin (i * periods_in_full)));
#else
				samp = (int) IMYP_ROUND (i *
					(max_val / nperiods));
#endif
			}
			else
			{
#if (defined HAVE_SIN) || (defined HAVE_LIBM)
				samp = (int)(max_val /* disable to get rectangular wave */ +
					IMYP_ROUND (max_val
						/* * sin ((i % nperiods_rounded) * periods_in_full)/3)); */
						* sin (i * periods_in_full)));
#else
				samp = (int) IMYP_ROUND ((i % nperiods_rounded) *
					(max_val / nperiods));
#endif
			}
			if ( is_uns == 0 )
			{
				samp -= max_val;
			}
			samp_with_volume = (samp * volume_level) / IMYP_MAX_IMY_VOLUME;
			if ( qual_bits == 16 )
			{
				/* "i" iterates over samples now, so this is not needed: */
				/*if ( (i-last_index)*2 >= imyp_bufsize )
				{
					break;
				}*/
				if ( is_le != 0 )
				{

					((char *)buf)[(i - last_index) * 2] =
						(char)(samp_with_volume & 0x0FF);
					((char *)buf)[(i - last_index) * 2 + 1] =
						(char)((samp_with_volume >> 8) & 0x0FF);
				}
				else
				{
					((char *)buf)[(i - last_index) * 2] =
						(char)((samp_with_volume >> 8) & 0x0FF);
					((char *)buf)[(i - last_index) * 2 + 1] =
						(char)(samp_with_volume & 0x0FF);
				}
			}
			else if ( qual_bits == 8 )
			{
				((char *)buf)[i - last_index] =
					(char)(samp_with_volume & 0x0FF);
			}
		}
	}
	else
	{
		for ( i = last_index; i < last_index + imyp_bufsize; i++ )
		{
			if ( imyp_sig_recvd != 0 )
			{
				return (int)(i - last_index);
			}
			if ( qual_bits == 16 )
			{
				((char *)buf)[(i - last_index) * 2] = 0;
				((char *)buf)[(i - last_index) * 2 + 1] = 0;
			}
			else if ( qual_bits == 8 )
			{
				((char *)buf)[(i - last_index)] = 0;
			}
		}
	}
	if ( start_index != NULL )
	{
		*start_index += imyp_bufsize;
	}
	return (int)(imyp_bufsize * qual_bytes);
}

/**
 * Pauses for the specified period of time.
 * \param milliseconds Number of milliseconds to pause.
 */
void
imyp_pause_select (
#ifdef IMYP_ANSIC
	const int milliseconds)
#else
	milliseconds)
	const int milliseconds;
#endif
{
	if ( milliseconds <= 0 )
	{
		return;
	}
#ifdef IMYP_HAVE_SELECT
	{
		struct timeval tv;
		tv.tv_sec = milliseconds / 1000;
		tv.tv_usec = ( milliseconds * 1000 ) % 1000000;
		select ( 0, NULL, NULL, NULL, &tv );
	}
#endif
}

/**
 * Outputs the given text.
 * \param text The text to output.
 */
void
imyp_put_text_stdout (
#ifdef IMYP_ANSIC
	const char * const text)
#else
	text)
	const char * const text;
#endif
{
	if ( (text != NULL) && (stdout != NULL) )
	{
		printf ("%s", text);
	}
}

/**
 * Generates a filename with the given extension from the given filename.
 * \param filename The base filename.
 * \param ext The extension to add.
 * \return The new filename (should be free()d after use).
 */
char *
imyp_generate_filename (
#ifdef IMYP_ANSIC
	const char * const filename,
	const char * const ext)
#else
	filename, ext)
	const char * const filename;
	const char * const ext;
#endif
{
	char * imy;
	size_t fnlen;
	size_t elen;
	int imy_index;
	size_t i;
	size_t target_fname_len;

	if ( (filename == NULL) || (ext == NULL) )
	{
		return NULL;
	}

	fnlen = strlen (filename);
	elen = strlen (ext);
	imy = strstr (filename, ".imy");
#ifdef HAVE_MALLOC
	if ( imy != NULL )
	{
		imy_index = imy - filename;
		imy = (char *) malloc (fnlen + 1);
		target_fname_len = fnlen + 1;
	}
	else
	{
		imy_index = (int)fnlen;
		imy = (char *) malloc (fnlen + elen + 1);
		target_fname_len = fnlen + elen + 1;
	}
	if ( imy == NULL )
	{
		return NULL;
	}
#ifdef HAVE_MEMSET
	memset (imy, 0, (size_t)target_fname_len);
#else
	for (i = 0; i < target_fname_len; i++ )
	{
		((char *)imy)[i] = '\0';
	}
#endif
	strncpy (imy, filename, fnlen);
	/* If ".imy" extension is present, change it to the provided one.
	   Else, append the requested extension. */
	for (i = (size_t)imy_index; i < fnlen; i++ )
	{
		imy[i] = '\0';
	}
	fnlen = (size_t)imy_index;
	strncpy (&imy[fnlen], ext, elen + 1);
	imy[fnlen + elen] = '\0';

	return imy;
#else
	return NULL;
#endif
}
