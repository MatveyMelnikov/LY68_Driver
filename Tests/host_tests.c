#include "unity_fixture.h"
#include "unity_config.h"
#include <stdio.h>

void unity_config_put_c(uint8_t c);

static void run_all_tests()
{
  RUN_TEST_GROUP(ly68_driver_test);
}

int main(int argc, char *argv[])
{
	return UnityMain(argc, (const char**)argv, run_all_tests);
}

void unity_config_put_c(uint8_t c)
{
	(void)putchar(c);
}
