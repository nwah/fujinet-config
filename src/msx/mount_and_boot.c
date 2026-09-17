#include "../mount_and_boot.h"
#include "../system.h"
// #include "../screen.h"
#include "../globals.h"
#include "../constants.h"

void boot_to_basic(void)
{
  // UNAPI alone rather than nothing at all, so BASIC and whatever is run
  // from it can still reach the FujiNet.
  system_boot_mode(MSX_MODE_UNAPI_BASIC);
}

void mount_and_boot_lobby(void)
{
  screen_mount_and_boot();
  fuji_set_boot_mode(2);
  // The lobby arrives as a ROM image, whatever the device slots hold.
  system_boot_mode(MSX_MODE_UNAPI_ROM);
}

void mount_and_boot(void)
{
  screen_mount_and_boot();

  if (!fuji_get_device_slots(deviceSlots, NUM_DEVICE_SLOTS))
  {
    screen_error("ERROR READING DEVICE SLOTS");
    die();
  }

  if (!fuji_get_host_slots(hostSlots, NUM_HOST_SLOTS))
  {
    screen_error("ERROR READING HOST SLOTS");
    die();
  }

  if (!fuji_mount_all())
  {
    screen_error("ERROR MOUNTING ALL");
    // wait_a_moment();
    state = HOSTS_AND_DEVICES;
  }
  else
  {
    fuji_set_boot_config(0);
    system_boot();
  }
}
