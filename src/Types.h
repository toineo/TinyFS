/*
 * Types.h
 *
 * Definition of the types used in the FS
 */

#ifndef TYPES_H_
#define TYPES_H_

#define USE_STD 0

#if USE_STD
#include <stdint.h>
#include <stdbool.h>
#endif

// Basic types
#if USE_STD

typedef uint8_t byte;

typedef uint32_t diskaddr_t;

typedef uint32_t tmp_size_t;

#else

typedef char byte; // TODO: switch to int as well

typedef unsigned int diskaddr_t;

typedef unsigned int tmp_size_t;

// Bool emulation
// Because of project constraints, we only use ints
typedef unsigned int bool;

#define true 1
#define false 0

#endif


// Kind of ugly to put an include here, but this prevents circular definition
// problems (as BinaryData uses byte) - defining a third file would, for now
// at least, be totally overkill
#include "../src/BinaryData.h"

#endif /* TYPES_H_ */
