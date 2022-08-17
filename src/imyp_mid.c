/*
 * A program for playing iMelody ringtones (IMY files).
 *	-- MIDI writer backend.
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
#include "imyp_mid.h"
#include "imyp_sig.h"

#include <stdio.h>

#ifndef IMYP_HAVE_MIDI
# error MIDI requested, but components not found.
#endif

#ifdef HAVE_STRING_H
# if ((!defined STDC_HEADERS) || (!STDC_HEADERS)) && (defined HAVE_MEMORY_H)
#  include <memory.h>
# endif
# include <string.h>
#endif

#ifdef HAVE_STDLIB_H
# include <stdlib.h>
#endif

#ifdef HAVE_MALLOC_H
# include <malloc.h>
#endif

#define IMYP_MIDI_FABS(x)		( ((x) >= 0.0)? (x) : (-(x)) )
#define IMYP_MIDI_EPSILON		0.0001
#define IMYP_MIDI_DURATION_DIVISOR	1

#include "midibase.h"
#include "midiinfo.h"
#include "midifile.h"

static MIDI_FILE * midi = NULL;

static const int octaves[9] =
{
	MIDI_OCTAVE_0, MIDI_OCTAVE_1, MIDI_OCTAVE_2, MIDI_OCTAVE_3,
	MIDI_OCTAVE_4, MIDI_OCTAVE_5, MIDI_OCTAVE_6, MIDI_OCTAVE_7,
	MIDI_OCTAVE_8
};

static const int midinotes[12] =
{
	MIDI_NOTE_C, MIDI_NOTE_C_SHARP, MIDI_NOTE_D, MIDI_NOTE_D_SHARP,
	MIDI_NOTE_E, MIDI_NOTE_F, MIDI_NOTE_F_SHARP, MIDI_NOTE_G,
	MIDI_NOTE_G_SHARP, MIDI_NOTE_A, MIDI_NOTE_A_SHARP, MIDI_NOTE_B
};

static const int durations[12] =
{
	MIDI_NOTE_SEMIDEMIQUAVER, MIDI_NOTE_DOTTED_SEMIDEMIQUAVER,
	MIDI_NOTE_SEMIQUAVER, MIDI_NOTE_DOTTED_SEMIQUAVER,
	MIDI_NOTE_QUAVER, MIDI_NOTE_TRIPLE_CROCHET, MIDI_NOTE_DOTTED_QUAVER,
	MIDI_NOTE_CROCHET, MIDI_NOTE_DOTTED_CROCHET, MIDI_NOTE_MINIM,
	MIDI_NOTE_DOTTED_MINIM, MIDI_NOTE_BREVE
};

/**
 * Play a specified tone using the MIDI writer.
 * \param freq The frequency of the tone (in Hz).
 * \param volume_level Volume of the tone (from 0 to 15).
 * \param duration The duration of the tone, in milliseconds.
 * \param buf The buffer for samples.
 * \param bufsize The buffer size, in samples.
 * \return 0 on success.
 */
int
imyp_midi_play_tune (
#ifdef IMYP_ANSIC
	const double freq,
	const int volume_level,
	const int duration,
	void * const buf IMYP_ATTR((unused)),
	int bufsize IMYP_ATTR((unused)))
#else
	freq, volume_level, duration, buf, bufsize)
	const double freq;
	const int volume_level;
	const int duration;
	void * const buf IMYP_ATTR((unused));
	int bufsize IMYP_ATTR((unused));
#endif
{
	BOOL res;
	unsigned int oct, note;
	int midinote = -1;
	int vol = MIDI_VOL_FULL;
	int dur = 0;

	if ( volume_level == 0 ) vol = MIDI_VOL_OFF;
	else if ( volume_level < 12 ) vol = MIDI_VOL_HALF;
	/* search for the right note basing on the frequency */
	for ( oct = 0; oct < IMYP_OCTAVES; oct++ )
	{
		for ( note = 0; note < IMYP_NOTES_PER_OCTAVE; note++ )
		{
			if ( IMYP_MIDI_FABS (freq-imyp_notes[oct][note]) < IMYP_MIDI_EPSILON )
			{
				midinote = octaves[oct] + midinotes[note];
				break;
			}
		}
		if ( note < IMYP_NOTES_PER_OCTAVE ) break;
	}
	if ( oct >= IMYP_OCTAVES ) return -1;
	/* set the correct duration */
	if ( duration/IMYP_MIDI_DURATION_DIVISOR <= durations[0] )
	{
		dur = durations[0];
	}
	else if ( duration/IMYP_MIDI_DURATION_DIVISOR >=
		durations[sizeof (durations) / sizeof (durations[0]) - 1] )
	{
		dur = durations[sizeof (durations) / sizeof (durations[0]) - 1];
	}
	else
	{
		for ( oct = 0; oct < sizeof (durations) / sizeof (durations[0]) - 1; oct++ )
		{
			if ( durations[oct] <= duration/IMYP_MIDI_DURATION_DIVISOR
				&& duration/IMYP_MIDI_DURATION_DIVISOR <= durations[oct+1] )
			{
				if ( duration/IMYP_MIDI_DURATION_DIVISOR >= (durations[oct+1]-durations[oct])/2 )
					dur = durations[oct+1];
				else
					dur = durations[oct];
				break;
			}
		}
	}
	res = midiTrackAddNote (midi, 1 /* track */, midinote /*int iNote*/,
		dur, vol, TRUE /*BOOL bAutoInc*/, TRUE /*BOOL bOverrideLength*/);
	if (res == TRUE) return 0;
	return -2;
}

/**
 * Pauses for the specified period of time.
 * \param milliseconds Number of milliseconds to pause.
 * \param is_note Whether the pause is for a note (1) or for silence (0).
 */
void
imyp_midi_pause (
#ifdef IMYP_ANSIC
	const int milliseconds, const int is_note IMYP_ATTR((unused)))
#else
	milliseconds, is_note)
	const int milliseconds;
	const int is_note IMYP_ATTR((unused));
#endif
{
	if ( milliseconds <= 0 ) return;
	/* let's respect the melody style: */
	/*if ( is_note == 1 ) return;*/
	midiTrackAddRest (midi, 1 /* track */,
		/* set the correct duration */
		(int)IMYP_ROUND( (double)milliseconds*0.001f/*MIDI_NOTE_SEMIQUAVER*/ ),
		TRUE /*FALSE*/ /*BOOL bOverridePPQN*/);
}

/**
 * Outputs the given text using the MIDI writer.
 * \param text The text to output.
 */
void
imyp_midi_put_text (
#ifdef IMYP_ANSIC
	const char * const text)
#else
	text)
	const char * const text;
#endif
{
	if ( text == NULL ) return;
	if ( strstr (text, "NAME:") == text )
	{
		midiTrackAddText (midi, 1 /* track */, textTrackName /* type */, &text[5]);
	}
}

/**
 * Initializes the MIDI writer library for use.
 * \param filename The output filename.
 * \param instrument The instrument to use (-1 means default).
 * \return 0 on success.
 */
int
imyp_midi_init (
#ifdef IMYP_ANSIC
	const char * const filename, const int instrument)
#else
	filename, instrument)
	const char * const filename;
	const int instrument;
#endif
{
	char * imy;
	size_t len;

	if ( filename == NULL ) return -1;
	len = strlen (filename);
	imy = strstr (filename, ".imy");
	if ( imy != NULL )
	{
		strncpy (imy, ".mid", 4);
		imy[4] = '\0';
		midi = midiFileCreate (filename, TRUE);
		strncpy (imy, ".imy", 4);
		imy[4] = '\0';
		if ( midi == NULL ) return -2;
	}
	else
	{
#ifdef HAVE_MALLOC
		imy = (char *) malloc ( len + 4+1 );
		if ( imy == NULL ) return -3;
		strncpy ( imy, filename, len );
		strncpy ( &imy[len], ".mid", 4+1);
		imy[len+4] = '\0';
		midi = midiFileCreate (imy, TRUE);
		free (imy);
		if ( midi == NULL ) return -4;
#else
		return -5;
#endif
	}
	midiFileSetTracksDefaultChannel (midi, 1, MIDI_CHANNEL_1);
	if ( instrument == -1 )
	{
		midiTrackAddProgramChange (midi, 1, MIDI_PATCH_ACOUSTIC_GRAND_PIANO);
	}
	else
	{
		midiTrackAddProgramChange (midi, 1, instrument);
	}
	midiTrackAddText (midi, 1 /* track */, textTrackName /* type */,
		"MIDI file generated by IMYplay; http://miniurl.pl/bogdro-soft");
	return 0;
}

/**
 * Closes the MIDI writer library.
 * \return 0 on success.
 */
int
imyp_midi_close (
#ifdef IMYP_ANSIC
	void
#endif
)
{
	if ( midi != NULL )
	{
		if ( midiFileClose (midi) == FALSE ) return -1;
	}
	return 0;
}

/**
 * Displays the version of the MIDI backend.
 */
void
imyp_midi_version (
#ifdef IMYP_ANSIC
	void
#endif
)
{
	/* this is an internal backend */
	printf ( "MIDI: 1.3\n" );
}

