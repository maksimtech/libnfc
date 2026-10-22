/*-
 * Free/Libre Near Field Communication (NFC) library
 *
 * Windows only: tests of the setenv()/unsetenv() replacements in
 * contrib/win32/stdlib.c, which Windows lacks.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "contrib/windows.h"

static int failures = 0;

#define CHECK(cond) \
  do { \
    if (!(cond)) { \
      fprintf(stderr, "%s:%d: check failed: %s\n", __FILE__, __LINE__, #cond); \
      failures++; \
    } \
  } while (0)

static int
value_is(const char *name, const char *expected)
{
  const char *value = getenv(name);
  return value != NULL && strcmp(value, expected) == 0;
}

static void
test_set_and_unset(void)
{
  CHECK(setenv("LIBNFC_TEST_VAR", "first", 1) == 0);
  CHECK(value_is("LIBNFC_TEST_VAR", "first"));

  unsetenv("LIBNFC_TEST_VAR");
  CHECK(getenv("LIBNFC_TEST_VAR") == NULL);
}

/* POSIX: with overwrite == 0 an existing variable is left alone and the call succeeds. */
static void
test_overwrite_flag(void)
{
  CHECK(setenv("LIBNFC_TEST_VAR", "first", 1) == 0);

  CHECK(setenv("LIBNFC_TEST_VAR", "second", 0) == 0);
  CHECK(value_is("LIBNFC_TEST_VAR", "first"));

  CHECK(setenv("LIBNFC_TEST_VAR", "second", 1) == 0);
  CHECK(value_is("LIBNFC_TEST_VAR", "second"));

  unsetenv("LIBNFC_TEST_VAR");
}

/* The names and values libnfc itself sets are short, but the function must not
 * assume that: a 40-character name and a 200-character value. */
static void
test_long_name_and_value(void)
{
  const char *name = "LIBNFC_TEST_A_VARIABLE_WITH_A_LONG_NAME";
  char value[201];
  memset(value, 'x', sizeof(value) - 1);
  value[sizeof(value) - 1] = '\0';

  CHECK(setenv(name, value, 1) == 0);
  CHECK(value_is(name, value));

  unsetenv(name);
  CHECK(getenv(name) == NULL);
}

int
main(void)
{
  test_set_and_unset();
  test_overwrite_flag();
  test_long_name_and_value();

  if (failures) {
    fprintf(stderr, "%d check(s) failed\n", failures);
    return 1;
  }
  printf("all checks passed\n");
  return 0;
}
