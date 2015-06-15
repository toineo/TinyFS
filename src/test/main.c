/*
 * main.c
 *
 */

#include "../Config.h"
#include "../Disk.h"
#include "../BasicFS.h"

#include <stdio.h>
#include <string.h>

Disk* d;

void print_disk(int ad_st, int size)
{
  for (int block = ad_st; block < ad_st + size; block++)
    printf("Disk[%d]: %s\n", block, (*(char **) d) + block * DiskSectorSize);
}

void print_disk_bin(int ad, int size)
{
  for (int block = ad; block < ad + size; block++)
  {
    printf("Disk[%d]:\n", block);

    for (int i = 0; i < DiskSectorSize; i++)
    {
      printf("%2hx ", (*(byte **) d)[block * DiskSectorSize + i]);
      if (i % 16 == 15)
        printf("\n");
    }
  }

}

int main()
{
  d = init_disk();

  char s1[] = "I have really no *********** for these tests.";
  char s2[] = "inspiration";
  char s3[] = " No inspiration at all.";
  char s4[] = "lol";
  char t[DiskSectorSize] = "blob";

  printf("t start value: %s\n", t);

  BasicFS* fs = create_fs(d);

  File root = get_root(fs);
  File f1 = create_file(fs);
  File f2 = create_file(fs);

  //print_disk_bin(0, 7);

  ByteArray buf = read_file_frame(fs, &f1, 0);

  bin_copy_in_place(str_to_bytearray(s1), buf.array);

  write_file_frame(fs, &f1, 0);
  read_file_frame(fs, &f1, 0);

  bin_copy_in_place(str_to_bytearray(s2), buf.array + 17);

  write_file_frame(fs, &f2, 0);
  read_file_frame(fs, &f2, 0);

  bin_copy_in_place(str_to_bytearray(s3), buf.array + sizeof(s1) - 1);

  write_file_frame(fs, &f1, 0);
  read_file_frame(fs, &f1, 0);

  bin_copy_in_place(str_to_bytearray(s4), buf.array + 10);

  write_file_frame(fs, &f2, 0);
  read_file_frame(fs, &f2, 0);

  memcpy(t, buf.array, buf.size);

  printf("t final value: %s\n", t);

  print_disk(4, 1);
  print_disk(6, 1);

  return 0;
}
