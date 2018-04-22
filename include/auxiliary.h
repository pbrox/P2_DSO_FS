/*
 * OPERATING SYSTEMS DESING - 16/17
 *
 * @file 	auxiliary.h
 * @brief 	Headers for the auxiliary functions required by filesystem.c.
 * @date	01/03/2017
 */

#include <stdint.h>

//Allocates an inode, sets it to 0
int ialloc();

//Frees an inode
int ifree(int inode);

//Allocates a block, sets it to 0
int balloc();

//Frees ablock
int bfree(int block);

//Searchs an inode by name
int nametoi(char * name);

//Return the number of blocks needed to fit the given size
int numblocks(int size);

//Computes the 32 CRC of the blocks which blk_id is inside of bk_array, if error checked in err
//-1 -> Error
uint32_t blk_32CRC(int * bk_array, int n_blk, int *err);

//Computes the 32 CRC off all indirect allocated blocks of the file system, not need of mounted
uint32_t ind_32CRC(char * inodes_map, char use_disk, int * err);