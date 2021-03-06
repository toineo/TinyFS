Basic FS Data scheme
====================

#TODO#
* DBSize
* rigid folder structure (for now)
* 4 should become DA_size
* Can use blocks bigger than disk sectors
* More efficient structure than the free table

#### File convention ####
In this file, we will use the following convention to describe organization of
binary data: a list of lines following format

    n: descr

denoting a piece of data. `n` is the number of byte used for this data, and 
`descr` its role. 

Theses lines are combined in the same way they would be in a C structure:

    n1: descr1
    n2: descr2
    etc...

would be stored on the disk (or in memory) with the first `n1` bytes assigned
to `descr1`, the following `n2` bytes to `descr2`, etc.

If `[bits]` is precised before the list, bits are used instead of bytes.

Global
------
The file system is organized thanks to a root node and a free list (FL), tracking
the set of free blocks.


Root node
---------
The root node stores the root of the filesystem (logically denoted by "/") -
which itself allows to access to all the file of the FS - and other informations,
including the information needed to access the FT.

Organization of the root node: 

    4: disk tail start
    4: free list head
**TODO**

**TODO**

* put the root folder in a different block, pointed by the root node


Free List (FL)
---------------
**TODO**

* List (convention = 0 => not set)
* disk "tail"

File
----
**TODO**

A file is composed of a special block, its _main node_, containing metadata of 
the file, and of one or more blocks containing the data of the file, hence 
called _data blocks_. The file is identified by the disk address of its main
node.

The main node of a file contains some information such as the logical file size 
(`log_size`), the "block" size (the number of blocks allocated to the file - 
`block_size`), and most importantly contains the list of data blocks addresses.

The list of data block addresses is given in the order of the file 
(that is, the n<sup>th</sup> address is the one of the block containing the 
bytes `(n-1) * DBSize` to `n * DBSize - 1` of the file).

**TODO**: to support large file, last addresses -> indirect blocks

However, the kernel driver doesn't support this feature yet, and thus only 
**TODO** blocks can be allocated to a file.

**TODO** Possible unallocated blocks, set to 0

File organization:
Main node:

    4: log_size
    1: attributes
    3: ext_size (converted to a uint32_t in the code, but stored on 3 bytes only)

Followed by a list of addresses:

    4: (disk) address of data block n

The `attributes` field is the following:

    [bits]
    1: folder bit (1 if the file is a folder)
    7: currently unused
<!--- we could also store a ref counter on some bits --->

And each data block follows the following format:

    DBSize: data


Folders
-------
Folders are represented on disk exactly as files are, with the folder bit (in
attributes) set to 1. The folder structure is encoded in the data blocks. A
folder references its child (files and thus folders as well) via their main node
address, associated with a logical file name.

Each entry (called _child entry_) in the folder data respects the following
format:

    1      : fn_size (filename size)
    fn_size: filename, in ascii
    4      : address of the main node of the file

An important property is that a folder data block may often have its last bytes
unused (for instance if no child entry is small enough to fit in its unused
bytes), _even if it is not the last data block of the folder_. Therefore, we
mark such unused bytes with the first byte set to 0 (in place of `fn_size`).

Thanks to this convention and to the existence of `block_size`, we do not
care (yet) about maintaining the `log_size` of a folder (which would in any
case be useful only for the last data block).

_Future improvement_: reduce space usage by splitting entries between blocks.

**TODO**


Kernel organization
------------------
Provides the following primitives:
**TODO**

Put this part in another file