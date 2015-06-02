/*
 * File.h
 *
 * Structure used to identify files
 *
 */

#ifndef FILE_H_
#define FILE_H_

typedef struct FileId
{
  diskaddr_t main_node;

  // We don't need that
  /*
   size_t log_size;
   size_t block_size;
   */

} File;

#endif /* FILE_H_ */
