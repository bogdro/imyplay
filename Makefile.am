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

SUBDIRS = doc src po test

ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST = mkinstalldirs imyplay.spi m4/ax_cflag.m4 \
	m4/ax_unus.m4 imy THANKS dosdist.bat dosfiles.txt Makefile.dos \
	setup/imyplay.bat setup/imyplay.nsi setup/imyplay.svg

# The 'x-pack' target is a hack that removes unnecessary files from the
# distribution that are generated during building and could not be
# removed by built-in automake features:
# - doc/imyplay.txi (can't be made "nodist_" because RPM building fails)
# - src/imyparsf.c (flex-generated files are always packaged by default)

x-pack:	doc/imyplay.info dist-gzip
	$(MAKE) -C doc html
	tar zxf imyplay-@VERSION@.tar.gz
	echo '\input texinfo'			 > imyplay-@VERSION@/doc/imyplay.txi
	echo '@setfilename imyplay.info'	>> imyplay-@VERSION@/doc/imyplay.txi
	echo '@settitle imyplay manual'		>> imyplay-@VERSION@/doc/imyplay.txi
	echo '@bye'				>> imyplay-@VERSION@/doc/imyplay.txi
	rm -f imyplay-@VERSION@/src/imyparsf.c
	tar zcf imyplay-@VERSION@.tar.gz imyplay-@VERSION@
	rm -fr imyplay-@VERSION@

x-gen-potfiles-in:
	/bin/egrep -l '(N|[^\w])_\("' src/*.c
