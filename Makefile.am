SUBDIRS = doc src po test

ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST = config.rpath mkinstalldirs imyplay.spi m4/ax_cflag.m4 \
	m4/ax_unus.m4 imy THANKS dosdist.bat dosfiles.txt

# The 'pack' target is a hack that removes unnecessary files from the
# distribution that are generated during building and could not be
# removed by built-in automake features:
# - doc/imyplay.txi (can't be made "nodist_" because RPM building fails)
# - src/imyparsf.c (flex-generated files are always packaged by default)

pack:	doc/imyplay.info dist-gzip
	mkdir -p imyplay-@VERSION@/doc
	echo '\input texinfo'                    > imyplay-@VERSION@/doc/imyplay.txi
	echo '@setfilename imyplay.info'        >> imyplay-@VERSION@/doc/imyplay.txi
	echo '@settitle imyplay manual'         >> imyplay-@VERSION@/doc/imyplay.txi
	echo '@bye'                             >> imyplay-@VERSION@/doc/imyplay.txi
	gunzip -f imyplay-@VERSION@.tar.gz
	tar --delete -f imyplay-@VERSION@.tar imyplay-@VERSION@/doc/imyplay.txi
	tar -rf imyplay-@VERSION@.tar imyplay-@VERSION@/doc/imyplay.txi
	tar --delete -f imyplay-@VERSION@.tar imyplay-@VERSION@/src/imyparsf.c
	gzip -9 -f imyplay-@VERSION@.tar
	rm -fr imyplay-@VERSION@
