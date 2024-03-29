/*
 * IMYplay - A program for playing iMelody ringtones (IMY files).
 *	-- unit test for the imyp_sdl.c file.
 *
 * Copyright (C) 2023 Bogdan Drozdowski, bogdro (at) users.sourceforge.net
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

#include "imyplay.h"
#include "imcommon.h"
#if (defined IMYP_HAVE_SDL2) || (defined IMYP_HAVE_SDL)
# include "imyp_sdl.h"
#endif

#include <stdio.h>

/* dummy variable provided by files not included in the test */
int imyp_sig_recvd = 0;
const char * const ver_msg_compiled = "compiled with version";
const char * const ver_msg_runtime = "running with version";

#if (defined IMYP_HAVE_SDL2) || (defined IMYP_HAVE_SDL)

/* ======================================================= */

START_TEST(test_sdl_pause_zero)
{
	printf ("test_sdl_pause_zero\n");
	imyp_sdl_pause (NULL, 0);
}
END_TEST

START_TEST(test_sdl_pause_nonzero)
{
	printf ("test_sdl_pause_nonzero\n");
	imyp_sdl_pause (NULL, 10);
}
END_TEST

/* ======================================================= */

START_TEST(test_sdl_put_text_null)
{
	printf ("test_sdl_put_text_null\n");
	imyp_sdl_put_text (NULL, NULL);
}
END_TEST

START_TEST(test_sdl_put_text_nonnull)
{
	printf ("test_sdl_put_text_nonnull\n");
	imyp_sdl_put_text (NULL, "imyp_sdl_put_text works\n");
}
END_TEST

/* ======================================================= */

START_TEST(test_sdl_play_null_data)
{
	char buf[1];
	int res;

	printf ("test_sdl_play_null_data\n");
	res = imyp_sdl_play_tune (NULL, 1000, 7, 500, buf, 1);
	ck_assert_int_ne (res, 0);
}
END_TEST

START_TEST(test_sdl_play_duration_zero)
{
	imyp_backend_data_t data;
	char buf[1];
	int res;

	printf ("test_sdl_play_duration_zero\n");
	res = imyp_sdl_play_tune (&data, 1000, 7, 0, buf, 1);
	ck_assert_int_ne (res, 0);
}
END_TEST

START_TEST(test_sdl_play_bufsize_zero)
{
	imyp_backend_data_t data;
	char buf[1];
	int res;

	printf ("test_sdl_play_bufsize_zero\n");
	res = imyp_sdl_play_tune (&data, 1000, 7, 500, buf, 0);
	ck_assert_int_ne (res, 0);
}
END_TEST

START_TEST(test_sdl_play_buf_null)
{
	imyp_backend_data_t data;
	int res;

	printf ("test_sdl_play_buf_null\n");
	res = imyp_sdl_play_tune (&data, 1000, 7, 500, NULL, 100);
	ck_assert_int_ne (res, 0);
}
END_TEST

/* ======================================================= */

START_TEST(test_sdl_init_close_fn_null)
{
	imyp_backend_data_t * data;
	int res;

	printf ("test_sdl_init_close_fn_null\n");
	res = imyp_sdl_init (&data, NULL);
	ck_assert_int_eq (res, 0);
	res = imyp_sdl_close (data);
	ck_assert_int_eq (res, 0);
}
END_TEST

START_TEST(test_sdl_init_close_rev_dev)
{
	imyp_backend_data_t * data;
	int res;

	printf ("test_sdlinit_close_rev_dev\n");
	res = imyp_sdl_init (&data, "s16le:44100");
	ck_assert_int_eq (res, 0);
	res = imyp_sdl_close (data);
	ck_assert_int_eq (res, 0);
}
END_TEST

START_TEST(test_sdl_init_close_invalid)
{
	imyp_backend_data_t * data;
	int res;

	printf ("test_sdl_init_close_invalid\n");
	res = imyp_sdl_init (&data, "filename");
	ck_assert_int_eq (res, 0);
	res = imyp_sdl_close (data);
	ck_assert_int_eq (res, 0);
}
END_TEST

START_TEST(test_sdl_init_close)
{
	imyp_backend_data_t * data;
	int res;

	printf ("test_sdlinit_close_rev_dev\n");
	res = imyp_sdl_init (&data, "44100:s16le");
	ck_assert_int_eq (res, 0);
	res = imyp_sdl_close (data);
	ck_assert_int_eq (res, 0);
}
END_TEST

/* ======================================================= */

START_TEST(test_sdl_ver_nonnull)
{
	imyp_backend_data_t data;

	printf ("test_sdl_ver_nonnull\n");
	imyp_sdl_version (&data);
}
END_TEST

START_TEST(test_sdl_ver_null)
{
	printf ("test_sdl_ver_null\n");
	imyp_sdl_version (NULL);
}
END_TEST

/* ======================================================= */

static Suite * imy_create_suite(void)
{
	Suite * s = suite_create("imyplay_sdl");

	TCase * tc_pause = tcase_create("pause");
	TCase * tc_put_text = tcase_create("put_text");
	TCase * tc_play = tcase_create("play");
	TCase * tc_init_close = tcase_create("init_close");
	TCase * tc_version = tcase_create("version");

	tcase_add_test (tc_pause, test_sdl_pause_zero);
	tcase_add_test (tc_pause, test_sdl_pause_nonzero);
	tcase_add_test (tc_put_text, test_sdl_put_text_null);
	tcase_add_test (tc_put_text, test_sdl_put_text_nonnull);
	tcase_add_test (tc_play, test_sdl_play_null_data);
	tcase_add_test (tc_play, test_sdl_play_duration_zero);
	tcase_add_test (tc_play, test_sdl_play_bufsize_zero);
	tcase_add_test (tc_play, test_sdl_play_buf_null);
	tcase_add_test (tc_init_close, test_sdl_init_close_fn_null);
	tcase_add_test (tc_init_close, test_sdl_init_close_rev_dev);
	tcase_add_test (tc_init_close, test_sdl_init_close_invalid);
	tcase_add_test (tc_init_close, test_sdl_init_close);
	tcase_add_test (tc_version, test_sdl_ver_nonnull);
	tcase_add_test (tc_version, test_sdl_ver_null);

	suite_add_tcase(s, tc_pause);
	suite_add_tcase(s, tc_put_text);
	suite_add_tcase(s, tc_play);
	suite_add_tcase(s, tc_init_close);
	suite_add_tcase(s, tc_version);
	return s;
}
#endif /* #if (defined IMYP_HAVE_SDL2) || (defined IMYP_HAVE_SDL) */

int main(void)
{
#if (defined IMYP_HAVE_SDL2) || (defined IMYP_HAVE_SDL)
	int failed;

	Suite * s = imy_create_suite();
	SRunner * sr = srunner_create(s);

	srunner_run_all(sr, CK_NORMAL);

	failed = srunner_ntests_failed(sr);
	srunner_free(sr);

	return failed;
#else
	return IMYP_RESULT_TEST_SKIPPED;
#endif
}
