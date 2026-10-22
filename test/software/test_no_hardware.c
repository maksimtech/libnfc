/*-
 * Free/Libre Near Field Communication (NFC) library
 *
 * Tests that need no NFC reader: they run on any CI machine.
 * Reader-dependent tests live in the parent directory.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 */

#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include <nfc/nfc.h>

static int failures = 0;

#define CHECK(cond) \
  do { \
    if (!(cond)) { \
      fprintf(stderr, "%s:%d: check failed: %s\n", __FILE__, __LINE__, #cond); \
      failures++; \
    } \
  } while (0)

/* CRC catalogue check values for the ASCII string "123456789":
 * CRC-16/ISO-IEC-14443-3-A = 0xBF05, CRC-16/ISO-IEC-14443-3-B (X-25) = 0x906E.
 * libnfc returns the CRC in transmission order, least significant byte first. */
static void
test_crc(void)
{
  const uint8_t check[] = { '1', '2', '3', '4', '5', '6', '7', '8', '9' };
  uint8_t crc[2];

  iso14443a_crc((uint8_t *)check, sizeof(check), crc);
  CHECK(crc[0] == 0x05 && crc[1] == 0xBF);

  iso14443b_crc((uint8_t *)check, sizeof(check), crc);
  CHECK(crc[0] == 0x6E && crc[1] == 0x90);

  /* Worked example from ISO/IEC 14443-3: CRC_A of 12 34 is 26 CF. */
  const uint8_t iso_example[] = { 0x12, 0x34 };
  iso14443a_crc((uint8_t *)iso_example, sizeof(iso_example), crc);
  CHECK(crc[0] == 0x26 && crc[1] == 0xCF);
}

static void
test_crc_append(void)
{
  uint8_t frame[4] = { 0x12, 0x34, 0x00, 0x00 };
  iso14443a_crc_append(frame, 2);
  CHECK(frame[0] == 0x12 && frame[1] == 0x34);
  CHECK(frame[2] == 0x26 && frame[3] == 0xCF);

  uint8_t frame_b[11] = { '1', '2', '3', '4', '5', '6', '7', '8', '9', 0x00, 0x00 };
  iso14443b_crc_append(frame_b, 9);
  CHECK(frame_b[9] == 0x6E && frame_b[10] == 0x90);
}

static void
test_strings(void)
{
  CHECK(strcmp(str_nfc_modulation_type(NMT_ISO14443A), "ISO/IEC 14443A") == 0);
  CHECK(strcmp(str_nfc_modulation_type(NMT_FELICA), "FeliCa") == 0);
  CHECK(strcmp(str_nfc_baud_rate(NBR_106), "106 kbps") == 0);
  CHECK(strcmp(str_nfc_baud_rate(NBR_424), "424 kbps") == 0);
  CHECK(strcmp(str_nfc_baud_rate(NBR_UNDEFINED), "undefined baud rate") == 0);
}

static void
test_version(void)
{
  const char *version = nfc_version();
  CHECK(version != NULL);
  CHECK(version != NULL && isdigit((unsigned char)version[0]));
}

/* The CI environment sets LIBNFC_AUTO_SCAN=false and no default device, so no
 * driver is probed and nothing can be found. */
static void
test_context_without_devices(void)
{
  nfc_context *context = NULL;
  nfc_init(&context);
  CHECK(context != NULL);
  if (!context) {
    return;
  }

  nfc_connstring connstrings[4];
  CHECK(nfc_list_devices(context, connstrings, 4) == 0);

  /* A connection string naming a driver that does not exist must not open. */
  CHECK(nfc_open(context, "no_such_driver:nothing") == NULL);

  nfc_exit(context);
}

int
main(void)
{
  test_crc();
  test_crc_append();
  test_strings();
  test_version();
  test_context_without_devices();

  if (failures) {
    fprintf(stderr, "%d check(s) failed\n", failures);
    return 1;
  }
  printf("all checks passed\n");
  return 0;
}
