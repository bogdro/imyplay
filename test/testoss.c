/*
 * IMYplay - A program for playing iMelody ringtones (IMY files).
 *	-- unit test for the imyp_oss.c file.
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
#ifdef IMYP_HAVE_OSS
# include "imyp_oss.h"
#endif

#include <stdio.h>

/* dummy variable provided by files not included in the test */
int imyp_sig_recvd = 0;

#ifdef IMYP_HAVE_OSS

/* ======================================================= */

START_TEST(test_oss_pause_zero)
{
	printf ("test_oss_pause_zero\n");
	imyp_oss_pause (NULL, 0);
}
END_TEST

START_TEST(test_oss_pause_nonzero)
{
	printf ("test_oss_pause_nonzero\n");
	imyp_oss_pause (NULL, 10);
}
END_TEST

/* ======================================================= */

START_TEST(test_oss_put_text_null)
{
	printf ("test_oss_put_text_null\n");
	imyp_oss_put_text (NULL, NULL);
}
END_TEST

START_TEST(test_oss_put_text_nonnull)
{
	printf ("test_oss_put_text_nonnull\n");
	imyp_oss_put_text (NULL, "imyp_oss_put_text works\n");
}
END_TEST

/* ======================================================= */

START_TEST(test_oss_play_null_data)
{
	char buf[1];
	int res;

	printf ("test_oss_play_null_data\n");
	res = imyp_oss_play_tune (NULL, 1000, 7, 500, buf, 1);
	ck_assert_int_ne (res, 0);
}
END_TEST

START_TEST(test_oss_play_duration_zero)
{
	union d
	{
		imyp_backend_data_t data;
		char a[100];
	} dt;
	char buf[1] = {0};
	int res;

	printf ("test_oss_play_duration_zero\n");
	memset (dt.a, 0, 100);
	res = imyp_oss_play_tune ((imyp_backend_data_t *)&dt, 1000, 7, 0, buf, 1);
	ck_assert_int_eq (res, 0);
}
END_TEST

START_TEST(test_oss_play_bufsize_zero)
{
	imyp_backend_data_t data;
	char buf[1];
	int res;

	printf ("test_oss_play_bufsize_zero\n");
	res = imyp_oss_play_tune (&data, 1000, 7, 500, buf, 0);
	ck_assert_int_ne (res, 0);
}
END_TEST

START_TEST(test_oss_play_buf_null)
{
	imyp_backend_data_t data;
	int res;

	printf ("test_oss_play_buf_null\n");
	res = imyp_oss_play_tune (&data, 1000, 7, 500, NULL, 100);
	ck_assert_int_ne (res, 0);
}
END_TEST

/* ======================================================= */

START_TEST(test_oss_init_close_fn_null)
{
	imyp_backend_data_t * data;
	int res;

	printf ("test_oss_init_close_fn_null\n");
	res = imyp_oss_init (&data, NULL);
	/* may fail on newer systems, so just check if doesn't crash */
	/*ck_assert_int_eq (res, 0);*/
	if ( res == 0 )
	{
		res = imyp_oss_close (data);
		/*ck_assert_int_eq (res, 0);*/
	}
}
END_TEST

START_TEST(test_oss_init_close)
{
	imyp_backend_data_t * data;
	int res;

	printf ("test_oss_init_close\n");
	res = imyp_oss_init (&data, "filename");
	/* may fail on newer systems, so just check if doesn't crash */
	/*ck_assert_int_eq (res, 0);*/
	if ( res == 0 )
	{
		res = imyp_oss_close (data);
		/*ck_assert_int_eq (res, 0);*/
	}
}
END_TEST

/* ======================================================= */

START_TEST(test_oss_ver_nonnull)
{
	imyp_backend_data_t data;

	printf ("test_oss_ver_nonnull\n");
	imyp_oss_version (&data);
}
END_TEST

START_TEST(test_oss_ver_null)
{
	printf ("test_oss_ver_null\n");
	imyp_oss_version (NULL);
}
END_TEST

/* ======================================================= */

static Suite * imy_create_suite(void)
{
	Suite * s = suite_create("imyplay_oss");

	TCase * tc_pause = tcase_create("pause");
	TCase * tc_put_text = tcase_create("put_text");
	TCase * tc_play = tcase_create("play");
	TCase * tc_init_close = tcase_create("init_close");
	TCase * tc_version = tcase_create("version");

	tcase_add_test (tc_pause, test_oss_pause_zero);
	tcase_add_test (tc_pause, test_oss_pause_nonzero);
	tcase_add_test (tc_put_text, test_oss_put_text_null);
	tcase_add_test (tc_put_text, test_oss_put_text_nonnull);
	tcase_add_test (tc_play, test_oss_play_null_data);
	tcase_add_test (tc_play, test_oss_play_duration_zero);
	tcase_add_test (tc_play, test_oss_play_bufsize_zero);
	tcase_add_test (tc_play, test_oss_play_buf_null);
	tcase_add_test (tc_init_close, test_oss_init_close_fn_null);
	tcase_add_test (tc_init_close, test_oss_init_close);
	tcase_add_test (tc_version, test_oss_ver_nonnull);
	tcase_add_test (tc_version, test_oss_ver_null);

	suite_add_tcase(s, tc_pause);
	suite_add_tcase(s, tc_put_text);
	suite_add_tcase(s, tc_play);
	suite_add_tcase(s, tc_init_close);
	suite_add_tcase(s, tc_version);
	return s;
}
#endif /* #ifdef IMYP_HAVE_OSS */

int main(void)
{
#ifdef IMYP_HAVE_OSS
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
