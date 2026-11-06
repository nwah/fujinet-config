#include <conio.h>
#include "../system.h"
#include "../globals.h"
#include "device_slots.h"

#define RG9SAV 0xFFE8

// default value
uint8_t system_fps = 60;

char response[256];

void system_boot(void)
{
  uint8_t i;

  for (i = 0; i < MSX_MAX_SLOTS; i++)
    if (msx_device_slot_is_rom((const char *)deviceSlots[i].file))
    {
      fuji_enable_user_rom();
      return;
    }

  fuji_set_cart_mode(FUJI_CART_MODE_UNAPI_BASIC);
}

bool system_create_new(uint8_t selected_host_slot, uint8_t selected_device_slot,
                       uint32_t selected_size, const char *path)
{
#ifdef MOCK_DEVICES
    /* do nothing */
    last_rc = FUJINET_RC_NOT_IMPLEMENTED;
#endif
    return false;
}

void system_set_fps()
{
  unsigned char *rg9sav_addr = RG9SAV;
  system_fps = *rg9sav_addr & 0b10 ? 50 : 60;
}
