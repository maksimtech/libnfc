# Tests that need no NFC reader, run with `ctest`.
ADD_EXECUTABLE(test_no_hardware test_no_hardware.c)
TARGET_LINK_LIBRARIES(test_no_hardware nfc)

IF(WIN32)
  # Windows finds the DLL only next to the executable
  ADD_CUSTOM_COMMAND(TARGET test_no_hardware POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:nfc> $<TARGET_FILE_DIR:test_no_hardware>)
ENDIF(WIN32)

ADD_TEST(NAME no_hardware COMMAND test_no_hardware)
# No driver must be probed and no device configured: the result must not depend
# on the readers plugged into the machine running the tests.
SET_TESTS_PROPERTIES(no_hardware PROPERTIES
  ENVIRONMENT "LIBNFC_AUTO_SCAN=false;LIBNFC_INTRUSIVE_SCAN=false;LIBNFC_LOG_LEVEL=0")
