/*
 * BinaryData.c
 *
 * Types and functions to manipulate binary data, in particular array of bytes
 * (instead of using C "strings" and their \0 encoding)
 *
 */

#include "BinaryData.h"

#include <stdlib.h>
#include <assert.h>
#include <string.h>

// ________ ByteArray manipulation functions ________
ByteArray concat_bytearray(ByteArray fst, ByteArray snd)
{
  ByteArray res;

  res.size = fst.size + snd.size;

  res.array = malloc(res.size);
  memcpy(res.array, fst.array, fst.size);
  memcpy(res.array + fst.size, snd.array, snd.size);

  return res;
}

// ________ Binary marshaling of usual types ________

// FIXME: do this using a macro
ByteArray int32_to_bin(int32_t n)
{
  ByteArray res;

  res.size = 4;
  res.array = malloc(4);

  int32_to_bin_inplace(n, res.array);

  return res;
}

ByteArray uint32_to_bin(uint32_t n)
{
  return int32_to_bin((int32_t) n);
}

void int32_to_bin_inplace(int32_t n, byte* tgt)
{
  for (int i = 0; i < 4; i++)
    tgt[i] = (n >> (8 * i)) & 0xFF;
}

int32_t bin_to_int32(ByteArray data)
{
  assert(data.size == 4);

  return bin_to_int32_inplace(data.array);
}

int32_t bin_to_int32_inplace(byte* data)
{
  int32_t res = 0;

  for (int i = 0; i < 4; i++)
    res += ((int32_t) data[i]) << 8 * i;

  return res;
}

void uint32_to_bin_inplace(uint32_t n, byte* tgt)
{
  // FIXME: only temporary
  int32_to_bin_inplace((int32_t) n, tgt);
}
