#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include "ext2_fs.h"


#define ERROR 1
#define SUCCESS 0

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

    /* collecting information from superblock and printing */
    __u32 numB = superBlock->s_blocks_count;
    __u32 numI = superBlock->s_inodes_count;
    __u32 bSize = 1024 << superBlock->s_log_block_size;
    __u32 iSize = superBlock->s_inode_size;
    __u32 bPerG = superBlock->s_blocks_per_group;
    __u32 iPerG = superBlock->s_inodes_per_group;
    __u32 nonresI = superBlock->s_first_ino;

    fprintf(stdout, "SUPERBLOCK,%d,%d,%d,%d,%d,%d,%d\n", numB, numI, bSize, 
                                                         iSize, bPerG, iPerG, 
                                                         nonresI);

    exit(SUCCESS);
}