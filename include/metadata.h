/*
 * OPERATING SYSTEMS DESING - 16/17
 *
 * @file 	metadata.h
 * @brief 	Definition of the structures and data types of the file system.
 * @date	01/03/2017
 */

#include <stdlib.h>
#include <strings.h>
#include <stdint.h>
#define MAX_CAPACITY 10485760
#define MIN_CAPACITY 51200


typedef struct {

	uint32_t crc; //Cyclic rendundacy check, to check its integrity 32 bits CRC used
	uint32_t size; //Size in bytes of the file 4 bytes
	uint32_t indirect; //Indirect block although it can be addressed with 16 bits, the using of a 32 bits CRC makes the struct to occupy 44 bytes (because of memory allocation), also all the i-nodes fits in a block with 44 bytes so no optimization achieved by using 16, 32bytes
	char name[32]; //32 character for name

} inode; //4+4+4+32 = 44 bytes

typedef struct {
	
	uint32_t magicNum; 
	uint32_t bk_num; //Number of blocks
	uint32_t crc; //Cyclic rendundacy check, to check its integrity 32 bits CRC used
	uint32_t in_num; // Numbe of inodes
	uint32_t in_size; //Size in bytes of an inode
	char in_map[5];
	char  bk_map[(MAX_CAPACITY/BLOCK_SIZE)/8];//max num bocks/8
	char padding[BLOCK_SIZE - 5*4 -645];  //Not touched, it is implicity initialited to 0 when declared as gobal


} superblock;

#define bitmap_getbit(bitmap_, i_) (bitmap_[i_ >> 3] & (1 << (i_ & 0x07)))
static inline void bitmap_setbit(char *bitmap_, int i_, int val_) {
  if (val_)
    bitmap_[(i_ >> 3)] |= (1 << (i_ & 0x07));
  else
    bitmap_[(i_ >> 3)] &= ~(1 << (i_ & 0x07));
}

inode * mem_inodes; //Impicity to 0
superblock mem_superblock; //Implicity to 0 