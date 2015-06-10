/*
 * Types.h
 *
 * Definition of the types used in the FS
 */

#ifndef TYPES_H_
#define TYPES_H_

#include "Config.h"

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

#include <preinit/lib/types.h>

#endif


// Kind of ugly to put an include here, but this prevents circular definition
// problems (as BinaryData uses byte) - defining a third file would, for now
// at least, be totally overkill
#include "BinaryData.h"

#endif /* TYPES_H_ */
