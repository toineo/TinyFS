/*
 * BasicFS.c
 *
 * Extremely basic/naive File System
 * It is only meant to be easy to prove correct and with a tiny memory usage.
 * The counterpart is that it isn't very easy to use, and most importantly
 * it is incredibly inefficient. It also has no tolerance to failure.
 *
 */

#include "BasicFS.h"

#include <stdlib.h>
#include <string.h>

#include "Config.h"
#include "Types.h"
#include "Disk.h"
#include "File.h"
#include "FSStaticAlloc.h"


// TODO: put these defs in a header?
// TODO: use these defs everywhere
#define FileMNodeLogSizeShift 0
#define FileMNodeAttrShift (FileMNodeLogSizeShift + 4)
#define FileMNodeBlockSizeShift (FileMNodeLogSizeShift + 4)
#define FileMNodeBlockSizeMask 0x00FFFFFF
#define FileMNodeFstAddrShift (FileMNodeBlockSizeShift + 4)

/****** Internals ******/

// Internal types
typedef enum
{
  Block, Logical
} size_type;

typedef enum
{
  DummyAttr
#if WITH_DIR
DirBit
#endif
} attr_type;

typedef enum
{
  RWBuffer, IOBuffer, FileMNodeBuffer
#if WITH_DIR
  , DirMNodeBuffer
#endif
} buffer_type;

typedef enum
{
  TgtFile
#if WITH_DIR
  , TgtFolder
#endif
} target_file_type;

// Internal functions

// TODO: descr
// TODO: add a flush parameter
diskaddr_t alloc_block(BasicFS* fs);

// TODO: descr
void flush_root_node(BasicFS* fs);

// Load file which main node is at <ad> in cur_file and its main node in <buf>
void load_block_in_buffer(BasicFS* fs, diskaddr_t ad, buffer_type buf);

// TODO: descr
void flush_buffer_to_block(BasicFS* fs, diskaddr_t ad, buffer_type buf);

// Load <dir> in the directory main node buffer and attempts to find
// the file with name <fname> inside <dir>. Returns 0 if it cannot
// find such file. If it finds the, file, returns the address associated
// in the folder entry. In addition, if <replace> is not 0, change the
// address (on disk) to <replace>
diskaddr_t load_dir_find_addr(BasicFS* fs, File* dir, char* fname, diskaddr_t replace);

// TODO
File get_file_at_address(BasicFS* fs, diskaddr_t ad);

// TODO
diskaddr_t get_nth_file_addr(BasicFS* fs, File* f, tmp_size_t nblock);

// Reads the <sz_tp> size of the currently loaded main node
tmp_size_t read_file_size(BasicFS* fs, size_type sz_tp, target_file_type ft);

// TODO: descr (+ the name is likely confusing)
void set_file_size(BasicFS* fs, size_type sz_tp, target_file_type ft, tmp_size_t size);

// Reads the nth data block address contained in the currently loaded main node
diskaddr_t read_nth_data_block_addr(BasicFS* fs, tmp_size_t n_addr, target_file_type ft);

// Writes the nth data block address in the currently loaded main node
void set_nth_data_block_addr(BasicFS* fs, tmp_size_t n_addr, target_file_type ft, diskaddr_t addr);

// Reads attributes of the currently loaded main node
uint8_t read_attribute(BasicFS* fs, attr_type attr);

/****** Exported functions ******/
BasicFS* create_fs(Disk* d)
{
  assert(sizeof(BasicFS) <= MemPageSize);

// TODO: integration -> page alloc
// BasicFS* fs = malloc(sizeof(BasicFS));
  BasicFS* fs = alloc_fs();

  fs->d = d;

  load_block_in_buffer(fs, 0, FileMNodeBuffer);

  fs->free_list = 0;
  fs->first_blank = 3; // Root file on block 1, with first data block on 2

// Creating root folder by hand
  set_file_size(fs, Logical, TgtFile, 0);
  set_file_size(fs, Block, TgtFile, 1);

// TODO: put the addresses (1 and 2) in definitions
  set_nth_data_block_addr(fs, 0, TgtFile, 2);

  flush_buffer_to_block(fs, 1, FileMNodeBuffer);

  ByteArray
  rwbuf =
  { .size = sizeof(fs->rw_frame), .array = fs->rw_frame};
  *(ByteArray *) &(fs->rw_buffer) = rwbuf;

  flush_root_node(fs);

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
  File
  root =
  { .main_node = 1};

  return root;

// TODO: if file contains "real" metadata, need to do the following:
// return get_file_at_address(fs, 1);
}

File create_file(BasicFS* fs
#if WITH_DIR
    , char* filename, File* dir, bool is_folder
#endif
    )
{
  diskaddr_t main_node_addr = alloc_block(fs);
  diskaddr_t first_block_addr = alloc_block(fs);

// In the main node, we do not care about setting all the address except the
// first to 0, as block_size is set to 1 (indicates that the other addresses
// are meaningless)

  set_file_size(fs, Logical, TgtFile, 0);
  set_file_size(fs, Block, TgtFile, 1);

  set_nth_data_block_addr(fs, 0, TgtFile, first_block_addr);

  flush_buffer_to_block(fs, main_node_addr, FileMNodeBuffer);

#if WITH_DIR
// TODO: update dir
#endif

  File
  res =
  { .main_node = main_node_addr};
  return res;
}

#if WITH_DIR
// Get file <filename> from folder <dir>
// The file is invalid (address 0) if it could not be found in the folder
File get_file(BasicFS* fs, char* filename, File* dir)
{
  File f;
  f.main_node = load_dir_find_addr(fs, dir, filename, 0);

// FIXME: that's it?

  return f;
}

// TODO: for later
void add_file_to_dir(BasicFS* fs, File* file, File* dir, const char* fname)
{
// TODO
// Requires a reference counter in file metadata
  assert(false);
}
#endif

// Basically, load the <frame>th frame of <file> in the kernel buffer
// and return a ByteArray on the data
ByteArray read_file_frame(BasicFS* fs, File* file, tmp_size_t frame)
{
  diskaddr_t tgt_block = get_nth_file_addr(fs, file, frame);

#if WITH_DIR
// Let's check the file is not a directory
  if (read_attribute(fs, DirBit))
  return NullByteArray;
#endif

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

diskaddr_t alloc_block(BasicFS* fs)
{
// TODO: choose between trying the free list first or the disk tail instead

// If needed, we could store the disk size in the root node
  if (fs->first_blank < get_disk_size(fs->d))
  {
    fs->first_blank++;
    flush_root_node(fs);

    return fs->first_blank - 1;
  }

// TODO: free list
  assert(false);

}

void flush_root_node(BasicFS* fs)
{
  uint32_to_bin_inplace(fs->first_blank, fs->io_frame);
  uint32_to_bin_inplace(fs->free_list, fs->io_frame + 4);

  flush_buffer_to_block(fs, 0, IOBuffer);
}

byte* select_buffer(BasicFS* fs, buffer_type buf)
{
  switch (buf)
  {
    case RWBuffer:
      return fs->rw_frame;

    case IOBuffer:
      return fs->io_frame;

    case FileMNodeBuffer:
      return fs->file_main_node;

#if WITH_DIR
    case DirMNodeBuffer:
      return fs->dir_main_node;
#endif

    default:
      assert(false);
  }

  return 0; // Remove 'no return' warnings
}

byte* select_buffer_from_target(BasicFS* fs, target_file_type ft)
{
  switch (ft)
  {
    case TgtFile:
      return fs->file_main_node;

#if WITH_DIR
    case TgtFolder:
      return fs->dir_main_node;
#endif WITH_DIR

    default:
      assert(false);
  }

  return 0; // Kill warnings
}

// /!\ This function loads lazily the buffers which support it:
// it doesn't reload if the required file is already loaded. However, not
// every buffer tracks the disk block it currently stores.
void load_block_in_buffer(BasicFS* fs, diskaddr_t ad, buffer_type buf)
{
// Do nothing if using a tracking buffer which already stores the wanted block
  if (buf == FileMNodeBuffer && fs->cur_file.main_node == ad)
    return;

  disk_read_block(fs->d, ad, select_buffer(fs, buf));

// Update "tracking" buffers
  if (buf == FileMNodeBuffer)
    fs->cur_file.main_node = ad;

}

void flush_buffer_to_block(BasicFS* fs, diskaddr_t ad, buffer_type buf)
{
  disk_write_block(fs->d, ad, select_buffer(fs, buf));
}

#if WITH_DIR
diskaddr_t load_dir_find_addr(BasicFS* fs, File* dir, char* fname, diskaddr_t replace)
{
  diskaddr_t file_addr = 0;
  uint8_t fname_len = strlen(fname); // TODO: own functions for strings

  load_block_in_buffer(fs, dir->main_node, DirMNodeBuffer);

  tmp_size_t dir_block_size = read_file_size(fs, Block, TgtFolder);

  for (tmp_size_t block = 0; block < dir_block_size; block++)
  {
    diskaddr_t datablock_addr = read_nth_data_block_addr(fs, block, TgtFolder);
    uint32_t cur_pos = FileMNodeFstAddrShift;

    load_block_in_buffer(fs, datablock_addr, IOBuffer);

// If we do not jump out of the loop (that is, if we exit it normally), it
// means that we didn't find the required file, and that file_addr is still 0
    while (cur_pos < DBSize)
    {
      uint8_t cur_fname_len = bin_to_uint8_inplace(fs->io_frame + cur_pos);

      // End of entries for this block
      if (cur_fname_len == 0)
        break;

      // Early mismatch detection
      if (cur_fname_len != fname_len)
      {
        cur_pos += cur_fname_len;
        continue;
      }

      // Compare names
      // TODO: own strcmp
      if (strcmp(fname, (char*) fs->io_frame + cur_pos + 1))
        cur_pos += cur_fname_len;  // Not equal
      else
      {
        file_addr = bin_to_uint32_inplace(fs->io_frame + cur_pos + cur_fname_len + 1);

        // We're asked to replace the address
        if (replace)
        {
          uint32_to_bin_inplace(replace, fs->io_frame + cur_pos + cur_fname_len + 1);
          flush_buffer_to_block(fs, datablock_addr, IOBuffer);
        }

        goto exit;
      }

    }
  }

  exit: return file_addr;

}
#endif

File get_file_at_address(BasicFS* fs, diskaddr_t ad)
{
  load_block_in_buffer(fs, ad, FileMNodeBuffer);

  return fs->cur_file;
}

diskaddr_t get_nth_file_addr(BasicFS* fs, File* f, tmp_size_t nblock)
{
  load_block_in_buffer(fs, f->main_node, FileMNodeBuffer);

  return read_nth_data_block_addr(fs, nblock, TgtFile);
}

tmp_size_t read_file_size(BasicFS* fs, size_type sz_tp, target_file_type ft)
{
  tmp_size_t res;

  switch (sz_tp)
  {
    case Logical:
      res = bin_to_uint32_inplace(select_buffer_from_target(fs, ft) + FileMNodeLogSizeShift);
      break;

    case Block:
      res = FileMNodeBlockSizeMask & bin_to_uint32_inplace(select_buffer_from_target(fs, ft) + FileMNodeBlockSizeShift);
      break;

    default:
      assert(false);
  }

  return res;
}

void set_file_size(BasicFS* fs, size_type sz_tp, target_file_type ft, tmp_size_t size)
{
  byte* tgt_buffer = select_buffer_from_target(fs, ft);

  switch (sz_tp)
  {
    case Logical:
      uint32_to_bin_inplace(size, tgt_buffer + FileMNodeLogSizeShift);
      break;

    case Block:
      {
        ByteArray data = uint32_to_bin(size);
        ByteArray mask = uint32_to_bin(FileMNodeBlockSizeMask);

        tgt_buffer += FileMNodeBlockSizeShift;

        for (int i = 0; i < data.size; i++)
          tgt_buffer[i] = (mask.array[i] & data.array[i]) | ((~mask.array[i]) & tgt_buffer[i]);
      }
      break;

    default:
      assert(false);
  }
}

diskaddr_t read_nth_data_block_addr(BasicFS* fs, tmp_size_t n_addr, target_file_type ft)
{
  byte* tgt_buffer = select_buffer_from_target(fs, ft);

// FIXME: no check on frame
  return bin_to_uint32_inplace(tgt_buffer + FileMNodeFstAddrShift + n_addr * 4);
}

void set_nth_data_block_addr(BasicFS* fs, tmp_size_t n_addr, target_file_type ft, diskaddr_t addr)
{
  byte* tgt_buffer = select_buffer_from_target(fs, ft);

// FIXME: no check on frame
  uint32_to_bin_inplace(addr, tgt_buffer + FileMNodeFstAddrShift + n_addr * 4);

}

uint8_t read_attribute(BasicFS* fs, attr_type attr)
{
  uint8_t attrs = bin_to_uint8_inplace(fs->file_main_node + FileMNodeAttrShift);

  switch (attr)
  {
#if WITH_DIR
    case DirBit:
    return (attrs >> 7) & 0x01; // We shouldn't need the masking, but just in case
#endif

    default:
      assert(false);
      return 0; // Simply to remove warnings about missing return
  }
}
