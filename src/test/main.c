/*
 * main.c
 *
 */

#include "../Config.h"
#include "../Disk.h"
#include "../BasicFS.h"

#include <stdio.h>

Disk* d;

void print_disk(int ad_st, int size)
{
  for (int block = ad_st; block < ad_st + size; block++)
    printf("Disk[%d]: %s\n", block, (*(char **) d) + block * DBSize);
}

void print_disk_bin(int ad, int size)
{
  for (int block = ad; block < ad + size; block++)
  {
    printf("Disk[%d]:\n", block);

    for (int i = 0; i < DBSize; i++)
    {
      printf("%2hx ", (*(byte **) d)[block * DBSize + i]);
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
  char t[DBSize] = "blob";

  printf("t start value: %s\n", t);

  BasicFS* fs = create_fs(d);

  File root = get_root(fs);
  File f1 = create_file(fs, "me1", &root, false);
  File f2 = create_file(fs, "me2", &root, false);

  print_disk_bin(0, 7);

  ByteArray buf = read_file_frame(fs, &f1, 0);

  return 0;
}
