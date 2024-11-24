#ifndef LY68_DRIVER_H
#define LY68_DRIVER_H

#include "ly68_driver_defs.h"

void ly68_driver_init(ly68_driver_handler handler);
void ly68_driver_destroy(void);
ly68_driver_status ly68_driver_reset(void);
ly68_driver_status ly68_driver_read_id(ly68_id *const id);
ly68_driver_status ly68_driver_read(
  const ly68_address address,
  const uint8_t *const data,
  const uint16_t data_size
);
ly68_driver_status ly68_driver_fast_read(
  const ly68_address address,
  uint8_t *const data,
  const uint16_t data_size
);
ly68_driver_status ly68_driver_write(
  const ly68_address address,
  const uint8_t *const data,
  const uint16_t data_size
);
ly68_driver_status ly68_driver_start_intermittent_write(
  const ly68_address address
);
ly68_driver_status ly68_driver_intermittent_write(
  const uint8_t *const data,
  const uint16_t data_size
);
void ly68_driver_stop_intermittent_write(void);

#endif
