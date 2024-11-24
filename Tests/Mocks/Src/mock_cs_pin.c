#include "mock_cs_pin.h"
#include "mock.h"
#include "unity_fixture.h"
#include <string.h>
#include <stdio.h>

// Defines -------------------------------------------------------------------

enum {
  OUTPUT_CHARS_ADDITIVE = 50U
};

// Static variables ----------------------------------------------------------

static const char *name = "CS pin";
static mock cs_pin_mock;

// Static prototypes ---------------------------------------------------------

// Implementations -----------------------------------------------------------

void mock_cs_pin_create(const uint8_t max_expectations)
{
  cs_pin_mock = mock_create(name, max_expectations);
}

void mock_cs_pin_destroy(void)
{
  mock_destroy(&cs_pin_mock);
}

void mock_cs_pin_expect_write(const bool *const is_active)
{
  mock_record_expectation(
    &cs_pin_mock,
    WRITE,
    (uint8_t*)is_active,
    sizeof(bool)
  );
}

void mock_cs_pin_verify_complete()
{
  mock_verify_complete(&cs_pin_mock);
}

void mock_cs_pin_write(bool *const is_active)
{
  expectation current_expectation = mock_get_expectation(&cs_pin_mock);

  mock_check_kind(&cs_pin_mock, &current_expectation, WRITE);
  mock_check_data(
    &cs_pin_mock,
    &current_expectation,
    (uint8_t*)is_active,
    sizeof(bool)
  );
}
