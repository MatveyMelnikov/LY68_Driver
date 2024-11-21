#ifndef LY68_DRIVER_DEFS_H
#define LY68_DRIVER_DEFS_H

#include <stdint.h>
#include <stdbool.h>

typedef enum
{
  LY68_OK = 0x00U,
  LY68_ERROR = 0x01U,
  LY68_BUSY = 0x02U,
  LY68_TIMEOUT = 0x03U,
  LY68_UNINITIALIZED = 0x04U
} ly68_driver_status;

enum
{
  LY68_CMD_RSTEN = 0x66U,
  LY68_CMD_RST = 0x99U,
  LY68_CMD_READ_ID = 0x9fU,
  LY68_CMD_READ = 0x03U,
  LY68_CMD_FAST_READ = 0x0bU,
  LY68_CMD_WRITE = 0x02U,
  LY68_TIMEOUT_VALUE = 0x64U,
  LY68_LAST_ADDRESS = 0x7FFFFFU,
  LY68_MF_ID = 0xdU
};

typedef struct {
  uint8_t mf_id;
  uint8_t kgd;
  uint8_t eid[7];
} ly68_id;

typedef union {
  uint32_t full;
  uint8_t bytes[3];
} ly68_address;

typedef struct {
  // data, data_size, timeout
  ly68_driver_status (*transmit)(
    const uint8_t *const,
    const uint16_t,
    const uint32_t
  );
  // data, data_size, timeout
  ly68_driver_status (*receive)(
    uint8_t *const,
    const uint16_t,
    const uint32_t
  );
  // is_high
  void (*set_chip_select)(bool);
} ly68_driver_handler;

#endif
