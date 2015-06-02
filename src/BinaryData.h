/*
 * BinaryData.h
 *
 * Types and functions to manipulate binary data, in particular array of bytes
 * (instead of using C "strings" and their \0 encoding
 *
 */

#ifndef BINARYDATA_H_
#define BINARYDATA_H_

#include <stdint.h>
#include <assert.h>
#include "../src/Types.h"

typedef struct ByteArray
{
  byte* array;
  uint32_t size;
} ByteArray;

// ByteArray manipulation
ByteArray concat_bytearray(ByteArray fst, ByteArray snd);

ByteArray int32_to_bin(int32_t n);
ByteArray uint32_to_bin(uint32_t n);

void int32_to_bin_inplace(int32_t n, byte* tgt);
void uint32_to_bin_inplace(uint32_t n, byte* tgt);

int32_t bin_to_int32(ByteArray data);

int32_t bin_to_int32_inplace(byte* data);

/*
 #define to_bin(x) _Generic((x),\
    int32_t : int32_to_bin,\
    uint32_t : uint32_to_bin) (x)

 #define to_bin_inplace(x, tgt) _Generic((x),\
    int32_t : int32_to_bin_inplace,\
    uint32_t : uint32_to_bin_inplace) (x, tgt)
 */

#endif /* BINARYDATA_H_ */
