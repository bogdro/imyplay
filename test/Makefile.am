if IMY_TESTS_ENABLED

TESTS = utiltest
check_PROGRAMS = utiltest
utiltest_SOURCES = utiltest.c
utiltest_CFLAGS = @CHECK_CFLAGS@ -I $(top_builddir)/src
utiltest_LDADD = $(top_builddir)/src/imyputil.o @CHECK_LIBS@ @LIBS@

zcompile: $(TESTS)
.PHONY: zcompile

endif
