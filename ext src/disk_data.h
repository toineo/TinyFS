/*
 * Disk.h
 *
 * Disk interface (relying on trusted disk driver)
 *
 */

#ifndef DISK_H_
#define DISK_H_

#include "fs_types.h"
#include "fs_config.h"

typedef struct Disk Disk;

typedef struct ByteArray ByteArray;

void init_disk_data();

int get_disk_size(int drv_nr);
diskaddr_t get_first_addr(int drv_nr);

void disk_read_block(int drv_nr, diskaddr_t addr, int tgt_nr);
void disk_write_block(int drv_nr, diskaddr_t addr, int src_nr);

//void bytearray_set(int arr_nr, int shift, byte value);
//byte bytearray_get(int arr_nr, int shift);
//
//// Same functions but operating on an uint32
//void bytearray_set_uint32(int arr_nr, int shift, uint32_t value);
//uint32_t bytearray_get_uint32(int arr_nr, int shift);

#endif /* DISK_H_ */
