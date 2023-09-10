/*
 * IMYplay - A program for playing iMelody ringtones (IMY files).
 *	-- unit test for the imyputil.c file.
 *
 * Copyright (C) 2018-2023 Bogdan Drozdowski, bogdro (at) users.sourceforge.net
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

#ifdef HAVE_ERRNO_H
# include <errno.h>
#else
static int errno = -1;
#endif

#ifdef HAVE_MALLOC_H
# include <malloc.h>
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

#include "imyputil.h"

/* dummy variable provided by files not included in the test */
int imyp_sig_recvd = 0;

/* ======================================================= */

#ifndef HAVE_STRCASECMP
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
#endif /* ! HAVE_STRCASECMP */

/* ======================================================= */

START_TEST(test_imyp_parse_system_allegro)
{
	printf ("test_imyp_parse_system_allegro\n");
	ck_assert_int_eq (imyp_parse_system ("allegro"), IMYP_CURR_ALLEGRO);
	ck_assert_int_eq (imyp_parse_system ("all"), IMYP_CURR_ALLEGRO);
	ck_assert_int_eq (imyp_parse_system ("allagro"), IMYP_CURR_NONE);
	ck_assert_int_eq (imyp_parse_system ("ballegro"), IMYP_CURR_NONE);
	ck_assert_int_eq (imyp_parse_system ("allegrob"), IMYP_CURR_NONE);
}
END_TEST

START_TEST(test_imyp_parse_system_midi)
{
	printf("test_imyp_parse_system_midi\n");
	ck_assert_int_eq (imyp_parse_system ("midi"), IMYP_CURR_MIDI);
	ck_assert_int_eq (imyp_parse_system ("mid"), IMYP_CURR_MIDI);
	ck_assert_int_eq (imyp_parse_system ("amidi"), IMYP_CURR_NONE);
	ck_assert_int_eq (imyp_parse_system ("bmid"), IMYP_CURR_NONE);
	ck_assert_int_eq (imyp_parse_system ("midia"), IMYP_CURR_NONE);
	ck_assert_int_eq (imyp_parse_system ("midb"), IMYP_CURR_NONE);
}
END_TEST

START_TEST(test_imyp_parse_system_sdl)
{
	printf("test_imyp_parse_system_sdl\n");
	ck_assert_int_eq (imyp_parse_system ("sdl"), IMYP_CURR_SDL);
	ck_assert_int_eq (imyp_parse_system ("asdl"), IMYP_CURR_NONE);
	ck_assert_int_eq (imyp_parse_system ("sdla"), IMYP_CURR_NONE);
}
END_TEST

START_TEST(test_imyp_parse_system_alsa)
{
	printf("test_imyp_parse_system_alsa\n");
	ck_assert_int_eq (imyp_parse_system ("alsa"), IMYP_CURR_ALSA);
	ck_assert_int_eq (imyp_parse_system ("balsa"), IMYP_CURR_NONE);
	ck_assert_int_eq (imyp_parse_system ("alsab"), IMYP_CURR_NONE);
}
END_TEST

START_TEST(test_imyp_parse_system_oss)
{
	printf("test_imyp_parse_system_oss\n");
	ck_assert_int_eq (imyp_parse_system ("oss"), IMYP_CURR_OSS);
	ck_assert_int_eq (imyp_parse_system ("aoss"), IMYP_CURR_NONE);
	ck_assert_int_eq (imyp_parse_system ("ossa"), IMYP_CURR_NONE);
}
END_TEST

START_TEST(test_imyp_parse_system_ao)
{
	printf("test_imyp_parse_system_ao\n");
	ck_assert_int_eq (imyp_parse_system ("ao"), IMYP_CURR_LIBAO);
	ck_assert_int_eq (imyp_parse_system ("libao"), IMYP_CURR_LIBAO);
	ck_assert_int_eq (imyp_parse_system ("aoa"), IMYP_CURR_NONE);
	ck_assert_int_eq (imyp_parse_system ("bao"), IMYP_CURR_NONE);
	ck_assert_int_eq (imyp_parse_system ("alibao"), IMYP_CURR_NONE);
	ck_assert_int_eq (imyp_parse_system ("libaoa"), IMYP_CURR_NONE);
}
END_TEST

START_TEST(test_imyp_parse_system_portaudio)
{
	printf("test_imyp_parse_system_portaudio\n");
	ck_assert_int_eq (imyp_parse_system ("portaudio"), IMYP_CURR_PORTAUDIO);
	ck_assert_int_eq (imyp_parse_system ("port"), IMYP_CURR_PORTAUDIO);
	ck_assert_int_eq (imyp_parse_system ("aportaudio"), IMYP_CURR_NONE);
	ck_assert_int_eq (imyp_parse_system ("aport"), IMYP_CURR_NONE);
	ck_assert_int_eq (imyp_parse_system ("portaudioa"), IMYP_CURR_NONE);
	ck_assert_int_eq (imyp_parse_system ("portb"), IMYP_CURR_NONE);
}
END_TEST

START_TEST(test_imyp_parse_system_jack)
{
	printf("test_imyp_parse_system_jack\n");
	ck_assert_int_eq (imyp_parse_system ("jack"), IMYP_CURR_JACK);
	ck_assert_int_eq (imyp_parse_system ("ajack"), IMYP_CURR_NONE);
	ck_assert_int_eq (imyp_parse_system ("jacka"), IMYP_CURR_NONE);
}
END_TEST

START_TEST(test_imyp_parse_system_pulseaudio)
{
	printf("test_imyp_parse_system_pulseaudio\n");
	ck_assert_int_eq (imyp_parse_system ("pulseaudio"), IMYP_CURR_PULSEAUDIO);
	ck_assert_int_eq (imyp_parse_system ("pulse"), IMYP_CURR_PULSEAUDIO);
	ck_assert_int_eq (imyp_parse_system ("apulseaudio"), IMYP_CURR_NONE);
	ck_assert_int_eq (imyp_parse_system ("apulse"), IMYP_CURR_NONE);
	ck_assert_int_eq (imyp_parse_system ("pulseaudiob"), IMYP_CURR_NONE);
	ck_assert_int_eq (imyp_parse_system ("pulseb"), IMYP_CURR_NONE);
}
END_TEST

START_TEST(test_imyp_parse_system_exec)
{
	printf("test_imyp_parse_system_exec\n");
	ck_assert_int_eq (imyp_parse_system ("exec"), IMYP_CURR_EXEC);
	ck_assert_int_eq (imyp_parse_system ("exe"), IMYP_CURR_EXEC);
	ck_assert_int_eq (imyp_parse_system ("aexec"), IMYP_CURR_NONE);
	ck_assert_int_eq (imyp_parse_system ("execa"), IMYP_CURR_NONE);
	ck_assert_int_eq (imyp_parse_system ("aexe"), IMYP_CURR_NONE);
	ck_assert_int_eq (imyp_parse_system ("exea"), IMYP_CURR_NONE);
}
END_TEST

START_TEST(test_imyp_parse_system_gstreamer)
{
	printf("test_imyp_parse_system_gstreamer\n");
	ck_assert_int_eq (imyp_parse_system ("gstreamer"), IMYP_CURR_GSTREAMER);
	ck_assert_int_eq (imyp_parse_system ("gst"), IMYP_CURR_GSTREAMER);
	ck_assert_int_eq (imyp_parse_system ("agstreamer"), IMYP_CURR_NONE);
	ck_assert_int_eq (imyp_parse_system ("agst"), IMYP_CURR_NONE);
	ck_assert_int_eq (imyp_parse_system ("gstreamera"), IMYP_CURR_NONE);
	ck_assert_int_eq (imyp_parse_system ("gsta"), IMYP_CURR_NONE);
}
END_TEST

START_TEST(test_imyp_parse_system_file)
{
	printf("test_imyp_parse_system_file\n");
	ck_assert_int_eq (imyp_parse_system ("file"), IMYP_CURR_FILE);
	ck_assert_int_eq (imyp_parse_system ("afile"), IMYP_CURR_NONE);
	ck_assert_int_eq (imyp_parse_system ("filea"), IMYP_CURR_NONE);
}
END_TEST

START_TEST(test_imyp_parse_system_pcspeaker)
{
	printf("test_imyp_parse_system_pcspeaker\n");
	ck_assert_int_eq (imyp_parse_system ("speaker"), IMYP_CURR_SPKR);
	ck_assert_int_eq (imyp_parse_system ("aspeaker"), IMYP_CURR_NONE);
	ck_assert_int_eq (imyp_parse_system ("speakerb"), IMYP_CURR_NONE);
	ck_assert_int_eq (imyp_parse_system ("pcspeaker"), IMYP_CURR_SPKR);
	ck_assert_int_eq (imyp_parse_system ("apcspeaker"), IMYP_CURR_NONE);
	ck_assert_int_eq (imyp_parse_system ("pcspeakerb"), IMYP_CURR_NONE);
	ck_assert_int_eq (imyp_parse_system ("spkr"), IMYP_CURR_SPKR);
	ck_assert_int_eq (imyp_parse_system ("aspkr"), IMYP_CURR_NONE);
	ck_assert_int_eq (imyp_parse_system ("spkrb"), IMYP_CURR_NONE);
	ck_assert_int_eq (imyp_parse_system ("pcspkr"), IMYP_CURR_SPKR);
	ck_assert_int_eq (imyp_parse_system ("apcspkr"), IMYP_CURR_NONE);
	ck_assert_int_eq (imyp_parse_system ("pcspkrb"), IMYP_CURR_NONE);
}
END_TEST

START_TEST(test_imyp_parse_system_null)
{
	printf("test_imyp_parse_system_null\n");
	ck_assert_int_eq (imyp_parse_system (NULL), IMYP_CURR_NONE);
}
END_TEST

/* ======================================================= */

START_TEST(test_imyp_get_format_s16le)
{
	printf("test_imyp_get_format_s16le\n");
	ck_assert_int_eq (imyp_get_format ("s16le"), IMYP_SAMPLE_FORMAT_S16LE);
	ck_assert_int_eq (imyp_get_format ("as16le"), IMYP_SAMPLE_FORMAT_UNKNOWN);
	ck_assert_int_eq (imyp_get_format ("s16lea"), IMYP_SAMPLE_FORMAT_UNKNOWN);
}
END_TEST

START_TEST(test_imyp_get_format_s16be)
{
	printf("test_imyp_get_format_s16be\n");
	ck_assert_int_eq (imyp_get_format ("s16be"), IMYP_SAMPLE_FORMAT_S16BE);
	ck_assert_int_eq (imyp_get_format ("as16be"), IMYP_SAMPLE_FORMAT_UNKNOWN);
	ck_assert_int_eq (imyp_get_format ("s16bea"), IMYP_SAMPLE_FORMAT_UNKNOWN);
}
END_TEST

START_TEST(test_imyp_get_format_u16le)
{
	printf("test_imyp_get_format_u16le\n");
	ck_assert_int_eq (imyp_get_format ("u16le"), IMYP_SAMPLE_FORMAT_U16LE);
	ck_assert_int_eq (imyp_get_format ("au16le"), IMYP_SAMPLE_FORMAT_UNKNOWN);
	ck_assert_int_eq (imyp_get_format ("u16lea"), IMYP_SAMPLE_FORMAT_UNKNOWN);
}
END_TEST

START_TEST(test_imyp_get_format_u16be)
{
	printf("test_imyp_get_format_u16be\n");
	ck_assert_int_eq (imyp_get_format ("u16be"), IMYP_SAMPLE_FORMAT_U16BE);
	ck_assert_int_eq (imyp_get_format ("au16be"), IMYP_SAMPLE_FORMAT_UNKNOWN);
	ck_assert_int_eq (imyp_get_format ("u16bea"), IMYP_SAMPLE_FORMAT_UNKNOWN);
}
END_TEST

START_TEST(test_imyp_get_format_s8le)
{
	printf("test_imyp_get_format_s8le\n");
	ck_assert_int_eq (imyp_get_format ("s8le"), IMYP_SAMPLE_FORMAT_S8LE);
	ck_assert_int_eq (imyp_get_format ("as8le"), IMYP_SAMPLE_FORMAT_UNKNOWN);
	ck_assert_int_eq (imyp_get_format ("s8lea"), IMYP_SAMPLE_FORMAT_UNKNOWN);
}
END_TEST

START_TEST(test_imyp_get_format_s8be)
{
	printf("test_imyp_get_format_s8be\n");
	ck_assert_int_eq (imyp_get_format ("s8be"), IMYP_SAMPLE_FORMAT_S8BE);
	ck_assert_int_eq (imyp_get_format ("as8be"), IMYP_SAMPLE_FORMAT_UNKNOWN);
	ck_assert_int_eq (imyp_get_format ("s8bea"), IMYP_SAMPLE_FORMAT_UNKNOWN);
}
END_TEST

START_TEST(test_imyp_get_format_u8le)
{
	printf("test_imyp_get_format_u8le\n");
	ck_assert_int_eq (imyp_get_format ("u8le"), IMYP_SAMPLE_FORMAT_U8LE);
	ck_assert_int_eq (imyp_get_format ("au8le"), IMYP_SAMPLE_FORMAT_UNKNOWN);
	ck_assert_int_eq (imyp_get_format ("u8lea"), IMYP_SAMPLE_FORMAT_UNKNOWN);
}
END_TEST

START_TEST(test_imyp_get_format_u8be)
{
	printf("test_imyp_get_format_u8be\n");
	ck_assert_int_eq (imyp_get_format ("u8be"), IMYP_SAMPLE_FORMAT_U8BE);
	ck_assert_int_eq (imyp_get_format ("au8be"), IMYP_SAMPLE_FORMAT_UNKNOWN);
	ck_assert_int_eq (imyp_get_format ("u8bea"), IMYP_SAMPLE_FORMAT_UNKNOWN);
}
END_TEST

START_TEST(test_imyp_get_format_null)
{
	printf("test_imyp_get_format_null\n");
	ck_assert_int_eq (imyp_get_format (NULL), IMYP_SAMPLE_FORMAT_UNKNOWN);
}
END_TEST

/* ======================================================= */

START_TEST(test_imyp_generate_filename_no_ext_mid)
{
	char * res;

	printf("test_imyp_generate_filename_no_ext_mid\n");
	res = imyp_generate_filename("test", ".mid");
	ck_assert_str_eq(res, "test.mid");
	free (res);
}
END_TEST

START_TEST(test_imyp_generate_filename_no_ext_raw)
{
	char * res;

	printf("test_imyp_generate_filename_no_ext_raw\n");
	res = imyp_generate_filename("test", ".raw");
	ck_assert_str_eq(res, "test.raw");
	free (res);
}
END_TEST

START_TEST(test_imyp_generate_filename_mid_mid)
{
	char * res;

	printf("test_imyp_generate_filename_mid_mid\n");
	res = imyp_generate_filename("test.mid", ".mid");
	ck_assert_str_eq(res, "test.mid");
	free (res);
}
END_TEST

START_TEST(test_imyp_generate_filename_raw_raw)
{
	char * res;

	printf("test_imyp_generate_filename_raw_raw\n");
	res = imyp_generate_filename("test.raw", ".raw");
	ck_assert_str_eq(res, "test.raw");
	free (res);
}
END_TEST

START_TEST(test_imyp_generate_filename_one_ext_mid)
{
	char * res;

	printf("test_imyp_generate_filename_one_ext_mid\n");
	res = imyp_generate_filename("test.imy", ".mid");
	ck_assert_str_eq(res, "test.mid");
	free (res);
}
END_TEST

START_TEST(test_imyp_generate_filename_one_ext_raw)
{
	char * res;

	printf("test_imyp_generate_filename_one_ext\n");
	res = imyp_generate_filename("test.imy", ".raw");
	ck_assert_str_eq(res, "test.raw");
	free (res);
}
END_TEST

START_TEST(test_imyp_generate_filename_two_ext_mid)
{
	char * res;

	printf("test_imyp_generate_filename_two_ext_mid\n");
	res = imyp_generate_filename("test.mid.imy", ".mid");
	ck_assert_str_eq(res, "test.mid.mid");
	free (res);
}
END_TEST

START_TEST(test_imyp_generate_filename_two_ext_raw)
{
	char * res;

	printf("test_imyp_generate_filename_two_ext\n");
	res = imyp_generate_filename("test.raw.imy", ".raw");
	ck_assert_str_eq(res, "test.raw.raw");
	free (res);
}
END_TEST

/* ======================================================= */

static Suite * imy_create_suite(void)
{
	Suite * s = suite_create("imyplay_util");

#ifndef HAVE_STRCASECMP
	TCase * tests_imyp_compare = tcase_create("imyp_compare");
#endif /* ! HAVE_STRCASECMP */
	TCase * tests_imyp_parse_system = tcase_create("imyp_parse_system");
	TCase * tests_imyp_get_format = tcase_create("imyp_get_format");
	TCase * tests_imyp_generate_filename = tcase_create("imyp_generate_filename");

#ifndef HAVE_STRCASECMP
	tcase_add_test (tests_imyp_compare, test_imyp_compare_both_null);
	tcase_add_test (tests_imyp_compare, test_imyp_compare_first_null);
	tcase_add_test (tests_imyp_compare, test_imyp_compare_second_null);
	tcase_add_test (tests_imyp_compare, test_imyp_compare_equal);
	tcase_add_test (tests_imyp_compare, test_imyp_compare_equal_case_ins);
	tcase_add_test (tests_imyp_compare, test_imyp_compare_equal_diff_len);
	tcase_add_test (tests_imyp_compare, test_imyp_compare_nonequal);
#endif /* ! HAVE_STRCASECMP */

	tcase_add_test (tests_imyp_parse_system, test_imyp_parse_system_allegro);
	tcase_add_test (tests_imyp_parse_system, test_imyp_parse_system_midi);
	tcase_add_test (tests_imyp_parse_system, test_imyp_parse_system_sdl);
	tcase_add_test (tests_imyp_parse_system, test_imyp_parse_system_alsa);
	tcase_add_test (tests_imyp_parse_system, test_imyp_parse_system_oss);
	tcase_add_test (tests_imyp_parse_system, test_imyp_parse_system_ao);
	tcase_add_test (tests_imyp_parse_system, test_imyp_parse_system_portaudio);
	tcase_add_test (tests_imyp_parse_system, test_imyp_parse_system_jack);
	tcase_add_test (tests_imyp_parse_system, test_imyp_parse_system_pulseaudio);
	tcase_add_test (tests_imyp_parse_system, test_imyp_parse_system_exec);
	tcase_add_test (tests_imyp_parse_system, test_imyp_parse_system_gstreamer);
	tcase_add_test (tests_imyp_parse_system, test_imyp_parse_system_file);
	tcase_add_test (tests_imyp_parse_system, test_imyp_parse_system_pcspeaker);
	tcase_add_test (tests_imyp_parse_system, test_imyp_parse_system_null);

	tcase_add_test (tests_imyp_get_format, test_imyp_get_format_s16le);
	tcase_add_test (tests_imyp_get_format, test_imyp_get_format_s16be);
	tcase_add_test (tests_imyp_get_format, test_imyp_get_format_u16le);
	tcase_add_test (tests_imyp_get_format, test_imyp_get_format_u16be);
	tcase_add_test (tests_imyp_get_format, test_imyp_get_format_s8le);
	tcase_add_test (tests_imyp_get_format, test_imyp_get_format_s8be);
	tcase_add_test (tests_imyp_get_format, test_imyp_get_format_u8le);
	tcase_add_test (tests_imyp_get_format, test_imyp_get_format_u8be);
	tcase_add_test (tests_imyp_get_format, test_imyp_get_format_null);

	tcase_add_test (tests_imyp_generate_filename, test_imyp_generate_filename_no_ext_mid);
	tcase_add_test (tests_imyp_generate_filename, test_imyp_generate_filename_mid_mid);
	tcase_add_test (tests_imyp_generate_filename, test_imyp_generate_filename_no_ext_raw);
	tcase_add_test (tests_imyp_generate_filename, test_imyp_generate_filename_raw_raw);
	tcase_add_test (tests_imyp_generate_filename, test_imyp_generate_filename_one_ext_mid);
	tcase_add_test (tests_imyp_generate_filename, test_imyp_generate_filename_one_ext_raw);
	tcase_add_test (tests_imyp_generate_filename, test_imyp_generate_filename_two_ext_mid);
	tcase_add_test (tests_imyp_generate_filename, test_imyp_generate_filename_two_ext_raw);

#ifndef HAVE_STRCASECMP
	suite_add_tcase(s, tests_imyp_compare);
#endif /* ! HAVE_STRCASECMP */
	suite_add_tcase(s, tests_imyp_parse_system);
	suite_add_tcase(s, tests_imyp_get_format);
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
