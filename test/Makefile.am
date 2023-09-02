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
utiltest_SOURCES = utiltest.c
utiltest_CFLAGS = @CHECK_CFLAGS@ -I $(top_builddir)/src
utiltest_LDADD = $(top_builddir)/src/imyputil.o @CHECK_LIBS@ @LIBS@

zcompile: $(TESTS)
.PHONY: zcompile

endif
