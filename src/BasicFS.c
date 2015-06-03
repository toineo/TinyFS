/*
 * BasicFS.c
 *
 * Extremely basic/naive File System
 * It is only meant to be easy to prove correct and with a tiny memory usage.
 * The counterpart is that it isn't very easy to use, and most importantly
 * it is incredibly inefficient. It also has no tolerance to failure.
 *
 */

#include <stdlib.h>

#include "Config.h"
#include "Types.h"
#include "Disk.h"
#include "File.h"

// As the frames are fixed (and of predetermined size), we store them directly
// in the file system object to simplify allocation (does this approach has
// any default - compared to the one were we allocate the frames by hand?)
typedef struct BasicFS
{
  Disk* d;

  // TODO: bitarray
  bool* free;

  // Frame for doing requested read and writes
  byte rw_frame[DBSize];

  // Frame for performing I/O for the driver's needs
  byte io_frame[DBSize];

  // Frame for storing the current file main node
  byte main_frame[DBSize];

  // File info of the current loaded file (in main_frame)
  File cur_file;

  // ByteArray pointing on rw_frame
  // As it never changes, we can save it once and for all (for the sake
  // of convenience)
  const ByteArray rw_buffer;

  // TODO: root_addr

} BasicFS;

/****** Internal functions declaration ******/
// Internal types
typedef enum
{
  Block, Logical
} size_type;

// Load file which main node is at <ad> in cur_file and its main node in fs->main_frame
void load_file_at_addr(BasicFS* fs, diskaddr_t ad);

// FIXME: refactor in 1 function only (load_file_at_addr/load_block_in_buffer)
void load_block_in_buffer(BasicFS* fs, diskaddr_t ad);

// TODO
File get_file_at_address(BasicFS* fs, diskaddr_t ad);

// TODO
diskaddr_t get_nth_file_addr(BasicFS* fs, File* f, tmp_size_t nblock);

// Reads the <sz_tp> size of the currently loaded main node
tmp_size_t read_file_size(BasicFS* fs, size_type sz_tp);

// Reads the nth data block address contained in the currently loaded main node
diskaddr_t read_nth_data_block_addr(BasicFS* fs, tmp_size_t frame);

/****** Exported functions ******/
BasicFS* create_fs(Disk* d)
{
  assert(sizeof(BasicFS) <= MemPageSize);

  // TODO: integration -> page alloc
  BasicFS* fs = malloc(sizeof(BasicFS));

  fs->d = d;

  load_file_at_addr(fs, 0);

  // TODO: free + rw_buffer
  assert(false);

  return fs;
}

BasicFS* retrieve_fs()
{
  BasicFS* fs;

  // TODO
  assert(false);

  return fs;
}

File get_root(BasicFS* fs)
{
  return get_file_at_address(fs, 0);
}

// Get file <filename> from folder <dir>
File get_file(BasicFS* fs, char* filename, File* dir)
{
  // TODO
  assert(false);
}

// Basically, load the <frame>th frame of <file> in the kernel buffer
// and return a ByteArray on the data
ByteArray read_file_frame(BasicFS* fs, File* file, tmp_size_t frame)
{
  assert(frame >= read_file_size(fs, Block));

  diskaddr_t tgt_block = get_nth_file_addr(fs, file, frame);

  load_block_in_buffer(fs, tgt_block);

  return fs->rw_buffer;
}

// Write the kernel buffer to the <frame>th frame of <file>
void write_file_frame(BasicFS* fs, File* file, tmp_size_t frame)
{
  // TODO
  assert(false);
}

/****** Internal functions ******/
// /!\ This function doesn't reload if the required file is already loaded
void load_file_at_addr(BasicFS* fs, diskaddr_t ad)
{
  // TODO: error handling

  if (!(fs->cur_file.main_node == ad))
    {
      disk_read_block(fs->d, ad, fs->main_frame);

      fs->cur_file.main_node = ad;
    }
}

// FIXME: refactor (same than load_file_at_addr)
void load_block_in_buffer(BasicFS* fs, diskaddr_t ad)
{
  disk_read_block(fs->d, ad, fs->rw_frame);
}

File get_file_at_address(BasicFS* fs, diskaddr_t ad)
{
  load_file_at_addr(fs, ad);

  return fs->cur_file;
}

diskaddr_t get_nth_file_addr(BasicFS* fs, File* f, tmp_size_t nblock)
{
  load_file_at_addr(fs, f->main_node);

  return read_nth_data_block_addr(fs, nblock);
}

tmp_size_t read_file_size(BasicFS* fs, size_type sz_tp)
{
  switch (sz_tp)
  {
    case Logical:
      return bin_to_int32_inplace(fs->main_frame);

    case Block:
      return 0x00FFFFFF & bin_to_int32_inplace(fs->main_frame + 4);

    default:
      assert(false);
      return 0; // Simply to prevent warnings about missing return
  }
}

diskaddr_t read_nth_data_block_addr(BasicFS* fs, tmp_size_t frame)
{
  // FIXME: no check on frame
  return bin_to_int32_inplace(fs->main_frame + 8 + frame * 4);
}
