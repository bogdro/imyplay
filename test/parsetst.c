/*
 * IMYplay - A program for playing iMelody ringtones (IMY files).
 *	-- unit test for the imyparse/f.c file.
 *
 * Copyright (C) 2025 Bogdan Drozdowski, bogdro (at) users.sourceforge.net
 * License: GNU General Public License, v3+
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "imcommon.h"
#include "imyparse.h"
#include "imypwrap.h"
#ifdef IMYP_HAVE_FILE
# include "imyp_fil.h"
#endif

#include <stdio.h>

#ifdef HAVE_ERRNO_H
# include <errno.h>
#else
static int errno = -1;
#endif

#ifdef HAVE_UNISTD_H
# include <unistd.h>	/* unlink() */
#endif

#define IMYP_TEST_FILE_NAME "aimytest.imy"
#define IMYP_TEST_OUT_NAME "aimytest.raw"

/* dummy variables and functions provided by files not included in the test */
int imyp_sig_recvd = 0;
const char * imyp_progname = "test";

void imyp_pause (const int milliseconds,
	imyp_backend_t * const curr,
	const int is_note IMYP_ATTR ((unused)),
	void * const buf,
	int bufsize)
{
	imyp_file_pause(curr->imyp_data, milliseconds, buf, bufsize);
}

void imyp_put_text (const char * const text,
	imyp_backend_t * const curr)
{
	imyp_file_put_text(curr->imyp_data, text);
}

int imyp_play_tune (const double freq, const int volume_level,
	const int duration, void * const buf, int bufsize,
	imyp_backend_t * const curr)
{
	return imyp_file_play_tune(curr->imyp_data, freq,
		volume_level, duration, buf, bufsize);
}

/* ======================================================= */

START_TEST(test_imyp_parse_file)
{
	FILE *f;
	int res;
	imyp_backend_t curr_lib = {NULL, IMYP_CURR_FILE};

	printf("test_imyp_parse_file\n");
	res = imyp_file_init (&(curr_lib.imyp_data), NULL, IMYP_TEST_OUT_NAME);
	ck_assert_int_eq(res, 0);

	f = fopen(IMYP_TEST_FILE_NAME, "w");
	if ( f == NULL )
	{
		ck_abort_msg("Cannot open file, errno=%d\n", errno);
	}
	fputs("BEGIN:IMELODY\n", f);
	fputs("VERSION:1.2\n", f);
	fputs("FORMAT:CLASS1.0\n", f);
	fputs("NAME:Test\n", f);
	fputs("BEAT:120\n", f);
	fputs("STYLE:S1\n", f);
	fputs("VOLUME:V15\n", f);
	fputs("MELODY:c1vibeond2vibeoffV3e3.aaVVbackonf4:*99backoffg5;v+\r\n ledon*0(E4*4V5\r\n &f4*33#g4V+r4V-ledonv22*xvz@3)V33(a1@z)&c0ledoff*8V-#B4*4&C5*x&A5#G5#B5R5x\n", f);
	fputs("END:IMELODY\n", f);
	fclose(f);

	res = imyp_play_file(IMYP_TEST_FILE_NAME, &curr_lib);
	unlink(IMYP_TEST_FILE_NAME);
	unlink(IMYP_TEST_OUT_NAME);
	imyp_file_close(curr_lib.imyp_data);

	ck_assert_int_eq(res, 0);
}
END_TEST

/* ======================================================= */

static Suite * imy_create_suite(void)
{
	Suite * s = suite_create("imyplay_parsers");

	TCase * tests_imyp_parse = tcase_create("imyp_parse");

	tcase_add_test(tests_imyp_parse, test_imyp_parse_file);

	suite_add_tcase(s, tests_imyp_parse);

	return s;
}

int main(void)
{
	int failed;

	Suite * s = imy_create_suite();
	SRunner * sr = srunner_create(s);

	srunner_run_all(sr, CK_NORMAL);

	failed = srunner_ntests_failed(sr);
	srunner_free(sr);

	return failed;
}
