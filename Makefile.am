SUBDIRS = doc src po

ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST = config.rpath mkinstalldirs imyplay.spi m4/ax_cflag.m4 m4/ax_unus.m4 \
	doc/imelody.txt imy THANKS dosdist.bat dosfiles.txt

pack:	doc/imyplay.info
	make dist-gzip
	mkdir -p imyplay-@VERSION@/doc
	echo '\input texinfo'			 > imyplay-@VERSION@/doc/imyplay.txi
	echo '@setfilename imyplay.info'	>> imyplay-@VERSION@/doc/imyplay.txi
	echo '@settitle imyplay manual'		>> imyplay-@VERSION@/doc/imyplay.txi
	echo '@bye'				>> imyplay-@VERSION@/doc/imyplay.txi
	gunzip -f imyplay-@VERSION@.tar.gz
	tar --delete -f imyplay-@VERSION@.tar imyplay-@VERSION@/doc/imyplay.txi
	tar -rf imyplay-@VERSION@.tar imyplay-@VERSION@/doc/imyplay.txi
	gzip -9 -f imyplay-@VERSION@.tar
	rm -fr imyplay-@VERSION@


