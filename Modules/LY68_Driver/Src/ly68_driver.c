#include "ly68_driver.h"
#include <stddef.h>

// Static variables ----------------------------------------------------------

static ly68_driver_handler module_handler;

// Static prototypes ---------------------------------------------------------

static ly68_address ly68_driver_get_reversed_address(ly68_address address);
static ly68_driver_status ly68_driver_transmit(
  const uint8_t *const data,
  const uint16_t data_size
);
static ly68_driver_status ly68_driver_receive(
  uint8_t *const data,
  const uint16_t data_size
);
static ly68_driver_status ly68_driver_set_chip_select(bool is_high);

// Implementations -----------------------------------------------------------

void ly68_driver_init(ly68_driver_handler handler)
{
  module_handler = handler;
  ly68_driver_set_chip_select(true);
}

void ly68_driver_destroy()
{
  module_handler = (ly68_driver_handler){ 0 };
}

ly68_driver_status ly68_driver_reset()
{
  uint8_t rsten_command = LY68_CMD_RSTEN;
  uint8_t rst_command = LY68_CMD_RST;
  ly68_driver_status status = LY68_OK;

  ly68_driver_set_chip_select(false);
  status |= ly68_driver_transmit(&rsten_command, sizeof(uint8_t));
  ly68_driver_set_chip_select(true);

  ly68_driver_set_chip_select(false);
  status |= ly68_driver_transmit(&rst_command, sizeof(uint8_t));
  ly68_driver_set_chip_select(true);

  return status;
}

ly68_driver_status ly68_driver_read_id(ly68_id *const id)
{
  typedef struct {
    uint8_t command;
    uint8_t addr[3]; // dont care
  } start_of_command;

  start_of_command start = {
    .command = LY68_CMD_READ_ID
  };
  *id = (ly68_id) { 0 };
  ly68_driver_status status = LY68_OK;

  ly68_driver_set_chip_select(false);
  status |= ly68_driver_transmit((uint8_t*)&start, sizeof(start_of_command));
  status |= ly68_driver_receive((uint8_t*)id, sizeof(ly68_id));
  ly68_driver_set_chip_select(true);

  return status;
}

ly68_driver_status ly68_driver_read(
  const ly68_address address,
  const uint8_t *const data,
  const uint16_t data_size
)
{
  uint8_t command = LY68_CMD_READ;
  ly68_address reversed_address = ly68_driver_get_reversed_address(address);
  ly68_driver_status status = LY68_OK;

  ly68_driver_set_chip_select(false);
  status |= ly68_driver_transmit((uint8_t*)&command, sizeof(uint8_t));
  status |= ly68_driver_transmit(
    reversed_address.bytes,
    sizeof(reversed_address.bytes)
  );
  status |= ly68_driver_receive((uint8_t*)data, data_size);
  ly68_driver_set_chip_select(true);

  return status;
}

ly68_driver_status ly68_driver_fast_read(
  const ly68_address address,
  uint8_t *const data,
  const uint16_t data_size
)
{
  uint8_t command = LY68_CMD_FAST_READ;
  ly68_address reversed_address = ly68_driver_get_reversed_address(address);
  uint8_t dummy_data = 0x0U;
  ly68_driver_status status = LY68_OK;

  ly68_driver_set_chip_select(false);
  status |= ly68_driver_transmit((uint8_t*)&command, sizeof(uint8_t));
  status |= ly68_driver_transmit(
    reversed_address.bytes,
    sizeof(reversed_address.bytes)
  );
  status |= ly68_driver_transmit((uint8_t*)&dummy_data, sizeof(uint8_t));
  status |= ly68_driver_receive((uint8_t*)data, data_size);
  ly68_driver_set_chip_select(true);

  return status;
}

ly68_driver_status ly68_driver_write(
  const ly68_address address,
  const uint8_t *const data,
  const uint16_t data_size
)
{
  uint8_t command = LY68_CMD_WRITE;
  ly68_address reversed_address = ly68_driver_get_reversed_address(address);
  ly68_driver_status status = LY68_OK;

  ly68_driver_set_chip_select(false);
  status |= ly68_driver_transmit((uint8_t*)&command, sizeof(uint8_t));
  status |= ly68_driver_transmit(
    reversed_address.bytes,
    sizeof(reversed_address.bytes)
  );
  status |= ly68_driver_transmit((uint8_t*)data, data_size);
  ly68_driver_set_chip_select(true);

  return status;
}

ly68_driver_status ly68_driver_start_intermittent_write(
  const ly68_address address
)
{
  uint8_t command = LY68_CMD_WRITE;
  ly68_address reversed_address = ly68_driver_get_reversed_address(address);
  ly68_driver_status status = LY68_OK;

  ly68_driver_set_chip_select(false);

  status |= ly68_driver_transmit((uint8_t*)&command, sizeof(uint8_t));
  status |= ly68_driver_transmit(
    reversed_address.bytes,
    sizeof(reversed_address.bytes)
  );

  return status;
}

ly68_driver_status ly68_driver_intermittent_write(
  const uint8_t *const data,
  const uint16_t data_size
)
{
  ly68_driver_status status = LY68_OK;

  status |= ly68_driver_transmit((uint8_t*)data, data_size);

  return status;
}

void ly68_driver_stop_intermittent_write()
{
  ly68_driver_set_chip_select(true);
}

static ly68_address ly68_driver_get_reversed_address(ly68_address address)
{
  return (ly68_address) {
    .bytes = {
      address.bytes[2],
      address.bytes[1],
      address.bytes[0]
    }
  };
}

static ly68_driver_status ly68_driver_transmit(
  const uint8_t *const data,
  const uint16_t data_size
)
{
  if (module_handler.transmit == NULL)
    return LY68_UNINITIALIZED;

  return module_handler.transmit(data, data_size, LY68_TIMEOUT_VALUE);
}

static ly68_driver_status ly68_driver_receive(
  uint8_t *const data,
  const uint16_t data_size
)
{
  if (module_handler.receive == NULL)
    return LY68_UNINITIALIZED;

  return module_handler.receive(data, data_size, LY68_TIMEOUT_VALUE);
}

static ly68_driver_status ly68_driver_set_chip_select(bool is_high)
{
  if (module_handler.set_chip_select == NULL)
    return LY68_UNINITIALIZED;

  module_handler.set_chip_select(is_high);

  return LY68_OK;
}
