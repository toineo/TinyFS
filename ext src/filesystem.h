/*
 * BasicFS.h
 *
 * TODO: description
 *
 */

#ifndef SRC_BASICFS_H_
#define SRC_BASICFS_H_

#include "fs_types.h"
#include "fs_config.h"
#include "disk_data.h"



typedef struct File
{
  diskaddr_t main_node;

  // We don't need that
  /*
   size_t log_size;
   size_t block_size;
   */

} File;


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
