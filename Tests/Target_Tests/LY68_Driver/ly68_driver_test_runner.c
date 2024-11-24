#include "unity_fixture.h"

TEST_GROUP_RUNNER(ly68_driver_test)
{
  RUN_TEST_CASE(ly68_driver_test, read_id_is_ok);
  RUN_TEST_CASE(ly68_driver_test, write_read_is_ok);
  RUN_TEST_CASE(ly68_driver_test, fast_read_is_ok);
  RUN_TEST_CASE(ly68_driver_test, seq_write_is_ok);
  RUN_TEST_CASE(ly68_driver_test, page_write_is_ok);
  RUN_TEST_CASE(ly68_driver_test, multiple_page_write_is_ok);
  RUN_TEST_CASE(ly68_driver_test, intermittent_write_is_ok);
  RUN_TEST_CASE(
    ly68_driver_test,
    multiple_page_intermittent_write_is_ok
  );
  RUN_TEST_CASE(ly68_driver_test, repeated_page_write_is_ok);
}
