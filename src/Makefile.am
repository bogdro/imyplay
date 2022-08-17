bin_PROGRAMS = imyplay
datadir = @datadir@
imyplay_SOURCES = imyplay.c imyplay.h imypwrap.c imypwrap.h	\
	imyp_sig.c imyp_sig.h imyp_all.c imyp_all.h \
	imyp_mid.c imyp_mid.h midifile.c midifile.h midiinfo.h midibase.h

imyplay_LDADD = $(INTLLIBS)
localedir = $(datadir)/locale
INCLUDES = -I../intl -DLOCALEDIR=\"$(localedir)\"
EXTRA_DIST = imyp_cfg.hin

