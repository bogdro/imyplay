;;
; A program for playing iMelody ringtones (IMY files).
;	-- DOS routines (currently for the PC-speaker backend).
;
; Copyright (C) 2012 Bogdan Drozdowski, bogdandr (at) op.pl
; License: GNU General Public License, v3+
;
; This program is free software; you can redistribute it and/or
; modify it under the terms of the GNU General Public License
; as published by the Free Software Foundation; either version 3
; of the License, or (at your option) any later version.
;
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
;
; You should have received a copy of the GNU General Public License
; along with this program; if not, write to the Free Software Foudation:
;		Free Software Foundation
;		51 Franklin Street, Fifth Floor
;		Boston, MA 02110-1301
;		USA
;;

cpu 386

; this is required, otherwise the symbols are undefined:
section .text

; define the symbol in different ways for different compilers:
global _imyp_spkr_dos_tone
global imyp_spkr_dos_tone_
global imyp_spkr_dos_tone

_imyp_spkr_dos_tone:
imyp_spkr_dos_tone_:
imyp_spkr_dos_tone:

%define timer_port	40h	; PIC port
%define keyboard_port	60h	; keyboard controller port

	push	ebp
	mov	ebp, esp
	pushfd
	push	eax
	push	edx

; [ebp] = old EBP
; [ebp+4] = return address
; [ebp+8] = the parameter (the frequency quotient)
%define	param ebp+8

	mov	edx, [param]
	cmp	edx, 19
	jb	.turn_off

	in	al, keyboard_port+1	; port B of the keyboard controller
	or  	al, 3			; set bits: 0 i 1 - enable the PC-speaker
					; and the gate from timer 2 to the speaker
	out 	keyboard_port+1, al

	mov	al, 0b6h

	out	timer_port+3, al	; send a command:
					; (bits 7-6) choose timer 2
					; (bits 5-4) lower bits first:
					; bits 0-7 first, then bits 8-15
					; (bits 3-1) mode 3: square wave
					; generator
					; (bit 0) a 16-bit binary counter

	mov	al, dl			; lower byte of the frequency quotient
	out	timer_port+2, al	; timer 2 port and bits 0-7
					; of the frequency quotient

	mov	al, dh
	out	timer_port+2, al	; bits 8-15 of the frequency quotient

	; sound is now enabled. Return to caller.

	pop	edx
	pop	eax
	xor	eax, eax		; return the value 0
	popfd
	pop	ebp
	ret

.turn_off:

	in	al, keyboard_port+1
	and	al, ~3			; clear bits: 0 i 1 - disable the PC-speaker
					; and the gate from timer 2 to the speaker

	out	keyboard_port+1, al


	; sound is now disabled. Return to caller.

	pop	edx
	pop	eax
	xor	eax, eax		; return the value 0
	popfd
	pop	ebp
	ret

