/*
 * Disk.c
 *
 * Fake disk stored in memory
 * Meant to model an actual hard drive (without errors, for now)
 *
 */

#include "Disk.h"

#include "Config.h"

#if USE_STD

#include <string.h>

#else

#include <preinit/lib/string.h>

#include "fs_partition.h"

#endif

typedef struct Disk
{
  byte content[DiskSectorSize * MemDiskSize];
} Disk;

#if USE_STD
static Disk fake_disk;
#endif

Disk* init_disk()
{
  return &fake_disk;
}

int get_disk_size(const Disk const* d)
{
  return MemDiskSize;
}

diskaddr_t get_first_addr(const Disk const* d)
{
  return 0;
}

void disk_read_block(const Disk const* d, diskaddr_t addr, byte* tgt)
{
  memcpy(tgt, d->content + addr * DiskSectorSize, DiskSectorSize);
}

void disk_write_block(Disk const* d, diskaddr_t addr, const byte const * src)
{
  memcpy((void*)d->content + addr * DiskSectorSize, src, DiskSectorSize);
}
