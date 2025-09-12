/*
 * IMYplay - A program for playing iMelody ringtones (IMY files).
 *	-- unit test for the imyp_cmd.c file.
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
#include "imypwrap.h"
#include "imyputil.h"
#include "imyparse.h"
#include "imyp_sig.h"
#include "imyp_cmd.h"

#include <stdio.h>

/* dummy variables and functions provided by files not included in the test */
volatile sig_atomic_t imyp_sig_recvd = 0;
char progname[] = {'c', 'm', 'd', 't', 'e', 's', 't', '\0'};
static const char * used_dev;
static const char * used_output;
static const char * used_output_file;
static const char * played_file;
static int used_exec;
static int used_file;
static int used_midi;
static int used_midi_instr;
static int used_wav;

void imyp_set_sigh (imyp_error_type * const error IMYP_ATTR ((unused)))
{
}

void imyp_pause (const int milliseconds IMYP_ATTR ((unused)),
	imyp_backend_t * const curr IMYP_ATTR ((unused)),
	const int is_note IMYP_ATTR ((unused)),
	void * const buf IMYP_ATTR ((unused)),
	int bufsize IMYP_ATTR ((unused)))
{
}

void imyp_put_text (const char * const text IMYP_ATTR ((unused)),
	imyp_backend_t * const curr IMYP_ATTR ((unused)))
{
}

int imyp_play_tune (const double freq IMYP_ATTR ((unused)),
	const int volume_level IMYP_ATTR ((unused)),
	const int duration IMYP_ATTR ((unused)),
	void * const buf IMYP_ATTR ((unused)),
	int bufsize IMYP_ATTR ((unused)),
	imyp_backend_t * const curr IMYP_ATTR ((unused)))
{
	return 0;
}

int imyp_lib_init (imyp_backend_t * const curr IMYP_ATTR ((unused)),
	const int want_midi,
	const char * const filename,
	const int want_exec,
	const int midi_instrument,
	const int want_file,
	const int want_wav,
	const char * const out_file)
{
	used_dev = filename;
	used_exec = want_exec;
	used_file = want_file;
	used_output_file = out_file;
	used_midi = want_midi;
	used_midi_instr = midi_instrument;
	used_wav = want_wav;
	return 0;
}

int imyp_init_selected (imyp_backend_t * const curr IMYP_ATTR ((unused)),
	const char output_system[],
	const char * const filename IMYP_ATTR ((unused)),
	const int midi_instrument IMYP_ATTR ((unused)),
	const char * const out_file IMYP_ATTR ((unused)))
{
	used_output = output_system;
	return 0;
}

int imyp_lib_close (imyp_backend_t * const curr IMYP_ATTR ((unused)))
{
	return 0;
}

void imyp_report_versions (const imyp_backend_t * const curr IMYP_ATTR ((unused)))
{
}

int imyp_play_file (const char * const file_name,
	imyp_backend_t * const curr IMYP_ATTR ((unused)))
{
	played_file = file_name;
	return 0;
}

/* ======================================================= */

START_TEST(test_imyp_parse_cmdline_no_argc)
{
	int res;
	char* argv[] = {progname, NULL};

	printf ("test_imyp_parse_cmdline_no_argc\n");
	res = imyplay_parse_cmdline (0, argv);
	ck_assert_int_eq (res, -1);
}
END_TEST

START_TEST(test_imyp_parse_cmdline_no_argv)
{
	int res;

	printf ("test_imyp_parse_cmdline_no_argv\n");
	res = imyplay_parse_cmdline (2, NULL);
	ck_assert_int_eq (res, -1);
}
END_TEST

START_TEST(test_imyp_parse_cmdline_argv0_null)
{
	int res;
	char* argv[] = {NULL};

	printf ("test_imyp_parse_cmdline_argv0_null\n");
	res = imyplay_parse_cmdline (1, argv);
	ck_assert_int_eq (res, -1);
}
END_TEST

START_TEST(test_imyp_parse_cmdline_help1)
{
	int res;
	char arg[] = {'-', '?', '\0'};
	char* argv[] = {progname, arg, NULL};

	printf ("test_imyp_parse_cmdline_help1\n");
	res = imyplay_parse_cmdline (sizeof(argv) / sizeof(argv[0]) - 1, argv);
	ck_assert_int_eq (res, 1);
}
END_TEST

START_TEST(test_imyp_parse_cmdline_help2)
{
	int res;
	char arg[] = {'-', 'h', '\0'};
	char* argv[] = {progname, arg, NULL};

	printf ("test_imyp_parse_cmdline_help2\n");
	res = imyplay_parse_cmdline (sizeof(argv) / sizeof(argv[0]) - 1, argv);
	ck_assert_int_eq (res, 1);
}
END_TEST

START_TEST(test_imyp_parse_cmdline_help3)
{
	int res;
	char arg[] = {'-', '-', 'h', 'e', 'l', 'p', '\0'};
	char* argv[] = {progname, arg, NULL};

	printf ("test_imyp_parse_cmdline_help3\n");
	res = imyplay_parse_cmdline (sizeof(argv) / sizeof(argv[0]) - 1, argv);
	ck_assert_int_eq (res, 1);
}
END_TEST

START_TEST(test_imyp_parse_cmdline_version1)
{
	int res;
	char arg[] = {'-', 'V', '\0'};
	char* argv[] = {progname, arg, NULL};

	printf ("test_imyp_parse_cmdline_version1\n");
	res = imyplay_parse_cmdline (sizeof(argv) / sizeof(argv[0]) - 1, argv);
	ck_assert_int_eq (res, 1);
}
END_TEST

START_TEST(test_imyp_parse_cmdline_version2)
{
	int res;
	char arg[] = {'-', '-', 'v', 'e', 'r', 's', 'i', 'o', 'n', '\0'};
	char* argv[] = {progname, arg, NULL};

	printf ("test_imyp_parse_cmdline_version2\n");
	res = imyplay_parse_cmdline (sizeof(argv) / sizeof(argv[0]) - 1, argv);
	ck_assert_int_eq (res, 1);
}
END_TEST

START_TEST(test_imyp_parse_cmdline_license1)
{
	int res;
	char arg[] = {'-', 'l', '\0'};
	char* argv[] = {progname, arg, NULL};

	printf ("test_imyp_parse_cmdline_license1\n");
	res = imyplay_parse_cmdline (sizeof(argv) / sizeof(argv[0]) - 1, argv);
	ck_assert_int_eq (res, 1);
}
END_TEST

START_TEST(test_imyp_parse_cmdline_license2)
{
	int res;
	char arg[] = {'-', '-', 'l', 'i', 'c', 'e', 'n', 's', 'e', '\0'};
	char* argv[] = {progname, arg, NULL};

	printf ("test_imyp_parse_cmdline_license2\n");
	res = imyplay_parse_cmdline (sizeof(argv) / sizeof(argv[0]) - 1, argv);
	ck_assert_int_eq (res, 1);
}
END_TEST

/* ======================================================= */

START_TEST(test_imyp_parse_cmdline_dev1)
{
	int res;
	char arg1[] = {'-', 'd', '\0'};
	char arg2[] = {'/', 'd', 'e', 'v', '/', 'd', 's', 'p', '\0'};
	char arg3[] = {'t', 'e', 's', 't', '.', 'i', 'm', 'y', '\0'};
	char* argv[] = {progname, arg1, arg2, arg3, NULL};

	printf ("test_imyp_parse_cmdline_dev1\n");
	played_file = "";
	res = imyplay_parse_cmdline (sizeof(argv) / sizeof(argv[0]) - 1, argv);
	ck_assert_int_eq (res, 0);
	ck_assert_str_eq (used_dev, "/dev/dsp");
	ck_assert_str_eq (played_file, "test.imy");
}
END_TEST

START_TEST(test_imyp_parse_cmdline_dev2)
{
	int res;
	char arg1[] = {'-', '-', 'd', 'e', 'v', 'i', 'c', 'e', '\0'};
	char arg2[] = {'/', 'd', 'e', 'v', '/', 'd', 's', 'p', '2', '\0'};
	char arg3[] = {'t', 'e', 's', 't', '.', 'i', 'm', 'y', '\0'};
	char* argv[] = {progname, arg1, arg2, arg3, NULL};

	printf ("test_imyp_parse_cmdline_dev2\n");
	played_file = "";
	res = imyplay_parse_cmdline (sizeof(argv) / sizeof(argv[0]) - 1, argv);
	ck_assert_int_eq (res, 0);
	ck_assert_str_eq (used_dev, "/dev/dsp2");
	ck_assert_str_eq (played_file, "test.imy");
}
END_TEST

START_TEST(test_imyp_parse_cmdline_dev_no_file)
{
	int res;
	char arg1[] = {'-', '-', 'd', 'e', 'v', 'i', 'c', 'e', '\0'};
	char arg2[] = {'/', 'd', 'e', 'v', '/', 'd', 's', 'p', '2', '\0'};
	char* argv[] = {progname, arg1, arg2, NULL};

	printf ("test_imyp_parse_cmdline_dev2\n");
	res = imyplay_parse_cmdline (sizeof(argv) / sizeof(argv[0]) - 1, argv);
	ck_assert_int_eq (res, -1);
}
END_TEST

/* ======================================================= */

START_TEST(test_imyp_parse_cmdline_output1)
{
	int res;
	char arg1[] = {'-', 'o', '\0'};
	char arg2[] = {'o', 's', 's', '\0'};
	char arg3[] = {'t', 'e', 's', 't', '.', 'i', 'm', 'y', '\0'};
	char* argv[] = {progname, arg1, arg2, arg3, NULL};

	printf ("test_imyp_parse_cmdline_output1\n");
	used_output = "";
	played_file = "";
	res = imyplay_parse_cmdline (sizeof(argv) / sizeof(argv[0]) - 1, argv);
	ck_assert_int_eq (res, 0);
	ck_assert_str_eq (used_output, "oss");
	ck_assert_str_eq (played_file, "test.imy");
}
END_TEST

START_TEST(test_imyp_parse_cmdline_output2)
{
	int res;
	char arg1[] = {'-', '-', 'o', 'u', 't', 'p', 'u', 't', '\0'};
	char arg2[] = {'o', 's', 's', '\0'};
	char arg3[] = {'t', 'e', 's', 't', '.', 'i', 'm', 'y', '\0'};
	char* argv[] = {progname, arg1, arg2, arg3, NULL};

	printf ("test_imyp_parse_cmdline_output2\n");
	used_output = "";
	played_file = "";
	res = imyplay_parse_cmdline (sizeof(argv) / sizeof(argv[0]) - 1, argv);
	ck_assert_int_eq (res, 0);
	ck_assert_str_eq (used_output, "oss");
	ck_assert_str_eq (played_file, "test.imy");
}
END_TEST

START_TEST(test_imyp_parse_cmdline_output_no_file)
{
	int res;
	char arg1[] = {'-', '-', 'o', 'u', 't', 'p', 'u', 't', '\0'};
	char arg2[] = {'o', 's', 's', '\0'};
	char* argv[] = {progname, arg1, arg2, NULL};

	printf ("test_imyp_parse_cmdline_output_no_file\n");
	res = imyplay_parse_cmdline (sizeof(argv) / sizeof(argv[0]) - 1, argv);
	ck_assert_int_eq (res, -1);
}
END_TEST

START_TEST(test_imyp_parse_cmdline_output_by_name)
{
	int res;
	char arg_name[] = {'i', 'm', 'y', 'p', 'l', 'a', 'y', '-', 'o', 's', 's', '\0'};
	char arg1[] = {'t', 'e', 's', 't', '.', 'i', 'm', 'y', '\0'};
	char* argv[] = {arg_name, arg1, NULL};

	printf ("test_imyp_parse_cmdline_output2\n");
	used_output = "";
	played_file = "";
	res = imyplay_parse_cmdline (sizeof(argv) / sizeof(argv[0]) - 1, argv);
	ck_assert_int_eq (res, 0);
	ck_assert_str_eq (used_output, "oss");
	ck_assert_str_eq (played_file, "test.imy");
}
END_TEST

/* ======================================================= */

#ifdef IMYP_HAVE_EXEC
START_TEST(test_imyp_parse_cmdline_exec1)
{
	int res;
	char arg1[] = {'-', 'e', '\0'};
	char arg2[] = {'e', 'c', 'h', 'o', '\0'};
	char arg3[] = {'t', 'e', 's', 't', '.', 'i', 'm', 'y', '\0'};
	char* argv[] = {progname, arg1, arg2, arg3, NULL};

	printf ("test_imyp_parse_cmdline_exec1\n");
	used_exec = 0;
	played_file = "";
	res = imyplay_parse_cmdline (sizeof(argv) / sizeof(argv[0]) - 1, argv);
	ck_assert_int_eq (res, 0);
	ck_assert_int_eq (used_exec, 1);
	ck_assert_str_eq (played_file, "test.imy");
}
END_TEST

START_TEST(test_imyp_parse_cmdline_exec2)
{
	int res;
	char arg1[] = {'-', '-', 'e', 'x', 'e', 'c', '\0'};
	char arg2[] = {'e', 'c', 'h', 'o', '\0'};
	char arg3[] = {'t', 'e', 's', 't', '.', 'i', 'm', 'y', '\0'};
	char* argv[] = {progname, arg1, arg2, arg3, NULL};

	printf ("test_imyp_parse_cmdline_exec2\n");
	used_exec = 0;
	played_file = "";
	res = imyplay_parse_cmdline (sizeof(argv) / sizeof(argv[0]) - 1, argv);
	ck_assert_int_eq (res, 0);
	ck_assert_int_eq (used_exec, 1);
	ck_assert_str_eq (played_file, "test.imy");
}
END_TEST

START_TEST(test_imyp_parse_cmdline_exec_no_file)
{
	int res;
	char arg1[] = {'-', '-', 'e', 'x', 'e', 'c', '\0'};
	char arg2[] = {'e', 'c', 'h', 'o', '\0'};
	char* argv[] = {progname, arg1, arg2, NULL};

	printf ("test_imyp_parse_cmdline_exec_no_file\n");
	res = imyplay_parse_cmdline (sizeof(argv) / sizeof(argv[0]) - 1, argv);
	ck_assert_int_eq (res, -1);
}
END_TEST
#endif

/* ======================================================= */

#ifdef IMYP_HAVE_FILE
START_TEST(test_imyp_parse_cmdline_file1)
{
	int res;
	char arg1[] = {'-', 'f', '\0'};
	char arg2[] = {'t', 'e', 's', 't', '.', 'r', 'a', 'w', '\0'};
	char arg3[] = {'t', 'e', 's', 't', '.', 'i', 'm', 'y', '\0'};
	char* argv[] = {progname, arg1, arg2, arg3, NULL};

	printf ("test_imyp_parse_cmdline_file1\n");
	used_file = 0;
	played_file = "";
	res = imyplay_parse_cmdline (sizeof(argv) / sizeof(argv[0]) - 1, argv);
	ck_assert_int_eq (res, 0);
	ck_assert_int_eq (used_file, 1);
	ck_assert_str_eq (played_file, "test.imy");
}
END_TEST

START_TEST(test_imyp_parse_cmdline_file2)
{
	int res;
	char arg1[] = {'-', '-', 'f', 'i', 'l', 'e', '\0'};
	char arg2[] = {'t', 'e', 's', 't', '.', 'r', 'a', 'w', '\0'};
	char arg3[] = {'t', 'e', 's', 't', '.', 'i', 'm', 'y', '\0'};
	char* argv[] = {progname, arg1, arg2, arg3, NULL};

	printf ("test_imyp_parse_cmdline_file2\n");
	used_file = 0;
	used_output_file = "";
	played_file = "";
	res = imyplay_parse_cmdline (sizeof(argv) / sizeof(argv[0]) - 1, argv);
	ck_assert_int_eq (res, 0);
	ck_assert_int_eq (used_file, 1);
	ck_assert_str_eq (used_output_file, "test.raw");
	ck_assert_str_eq (played_file, "test.imy");
}
END_TEST

START_TEST(test_imyp_parse_cmdline_file_no_file)
{
	int res;
	char arg1[] = {'-', '-', 'f', 'i', 'l', 'e', '\0'};
	char arg2[] = {'t', 'e', 's', 't', '.', 'r', 'a', 'w', '\0'};
	char* argv[] = {progname, arg1, arg2, NULL};

	printf ("test_imyp_parse_cmdline_file_no_file\n");
	res = imyplay_parse_cmdline (sizeof(argv) / sizeof(argv[0]) - 1, argv);
	ck_assert_int_eq (res, -1);
}
END_TEST
#endif

/* ======================================================= */

#ifdef IMYP_HAVE_MIDI
START_TEST(test_imyp_parse_cmdline_midi)
{
	int res;
	char arg1[] = {'-', '-', 't', 'o', '-', 'm' ,'i', 'd', 'i', '\0'};
	char arg2[] = {'t', 'e', 's', 't', '.', 'i', 'm', 'y', '\0'};
	char* argv[] = {progname, arg1, arg2, NULL};

	printf ("test_imyp_parse_cmdline_midi\n");
	used_midi = 0;
	played_file = "";
	res = imyplay_parse_cmdline (sizeof(argv) / sizeof(argv[0]) - 1, argv);
	ck_assert_int_eq (res, 0);
	ck_assert_int_eq (used_midi, 1);
	ck_assert_str_eq (played_file, "test.imy");
}
END_TEST

START_TEST(test_imyp_parse_cmdline_midi_with_instr)
{
	int res;
	char arg1[] = {'-', '-', 't', 'o', '-', 'm' ,'i', 'd', 'i', '\0'};
	char arg2[] = {'-', '-', 'm' ,'i', 'd', 'i', '-', 'i', 'n', 's', 't', 'r', '\0'};
	char arg3[] = {'2', '\0'};
	char arg4[] = {'t', 'e', 's', 't', '.', 'i', 'm', 'y', '\0'};
	char* argv[] = {progname, arg1, arg2, arg3, arg4, NULL};

	printf ("test_imyp_parse_cmdline_midi_with_instr\n");
	used_midi = 0;
	used_midi_instr = 0;
	played_file = "";
	res = imyplay_parse_cmdline (sizeof(argv) / sizeof(argv[0]) - 1, argv);
	ck_assert_int_eq (res, 0);
	ck_assert_int_eq (used_midi, 1);
	ck_assert_int_eq (used_midi_instr, 2);
	ck_assert_str_eq (played_file, "test.imy");
}
END_TEST

START_TEST(test_imyp_parse_cmdline_midi_with_instr_no_param)
{
	int res;
	char arg1[] = {'-', '-', 't', 'o', '-', 'm' ,'i', 'd', 'i', '\0'};
	char arg2[] = {'-', '-', 'm' ,'i', 'd', 'i', '-', 'i', 'n', 's', 't', 'r', '\0'};
	char arg3[] = {'t', 'e', 's', 't', '.', 'i', 'm', 'y', '\0'};
	char* argv[] = {progname, arg1, arg2, arg3, NULL};

	printf ("test_imyp_parse_cmdline_midi_with_instr_no_param\n");
	res = imyplay_parse_cmdline (sizeof(argv) / sizeof(argv[0]) - 1, argv);
	ck_assert_int_eq (res, -1);
}
END_TEST

START_TEST(test_imyp_parse_cmdline_midi_no_file)
{
	int res;
	char arg1[] = {'-', '-', 't', 'o', '-', 'm' ,'i', 'd', 'i', '\0'};
	char* argv[] = {progname, arg1, NULL};

	printf ("test_imyp_parse_cmdline_midi_no_file\n");
	res = imyplay_parse_cmdline (sizeof(argv) / sizeof(argv[0]) - 1, argv);
	ck_assert_int_eq (res, -1);
}
END_TEST
#endif

/* ======================================================= */

#ifdef IMYP_HAVE_MIDI
START_TEST(test_imyp_parse_cmdline_wav)
{
	int res;
	char arg1[] = {'-', '-', 't', 'o', '-', 'w' ,'a', 'v', '\0'};
	char arg2[] = {'t', 'e', 's', 't', '.', 'i', 'm', 'y', '\0'};
	char* argv[] = {progname, arg1, arg2, NULL};

	printf ("test_imyp_parse_cmdline_wav\n");
	used_wav = 0;
	played_file = "";
	res = imyplay_parse_cmdline (sizeof(argv) / sizeof(argv[0]) - 1, argv);
	ck_assert_int_eq (res, 0);
	ck_assert_int_eq (used_wav, 1);
	ck_assert_str_eq (played_file, "test.imy");
}
END_TEST

START_TEST(test_imyp_parse_cmdline_wav_no_file)
{
	int res;
	char arg1[] = {'-', '-', 't', 'o', '-', 'w' ,'a', 'v', '\0'};
	char* argv[] = {progname, arg1, NULL};

	printf ("test_imyp_parse_cmdline_wav_no_file\n");
	res = imyplay_parse_cmdline (sizeof(argv) / sizeof(argv[0]) - 1, argv);
	ck_assert_int_eq (res, -1);
}
END_TEST
#endif

/* ======================================================= */

static Suite * imy_create_suite(void)
{
	Suite * s = suite_create("imyplay_cmdline");

	TCase * tests_imyp_parse_cmdline = tcase_create("tests_imyp_parse_cmdline");

	tcase_add_test (tests_imyp_parse_cmdline, test_imyp_parse_cmdline_no_argc);
	tcase_add_test (tests_imyp_parse_cmdline, test_imyp_parse_cmdline_no_argv);
	tcase_add_test (tests_imyp_parse_cmdline, test_imyp_parse_cmdline_argv0_null);
	tcase_add_test (tests_imyp_parse_cmdline, test_imyp_parse_cmdline_help1);
	tcase_add_test (tests_imyp_parse_cmdline, test_imyp_parse_cmdline_help2);
	tcase_add_test (tests_imyp_parse_cmdline, test_imyp_parse_cmdline_help3);
	tcase_add_test (tests_imyp_parse_cmdline, test_imyp_parse_cmdline_version1);
	tcase_add_test (tests_imyp_parse_cmdline, test_imyp_parse_cmdline_version2);
	tcase_add_test (tests_imyp_parse_cmdline, test_imyp_parse_cmdline_license1);
	tcase_add_test (tests_imyp_parse_cmdline, test_imyp_parse_cmdline_license2);

	tcase_add_test (tests_imyp_parse_cmdline, test_imyp_parse_cmdline_dev1);
	tcase_add_test (tests_imyp_parse_cmdline, test_imyp_parse_cmdline_dev2);
	tcase_add_test (tests_imyp_parse_cmdline, test_imyp_parse_cmdline_dev_no_file);

	tcase_add_test (tests_imyp_parse_cmdline, test_imyp_parse_cmdline_output1);
	tcase_add_test (tests_imyp_parse_cmdline, test_imyp_parse_cmdline_output2);
	tcase_add_test (tests_imyp_parse_cmdline, test_imyp_parse_cmdline_output_no_file);
	tcase_add_test (tests_imyp_parse_cmdline, test_imyp_parse_cmdline_output_by_name);

#ifdef IMYP_HAVE_EXEC
	tcase_add_test (tests_imyp_parse_cmdline, test_imyp_parse_cmdline_exec1);
	tcase_add_test (tests_imyp_parse_cmdline, test_imyp_parse_cmdline_exec2);
	tcase_add_test (tests_imyp_parse_cmdline, test_imyp_parse_cmdline_exec_no_file);
#endif
#ifdef IMYP_HAVE_FILE
	tcase_add_test (tests_imyp_parse_cmdline, test_imyp_parse_cmdline_file1);
	tcase_add_test (tests_imyp_parse_cmdline, test_imyp_parse_cmdline_file2);
	tcase_add_test (tests_imyp_parse_cmdline, test_imyp_parse_cmdline_file_no_file);
#endif
#ifdef IMYP_HAVE_MIDI
	tcase_add_test (tests_imyp_parse_cmdline, test_imyp_parse_cmdline_midi);
	tcase_add_test (tests_imyp_parse_cmdline, test_imyp_parse_cmdline_midi_with_instr);
	tcase_add_test (tests_imyp_parse_cmdline, test_imyp_parse_cmdline_midi_with_instr_no_param);
	tcase_add_test (tests_imyp_parse_cmdline, test_imyp_parse_cmdline_midi_no_file);
#endif
#ifdef IMYP_HAVE_WAV
	tcase_add_test (tests_imyp_parse_cmdline, test_imyp_parse_cmdline_wav);
	tcase_add_test (tests_imyp_parse_cmdline, test_imyp_parse_cmdline_wav_no_file);
#endif
	suite_add_tcase(s, tests_imyp_parse_cmdline);

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
