/*
 * Disk.c
 *
 * Fake disk stored in memory
 * Meant to model an actual hard drive (without errors, for now)
 *
 */

#include "Disk.h"

#include "Config.h"

#include "stdlib.h"
#include "string.h"

typedef struct Disk
{
  byte * content;
} Disk;

Disk* init_disk()
{
  Disk* d = malloc(sizeof(Disk));

  d->content = malloc(sizeof(byte) * DBSize * MemDiskSize);

  return d;
}

int get_disk_size(const Disk const* d)
{
  return MemDiskSize;
}

void disk_read_block(const Disk const* d, diskaddr_t addr, byte* tgt)
{
  memcpy(tgt, d->content + addr * DBSize, DBSize);
}

void disk_write_block(Disk const* d, diskaddr_t addr, const byte const * src)
{
  memcpy(d->content + addr * DBSize, src, DBSize);
}
