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
// Load file which main node is at <ad> in cur_file and its main node in fs->main_frame
void load_file_at_address(BasicFS* fs, diskaddr_t ad);
File get_file_at_address(BasicFS* fs, diskaddr_t ad);

/****** Exported functions ******/
BasicFS* create_fs(Disk* d)
{
  assert(sizeof(BasicFS) <= MemPageSize);

  // TODO: integration -> page alloc
  BasicFS* fs = malloc(sizeof(BasicFS));

  fs->d = d;

  load_file_at_address(fs, 0);

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
  // TODO
  assert(false);
}

// Write the kernel buffer to the <frame>th frame of <file>
void write_file_frame(BasicFS* fs, File* file, tmp_size_t frame)
{
  // TODO
  assert(false);
}

/****** Internal functions ******/
void load_file_at_address(BasicFS* fs, diskaddr_t ad)
{
  // TODO: error handling

  disk_read_block(fs->d, ad, fs->main_frame);

  fs->cur_file.main_node = ad;
}

File get_file_at_address(BasicFS* fs, diskaddr_t ad)
{
  if (!(fs->cur_file.main_node == ad))
    load_file_at_address(fs, ad);

  return fs->cur_file;
}
