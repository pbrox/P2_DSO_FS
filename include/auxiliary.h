/*
 * OPERATING SYSTEMS DESING - 16/17
 *
 * @file 	auxiliary.h
 * @brief 	Headers for the auxiliary functions required by filesystem.c.
 * @date	01/03/2017
 */

//Allocates an inode, sets it to 0
int ialloc();

//Frees an inode
int ifree(int inode);

//Allocates a block, sets it to 0
int balloc();

//Frees ablock
int bfree(int block);
