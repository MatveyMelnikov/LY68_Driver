#include "unity_fixture.h"
#include "ly68_driver.h"
#include "main.h"
#include <string.h>
#include <stdint.h>

// Global variables ----------------------------------------------------------

extern SPI_HandleTypeDef hspi1;

// Defines -------------------------------------------------------------------

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
