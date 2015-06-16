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


// -> int
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

void create_fs(int disk_nr, int fs_nr);

File get_root(int fs_nr);

File create_file(int fs_nr
#if WITH_DIR
    , char* filename, File* dir, bool is_folder
#endif
    );

File get_file(int fs_nr, char* filename, File* dir);

#if WITH_DIR
// TODO: for later
void add_file_to_dir(BasicFS* fs, File* file, File* dir, const char* fname);
#endif

int read_file_frame(int fs_nr, File* file, _size_t frame);

void write_file_frame(int fs_nr, File* file, _size_t frame);

#endif /* SRC_BASICFS_H_ */
