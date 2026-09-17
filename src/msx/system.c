#include <conio.h>
#include "../system.h"
#include "../globals.h"
#include "device_slots.h"

#define RG9SAV 0xFFE8

// default value
uint8_t system_fps = 60;

char response[256];

// Switch the adapter to `mode` and reset into it.
//
// The write to IO_CONTROL cannot run from ROM: it changes what the ROM we are
// running from is. So a stub goes in RAM and we jump to it. And the register
// is not where CONFIG can see it. The adapter's slot is expanded, with UNAPI
// in subslot 0 and CONFIG in subslot 1, and the IO window lives in subslot 0's
// page 2 -- BFFFh from here is CONFIG's own ROM, where a write does nothing.
// So the stub first brings page 2 to subslot 0 of our primary slot.
//
// ENASLT rather than a write to FFFFh: the subslot register is only decoded
// while page 3 selects the adapter, and the stub is *in* page 3, so switching
// page 3 by hand would take the stub out from under itself. ENASLT does that
// from the BIOS in page 0 and restores page 3 before it returns. On a board
// whose slot is not expanded page 2 is already the adapter, and the subslot
// part of the request goes nowhere.
//
//   C000   DB A8        IN   A,(0A8h)
//   C002   E6 0C        AND  00001100b   ; page 1's primary slot -- ours
//   C004   0F 0F        RRCA / RRCA
//   C006   F6 80        OR   80h         ; expanded, subslot 0
//   C008   21 00 80     LD   HL,8000h    ; the page to change
//   C00B   CD 24 00     CALL ENASLT
//   C00E   21 FF BF     LD   HL,0BFFFh
//   C011   36 xx        LD   (HL),mode
//   C013   C3 00 00     JP   0
void system_boot_mode(uint8_t mode)
{
  volatile uint8_t *code = (volatile uint8_t *)0xC000;

  code[0]  = 0xDB; code[1]  = 0xA8;
  code[2]  = 0xE6; code[3]  = 0x0C;
  code[4]  = 0x0F; code[5]  = 0x0F;
  code[6]  = 0xF6; code[7]  = 0x80;
  code[8]  = 0x21; code[9]  = 0x00; code[10] = 0x80;
  code[11] = 0xCD; code[12] = 0x24; code[13] = 0x00;
  code[14] = 0x21; code[15] = 0xFF; code[16] = 0xBF;
  code[17] = 0x36; code[18] = mode;
  code[19] = 0xC3; code[20] = 0x00; code[21] = 0x00;

  __asm
    jp 0xC000
  __endasm;
}

// Boot whatever mount_all left loaded. With a ROM among the device slots it
// shares the adapter's slot with UNAPI, UNAPI in subslot 0; without one,
// UNAPI has the cartridge to itself and the machine comes up in BASIC. Either
// way UNAPI stays resident, so whatever runs next can reach the FujiNet.
void system_boot(void)
{
  uint8_t i;

  for (i = 0; i < MSX_MAX_SLOTS; i++)
  {
    if (msx_device_slot_is_rom((const char *)deviceSlots[i].file))
    {
      system_boot_mode(MSX_MODE_UNAPI_ROM);
      return;
    }
  }

  system_boot_mode(MSX_MODE_UNAPI_BASIC);
}

void system_create_new(uint8_t selected_host_slot, uint8_t selected_device_slot,
                       uint32_t selected_size, const char *path)
{
#ifdef MOCK_DEVICES
    /* do nothing */
    last_rc = FUJINET_RC_NOT_IMPLEMENTED;
#else
    // last_rc = fujinet_create_new(selected_host_slot, selected_device_slot, selected_size, path);
#endif
}

void system_set_fps()
{
  unsigned char *rg9sav_addr = RG9SAV;
  system_fps = *rg9sav_addr & 0b10 ? 50 : 60;
}
