/*
 * Disk.h
 *
 * Disk interface (relying on trusted disk driver)
 *
 */

#ifndef DISK_H_
#define DISK_H_

#include "Types.h"

#define MemDiskSize 500

typedef struct Disk Disk;

Disk* init_disk();

int get_disk_size(const Disk const* d);
diskaddr_t get_first_addr(const Disk const* d);

void disk_read_block(const Disk const* d, diskaddr_t addr, byte* tgt);
void disk_write_block(Disk const* d, diskaddr_t addr, const byte const* src);

#endif /* DISK_H_ */
