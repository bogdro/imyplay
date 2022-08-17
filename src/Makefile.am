bin_PROGRAMS = imyplay
datadir = @datadir@
imyplay_SOURCES = imyplay.c imyplay.h imypwrap.c imypwrap.h \
	imyp_sig.c imyp_sig.h imyparse.h imyputil.c imyputil.h

imyplay_LDADD = $(INTLLIBS)
localedir = $(datadir)/locale
INCLUDES = -I../intl -DLOCALEDIR=\"$(localedir)\"
EXTRA_DIST = imyp_cfg.hin

if ALLEGRO
imyplay_SOURCES += imyp_all.c imyp_all.h
endif

if SDL
imyplay_SOURCES += imyp_sdl.c imyp_sdl.h
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
imyplay_SOURCES += imyp_mid.c imyp_mid.h midifile.c midifile.h midiinfo.h midibase.h
endif

if EXEC
imyplay_SOURCES += imyp_exe.c imyp_exe.h
endif

if GSTREAMER
imyplay_SOURCES += imyp_gst.c imyp_gst.h
endif

if GSTREAMER
imyplay_SOURCES += imyp_fil.c imyp_fil.h
endif

if FLEX
imyplay_SOURCES += imyparsf.l
else
imyplay_SOURCES += imyparse.c
endif
