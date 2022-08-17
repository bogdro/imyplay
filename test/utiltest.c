/*
 * A program for playing iMelody ringtones (IMY files).
 *	-- unit test.
 *
 * Copyright (C) 2018-2019 Bogdan Drozdowski, bogdandr (at) op.pl
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
 * along with this program; if not, write to the Free Software Foudation:
 *		Free Software Foundation
 *		51 Franklin Street, Fifth Floor
 *		Boston, MA 02110-1301
 *		USA
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <check.h>

/* compatibility with older check versions */
#ifndef ck_abort
# define ck_abort() ck_abort_msg(NULL)
# define ck_abort_msg fail
# define ck_assert(C) ck_assert_msg(C, NULL)
# define ck_assert_msg fail_unless
#endif

#ifndef _ck_assert_int
# define _ck_assert_int(X, O, Y) ck_assert_msg((X) O (Y), "Assertion '"#X#O#Y"' failed: "#X"==%d, "#Y"==%d", X, Y)
# define ck_assert_int_eq(X, Y) _ck_assert_int(X, ==, Y)
# define ck_assert_int_ne(X, Y) _ck_assert_int(X, !=, Y)
#endif

#ifndef _ck_assert_str
# define _ck_assert_str(C, X, O, Y) ck_assert_msg(C, "Assertion '"#X#O#Y"' failed: "#X"==\"%s\", "#Y"==\"%s\"", X, Y)
# define ck_assert_str_eq(X, Y) _ck_assert_str(!strcmp(X, Y), X, ==, Y)
# define ck_assert_str_ne(X, Y) _ck_assert_str(strcmp(X, Y), X, !=, Y)
#endif


#ifdef HAVE_ERRNO_H
# include <errno.h>
#else
static int errno = -1;
#endif

#ifdef HAVE_MALLOC_H
# include <malloc.h>
#endif

#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

#include <stdio.h>

#ifdef HAVE_STDLIB_H
# include <stdlib.h>
#endif

#ifdef HAVE_STRING_H
# if (!defined STDC_HEADERS) && (defined HAVE_MEMORY_H)
#  include <memory.h>
# endif
# include <string.h>
#endif

#ifdef HAVE_FCNTL_H
# include <fcntl.h>
#else
# define O_RDONLY	0
# define O_WRONLY	1
# define O_RDWR		2
# define O_TRUNC	01000
#endif

#include "imyputil.h"

/*
typedef ssize_t (*def_write)(int fd, const void *buf, size_t count);
static def_write orig_write;
static size_t nwritten = 0;

ssize_t write(int fd, const void *buf, size_t count)
{
	nwritten = count;
	return (*orig_write)(fd, buf, count);
}
*/

/* dummy variable provided by files not included in the test */
int imyp_sig_recvd = 0;

/* ======================================================= */

START_TEST(test_imyp_compare_both_null)
{
	printf("test_imyp_compare_both_null\n");
	ck_assert_int_eq(imyp_compare(NULL, NULL), 0);
}
END_TEST

START_TEST(test_imyp_compare_first_null)
{
	printf("test_imyp_compare_first_null\n");
	ck_assert(imyp_compare(NULL, "aaa") < 0);
}
END_TEST

START_TEST(test_imyp_compare_second_null)
{
	printf("test_imyp_compare_second_null\n");
	ck_assert(imyp_compare("aaa", NULL) > 0);
}
END_TEST

START_TEST(test_imyp_compare_equal)
{
	printf("test_imyp_compare_equal\n");
	ck_assert_int_eq(imyp_compare("aaa", "aaa"), 0);
}
END_TEST

START_TEST(test_imyp_compare_equal_case_ins)
{
	printf("test_imyp_compare_equal_case_ins\n");
	ck_assert_int_eq(imyp_compare("aaa", "aAa"), 0);
}
END_TEST

START_TEST(test_imyp_compare_equal_diff_len)
{
	printf("test_imyp_compare_equal_diff_len\n");
	ck_assert_int_ne(imyp_compare("aaa", "aa"), 0);
}
END_TEST

START_TEST(test_imyp_compare_nonequal)
{
	printf("test_imyp_compare_nonequal\n");
	ck_assert_int_ne(imyp_compare("aaa", "bbb"), 0);
}
END_TEST

/* ======================================================= */

START_TEST(test_imyp_generate_filename_no_ext_mid)
{
	char * res;

	printf("test_imyp_generate_filename_no_ext_mid\n");
	res = imyp_generate_filename("test", ".mid");
	ck_assert_str_eq(res, "test.mid");
}
END_TEST

START_TEST(test_imyp_generate_filename_no_ext_raw)
{
	char * res;

	printf("test_imyp_generate_filename_no_ext_raw\n");
	res = imyp_generate_filename("test", ".raw");
	ck_assert_str_eq(res, "test.raw");
}
END_TEST

START_TEST(test_imyp_generate_filename_mid_mid)
{
	char * res;

	printf("test_imyp_generate_filename_mid_mid\n");
	res = imyp_generate_filename("test.mid", ".mid");
	ck_assert_str_eq(res, "test.mid");
}
END_TEST

START_TEST(test_imyp_generate_filename_raw_raw)
{
	char * res;

	printf("test_imyp_generate_filename_raw_raw\n");
	res = imyp_generate_filename("test.raw", ".raw");
	ck_assert_str_eq(res, "test.raw");
}
END_TEST

START_TEST(test_imyp_generate_filename_one_ext_mid)
{
	char * res;

	printf("test_imyp_generate_filename_one_ext_mid\n");
	res = imyp_generate_filename("test.imy", ".mid");
	ck_assert_str_eq(res, "test.mid");
}
END_TEST

START_TEST(test_imyp_generate_filename_one_ext_raw)
{
	char * res;

	printf("test_imyp_generate_filename_one_ext\n");
	res = imyp_generate_filename("test.imy", ".raw");
	ck_assert_str_eq(res, "test.raw");
}
END_TEST

START_TEST(test_imyp_generate_filename_two_ext_mid)
{
	char * res;

	printf("test_imyp_generate_filename_two_ext_mid\n");
	res = imyp_generate_filename("test.mid.imy", ".mid");
	ck_assert_str_eq(res, "test.mid.mid");
}
END_TEST

START_TEST(test_imyp_generate_filename_two_ext_raw)
{
	char * res;

	printf("test_imyp_generate_filename_two_ext\n");
	res = imyp_generate_filename("test.raw.imy", ".raw");
	ck_assert_str_eq(res, "test.raw.raw");
}
END_TEST

/* ======================================================= */

static Suite * imy_create_suite(void)
{
	Suite * s = suite_create("imyplay_util");

	TCase * tests_imyp_compare = tcase_create("imyp_compare");
	TCase * tests_imyp_generate_filename = tcase_create("imyp_generate_filename");

	tcase_add_test(tests_imyp_compare, test_imyp_compare_both_null);
	tcase_add_test(tests_imyp_compare, test_imyp_compare_first_null);
	tcase_add_test(tests_imyp_compare, test_imyp_compare_second_null);
	tcase_add_test(tests_imyp_compare, test_imyp_compare_equal);
	tcase_add_test(tests_imyp_compare, test_imyp_compare_equal_case_ins);
	tcase_add_test(tests_imyp_compare, test_imyp_compare_equal_diff_len);
	tcase_add_test(tests_imyp_compare, test_imyp_compare_nonequal);

	tcase_add_test(tests_imyp_generate_filename, test_imyp_generate_filename_no_ext_mid);
	tcase_add_test(tests_imyp_generate_filename, test_imyp_generate_filename_mid_mid);
	tcase_add_test(tests_imyp_generate_filename, test_imyp_generate_filename_no_ext_raw);
	tcase_add_test(tests_imyp_generate_filename, test_imyp_generate_filename_raw_raw);
	tcase_add_test(tests_imyp_generate_filename, test_imyp_generate_filename_one_ext_mid);
	tcase_add_test(tests_imyp_generate_filename, test_imyp_generate_filename_one_ext_raw);
	tcase_add_test(tests_imyp_generate_filename, test_imyp_generate_filename_two_ext_mid);
	tcase_add_test(tests_imyp_generate_filename, test_imyp_generate_filename_two_ext_raw);

	suite_add_tcase(s, tests_imyp_compare);
	suite_add_tcase(s, tests_imyp_generate_filename);

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
