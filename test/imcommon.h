/*
 * IMYplay - A program for playing iMelody ringtones (IMY files).
 *	-- unit test common header file.
 *
 * Copyright (C) 2023-2025 Bogdan Drozdowski, bogdro (at) users.sourceforge.net
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

#ifndef IMYP_TEST_COMMON
# define IMYP_TEST_COMMON 1

# include <check.h>

/* compatibility with older check versions */
# ifndef ck_abort
#  define ck_abort() ck_abort_msg(NULL)
#  define ck_abort_msg fail
#  define ck_assert(C) ck_assert_msg(C, NULL)
#  define ck_assert_msg fail_unless
# endif

# ifndef _ck_assert_int
#  define _ck_assert_int(X, O, Y) ck_assert_msg((X) O (Y), "Assertion '"#X#O#Y"' failed: "#X"==%d, "#Y"==%d", X, Y)
#  define ck_assert_int_eq(X, Y) _ck_assert_int(X, ==, Y)
#  define ck_assert_int_ne(X, Y) _ck_assert_int(X, !=, Y)
# endif

# ifndef _ck_assert_str
#  define _ck_assert_str(C, X, O, Y) ck_assert_msg(C, "Assertion '"#X#O#Y"' failed: "#X"==\"%s\", "#Y"==\"%s\"", X, Y)
#  define ck_assert_str_eq(X, Y) _ck_assert_str(!strcmp(X, Y), X, ==, Y)
#  define ck_assert_str_ne(X, Y) _ck_assert_str(strcmp(X, Y), X, !=, Y)
# endif

# define IMYP_RESULT_TEST_SKIPPED 77 /* Automake */

const char * imyp_progname = "test";

#endif /* IMYP_TEST_COMMON */
