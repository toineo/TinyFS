/*
 * BinaryData.h
 *
 * Types and functions to manipulate binary data, in particular array of bytes
 * (instead of using C "strings" and their \0 encoding
 *
 */

#ifndef BINARYDATA_H_
#define BINARYDATA_H_

#include "Types.h"
#include "Config.h"

#if USE_STD

#include <stdint.h>
#include <assert.h>


#else

// TODO: use the project one instead of removing it
#define assert(x)

// FIXME: cleaner management of this case
#define uint32_t unsigned int
#define int32_t int
#define uint8_t unsigned char

#endif

typedef struct ByteArray
{
  byte* array;
  tmp_size_t size;
} ByteArray;

const ByteArray NullByteArray;

// ByteArray manipulation
ByteArray concat_bytearray(ByteArray fst, ByteArray snd);

void bin_copy_in_place(ByteArray src, byte* tgt);

// For null terminated strings
ByteArray str_to_bytearray(char* str);

ByteArray int32_to_bin(int32_t n);
ByteArray uint32_to_bin(uint32_t n);

void int32_to_bin_inplace(int32_t n, byte* tgt);
void uint32_to_bin_inplace(uint32_t n, byte* tgt);

int32_t bin_to_int32(ByteArray data);

uint8_t bin_to_uint8_inplace(byte* data);
int32_t bin_to_int32_inplace(byte* data);
uint32_t bin_to_uint32_inplace(byte* data);

/*
 #define to_bin(x) _Generic((x),\
    int32_t : int32_to_bin,\
    uint32_t : uint32_to_bin) (x)

 #define to_bin_inplace(x, tgt) _Generic((x),\
    int32_t : int32_to_bin_inplace,\
    uint32_t : uint32_to_bin_inplace) (x, tgt)
 */

#endif /* BINARYDATA_H_ */
