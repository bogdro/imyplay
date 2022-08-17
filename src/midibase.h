/*
 * midibase.h -  Header file for Steevs MIDI Library
 * Version 1.2
 *
 *  AUTHOR: Steven Goodwin (StevenGoodwin@gmail.com)
 *          Copyright 1998-2008, Steven Goodwin
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
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
#ifndef _MIDIBASE_H
#define _MIDIBASE_H

#ifdef __cplusplus
extern "C" {
#endif

/*
** Types because we're dealing with files, and need to be careful
*/
typedef	unsigned char		BYTE;
typedef	unsigned short		WORD;
typedef	unsigned long		DWORD;
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
