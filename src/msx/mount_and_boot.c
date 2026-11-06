#include "../mount_and_boot.h"
#include "../system.h"
#include "../globals.h"
#include "../constants.h"

void boot_to_basic(void)
{
  fuji_set_cart_mode(FUJI_CART_MODE_UNAPI_BASIC);
}

void mount_and_boot_lobby(void)
{
  screen_mount_and_boot();
  fuji_set_boot_mode(FUJI_BOOT_MODE_LOBBY);
  fuji_enable_user_rom();
}

void mount_and_boot(void)
{
  screen_mount_and_boot();

  if (!fuji_get_device_slots(deviceSlots, FUJI_MAX_DISKS))
  {
    screen_error("ERROR READING DEVICE SLOTS");
    die();
  }

  if (!fuji_get_host_slots(hostSlots, FUJI_MAX_HOSTS))
  {
    screen_error("ERROR READING HOST SLOTS");
    die();
  }

  if (!fuji_mount_all())
  {
    screen_error("ERROR MOUNTING ALL");
    state = HOSTS_AND_DEVICES;
  }
  else
  {
    fuji_set_boot_config(0);
    system_boot();
  }
}
