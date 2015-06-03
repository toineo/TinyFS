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
// FIXME: better names (in particular, unify buffer names)
typedef struct BasicFS
{
  Disk* d;

  // Free blocks management
  diskaddr_t first_blank; // First address of the "disk tail" (set of never allocated blocks)
  diskaddr_t free_list; // Chained list of deallocated blocks

  // Frame for doing requested read and writes
  byte rw_frame[DBSize];

  // Frame for performing I/O for the driver's needs
  byte io_frame[DBSize];

  // Frame for storing the main nodes
  byte file_main_node[DBSize]; // Current file main node
  byte dir_main_node[DBSize]; // For storing directory main node (for operations involving both a file and a dir)

  // File info of the current loaded file (in main_frame)
  File cur_file;

  // ByteArray pointing on rw_frame
  // As it never changes, we can save it once and for all (for the sake
  // of convenience)
  const ByteArray rw_buffer;

  // TODO: root_addr

} BasicFS;

// TODO: define macros to get all the data shifts with nice names (instead of hardcoded numbers for now)

/****** Internal functions declaration ******/
// Internal types
typedef enum
{
  Block, Logical
} size_type;

typedef enum
{
  DirBit
} attr_type;

typedef enum
{
  RWBuffer, IOBuffer, FileMNodeBuffer, DirMNodeBuffer
} buffer_type;

typedef enum
{
  TgtFile, TgtFolder
} target_file_type;

// Load file which main node is at <ad> in cur_file and its main node in fs->main_frame
void load_file_at_addr(BasicFS* fs, diskaddr_t ad);

// TODO: descr
void load_block_in_buffer(BasicFS* fs, diskaddr_t ad, buffer_type buf);

// Load <dir> in the directory main node buffer and attempts to find
// the file with name <fname> inside <dir>. Returns 0 if it cannot
// find such file.
diskaddr_t load_dir_find_addr(BasicFS* fs, File* dir, char* fname);

// TODO
File get_file_at_address(BasicFS* fs, diskaddr_t ad);

// TODO
diskaddr_t get_nth_file_addr(BasicFS* fs, File* f, tmp_size_t nblock);

// Reads the <sz_tp> size of the currently loaded main node
tmp_size_t read_file_size(BasicFS* fs, size_type sz_tp, target_file_type ft);

// Reads the nth data block address contained in the currently loaded main node
diskaddr_t read_nth_data_block_addr(BasicFS* fs, tmp_size_t frame);

// Reads attributes of the currently loaded main node
uint8_t read_attribute(BasicFS* fs, attr_type attr);

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

  fs->free_list = 0;

  // TODO: write

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

File create_file(BasicFS* fs, char* filename, File* dir)
{
  // TODO
  assert(false);
}

// Get file <filename> from folder <dir>
File get_file(BasicFS* fs, char* filename, File* dir)
{
  // TODO
  assert(false);
}

// TODO: for later
void add_file_to_dir(BasicFS* fs, File* file, File* dir, const char* fname)
{
  // TODO
  // Requires a reference counter in file metadata
  assert(false);
}

// Basically, load the <frame>th frame of <file> in the kernel buffer
// and return a ByteArray on the data
ByteArray read_file_frame(BasicFS* fs, File* file, tmp_size_t frame)
{
  diskaddr_t tgt_block = get_nth_file_addr(fs, file, frame);

  // Let's check the file is not a directory
  if (read_attribute(fs, DirBit))
    return NullByteArray;

  assert(frame < read_file_size(fs, Block, TgtFile));

  load_block_in_buffer(fs, tgt_block, RWBuffer);

  return fs->rw_buffer;
}

// Write the kernel buffer to the <frame>th frame of <file>
// TODO: add a size parameter
void write_file_frame(BasicFS* fs, File* file, tmp_size_t frame)
{
  if (frame >= read_file_size(fs, Block, TgtFile))
    assert(false); // TODO: allocation!

  diskaddr_t tgt_block = get_nth_file_addr(fs, file, frame);

  disk_write_block(fs->d, tgt_block, fs->rw_frame);
}

/*
 *
 *
 */

/****** Internal functions ******/
// /!\ This function doesn't reload if the required file is already loaded
// FIXME: deprecated (use later a #pragma GCC poison load_file_at_addr)
#define load_file_at_addr(f,a) \
  load_file_at_addr_(f,a)\
  _Pragma ("GCC warning \"Deprecated: load_file_at_addr\"")

void load_file_at_addr_(BasicFS* fs, diskaddr_t ad)
{
  // TODO: error handling

  if (!(fs->cur_file.main_node == ad))
  {
    disk_read_block(fs->d, ad, fs->file_main_node);

    fs->cur_file.main_node = ad;
  }
}

// /!\ This function loads lazily the buffers which support it:
// it doesn't reload if the required file is already loaded. However, not
// every buffer tracks the disk block it currently stores.
void load_block_in_buffer(BasicFS* fs, diskaddr_t ad, buffer_type buf)
{
  byte* tgt_buffer = 0;

  // RWBuffer, IOBuffer, FileMNodeBuffer, DirMNodeBuffer
  switch (buf)
  {
    case RWBuffer:
      tgt_buffer = fs->rw_frame;
      break;

    case IOBuffer:
      tgt_buffer = fs->io_frame;
      break;

    case FileMNodeBuffer:
      if (fs->cur_file.main_node == ad)
        return;

      tgt_buffer = fs->file_main_node;
      fs->cur_file.main_node = ad;
      break;

    case DirMNodeBuffer:
      tgt_buffer = fs->dir_main_node;
      break;

    default:
      assert(false);
  }

  disk_read_block(fs->d, ad, tgt_buffer);
}

diskaddr_t load_dir_find_addr(BasicFS* fs, File* dir, char* fname)
{
  load_block_in_buffer(fs, dir->main_node, DirMNodeBuffer);

  tmp_size_t dir_block_size = read_file_size(fs, Block, TgtFolder);

  for (tmp_size_t block = 0; block < dir_block_size; block++)

    // TODO
    assert(false);
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

tmp_size_t read_file_size(BasicFS* fs, size_type sz_tp, target_file_type ft)
{
  byte* tgt_buffer;

  switch (ft)
  {
    case TgtFile:
      tgt_buffer = fs->file_main_node;
      break;

    case TgtFolder:
      tgt_buffer = fs->dir_main_node;
      break;

    default:
      assert(false);
  }

  switch (sz_tp)
  {
    case Logical:
      return bin_to_uint32_inplace(fs->file_main_node);

    case Block:
      return 0x00FFFFFF & bin_to_uint32_inplace(fs->file_main_node + 4);

    default:
      assert(false);
      return 0; // Simply to remove warnings about missing return
  }
}

diskaddr_t read_nth_data_block_addr(BasicFS* fs, tmp_size_t frame)
{
// FIXME: no check on frame
  return bin_to_int32_inplace(fs->file_main_node + 8 + frame * 4);
}

uint8_t read_attribute(BasicFS* fs, attr_type attr)
{
  uint8_t attrs = bin_to_int8_inplace(fs->file_main_node + 4);

  switch (attr)
  {
    case DirBit:
      return (attrs >> 7) & 0x01; // We shouldn't need the masking, but just in case

    default:
      assert(false);
      return 0; // Simply to remove warnings about missing return
  }
}
