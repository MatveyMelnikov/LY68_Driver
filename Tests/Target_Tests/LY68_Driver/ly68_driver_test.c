#include "unity_fixture.h"
#include "ly68_driver.h"
#include "main.h"
#include <string.h>
#include <stdint.h>

// Global variables ----------------------------------------------------------

extern SPI_HandleTypeDef hspi1;

// Defines -------------------------------------------------------------------

enum {
  MULTIPLE_WRITE_AMOUNT = 100U,
  PAGE_SIZE = 1024U
};

// Static variables ----------------------------------------------------------

// Static functions ----------------------------------------------------------

static ly68_driver_status ly68_transmit(
  const uint8_t *const data,
  const uint16_t data_size,
  const uint32_t timeout
)
{
  return (ly68_driver_status)HAL_SPI_Transmit(
    &hspi1,
    (uint8_t*)data,
    data_size,
    timeout
  );
}

static ly68_driver_status ly68_receive(
  uint8_t *const data,
  const uint16_t data_size,
  const uint32_t timeout
)
{
  return (ly68_driver_status)HAL_SPI_Receive(
    &hspi1,
    (uint8_t*)data,
    data_size,
    timeout
  );
}

static void ly68_set_chip_select(bool is_high)
{
  HAL_GPIO_WritePin(
    LY68_CE_GPIO_Port,
    LY68_CE_Pin,
    is_high ? GPIO_PIN_SET : GPIO_PIN_RESET
  );
}

// Tests ---------------------------------------------------------------------

TEST_GROUP(ly68_driver_test);

TEST_SETUP(ly68_driver_test)
{
  ly68_driver_init(
    (ly68_driver_handler) {
      .transmit = ly68_transmit,
      .receive = ly68_receive,
      .set_chip_select = ly68_set_chip_select
    }
  );
  ly68_driver_reset();
}

TEST_TEAR_DOWN(ly68_driver_test)
{
  ly68_driver_destroy();
}

TEST(ly68_driver_test, read_id_is_ok)
{
  ly68_driver_status status = LY68_OK;
  ly68_id id = { 0 };

  status = ly68_driver_read_id(&id);

  TEST_ASSERT_EQUAL(LY68_OK, status);
  TEST_ASSERT_EQUAL(LY68_MF_ID, id.mf_id);
}

TEST(ly68_driver_test, write_read_is_ok)
{
  ly68_driver_status status = LY68_OK;
  ly68_address address = { .full = 10203U };
  uint8_t t_data[] = { 0xaa, 0xbb, 0xcc, 0xdd };
  uint8_t r_data[sizeof(t_data)] = { 0 };
  
  status = ly68_driver_write(address, t_data, sizeof(t_data));
  status |= ly68_driver_read(address, r_data, sizeof(r_data));

  TEST_ASSERT_EQUAL(LY68_OK, status);
  TEST_ASSERT_EQUAL_INT8_ARRAY(t_data, r_data, sizeof(t_data));
}

TEST(ly68_driver_test, fast_read_is_ok)
{
  ly68_driver_status status = LY68_OK;
  ly68_address address = { .full = 10203U };
  uint8_t t_data[] = { 0xaa, 0xbb, 0xcc, 0xdd };
  uint8_t r_data[sizeof(t_data)] = { 0 };
  
  status = ly68_driver_write(address, t_data, sizeof(t_data));
  status |= ly68_driver_fast_read(address, r_data, sizeof(r_data));

  TEST_ASSERT_EQUAL(LY68_OK, status);
  TEST_ASSERT_EQUAL_INT8_ARRAY(t_data, r_data, sizeof(t_data));
}

TEST(ly68_driver_test, seq_write_is_ok)
{
  ly68_driver_status status = LY68_OK;
  uint8_t t_data[] = { 0xaa, 0xbb, 0xcc };
  uint8_t r_data[sizeof(t_data)] = { 0 };

  status = ly68_driver_write(
    (ly68_address) { .full = 0U },
    &t_data[0],
    sizeof(uint8_t)
  );
  status |= ly68_driver_write(
    (ly68_address) { .full = 1U },
    &t_data[1],
    sizeof(uint8_t)
  );
  status |= ly68_driver_write(
    (ly68_address) { .full = 2U },
    &t_data[2],
    sizeof(uint8_t)
  );
  status |= ly68_driver_read(
    (ly68_address) { .full = 0U },
    r_data,
    sizeof(r_data)
  );

  TEST_ASSERT_EQUAL(LY68_OK, status);
  TEST_ASSERT_EQUAL_INT8_ARRAY(t_data, r_data, sizeof(t_data));
}

TEST(ly68_driver_test, page_write_is_ok)
{
  ly68_driver_status status = LY68_OK;
  ly68_address address = { .full = 0U };
  uint8_t t_data[PAGE_SIZE] = { 0 };
  uint8_t r_data[PAGE_SIZE] = { 0 };

  *((uint16_t*)&t_data[0]) = 0x103;
  *((uint16_t*)&t_data[256]) = 0x301;
  *((uint16_t*)&t_data[512]) = 0x103;
  *((uint16_t*)&t_data[1022]) = 0x301;
  
  status = ly68_driver_write(address, t_data, sizeof(t_data));
  status |= ly68_driver_read(address, r_data, sizeof(r_data));

  TEST_ASSERT_EQUAL(LY68_OK, status);
  TEST_ASSERT_EQUAL_INT8_ARRAY(t_data, r_data, sizeof(t_data));
}

TEST(ly68_driver_test, multiple_page_write_is_ok)
{
  ly68_driver_status status = LY68_OK;
  uint8_t t_data[PAGE_SIZE] = { 0 };
  uint8_t r_data[PAGE_SIZE] = { 0 };

  *((uint16_t*)&t_data[0]) = 0x103;
  *((uint16_t*)&t_data[256]) = 0x301;
  *((uint16_t*)&t_data[512]) = 0x103;
  *((uint16_t*)&t_data[1022]) = 0x301;

  for (
    ly68_address addr = { 0 };
    addr.full < PAGE_SIZE * MULTIPLE_WRITE_AMOUNT;
    addr.full += PAGE_SIZE
  )
  {
    status = ly68_driver_write(addr, t_data, sizeof(t_data));
  }

  for (
    ly68_address addr = { 0 };
    addr.full < PAGE_SIZE * MULTIPLE_WRITE_AMOUNT;
    addr.full += PAGE_SIZE
  )
  {
    status |= ly68_driver_read(addr, r_data, sizeof(r_data));

    TEST_ASSERT_EQUAL(LY68_OK, status);
    TEST_ASSERT_EQUAL_INT8_ARRAY(t_data, r_data, sizeof(t_data));
  }
}

TEST(ly68_driver_test, intermittent_write_is_ok)
{
  ly68_driver_status status = LY68_OK;
  ly68_address address = { .full = 10203U };
  uint8_t t_data[] = { 0xaa, 0xbb, 0xcc, 0xdd };
  uint8_t r_data[sizeof(t_data)] = { 0 };

  status = ly68_driver_start_intermittent_write(address);
  status |= ly68_driver_intermittent_write(t_data, sizeof(t_data));
  ly68_driver_stop_intermittent_write();

  status |= ly68_driver_read(address, r_data, sizeof(r_data));

  TEST_ASSERT_EQUAL(LY68_OK, status);
  TEST_ASSERT_EQUAL_INT8_ARRAY(t_data, r_data, sizeof(t_data));
}

TEST(ly68_driver_test, multiple_page_intermittent_write_is_ok)
{
  ly68_driver_status status = LY68_OK;
   ly68_address start_address = { .full = 0x19000U };
  uint8_t t_data[PAGE_SIZE] = { 0 };
  uint8_t r_data[PAGE_SIZE] = { 0 };

  *((uint16_t*)&t_data[0]) = 0x103;
  *((uint16_t*)&t_data[256]) = 0x301;
  *((uint16_t*)&t_data[512]) = 0x103;
  *((uint16_t*)&t_data[1022]) = 0x301;

  status = ly68_driver_start_intermittent_write(start_address);
  for (uint8_t i = 0; i < MULTIPLE_WRITE_AMOUNT; i++)
  {
    status |= ly68_driver_intermittent_write(t_data, sizeof(t_data));
  }
  ly68_driver_stop_intermittent_write();

  for (
    ly68_address addr = { .full = 0x19000U };
    addr.full < PAGE_SIZE * MULTIPLE_WRITE_AMOUNT;
    addr.full += PAGE_SIZE
  )
  {
    status |= ly68_driver_read(addr, r_data, sizeof(r_data));

    TEST_ASSERT_EQUAL(LY68_OK, status);
    TEST_ASSERT_EQUAL_INT8_ARRAY(t_data, r_data, sizeof(t_data));
  }
}

TEST(ly68_driver_test, repeated_page_write_is_ok)
{
  ly68_driver_status status = LY68_OK;
  ly68_address address = { .full = 0U };
  uint8_t t_data[PAGE_SIZE] = { 0 };
  uint8_t r_data[PAGE_SIZE] = { 0 };
  
  status = ly68_driver_write(address, t_data, sizeof(t_data));
  *((uint16_t*)&t_data[0]) = 0x123;
  status |= ly68_driver_write(address, t_data, sizeof(t_data));
  *((uint16_t*)&t_data[256]) = 0x321;
  status |= ly68_driver_write(address, t_data, sizeof(t_data));
  *((uint16_t*)&t_data[512]) = 0x123;
  status |= ly68_driver_write(address, t_data, sizeof(t_data));
  *((uint16_t*)&t_data[1022]) = 0x321;
  status |= ly68_driver_write(address, t_data, sizeof(t_data));
  status |= ly68_driver_read(address, r_data, sizeof(r_data));

  TEST_ASSERT_EQUAL(LY68_OK, status);
  TEST_ASSERT_EQUAL_INT8_ARRAY(t_data, r_data, sizeof(t_data));
}
