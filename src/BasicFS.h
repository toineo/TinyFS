/*
 * BasicFS.h
 *
 * TODO: description
 *
 */

#ifndef SRC_BASICFS_H_
#define SRC_BASICFS_H_

#include "Types.h"
#include "Config.h"
#include "Disk.h"
#include "File.h"

// As the buffers are fixed (and of predetermined size), we store them directly
// in the file system object to simplify allocation (does this approach has
// any downside - compared to the one were we allocate the buffers by hand?)
// FIXME: better names (in particular, unify buffer names)
typedef struct BasicFS
{
  Disk* d;
  _size_t disk_size;

  // Free blocks management
  diskaddr_t first_blank; // First address of the "disk tail" (set of never yet allocated blocks)
  diskaddr_t free_list; // Chained list of deallocated blocks

  // Frame for doing requested read and writes
  byte rw_frame[DiskSectorSize];

  // Frame for performing I/O for the driver's needs
  byte io_frame[DiskSectorSize];

  // Frame for storing the main nodes
  byte file_main_node[DiskSectorSize]; // Current file main node
  byte dir_main_node[DiskSectorSize]; // For storing directory main node (for operations involving both a file and a dir)

  // File info of the current loaded file (in main_frame)
  File cur_file;

  // ByteArray pointing on rw_frame
  // As it never changes, we can save it once and for all (for the sake
  // of convenience)
  const ByteArray rw_buffer;

  diskaddr_t root_addr;

} BasicFS;

// TODO: comments

typedef struct BasicFS BasicFS;

BasicFS* create_fs(Disk* d);

File get_root(BasicFS* fs);

File create_file(BasicFS* fs
#if WITH_DIR
    , char* filename, File* dir, bool is_folder
#endif
    );

File get_file(BasicFS* fs, char* filename, File* dir);

#if WITH_DIR
// TODO: for later
void add_file_to_dir(BasicFS* fs, File* file, File* dir, const char* fname);
#endif

ByteArray read_file_frame(BasicFS* fs, File* file, _size_t frame);

void write_file_frame(BasicFS* fs, File* file, _size_t frame);

#endif /* SRC_BASICFS_H_ */
