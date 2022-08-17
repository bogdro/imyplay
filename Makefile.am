SUBDIRS = doc src po

ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST = config.rpath mkinstalldirs imyplay.spi m4/ax_cflag.m4 m4/ax_unus.m4 \
	doc/imelody.txt imy

pack:	doc/imyplay.info
	make dist-gzip
	mkdir -p imyplay-@VERSION@/doc
	echo '\input texinfo'			 > imyplay-@VERSION@/doc/imyplay.texi
	echo '@setfilename imyplay.info'	>> imyplay-@VERSION@/doc/imyplay.texi
	echo '@settitle imyplay manual'		>> imyplay-@VERSION@/doc/imyplay.texi
	echo '@bye'				>> imyplay-@VERSION@/doc/imyplay.texi
	gunzip -f imyplay-@VERSION@.tar.gz
	tar --delete -f imyplay-@VERSION@.tar imyplay-@VERSION@/doc/imyplay.texi
	tar -rf imyplay-@VERSION@.tar imyplay-@VERSION@/doc/imyplay.texi
	gzip -9 -f imyplay-@VERSION@.tar
	rm -fr imyplay-@VERSION@

dosdist:
	cp src/imyplay.exe .
	zip -9 -r -T AUTHORS ChangeLog COPYING doc imy po/imyplay.pot po/pl.gmo README imyplay.exe
	rm -f imyplay.exe


