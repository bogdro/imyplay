/*
 * A program for playing iMelody ringtones (IMY files).
 *	-- OSS backend.
 *
 * Copyright (C) 2009-2011 Bogdan Drozdowski, bogdandr (at) op.pl
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
#include "imyp_oss.h"
#include "imyp_sig.h"

#include <stdio.h>

#if (defined HAVE_IOCTL) && (defined HAVE_SYS_SOUNDCARD_H) && (defined HAVE_SYS_IOCTL_H) \
 && (defined HAVE_FCNTL_H) && (defined HAVE_UNISTD_H) && (defined HAVE_OPEN)		\
 && (defined HAVE_CLOSE) && (defined HAVE_WRITE) && (defined HAVE_SELECT)		\
 && ((defined HAVE_SYS_SELECT_H) || (((defined TIME_WITH_SYS_TIME)				\
	|| (defined HAVE_SYS_TIME_H) || (defined HAVE_TIME_H))				\
 && (defined HAVE_UNISTD_H)))
# include <sys/soundcard.h>
#else
# error OSS requested, but components not found.
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
# include <unistd.h>	/* select() (the old way), write(), close() */
#endif

#ifdef HAVE_FCNTL_H
# include <fcntl.h>	/* open() */
#endif

#ifdef HAVE_SYS_IOCTL_H
# include <sys/ioctl.h>
#endif

/* select () - the new way */
#ifdef HAVE_SYS_SELECT_H
# include <sys/select.h>
#endif

#ifndef O_WRONLY
# define O_WRONLY 	1
#endif

static int pcm_fd;
static int glob_format;
static int glob_speed;

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
imyp_oss_play_tune (
#ifdef IMYP_ANSIC
	const double freq,
	const int volume_level,
	const int duration,
	void * const buf,
	int bufsize)
#else
	freq, volume_level, duration, buf, bufsize)
	const double freq;
	const int volume_level;
	const int duration;
	void * const buf;
	int bufsize;
#endif
{
	unsigned int quality = 16;
	int i;
	int samp; 	/* better than float */
	int res;
	int is_le = 1;
	int is_uns = 0;

	if ( (buf == NULL) || (bufsize <= 0) ) return -1;

	if ( (glob_format == AFMT_S8) || (glob_format == AFMT_U8) )
	{
		if ( glob_format == AFMT_U8 )
		{
			is_uns = 1;
		}
		else
		{
			is_uns = 0;
		}
		quality = 8;
	}
	else if ( (glob_format == AFMT_S16_LE) || (glob_format == AFMT_S16_BE)
		|| (glob_format == AFMT_U16_LE) || (glob_format == AFMT_U16_BE) )
	{
		if ( (glob_format == AFMT_S16_BE) || (glob_format == AFMT_U16_BE) )
		{
			is_le = 0;
		}
		else
		{
			is_le = 1;
		}
		if ( (glob_format == AFMT_U16_LE) || (glob_format == AFMT_U16_BE) )
		{
			is_uns = 1;
		}
		else
		{
			is_uns = 0;
		}
		quality = 16;
	}

	bufsize = IMYP_MIN (bufsize, (duration * (int)glob_speed * ((int)quality/8)) / 1000);

	if ( freq > 0.0 )
	{
#define NSAMP ((glob_speed)/(freq))
		for ( i=0; i < bufsize; i++ )
		{
			if ( sig_recvd != 0 )
			{
				return -2;
			}
#if (defined HAVE_SIN) || (defined HAVE_LIBM)
			samp = (int)(((1<<(quality-1))-1) /* disable to get rectangular wave */ +
				/* The "/3" is required to have a full sine wave, not
				trapese-like wave */
				IMYP_ROUND (((1<<(quality-1))-1)
					* sin ((i%((int)IMYP_ROUND(NSAMP)))*(2*M_PI/NSAMP))/3));
				/*if ( i < NSAMP ) printf("buf[%d]=%d\n", i, buf[i]);*/;
#else
			samp = (int) IMYP_ROUND ((i%((int)IMYP_ROUND(NSAMP)))*
				(((1<<(quality-1))-1)/NSAMP));
#endif
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
				return -2;
			}
			((char *)buf)[i] = 0;
		}
	}
	res = write (pcm_fd, buf, (size_t)bufsize);
	if ( res == bufsize )
	{
		return 0;
	}
	return res;
}

/**
 * Pauses for the specified period of time.
 * \param milliseconds Number of milliseconds to pause.
 */
void
imyp_oss_pause (
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
imyp_oss_put_text (
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
 * Initializes the OSS subsystem for use.
 * \param dev_file The device to open.
 * \return 0 on success.
 */
int
imyp_oss_init (
#ifdef IMYP_ANSIC
	const char * const dev_file)
#else
	dev_file)
	const char * const dev_file;
#endif
{
	int res;
	unsigned int i;
	const int formats[] = {AFMT_S16_LE, AFMT_U16_LE, AFMT_S16_BE, AFMT_U16_LE,
		AFMT_S8, AFMT_U8};
	const int speeds[] = {44100, 22050, 11025};
	int format;

	if ( dev_file == NULL )
	{
		pcm_fd = open ("/dev/dsp", O_WRONLY);
		if ( pcm_fd < 0 )
		{
			return -1;
		}
	}
	else
	{
		pcm_fd = open (dev_file, O_WRONLY);
		if ( pcm_fd < 0 )
		{
			pcm_fd = open ("/dev/dsp", O_WRONLY);
			if ( pcm_fd < 0 ) return -1;
		}
	}

	for ( i = 0; i < sizeof (formats) / sizeof (formats[0]); i++ )
	{
		format = formats[i];
		res = ioctl (pcm_fd, SNDCTL_DSP_SETFMT, &format);
		if ( res < 0 )
		{
			close (pcm_fd);
			return -2;
		}

		if ( format == formats[i] )
		{
			glob_format = format;
			break;
		}
	}
	if ( i == sizeof (formats) / sizeof (formats[0]) )
	{
		close (pcm_fd);
		return -3;
	}

	format = 1;
	res = ioctl (pcm_fd, SNDCTL_DSP_CHANNELS, &format);
	if ( res < 0 )
	{
		close (pcm_fd);
		return -4;
	}

	if ( format != 1 )
	{
		close (pcm_fd);
		return -5;
	}

	for ( i = 0; i < sizeof (speeds) / sizeof (speeds[0]); i++ )
	{
		format = speeds[i];
		res = ioctl (pcm_fd, SNDCTL_DSP_SPEED, &format);
		if ( res < 0 )
		{
			close (pcm_fd);
			return -6;
		}

		if ( format == speeds[i] )
		{
			glob_speed = format;
			break;
		}
	}
	if ( i == sizeof (speeds) / sizeof (speeds[0]) )
	{
		close (pcm_fd);
		return -7;
	}

	return 0;
}

/**
 * Closes the OSS subsystem.
 * \return 0 on success.
 */
int
imyp_oss_close (
#ifdef IMYP_ANSIC
	void
#endif
)
{
	if ( pcm_fd >= 0 ) return close (pcm_fd);
	return 0;
}
