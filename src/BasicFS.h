/*
 * BasicFS.h
 *
 * TODO: description
 *
 */

#ifndef SRC_BASICFS_H_
#define SRC_BASICFS_H_

#include "Types.h"
#include "Disk.h"
#include "File.h"

// TODO: comments

typedef struct BasicFS BasicFS;

BasicFS* create_fs(Disk* d);

File get_root(BasicFS* fs);

File create_file(BasicFS* fs, char* filename, File* dir, bool is_folder);

File get_file(BasicFS* fs, char* filename, File* dir);

// TODO: for later
void add_file_to_dir(BasicFS* fs, File* file, File* dir, const char* fname);

ByteArray read_file_frame(BasicFS* fs, File* file, tmp_size_t frame);

void write_file_frame(BasicFS* fs, File* file, tmp_size_t frame);

#endif /* SRC_BASICFS_H_ */
