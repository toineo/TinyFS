/*
 * filesystem.c
 *
 * Extremely basic/naive File System
 * It is only meant to be easy to prove correct and with a tiny memory usage.
 * The counterpart is that it isn't very easy to use, and most importantly
 * it is incredibly inefficient. It also has no tolerance to failure.
 *
 */

#include "filesystem.h"



// TODO: put these defs in a header?
// TODO: use these defs everywhere
#define FileMNodeLogSizeShift 0
#define FileMNodeAttrShift (FileMNodeLogSizeShift + 4)
#define FileMNodeBlockSizeShift (FileMNodeLogSizeShift + 4)
#define FileMNodeBlockSizeMask 0x00FFFFFF
#define FileMNodeFstAddrShift (FileMNodeBlockSizeShift + 4)




// As the buffers are fixed (and of predetermined size), we store them directly
// in the file system object to simplify allocation (does this approach has
// any downside - compared to the one were we allocate the buffers by hand?)
// FIXME: better names (in particular, unify buffer names)
typedef struct filesystem
{
  int disk_index;
  _size_t disk_size;

  // Free blocks management
  diskaddr_t first_blank; // First address of the "disk tail" (set of never yet allocated blocks)
  diskaddr_t free_list; // Chained list of deallocated blocks

  // Frame for doing requested read and writes
  int rw_buffer_index;

  // Frame for performing I/O for the driver's needs
  byte io_buffer_index;

  // Frame for storing the main nodes
  byte fmainnode_buffer_index; // Current file main node
  byte dmainnode_buffer_index; // For storing directory main node (for operations involving both a file and a dir)

  // File info of the current loaded file (in main_frame)
  File cur_file;

  diskaddr_t root_addr;

} filesystem;




/****** Static objects ******/
filesystem fs[n_fs];

static bool is_fs_init[n_fs] = FALSE; // FIXME init


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
diskaddr_t alloc_block(int fs_nr);

// TODO: descr
void flush_root_node(int fs_nr);

// Load file which main node is at <ad> in cur_file and its main node in <buf>
void load_block_in_buffer(int fs_nr, diskaddr_t ad, buffer_type buf);

// TODO: descr
void flush_buffer_to_block(int fs_nr, diskaddr_t ad, buffer_type buf);

// Load <dir> in the directory main node buffer and attempts to find
// the file with name <fname> inside <dir>. Returns 0 if it cannot
// find such file. If it finds the, file, returns the address associated
// in the folder entry. In addition, if <replace> is not 0, change the
// address (on disk) to <replace>
#if WITH_DIR
diskaddr_t load_dir_find_addr(int fs_nr, File* dir, char* fname, diskaddr_t replace);
#endif

// TODO
File get_file_at_address(int fs_nr, diskaddr_t ad);

// TODO
diskaddr_t get_nth_file_addr(int fs_nr, File* f, _size_t nblock);

// Reads the <sz_tp> size of the currently loaded main node
_size_t read_file_size(int fs_nr, size_type sz_tp, target_file_type ft);

// TODO: descr (+ the name is likely confusing)
void set_file_size(int fs_nr, size_type sz_tp, target_file_type ft, _size_t size);

// Reads the nth data block address contained in the currently loaded main node
diskaddr_t read_nth_data_block_addr(int fs_nr, _size_t n_addr, target_file_type ft);

// Writes the nth data block address in the currently loaded main node
void set_nth_data_block_addr(int fs_nr, _size_t n_addr, target_file_type ft, diskaddr_t addr);

// Reads attributes of the currently loaded main node
uint8_t read_attribute(int fs_nr, attr_type attr);

/****** Exported functions ******/
void create_fs(int disk_nr, int fs_nr)
{
  int i;

  // initialize once.
  if (is_fs_init[fs_nr] == TRUE)
  {
    return;
  }
  is_fs_init[fs_nr] = TRUE;
  fs[fs_nr].disk_index = disk_nr;

  for (i = 0; i < n_ba_per_fs; i++)
    init_bytearray(fs_nr * n_ba_per_fs + i);

  fs[fs_nr].fmainnode_buffer_index = fs_nr * n_ba_per_fs;
  fs[fs_nr].dmainnode_buffer_index = fs_nr * n_ba_per_fs + 1;
  fs[fs_nr].io_buffer_index = fs_nr * n_ba_per_fs + 2;
  fs[fs_nr].rw_buffer_index = fs_nr * n_ba_per_fs + 3;

  load_block_in_buffer(fs_nr, 0, FileMNodeBuffer);

  fs[fs_nr].free_list = 0;
  fs[fs_nr].first_blank = 3 + get_first_addr(disk_nr); // Root file on block 1, with first data block on 2

  fs[fs_nr].disk_size = get_disk_size(d);
  fs[fs_nr].root_addr = get_first_addr(d);

// Creating root folder by hand
  set_file_size(fs_nr, Logical, TgtFile, 0);
  set_file_size(fs_nr, Block, TgtFile, 1);

// TODO: put the addresses (1 and 2) in definitions
  set_nth_data_block_addr(fs_nr, 0, TgtFile, 2);

  flush_buffer_to_block(fs_nr, fs[fs_nr].root_addr + 1, FileMNodeBuffer);


  flush_root_node(fs_nr);

  return;
}

filesystem* retrieve_fs()
{
  filesystem* fs = 0; // FIXME: temporary

// TODO
  assert(false);

  return fs;
}

File get_root(int fs_nr)
{
  File
  root =
  { .main_node = fs[fs_nr].root_addr + 1};

  return root;

// TODO: if file contains "real" metadata, need to do the following:
// return get_file_at_address(fs, 1);
}

File create_file(int fs_nr
#if WITH_DIR
    , char* filename, File* dir, bool is_folder
#endif
    )
{
  diskaddr_t main_node_addr = alloc_block(fs_nr);
  diskaddr_t first_block_addr = alloc_block(fs_nr);

// In the main node, we do not care about setting all the address except the
// first to 0, as block_size is set to 1 (indicates that the other addresses
// are meaningless)

  set_file_size(fs_nr, Logical, TgtFile, 0);
  set_file_size(fs_nr, Block, TgtFile, 1);

  set_nth_data_block_addr(fs_nr, 0, TgtFile, first_block_addr);

  flush_buffer_to_block(fs_nr, main_node_addr, FileMNodeBuffer);

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
File get_file(int fs_nr, char* filename, File* dir)
{
  File f;
  f.main_node = load_dir_find_addr(fs_nr, dir, filename, 0);

// FIXME: that's it?

  return f;
}

// TODO: for later
void add_file_to_dir(int fs_nr, File* file, File* dir, const char* fname)
{
// TODO
// Requires a reference counter in file metadata
  assert(false);
}
#endif

// Basically, load the <frame>th frame of <file> in the kernel buffer
// and return the index of the ByteArray, representing the buffer with the data
int read_file_frame(int fs_nr, File* file, _size_t frame)
{
  diskaddr_t tgt_block = get_nth_file_addr(fs_nr, file, frame);

#if WITH_DIR
// Let's check the file is not a directory
  if (read_attribute(fs_nr, DirBit))
  return NullByteArray;
#endif

  assert(frame < read_file_size(fs_nr, Block, TgtFile));

  load_block_in_buffer(fs_nr, tgt_block, RWBuffer);

  return fs[fs_nr].rw_buffer_index;
}

// Write the kernel buffer to the <frame>th frame of <file>
// TODO: add a size parameter
void write_file_frame(int fs_nr, File* file, _size_t frame)
{
  if (frame >= read_file_size(fs_nr, Block, TgtFile))
    assert(false); // TODO: allocation!

  diskaddr_t tgt_block = get_nth_file_addr(fs_nr, file, frame);

  disk_write_block(fs[fs_nr].disk_index, tgt_block, fs[fs_nr].rw_buffer_index);
}

/*
 *
 *
 */

/****** Internal functions ******/

diskaddr_t alloc_block(int fs_nr)
{
// TODO: choose between trying the free list first or the disk tail instead

// If needed, we could store the disk size in the root node
  if (fs[fs_nr].first_blank < fs[fs_nr].disk_size)
  {
    fs[fs_nr].first_blank++;
    flush_root_node(fs_nr);

    return fs[fs_nr].first_blank - 1;
  }

// TODO: free list
  assert(false);
  return 0;

}

void flush_root_node(int fs_nr)
{
  bytearray_set_uint32(fs[fs_nr].io_buffer_index, 0, fs[fs_nr].first_blank);
  bytearray_set_uint32(fs[fs_nr].io_buffer_index, 4, fs[fs_nr].free_list);

  flush_buffer_to_block(fs_nr, fs[fs_nr].root_addr, IOBuffer);
}

int select_buffer(int fs_nr, buffer_type buf)
{
  switch (buf)
  {
    case RWBuffer:
      return fs[fs_nr].rw_buffer_index;

    case IOBuffer:
      return fs[fs_nr].io_buffer_index;

    case FileMNodeBuffer:
      return fs[fs_nr].fmainnode_buffer_index;

#if WITH_DIR
    case DirMNodeBuffer:
      return fs[fs_nr].dir_main_node;
#endif

    default:
      assert(false);
  }

  return 0; // Remove 'no return' warnings
}

int select_buffer_from_target(int fs_nr, target_file_type ft)
{
  switch (ft)
  {
    case TgtFile:
      return fs[fs_nr].fmainnode_buffer_index;

#if WITH_DIR
    case TgtFolder:
      return fs[fs_nr].dir_main_node;
#endif

    default:
      assert(false);
  }

  return 0; // Kill warnings
}

// /!\ This function loads lazily the buffers which support it:
// it doesn't reload if the required file is already loaded. However, not
// every buffer tracks the disk block it currently stores.
void load_block_in_buffer(int fs_nr, diskaddr_t ad, buffer_type buf)
{
// Do nothing if using a tracking buffer which already stores the wanted block
  if (buf == FileMNodeBuffer && fs[fs_nr].cur_file.main_node == ad)
    return;

  disk_read_block(fs[fs_nr].disk_index, ad, select_buffer(fs, buf));

// Update "tracking" buffers
  if (buf == FileMNodeBuffer)
    fs[fs_nr].cur_file.main_node = ad;

}

void flush_buffer_to_block(int fs_nr, diskaddr_t ad, buffer_type buf)
{
  disk_write_block(fs[fs_nr].disk_index, ad, select_buffer(fs_nr, buf));
}

// TODO: switch to static fs objects
#if WITH_DIR
diskaddr_t load_dir_find_addr(int fs_nr, File* dir, char* fname, diskaddr_t replace)
{
  diskaddr_t file_addr = 0;
  uint8_t fname_len = strlen(fname); // TODO: own functions for strings

  load_block_in_buffer(fs, dir->main_node, DirMNodeBuffer);

  _size_t dir_block_size = read_file_size(fs, Block, TgtFolder);

  for (_size_t block = 0; block < dir_block_size; block++)
  {
    diskaddr_t datablock_addr = read_nth_data_block_addr(fs, block, TgtFolder);
    uint32_t cur_pos = FileMNodeFstAddrShift;

    load_block_in_buffer(fs, datablock_addr, IOBuffer);

// If we do not jump out of the loop (that is, if we exit it normally), it
// means that we didn't find the required file, and that file_addr is still 0
    while (cur_pos < DBSize)
    {
      uint8_t cur_fname_len = bin_to_uint8_inplace(fs->io_buffer_index + cur_pos);

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
      if (strcmp(fname, (char*) fs->io_buffer_index + cur_pos + 1))
        cur_pos += cur_fname_len;  // Not equal
      else
      {
        file_addr = bin_to_uint32_inplace(fs->io_buffer_index + cur_pos + cur_fname_len + 1);

        // We're asked to replace the address
        if (replace)
        {
          uint32_to_bin_inplace(replace, fs->io_buffer_index + cur_pos + cur_fname_len + 1);
          flush_buffer_to_block(fs, datablock_addr, IOBuffer);
        }

        goto exit;
      }

    }
  }

  exit: return file_addr;

}
#endif

File get_file_at_address(int fs_nr, diskaddr_t ad)
{
  load_block_in_buffer(fs_nr, ad, FileMNodeBuffer);

  return fs[fs_nr].cur_file;
}

diskaddr_t get_nth_file_addr(int fs_nr, File* f, _size_t nblock)
{
  load_block_in_buffer(fs_nr, f->main_node, FileMNodeBuffer);

  return read_nth_data_block_addr(fs_nr, nblock, TgtFile);
}

_size_t read_file_size(int fs_nr, size_type sz_tp, target_file_type ft)
{
  _size_t res = 0;

  switch (sz_tp)
  {
    case Logical:
      res = bytearray_get_uint32(select_buffer_from_target(fs_nr, ft), FileMNodeLogSizeShift);
      break;

    case Block:
      res = FileMNodeBlockSizeMask & bytearray_get_uint32(select_buffer_from_target(fs_nr, ft), FileMNodeBlockSizeShift);
      break;

    default:
      assert(false);
  }

  return res;
}

void set_file_size(int fs_nr, size_type sz_tp, target_file_type ft, _size_t size)
{
  int tgt_buffer = select_buffer_from_target(fs_nr, ft);

  switch (sz_tp)
  {
    case Logical:
      bytearray_set_uint32(tgt_buffer, FileMNodeLogSizeShift, size);
      break;

    case Block:
      {
        uint32_to_bin_inplace(size, data);
        uint32_to_bin_inplace(FileMNodeBlockSizeMask, mask);

        tgt_buffer += ;

        // FIXME
        int i;
        for (i = 0; i < 4; i++)
          bytearray_set(tgt_buffer, FileMNodeBlockSizeShift + i,
              (mask[i] & data[i]) | ((~mask[i]) & tgt_buffer[i]));
      }
      break;

    default:
      assert(false);
  }
}

diskaddr_t read_nth_data_block_addr(int fs_nr, _size_t n_addr, target_file_type ft)
{
  int tgt_buffer = select_buffer_from_target(fs_nr, ft);

// FIXME: no check on frame
  return bytearray_get_uint32(tgt_buffer, FileMNodeFstAddrShift + n_addr * 4);
}

void set_nth_data_block_addr(int fs_nr, _size_t n_addr, target_file_type ft, diskaddr_t addr)
{
  int tgt_buffer = select_buffer_from_target(fs_nr, ft);

// FIXME: no check on frame
  bytearray_set_uint32(tgt_buffer, FileMNodeFstAddrShift + n_addr * 4, addr);

}

uint8_t read_attribute(int fs_nr, attr_type attr)
{
  uint8_t attrs = bytearray_get(fs[fs_nr].fmainnode_buffer_index, FileMNodeAttrShift);

  switch (attr)
  {
#if WITH_DIR
    case DirBit:
    return (attrs >> 7) & 0x01; // We shouldn't need the masking, but just in case
#endif

    default:
      // This code is uselessly using attrs to remove warning about its non usage
      attrs = 0;
      assert(false);
      return attrs; // Simply to remove warnings about missing return
  }
}
