/*
 * Disk.c
 *
 * Disk interface (relying on trusted disk driver)
 *
 */

#include "disk_data.h"

#include "fs_config.h"

#include <preinit/lib/string.h>
#include <preinit/lib/debug.h>
#include <preinit/dev/disk.h>
#include <sys/preinit/lib/gcc.h>

//#define drv_nr 0




typedef struct Disk
{
  diskaddr_t first_addr;
  _size_t size;
  struct disk_dev * drv;

} Disk;

// Now all ByteArrays have a fixed size, equal to a disk sector size
typedef struct ByteArray
{
  byte* array;
  //_size_t size;
} ByteArray;


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
    } gcc_packed partition[4];
    uint8_t signature[2];
} gcc_packed;




// Static objects
static ByteArray bytearrays[n_bytearray];
static byte bytearrays_data[n_bytearray][DiskSectorSize];

static Disk disks[n_disks];
static struct mbr mbr[n_disks];
static bool is_disk_init[n_disks]; // FIXME init
static bool is_bytearray_init[n_bytearray]; // FIXME init

// Do we return a disk pointer or do we want a get_disk() function?
// FIXME: we always use the second partition of a drive...
Disk* init_disk(int drv_nr)
{
  if (drv_nr >= n_disks)
    KERN_PANIC("Requesting an inexistent drive!\n");

  // initialize once.
  if (is_disk_init[drv_nr] == TRUE)
  {
      return &disks[drv_nr];
  }
  is_disk_init[drv_nr] = TRUE;

  // find disk driver.
  //disk_part_debug ("drv_nr: %x\n", drv_nr);
  if ((disks[drv_nr].drv = disk_get_dev (drv_nr)) == NULL)
      KERN_PANIC("Failed to get disk driver!\n");


  // read physical mbr from disk.
  disk_xfer (disks[drv_nr].drv, 0, (uintptr_t) &mbr[drv_nr], 1, FALSE);

  disks[drv_nr].first_addr = mbr[drv_nr].partition[2].first_lba;
  disks[drv_nr].size = mbr[drv_nr].partition[2].sectors_count;

  return &disks[drv_nr];
}

void init_bytearray(int ba_nr)
{

  if (ba_nr >= n_bytearray)
    KERN_PANIC("Requesting an inexistent byte array!\n");

  // initialize once.
  if (is_bytearray_init[ba_nr] == TRUE)
  {
      return;
  }
  is_bytearray_init[ba_nr] = TRUE;

  bytearrays[ba_nr].array = bytearrays_data[ba_nr];
}

int get_disk_size(int drv_nr)
{
  return disks[drv_nr].size;
}

diskaddr_t get_first_addr(int drv_nr)
{
  return disks[drv_nr].first_addr;
}

void disk_read_block(int drv_nr, diskaddr_t addr, int tgt_nr)
{
  disk_xfer (disks[drv_nr].drv, addr, (uintptr_t) bytearrays_data[tgt_nr], 1, FALSE);
}

void disk_write_block(int drv_nr, diskaddr_t addr, int src_nr)
{
  disk_xfer (disks[drv_nr].drv, addr, (uintptr_t) bytearrays_data[src_nr], 1, TRUE);
}


//ByteArray* get_bytearray(int arr_nr);

// ************ FIXME ************
// Check that the indexes and shifts are always within bounds
void bytearray_set(int arr_nr, int shift, byte value)
{
  bytearrays[arr_nr].array[shift] = value;
}
byte bytearray_get(int arr_nr, int shift)
{
  return bytearrays[arr_nr].array[shift];
}

// Same functions but operating on an uint32
void bytearray_set_uint32(int arr_nr, int shift, uint32_t value)
{
  int i;
  for (i = 0; i < 4; i++)
    bytearrays[arr_nr].array[shift + i] = (value >> (8 * i)) & 0xFF;
}

uint32_t bytearray_get_uint32(int arr_nr, int shift)
{
  uint32_t res = 0;
  int i;

  for (i = 0; i < 4; i++)
    res += ((uint32_t) bytearrays[arr_nr].array[shift + i]) << 8 * i;

  return res;
}
