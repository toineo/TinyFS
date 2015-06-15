/*
 * Config.h
 *
 */

#ifndef CONFIG_H_
#define CONFIG_H_


#define n_disks 1

#define n_fs 1

// Number of byte arrays used by one filesystem; this is directly determined
// by how the filesystem is implemented
#define n_ba_per_fs 4

#define n_bytearray (n_ba_per_fs * n_fs)

// Size of the disk blocks (for now, exactly disk sectors)
#define DiskSectorSize 512

// Enforce project constraints
#define WITH_DIR 0
#define USE_STD 0

// TODO: implement a frame size, not necessarily equal to DBSize

// TODO: link this
#define MemPageSize 4096

#endif /* CONFIG_H_ */
