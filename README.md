# IMYplay #

IMYplay is a program for playing iMelody ringtones (IMY files),
conforming to the iMelody (<https://en.wikipedia.org/wiki/IMelody>)
specification, as written in `doc/imelody.txt`.

Read the info documentation (type `info doc/imyplay.info`) to get more information.

IMYplay can use the following outputs to produce sound:

1.   the Allegro game library (<http://alleg.sf.net>),
2.   SDL (Simple DirectMedia Layer; <http://www.libsdl.org>),
3.   ALSA (Advanced Linux Sound Architecture; <http://alsa-project.org>),
4.   OSS (Open Sound System),
5.   libao (<http://xiph.org/ao/>),
6.   PortAudiov19 (<http://www.portaudio.com/>),
7.   PulseAudio (<http://www.pulseaudio.org/>),
8.   JACK1/JACK2 (<http://jackaudio.org/>),
9.   GStreamer (<http://gstreamer.freedesktop.org/>, gstreamer-plugin-base required).

It can also:
-   convert IMY ringtones to MIDI files,
-   write raw samples to an output file,
-   use the PC-speaker,
-   call an external program on each note.

IMYplay's homepage: <https://imyplay.sourceforge.io/>.

Author: Bogdan Drozdowski, bogdro (at) users . sourceforge . net

License: GPLv3+

IMYplay, in various versions, has been successfully compiled on the following systems:

-   Fedora Core 4 GNU/Linux, i686
-   Fedora 12 GNU/Linux, i686
-   Mandriva 2008.1, 2011 GNU/Linux, i686
-   OpenMandriva 3.0, 4.2, 4.3 GNU/Linux, amd64
-   OpenBSD 3.8, i586
-   Debian 5.0 GNU/Linux, i686
-   FreeDOS (DJGPP + NASM)
-   DOSBox (DJGPP + NASM)
-   FreeBSD
-   Solaris/SunOS
-   macOS

# WARNING #


The `dev` branch may contain code which is a work in progress and committed just for tests. The code here may not work properly or even compile.

The `master` branch may contain code which is committed just for quality tests.

The tags, matching the official packages on SourceForge, should be the most reliable points.
