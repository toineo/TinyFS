/*
 * Disk.c
 *
 * Disk interface (relying on trusted disk driver)
 *
 */

#include "disk_data.h"

#include "Config.h"

#include <preinit/lib/string.h>
#include <preinit/lib/debug.h>
#include <preinit/dev/disk.h>
#include <sys/preinit/lib/gcc.h>

#define drv_nr 0

static bool is_disk_init = FALSE;


typedef struct Disk
{
  diskaddr_t first_addr;
  _size_t size;
  struct disk_dev * drv;

} Disk;

static Disk static_disk;

// Internal struct simply to read the mbr
struct mbr
{
    uint8_t bootloader[436];
    uint8_t disk_sig[10];
    struct
    {
        uint8_t bootable;
#define INACTIVE_PARTITION      0x00
#define BOOTABLE_PARTITION      0x80
        uint8_t first_chs[3];
        uint8_t id;
#define EMPTY_PARTITION         0x00
#define LINUX_PARTITION         0x83
#define SWAP_PARTITION          0x82
#define EXTENDED_PARTITION      0x5
        uint8_t last_chs[3];
        uint32_t first_lba;
        uint32_t sectors_count;
    }gcc_packed partition[4];
    uint8_t signature[2];
} gcc_packed;

static struct mbr mbr;

// Do we return a disk pointer or do we want a get_disk() function?
Disk* init_disk()
{
  // init_vbr();
  // initialize once.
  if (is_disk_init == TRUE)
  {
      return &static_disk;
  }
  is_disk_init = TRUE;

  // find disk driver.
  //disk_part_debug ("drv_nr: %x\n", drv_nr);
  if ((static_disk.drv = disk_get_dev (drv_nr)) == NULL)
      KERN_PANIC("Failed to get disk driver!\n");


  // read physical mbr from disk.
  disk_xfer (static_disk.drv, 0, (uintptr_t) &mbr, 1, FALSE);

  static_disk.first_addr = mbr.partition[2].first_lba;
  static_disk.size = mbr.partition[2].sectors_count;

  return &static_disk;
}

int get_disk_size(const Disk const* d)
{
  return d->size;
}

diskaddr_t get_first_addr(const Disk const* d)
{
  return d->first_addr;
}

void disk_read_block(const Disk const* d, diskaddr_t addr, byte* tgt)
{
  disk_xfer (d->drv, addr, (uintptr_t) tgt, 1, FALSE);
}

void disk_write_block(Disk const* d, diskaddr_t addr, const byte const * src)
{
  disk_xfer (d->drv, addr, (uintptr_t) src, 1, TRUE);
}
