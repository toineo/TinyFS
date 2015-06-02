/*
 * Disk.h
 *
 * Fake disk stored in memory
 * Meant to model an actual hard drive (without errors, for now)
 *
 */

#ifndef DISK_H_
#define DISK_H_

#include "Types.h"

#define MemDiskSize 500

typedef struct Disk Disk;

Disk* init_disk();

int get_disk_size(const Disk const* d);

void disk_read_block(const Disk const* d, diskaddr_t addr, byte* tgt);
void disk_write_block(Disk const* d, diskaddr_t addr, const byte const* src);

#endif /* DISK_H_ */
