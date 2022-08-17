SUBDIRS = doc src po test

ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST = config.rpath mkinstalldirs imyplay.spi m4/ax_cflag.m4 \
	m4/ax_unus.m4 imy THANKS dosdist.bat dosfiles.txt Makefile.dos

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
