#include "unity_fixture.h"
#include "ly68_driver.h"
#include "mock_spi.h"
#include "mock_cs_pin.h"
#include <string.h>
#include <stdint.h>

// Defines -------------------------------------------------------------------

enum {
  SPI_MOCK_SIZE = 10U,
  CS_PIN_MOCK_SIZE = 20U
};

// Static variables ----------------------------------------------------------

static bool expected_active_cs = true;
static bool expected_inactive_cs = false;

// Static functions ----------------------------------------------------------

static ly68_driver_status ly68_transmit(
  const uint8_t *const data,
  const uint16_t data_size,
  const uint32_t timeout
)
{
  (void)timeout;

  return (ly68_driver_status)mock_spi_transmit(
    (uint8_t *const)data,
    data_size
  );
}

static ly68_driver_status ly68_receive(
  uint8_t *const data,
  const uint16_t data_size,
  const uint32_t timeout
)
{
  (void)timeout;

  return (ly68_driver_status)mock_spi_receive(
    (uint8_t *const)data,
    data_size
  );
}

static void ly68_set_chip_select(bool is_high)
{
  mock_cs_pin_write(&is_high); // ref of argument here is ok
}

// Tests ---------------------------------------------------------------------

TEST_GROUP(ly68_driver_test);

TEST_SETUP(ly68_driver_test)
{
  bool expected_cs = true;

  mock_spi_create(SPI_MOCK_SIZE);
  mock_cs_pin_create(CS_PIN_MOCK_SIZE);

  mock_cs_pin_expect_write(&expected_cs);

  ly68_driver_init(
    (ly68_driver_handler) {
      .transmit = ly68_transmit,
      .receive = ly68_receive,
      .set_chip_select = ly68_set_chip_select
    }
  );
}

TEST_TEAR_DOWN(ly68_driver_test)
{
  mock_spi_verify_complete();
  mock_cs_pin_verify_complete();

  ly68_driver_destroy();
}

TEST(ly68_driver_test, init_is_ok)
{
  // expect in setup
}

TEST(ly68_driver_test, reset_is_ok)
{
  uint8_t expected_data[] = { 0x66U, 0x99U };

  mock_cs_pin_expect_write(&expected_inactive_cs);
  mock_spi_expect_transmit(expected_data, sizeof(uint8_t));
  mock_cs_pin_expect_write(&expected_active_cs);
  mock_cs_pin_expect_write(&expected_inactive_cs);
  mock_spi_expect_transmit(expected_data + 1, sizeof(uint8_t));
  mock_cs_pin_expect_write(&expected_active_cs);

  ly68_driver_status status = ly68_driver_reset();

  TEST_ASSERT_EQUAL(LY68_OK, status);
}

TEST(ly68_driver_test, read_id_is_ok)
{
  ly68_id expected_id = { 
    .mf_id = 0x13,
    .kgd = 0xcc,
    .eid = { 0xaa, 0xbb, 0xcc, 0xaa, 0xbb, 0xcc, 0x00 }
  };
  uint8_t expected_transmit_data[] = { 0x9fU, 0x0, 0x0, 0x0 };
  uint8_t received_data[] = {
    0x13, 0xcc, 0xaa, 0xbb, 0xcc,
    0xaa, 0xbb, 0xcc, 0x00 
  };

  mock_cs_pin_expect_write(&expected_inactive_cs);
  mock_spi_expect_transmit(
    expected_transmit_data,
    sizeof(expected_transmit_data)
  );
  mock_spi_expect_receive(received_data, sizeof(received_data));
  mock_cs_pin_expect_write(&expected_active_cs);

  ly68_id result_id = { 0 };
  ly68_driver_status status = ly68_driver_read_id(&result_id);

  TEST_ASSERT_EQUAL(LY68_OK, status);
  TEST_ASSERT_EQUAL_INT8_ARRAY(
    (uint8_t*)&expected_id,
    (uint8_t*)&result_id,
    sizeof(ly68_id)
  );
}

TEST(ly68_driver_test, read_is_ok)
{
  ly68_address expected_address = { .full = 0x10203U };
  uint8_t expected_transmit_command = 0x03U;
  uint8_t expected_transmit_addr[] = { 0x01, 0x02, 0x03 };
  uint8_t expected_received_data[10] = {
    0x13, 0xcc, 0xaa, 0xbb, 0xcc,
    0xaa, 0xbb, 0xcc, 0x00, 0xaa
  };

  mock_cs_pin_expect_write(&expected_inactive_cs);
  mock_spi_expect_transmit(
    &expected_transmit_command,
    sizeof(uint8_t)
  );
  mock_spi_expect_transmit(
    expected_transmit_addr,
    sizeof(expected_transmit_addr)
  );
  mock_spi_expect_receive(
    expected_received_data,
    sizeof(expected_received_data)
  );
  mock_cs_pin_expect_write(&expected_active_cs);

  uint8_t received_data[10];
  ly68_driver_status status = ly68_driver_read(
    expected_address,
    received_data,
    sizeof(received_data)
  );

  TEST_ASSERT_EQUAL(LY68_OK, status);
  TEST_ASSERT_EQUAL_INT8_ARRAY(
    expected_received_data,
    received_data,
    sizeof(expected_received_data)
  );
}

TEST(ly68_driver_test, fast_read_is_ok)
{
  ly68_address expected_address = { .full = 0x10203U };
  uint8_t expected_transmit_command = 0x0bU;
  uint8_t expected_transmit_addr[] = { 0x01, 0x02, 0x03 };
  uint8_t expected_transmit_dummy = 0x0U;
  uint8_t expected_received_data[10] = {
    0x13, 0xcc, 0xaa, 0xbb, 0xcc,
    0xaa, 0xbb, 0xcc, 0x00, 0xaa
  };

  mock_cs_pin_expect_write(&expected_inactive_cs);
  mock_spi_expect_transmit(
    &expected_transmit_command,
    sizeof(uint8_t)
  );
  mock_spi_expect_transmit(
    expected_transmit_addr,
    sizeof(expected_transmit_addr)
  );
  mock_spi_expect_transmit(
    &expected_transmit_dummy,
    sizeof(expected_transmit_dummy)
  );
  mock_spi_expect_receive(
    expected_received_data,
    sizeof(expected_received_data)
  );
  mock_cs_pin_expect_write(&expected_active_cs);

  uint8_t received_data[10];
  ly68_driver_status status = ly68_driver_fast_read(
    expected_address,
    received_data,
    sizeof(received_data)
  );

  TEST_ASSERT_EQUAL(LY68_OK, status);
  TEST_ASSERT_EQUAL_INT8_ARRAY(
    expected_received_data,
    received_data,
    sizeof(expected_received_data)
  );
}

TEST(ly68_driver_test, write_is_ok)
{
  ly68_address expected_address = { .full = 0x10203U };
  uint8_t expected_transmit_command = 0x02U;
  uint8_t expected_transmit_addr[] = { 0x01, 0x02, 0x03 };
  uint8_t expected_transmit_data[10] = {
    0x13, 0xcc, 0xaa, 0xbb, 0xcc,
    0xaa, 0xbb, 0xcc, 0x00, 0xaa
  };

  mock_cs_pin_expect_write(&expected_inactive_cs);
  mock_spi_expect_transmit(
    &expected_transmit_command,
    sizeof(uint8_t)
  );
  mock_spi_expect_transmit(
    expected_transmit_addr,
    sizeof(expected_transmit_addr)
  );
  mock_spi_expect_transmit(
    expected_transmit_data,
    sizeof(expected_transmit_data)
  );
  mock_cs_pin_expect_write(&expected_active_cs);

  ly68_driver_status status = ly68_driver_write(
    expected_address,
    expected_transmit_data,
    sizeof(expected_transmit_data)
  );

  TEST_ASSERT_EQUAL(LY68_OK, status);
}

TEST(ly68_driver_test, start_intermittent_write_is_ok)
{
  ly68_address expected_address = { .full = 0x10203U };
  uint8_t expected_transmit_command = 0x02U;
  uint8_t expected_transmit_addr[] = { 0x01, 0x02, 0x03 };

  mock_cs_pin_expect_write(&expected_inactive_cs);
  mock_spi_expect_transmit(
    &expected_transmit_command,
    sizeof(uint8_t)
  );
  mock_spi_expect_transmit(
    expected_transmit_addr,
    sizeof(expected_transmit_addr)
  );

  ly68_driver_status status = ly68_driver_start_intermittent_write(
    expected_address
  );

  TEST_ASSERT_EQUAL(LY68_OK, status);
}

TEST(ly68_driver_test, intermittent_write_is_ok)
{
  uint8_t expected_transmit_data[10] = {
    0x13, 0xcc, 0xaa, 0xbb, 0xcc,
    0xaa, 0xbb, 0xcc, 0x00, 0xaa
  };

  mock_spi_expect_transmit(
    expected_transmit_data,
    sizeof(expected_transmit_data)
  );

  ly68_driver_status status = ly68_driver_intermittent_write(
    expected_transmit_data,
    sizeof(expected_transmit_data)
  );

  TEST_ASSERT_EQUAL(LY68_OK, status);
}

TEST(ly68_driver_test, stop_intermittent_write_is_ok)
{
  mock_cs_pin_expect_write(&expected_active_cs);

  ly68_driver_stop_intermittent_write();
}
