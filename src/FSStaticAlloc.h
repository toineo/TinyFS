/*
 * FSStaticAlloc.h
 *
 * Static FS allocation: the file system is simply represented by
 * a big static structure
 *
 * This avoids the complexity of requesting a new memory page
 *
 */

#ifndef SRC_FSSTATICALLOC_H_
#define SRC_FSSTATICALLOC_H_

#include "BasicFS.h"

static BasicFS fs;

BasicFS* alloc_fs()
{
  return &fs;
}

#endif /* SRC_FSSTATICALLOC_H_ */
