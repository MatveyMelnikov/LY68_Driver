#ifndef MOCK_CS_PIN_H
#define MOCK_CS_PIN_H

#include <stdint.h>
#include <stdbool.h>

void mock_cs_pin_create(const uint8_t max_expectations);
void mock_cs_pin_destroy(void);
void mock_cs_pin_expect_write(const bool *const is_active);
void mock_cs_pin_verify_complete(void);

void mock_cs_pin_write(bool *const is_active);

#endif
