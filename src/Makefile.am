#
# IMYplay - A program for playing iMelody ringtones (IMY files).
#	-- Makefile.am, part of the build system.
#
# Copyright (C) 2009-2025 Bogdan Drozdowski, bogdro (at) users . sourceforge . net
# License: GNU General Public License, v3+
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.
#

bin_PROGRAMS = imyplay
datadir = @datadir@
imyplay_SOURCES = imyplay.c imyplay.h imypwrap.c imypwrap.h \
	imyp_sig.c imyp_sig.h imyparse.h imyputil.c imyputil.h \
	iparsers.c iparsers.h imyp_cmd.c imyp_cmd.h

imyplay_LDADD = $(INTLLIBS)
localedir = $(datadir)/locale
AM_CPPFLAGS = -I../intl -DLOCALEDIR=\"$(localedir)\"
EXTRA_DIST = imyp_cfg.hin imypdos.asm

if ALLEGRO
imyplay_SOURCES += imyp_all.c imyp_all.h
LIBS += @ALLEGRO_LDFLAGS@
endif

# SDLv2 has functions with the same names as those in SDLv1,
# so enable at most one of the libraries at the same time
if SDL2
 imyplay_SOURCES += imyp_sdl.c imyp_sdl.h
else
if SDL
 imyplay_SOURCES += imyp_sdl.c imyp_sdl.h
endif
endif

if ALSA
imyplay_SOURCES += imyp_als.c imyp_als.h
endif

if OSS
imyplay_SOURCES += imyp_oss.c imyp_oss.h
endif

if LIBAO
imyplay_SOURCES += imyp_ao.c imyp_ao.h
endif

if PORTAUDIO
imyplay_SOURCES += imyp_por.c imyp_por.h
endif

if PULSEAUDIO
imyplay_SOURCES += imyp_pul.c imyp_pul.h
endif

if JACK
imyplay_SOURCES += imyp_jck.c imyp_jck.h
endif

if MIDI
imyplay_SOURCES += imyp_mid.c imyp_mid.h midifile.c midifile.h \
	midiinfo.h midibase.h
endif

if EXEC
imyplay_SOURCES += imyp_exe.c imyp_exe.h
endif

if GSTREAMER
imyplay_SOURCES += imyp_gst.c imyp_gst.h
endif

if FILE
imyplay_SOURCES += imyp_fil.c imyp_fil.h
endif

if SPKR
imyplay_SOURCES += imyp_spk.c imyp_spk.h
endif

if WAV
imyplay_SOURCES += imyp_wav.c imyp_wav.h
endif

if FLEX

imyplay_SOURCES += imyparsf.l
#BUILT_SOURCES = imyparsf.c

# WARNING: enabling nodist_imyplay_SOURCES in this form makes
# the program fail to link:
#nodist_imyplay_SOURCES = imyparsf.c

imyplay_DISTCLEANFILES = imyparsf.c
CLEANFILES = imyparsf.c

else

imyplay_SOURCES += imyparse.c

endif
