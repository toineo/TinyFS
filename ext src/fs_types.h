/*
 * Types.h
 *
 * Definition of the types used in the FS
 */

#ifndef TYPES_H_
#define TYPES_H_

#include "fs_config.h"

#include <preinit/lib/types.h>

typedef char byte; // TODO: switch to int as well

typedef uint64_t diskaddr_t;

typedef unsigned int _size_t;


#define false FALSE
#define true TRUE


#endif /* TYPES_H_ */
