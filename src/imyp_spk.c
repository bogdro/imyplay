/*
 * A program for playing iMelody ringtones (IMY files).
 *	-- PC-speaker backend.
 *
 * Copyright (C) 2009-2012 Bogdan Drozdowski, bogdandr (at) op.pl
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

#include "imyplay.h"
#include "imyp_spk.h"
#include "imyp_sig.h"

#ifdef IMYP_HAVE_SPKR
# include <stdio.h> /* anything, actually */
#else
# error PC-speaker requested, but components not found.
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
# include <unistd.h>	/* select() (the old way), close() */
#endif

/* select () - the new way */
#ifdef HAVE_SYS_SELECT_H
# include <sys/select.h>
#endif

#ifdef IMYP_IS_DOS
# ifdef __cplusplus
extern "C" {
# endif

extern int imyp_spkr_dos_tone PARAMS ((const int frequency));

# ifdef __cplusplus
}
# endif

#else

# ifdef HAVE_SYS_IOCTL_H
#  include <sys/ioctl.h>
# endif

# ifdef HAVE_SYS_KD_H
#  include <sys/kd.h>
# endif

# ifdef HAVE_FCNTL_H
#  include <fcntl.h>	/* open() */
# endif

# ifndef O_WRONLY
#  define O_WRONLY 	1
# endif

static int raw_console = -1;
#endif

/**
 * Play a specified tone.
 * \param freq The frequency of the tone (in Hz).
 * \param volume_level Volume of the tone (from 0 to 15).
 * \param duration The duration of the tone, in milliseconds.
 * \param buf The buffer for samples.
 * \param bufsize The buffer size, in samples.
 * \return 0 on success.
 */
int
imyp_spkr_play_tune (
#ifdef IMYP_ANSIC
	const double freq,
	const int volume_level IMYP_ATTR ((unused)),
	const int duration,
	void * const buf IMYP_ATTR ((unused)),
	int bufsize IMYP_ATTR ((unused)))
#else
	freq, volume_level, duration, buf, bufsize)
	const double freq;
	const int volume_level IMYP_ATTR ((unused));
	const int duration;
	void * const buf IMYP_ATTR ((unused));
	int bufsize IMYP_ATTR ((unused));
#endif
{
	int ioctl_res = 0;
	int ioctl_param;

	/* The PC speaker can't generate sounds with a frequency
	   less than 0x1234DD / 0x10000 = 18.2 Hz.
	   Remember to "modprobe snd-pcsp" under Linux.
	*/
	if ( freq >= 19 )
	{
		ioctl_param = 0x1234DD / (int)IMYP_ROUND (freq);
#ifdef IMYP_IS_DOS
		/* TODO */
		/* turn off any sounds being currently generated: */
		imyp_spkr_dos_tone (0);
		/* start generating the tone: */
		ioctl_res = imyp_spkr_dos_tone (ioctl_param);
		/* pause (the sound is still being generated): */
		imyp_spkr_pause (duration);
		/* stop generating the tone: */
		imyp_spkr_dos_tone (0);
#else
		/* turn off any sounds being currently generated: */
		ioctl (raw_console, KIOCSOUND, 0);
		/* start generating the tone: */
		ioctl_res = ioctl (raw_console, KIOCSOUND, ioctl_param);
		/* pause (the sound is still being generated): */
		imyp_spkr_pause (duration);
		/* stop generating the tone: */
		ioctl (raw_console, KIOCSOUND, 0);
#endif
	}
	else
	{
		imyp_spkr_pause (duration);
	}

	return ioctl_res;
}

/**
 * Pauses for the specified period of time.
 * \param milliseconds Number of milliseconds to pause.
 */
void
imyp_spkr_pause (
#ifdef IMYP_ANSIC
	const int milliseconds)
#else
	milliseconds)
	const int milliseconds;
#endif
{
	if ( milliseconds <= 0 ) return;
#if ((defined HAVE_SYS_SELECT_H) || (defined TIME_WITH_SYS_TIME)\
	|| (defined HAVE_SYS_TIME_H) || (defined HAVE_TIME_H))	\
	&& (defined HAVE_SELECT)
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
imyp_spkr_put_text (
#ifdef IMYP_ANSIC
	const char * const text)
#else
	text)
	const char * const text;
#endif
{
	if ( (text != NULL) && (stdout != NULL) ) printf ("%s", text);
}

/**
 * Initializes the PC-speaker backend for use.
 * \param dev The device to open instead of "/dev/console".
 * \return 0 on success.
 */
int
imyp_spkr_init (
#ifdef IMYP_ANSIC
	const char * const dev)
#else
	dev)
	const char * const dev;
#endif
{
	/* DOS doesn't need any initialization */
#ifndef IMYP_IS_DOS
	if ( dev == NULL )
	{
		raw_console = open ("/dev/console", O_WRONLY);
		if ( raw_console < 0 )
		{
			return -1;
		}
	}
	else
	{
		raw_console = open (dev, O_WRONLY);
		if ( raw_console < 0 )
		{
			raw_console = open ("/dev/console", O_WRONLY);
			if ( raw_console < 0 )
			{
				return -1;
			}
		}
	}
#endif
	return 0;
}

/**
 * Closes the PC-speaker backend.
 * \return 0 on success.
 */
int
imyp_spkr_close (
#ifdef IMYP_ANSIC
	void
#endif
)
{
#ifdef IMYP_IS_DOS
	return 0;
#else
	return close (raw_console);
#endif
}

/**
 * Displays the version of the PC-speaker backend.
 */
void
imyp_spkr_version (
#ifdef IMYP_ANSIC
	void
#endif
)
{
	/* this is an internal backend */
	printf ( "PC-Speaker\n" );
}

