/*
 * A program for playing iMelody ringtones (IMY files).
 *	-- utility functions.
 *
 * Copyright (C) 2012 Bogdan Drozdowski, bogdandr (at) op.pl
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

#ifdef HAVE_MATH_H
# include <math.h>	/* sin() */
#endif

#ifndef M_PI
# define M_PI 3.14159265358979323846
#endif

#include "imyplay.h"
#include "imyputil.h"
#include "imyp_sig.h"

#define IMYP_TOUPPER(c) ((char)( ((c) >= 'a' && (c) <= 'z')? ((c) & 0x5F) : (c) ))

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

	if ( (string1 == NULL) && (string2 == NULL) ) return 0;
	else if ( string1 == NULL ) return -1;
	else if ( string2 == NULL ) return 1;
	else
	{
		/* both strings not-null */
		len1 = strlen (string1);
		len2 = strlen (string2);
		if ( len1 < len2 ) return -1;
		else if ( len1 > len2 ) return 1;
		else
		{
			/* both lengths equal */
			for ( i = 0; i < len1; i++ )
			{
				c1 = IMYP_TOUPPER (string1[i]);
				c2 = IMYP_TOUPPER (string2[i]);
				if ( c1 < c2 ) return -1;
				else if ( c1 > c2 ) return 1;
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
IMYP_CURR_LIB
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
 * \return the buffer size used
 */
int
imyp_generate_samples (
#ifdef IMYP_ANSIC
	const double freq,
	const int volume_level,
	const int duration,
	void * const buf,
	int bufsize,
	const int is_le,
	const int is_uns,
	int unsigned quality,
	const unsigned int samp_rate)
#else
	freq, volume_level, duration, buf, bufsize, is_le, is_uns, quality, samp_rate)
	const double freq;
	const int volume_level;
	const int duration;
	void * const buf;
	int bufsize;
	const int is_le;
	const int is_uns;
	unsigned int quality;
	const unsigned int samp_rate;
#endif
{
	int samp;	/* better than float */
	double nperiods;
	int i;

	if ( (buf == NULL) || (bufsize <= 0) || (samp_rate <= 0) || (duration <= 0) )
	{
		return 0;
	}

	if ( (quality != 8) && (quality != 16) )
	{
		quality = 16;
	}

	bufsize = IMYP_MIN (bufsize, (duration * (int)samp_rate * ((int)quality/8)) / 1000);
	if ( freq > 0.0 )
	{
		nperiods = samp_rate/freq;
		for ( i=0; i < bufsize; i++ )
		{
			if ( sig_recvd != 0 )
			{
				if ( quality == 16 )
				{
					return i*2;
				}
				else
				{
					return i;
				}
			}
			if ( (int)IMYP_ROUND(nperiods) == 0 )
			{
				/* not a single full period fits in the buffer */
#if (defined HAVE_SIN) || (defined HAVE_LIBM)
				samp = (int)(((1<<(quality-1))-1) /* disable to get rectangular wave */ +
					/* The "/3" is required to have a full sine wave, not
					trapese-like wave */
					IMYP_ROUND (((1<<(quality-1))-1)
						* sin (i*(2*M_PI/nperiods))/3));
#else
				samp = (int) IMYP_ROUND (i*
					(((1<<(quality-1))-1)/nperiods));
#endif
			}
			else
			{
#if (defined HAVE_SIN) || (defined HAVE_LIBM)
				samp = (int)(((1<<(quality-1))-1) /* disable to get rectangular wave */ +
					/* The "/3" is required to have a full sine wave, not
					trapese-like wave */
					IMYP_ROUND (((1<<(quality-1))-1)
						* sin ((i%((int)IMYP_ROUND(nperiods)))*(2*M_PI/nperiods))/3));
#else
				samp = (int) IMYP_ROUND ((i%((int)IMYP_ROUND(nperiods)))*
					(((1<<(quality-1))-1)/nperiods));
#endif
			}
			if ( is_uns == 0 )
			{
				samp -= (1<<(quality-1));
			}
			if ( quality == 16 )
			{
				if ( i*2 >= bufsize ) break;
				if ( is_le != 0 )
				{
					((char *)buf)[i*2] =
						(char)(((samp * volume_level) / IMYP_MAX_IMY_VOLUME) & 0x0FF);
					((char *)buf)[i*2+1] =
						(char)((((samp * volume_level) / IMYP_MAX_IMY_VOLUME) >> 8) & 0x0FF);
				}
				else
				{
					((char *)buf)[i*2] =
						(char)((((samp * volume_level) / IMYP_MAX_IMY_VOLUME) >> 8) & 0x0FF);
					((char *)buf)[i*2+1] =
						(char)(((samp * volume_level) / IMYP_MAX_IMY_VOLUME) & 0x0FF);
				}
			}
			else if ( quality == 8 )
			{
				((char *)buf)[i] = (char)(((samp * volume_level) / IMYP_MAX_IMY_VOLUME) & 0x0FF);
			}
		}
	}
	else
	{
		for ( i=0; i < bufsize; i++ )
		{
			if ( sig_recvd != 0 )
			{
				return i;
			}
			((char *)buf)[i] = 0;
		}
	}
	return bufsize;
}

