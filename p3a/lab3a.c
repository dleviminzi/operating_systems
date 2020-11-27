#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include "ext2_fs.h"


#define ERROR 1
#define SUCCESS 0

void super_block(struct ext2_super_block* superBlock)
{
    /* collecting information from superblock and printing */
    __u32 numB = superBlock->s_blocks_count;
    __u32 numI = superBlock->s_inodes_count;
    __u32 bSize = 1024 << superBlock->s_log_block_size;
    __u32 iSize = superBlock->s_inode_size;
    __u32 bPerG = superBlock->s_blocks_per_group;
    __u32 iPerG = superBlock->s_inodes_per_group;
    __u32 nonresI = superBlock->s_first_ino;

    fprintf(stdout, "SUPERBLOCK,%d,%d,%d,%d,%d,%d,%d\n", numB, numI, bSize, iSize, bPerG, iPerG, nonresI);
}

void group_descriptors(struct ext2_super_block *superBlock, struct ext2_group_desc *group_desc)
{
  //print information about group
  //we assume 1 group in file system
  int group_num = 0;
  __u32 num_blocks = superBlock->s_blocks_per_group;
  __u32 num_inodes = superBlock->s_inodes_per_group;
  __u32 free_blocks = group_desc->bg_free_blocks_count;
  __u32 free_inodes = group_desc->bg_free_inodes_count;
  __u32 free_block_bitmap = group_desc->bg_block_bitmap;
  __u32 free_inode_bitmap = group_desc->bg_inode_bitmap;
  __u32 first_inodes_pos = group_desc->bg_inode_table;
  fprintf(stdout, "GROUP,%d,%d,%d,%d,%d,%d,%d,%d\n", group_num, num_blocks, num_inodes, free_blocks, free_inodes, free_block_bitmap, free_inode_bitmap, first_inodes_pos);
}

void block_bitmap(char* bitmap, __u32 num_blocks)
{
  //iterate through all the bytes in the bitmap
  __u32 i = 0;
  while(1)
    {
      //current byte
      char cur_byte = bitmap[i];
      //examine each of the 8 blocks represented in this byte
      int j;
      int testval = 1<<7;
      for(j = 0; j < 8; j++)
	{
	  __u32 block_num = (i*8) + (j+1);
	  if((testval & cur_byte) == 0)
	    {
	      //the block is free
	      fprintf(stdout, "BFREE,%d\n", block_num);
	    }
	  //check if we have examined every block
	  if(block_num == num_blocks)
	    return;
	  
	  testval = testval>>1;
	}
      i++;
    }
}

void inode_bitmap(char* bitmap, __u32 num_inodes)
{
    //iterate through all the bytes in the bitmap
  __u32 i = 0;
  while(1)
    {
      //current byte
      char cur_byte = bitmap[i];
      //examine each of the 8 blocks represented in this byte
      int j;
      int testval = 1<<7;
      for(j = 0; j < 8; j++)
	{
	  __u32 inode_num = (i*8) + (j+1);
	  if((testval & cur_byte) == 0)
	    {
	      //the block is free
	      fprintf(stdout, "IFREE,%d\n", inode_num);
	    }
	  testval = testval>>1;

	  if(inode_num == num_inodes)
	    return;
	}
      i++;
    }

}

int main(int argc, char *argv[]) {

    /* argument handling */
    if (argc < 2 || argc > 2) {
        fprintf(stderr, "Must pass exactly one img file argument.\n");      /* I doubt it matters, but we could use cstdio here if we want */
        exit(ERROR);
    }

    /* allocating superBlock object (will be populated using pread) */
    struct ext2_super_block *superBlock = malloc(sizeof(struct ext2_super_block));

    /* opening img and reading super block into object created above */
    int imgFD = open(argv[1], O_RDONLY);

    if (pread(imgFD, (void *) superBlock, sizeof(struct ext2_super_block), 1024) < 1) {
        fprintf(stderr, "Failed to load superblock into memory\n");
        exit(ERROR);
    }

    //verify that we have found the superblock
    if(superBlock->s_magic != EXT2_SUPER_MAGIC)
      {
	fprintf(stderr, "Corrupted superblock\n");
	free(superBlock);
        exit(ERROR);
      }

    super_block(superBlock);

    __u32 bSize = 1024 << superBlock->s_log_block_size;
    struct ext2_group_desc *group_desc = malloc(sizeof(struct ext2_group_desc));
    if(pread(imgFD, (void *) group_desc, sizeof(struct ext2_group_desc), 1024 + bSize) < 1)
      {
	fprintf(stderr, "Failed to load group descriptors into memory\n");
	free(group_desc);
	free(superBlock);
	exit(ERROR);
      }
    
    group_descriptors(superBlock, group_desc);

    __u32 block_bitmap_pos = group_desc->bg_block_bitmap;
    __u32 block_bitmap_offset = (block_bitmap_pos - 1)*bSize;
    //store the block in a buffer
    char buff_block[bSize];
    if(pread(imgFD, (void*)buff_block, bSize, block_bitmap_offset + 1024) < 1){
      fprintf(stderr, "Failed to read block bitmap\n");
      free(group_desc);
      free(superBlock);
      exit(ERROR);
    }

    //process the bitmap
    __u32 block_count = superBlock->s_blocks_count;
    block_bitmap((char*)buff_block, block_count);

    //process the inode bitmap
    __u32 inode_bitmap_pos = group_desc->bg_inode_bitmap;
    __u32 inode_bitmap_offset = (inode_bitmap_pos - 1)*bSize;
    //store the block in a buffer
    char buff_inode[bSize];
    if(pread(imgFD, (void*)buff_inode, bSize, inode_bitmap_offset + 1024) < 1){
      fprintf(stderr, "Failed to read inode bitmap\n");
      free(group_desc);
      free(superBlock);
      exit(ERROR);
    }

    __u32 inode_count = superBlock->s_inodes_count;
    inode_bitmap((char*)buff_inode, inode_count);
    exit(SUCCESS);
}
