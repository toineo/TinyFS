/*
 * Config.h
 *
 */

#ifndef CONFIG_H_
#define CONFIG_H_

// Size of the disk blocks (for now, exactly disk sectors)
#define DBSize 512

// Enforce project constraints
#define WITH_DIR 0
#define USE_STD 0

// TODO: implement a frame size, not necessarily equal to DBSize

// TODO: link this
#define MemPageSize 4096

#endif /* CONFIG_H_ */
