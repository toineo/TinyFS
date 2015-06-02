/*
 * Types.h
 *
 * Definition of the types used in the FS
 */

#ifndef TYPES_H_
#define TYPES_H_

#include <stdint.h>
#include <stdbool.h>

// Basic types
typedef char* string;
typedef uint8_t byte;

typedef uint32_t diskaddr_t;

typedef uint32_t tmp_size_t;

// Kind of ugly to put an include here, but this prevents circular definition
// problems (as BinaryData uses byte) - defining a third file would, for now
// at least, be totally overkill
#include "../src/BinaryData.h"

#endif /* TYPES_H_ */
