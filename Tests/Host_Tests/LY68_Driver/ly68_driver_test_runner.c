#include "unity_fixture.h"

TEST_GROUP_RUNNER(ly68_driver_test)
{
  RUN_TEST_CASE(ly68_driver_test, init_is_ok);
  RUN_TEST_CASE(ly68_driver_test, reset_is_ok);
  RUN_TEST_CASE(ly68_driver_test, read_id_is_ok);
  RUN_TEST_CASE(ly68_driver_test, read_is_ok);
  RUN_TEST_CASE(ly68_driver_test, fast_read_is_ok);
  RUN_TEST_CASE(ly68_driver_test, write_is_ok);
  RUN_TEST_CASE(ly68_driver_test, start_intermittent_write_is_ok);
  RUN_TEST_CASE(ly68_driver_test, intermittent_write_is_ok);
  RUN_TEST_CASE(ly68_driver_test, stop_intermittent_write_is_ok);
}
