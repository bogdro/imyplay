/*
 * midibase.h -  Header file for Steevs MIDI Library
 * Version 1.2
 *
 *  AUTHOR: Steven Goodwin (StevenGoodwin@gmail.com)
 *          Copyright 1998-2008, Steven Goodwin
 *
 * Changes: Bogdan Drozdowski <bogdro@users.sourceforge.net>:
 *	2021-09-28: adapt to 64-bit systems
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation; either version 2 of
 *  the License,or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foudation:
 *		Free Software Foundation
 *		51 Franklin Street, Fifth Floor
 *		Boston, MA 02110-1301
 *		USA
 */
#ifndef _MIDIBASE_H
#define _MIDIBASE_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef HAVE_STDINT_H
# include <stdint.h>
#else
/*# if (defined __MSDOS) || (defined __MSDOS__) || (defined MSDOS)*/
typedef	unsigned long		uint32_t;
/*# endif / * DOS */
#endif

/*
** Types because we're dealing with files, and need to be careful
*/
typedef	unsigned char		BYTE;
typedef	unsigned short		WORD;
typedef	uint32_t		DWORD;	/* MUST be 32 bits! */

typedef	int					BOOL;

#ifndef TRUE
#define TRUE	1
#endif
#ifndef FALSE
#define FALSE	0
#endif


#ifdef __cplusplus
}
#endif

#endif /* _MIDIBASE_H */
