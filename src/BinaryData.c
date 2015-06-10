/*
 * BinaryData.c
 *
 * Types and functions to manipulate binary data, in particular array of bytes
 * (instead of using C "strings" and their \0 encoding)
 *
 */

#include "BinaryData.h"


#if USE_STD

#include <string.h>

#else

#include <preinit/lib/string.h>

#endif

// TODO

// Switch to big-endian

// TODO

// Convenience constant
const ByteArray NullByteArray =
{ .size = 0, .array = 0};

// ________ ByteArray manipulation functions ________
void bin_copy_in_place(ByteArray src, byte* tgt)
{
  memcpy(tgt, src.array, src.size);
}

ByteArray str_to_bytearray(char* str)
{
  ByteArray res;

  res.array = (byte*) str;
  res.size = strlen(str);

  return res;
}

// ________ Binary marshaling of usual types ________
void int32_to_bin_inplace(int32_t n, byte* tgt)
{
  int i;
  for (i = 0; i < 4; i++)
    tgt[i] = (n >> (8 * i)) & 0xFF;
}


void uint32_to_bin_inplace(uint32_t n, byte* tgt)
{
  // FIXME: only temporary
  int32_to_bin_inplace((int32_t) n, tgt);
}

int32_t bin_to_int32(ByteArray data)
{
  assert(data.size == 4);

  return bin_to_int32_inplace(data.array);
}

uint8_t bin_to_uint8_inplace(byte* data)
{
  return *data;
}

int32_t bin_to_int32_inplace(byte* data)
{
  int32_t res = 0;
  int i;

  for (i = 0; i < 4; i++)
    res += ((int32_t) data[i]) << 8 * i;

  return res;
}

uint32_t bin_to_uint32_inplace(byte* data)
{
  uint32_t res = 0;
  int i;

  for (i = 0; i < 4; i++)
    res += ((uint32_t) data[i]) << 8 * i;

  return res;
}

