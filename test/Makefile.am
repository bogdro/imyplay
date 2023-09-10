#
# IMYplay - A program for playing iMelody ringtones (IMY files).
#	-- Makefile.am, part of the build system.
#
# Copyright (C) 2009-2023 Bogdan Drozdowski, bogdro (at) users . sourceforge . net
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

if IMY_TESTS_ENABLED

TESTS = utiltest
check_PROGRAMS = utiltest

COMMON_LIBS = $(top_builddir)/src/imyputil.o @CHECK_LIBS@ @LIBS@
COMMON_CFLAGS = @CHECK_CFLAGS@ -I $(top_builddir)/src

utiltest_SOURCES = utiltest.c
utiltest_CFLAGS = $(COMMON_CFLAGS)
utiltest_LDADD = $(COMMON_LIBS)

if ALLEGRO
TESTS += testalle
check_PROGRAMS += testalle

testalle_SOURCES = testalle.c
testalle_CFLAGS = $(COMMON_CFLAGS)
testalle_LDADD = $(COMMON_LIBS) $(top_builddir)/src/imyp_all.o @ALLEGRO_LDFLAGS@
endif

if ALSA
TESTS += testalsa
check_PROGRAMS += testalsa

testalsa_SOURCES = testalsa.c
testalsa_CFLAGS = $(COMMON_CFLAGS)
testalsa_LDADD = $(COMMON_LIBS) $(top_builddir)/src/imyp_als.o
endif

if LIBAO
TESTS += testao
check_PROGRAMS += testao

testao_SOURCES = testao.c
testao_CFLAGS = $(COMMON_CFLAGS)
testao_LDADD = $(COMMON_LIBS) $(top_builddir)/src/imyp_ao.o
endif

if GSTREAMER
TESTS += testgst
check_PROGRAMS += testgst

testgst_SOURCES = testgst.c
testgst_CFLAGS = $(COMMON_CFLAGS)
testgst_LDADD = $(COMMON_LIBS) $(top_builddir)/src/imyp_gst.o
endif

if JACK
TESTS += testjack
check_PROGRAMS += testjack

testjack_SOURCES = testjack.c
testjack_CFLAGS = $(COMMON_CFLAGS)
testjack_LDADD = $(COMMON_LIBS) $(top_builddir)/src/imyp_jck.o
endif

if OSS
TESTS += testoss
check_PROGRAMS += testoss

testoss_SOURCES = testoss.c
testoss_CFLAGS = $(COMMON_CFLAGS)
testoss_LDADD = $(COMMON_LIBS) $(top_builddir)/src/imyp_oss.o
endif

if PORTAUDIO
TESTS += testport
check_PROGRAMS += testport

testport_SOURCES = testport.c
testport_CFLAGS = $(COMMON_CFLAGS)
testport_LDADD = $(COMMON_LIBS) $(top_builddir)/src/imyp_por.o
endif

if PULSEAUDIO
TESTS += testpuls
check_PROGRAMS += testpuls

testpuls_SOURCES = testpuls.c
testpuls_CFLAGS = $(COMMON_CFLAGS)
testpuls_LDADD = $(COMMON_LIBS) $(top_builddir)/src/imyp_pul.o
endif

zcompile: $(TESTS)
.PHONY: zcompile

endif
