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

typedef struct ByteArray ByteArray;

Disk* init_disk();

int get_disk_size(int drv_nr);
diskaddr_t get_first_addr(int drv_nr);

void disk_read_block(int drv_nr, diskaddr_t addr, byte* tgt);
void disk_write_block(int drv_nr, diskaddr_t addr, const byte const * src);

// Do we really need it? We could just use indexes instead, couldn't we?
ByteArray* get_bytearray(int arr_nr);

void bytearray_set(int arr_nr, int shift, byte value);
byte bytearray_get(int arr_nr, int shift);

#endif /* DISK_H_ */
