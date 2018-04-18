/*
 * OPERATING SYSTEMS DESING - 16/17
 *
 * @file 	filesystem.c
 * @brief 	Implementation of the core file system funcionalities and auxiliary functions.
 * @date	01/03/2017
 */

#include "include/filesystem.h"		// Headers for the core functionality
#include "include/auxiliary.h"		// Headers for auxiliary functions
#include "include/metadata.h"		// Type and structure declaration of the file system
#include "include/crc.h"			// Headers for the CRC functionality


/* 
 * @brief 	Generates the proper file system structure in a storage device, as designed by the student.
 * @return 	0 if success, -1 otherwise.
 */
int mkFS(long deviceSize) 
{	
	//Falta corregir errores
	//Falta el  CRC
	//Cómo poner estos dos bloques (superbk y inodes_bk) a 0 antes de usarlos?

	if(deviceSize > MAX_CAPACITY || deviceSize < MIN_CAPACITY) return -1;

	mem_superblock.crc = 0; //by now
	mem_superblock.magicNum = MAGIC_NUM;  //Setting magic number
	mem_superblock.bk_num = deviceSize/BLOCK_SIZE; //Setting the numBlocks
	mem_superblock.in_num = NUM_INODES; //Setting the inodes
	mem_superblock.in_size = sizeof(inode);//setting the inode size
	bzero(mem_superblock.in_map, mem_superblock.in_num/8); //Sets all inodes as free

	mem_superblock.bk_map = (char*) calloc(mem_superblock.bk_num/8 + ((mem_superblock.bk_num%8 != 0) ? 1 : 0),  sizeof(char)); //Creates the bockmap
	bitmap_setbit(mem_superblock.bk_map,  0, 1); //Set first two bocks as used
	bitmap_setbit(mem_superblock.bk_map,  1, 1);

	mem_inodes = (inode*)calloc(BLOCK_SIZE, sizeof(inode)); //Padded to block size with 0, but inodes will only extist until *(NUM_INODES-1)
	mem_superblock.padding = (char*) calloc(BLOCK_SIZE - 5*sizeof(uint32_t) - 7*sizeof(char),sizeof(char)); //Filling the pading so the structure occupies a block


	unmountFS(); //Write changes in the disk
	return 0;

}

/*
 * @brief 	Mounts a file system in the simulated device.
 * @return 	0 if success, -1 otherwise.
 */
int mountFS(void)
{
	return -1;
}

/*
 * @brief 	Unmounts the file system from the simulated device.
 * @return 	0 if success, -1 otherwise.
 */
int unmountFS(void)
{

	//Write superbock and inode bock

	return -1;
}

/*
 * @brief	Creates a new file, provided it it doesn't exist in the file system.
 * @return	0 if success, -1 if the file already exists, -2 in case of error.
 */
int createFile(char *fileName)
{
	return -2;
}

/*
 * @brief	Deletes a file, provided it exists in the file system.
 * @return	0 if success, -1 if the file does not exist, -2 in case of error..
 */
int removeFile(char *fileName)
{
	return -2;
}

/*
 * @brief	Opens an existing file.
 * @return	The file descriptor if possible, -1 if file does not exist, -2 in case of error..
 */
int openFile(char *fileName)
{
	return -2;
}

/*
 * @brief	Closes a file.
 * @return	0 if success, -1 otherwise.
 */
int closeFile(int fileDescriptor)
{
	return -1;
}

/*
 * @brief	Reads a number of bytes from a file and stores them in a buffer.
 * @return	Number of bytes properly read, -1 in case of error.
 */
int readFile(int fileDescriptor, void *buffer, int numBytes)
{
	return -1;
}

/*
 * @brief	Writes a number of bytes from a buffer and into a file.
 * @return	Number of bytes properly written, -1 in case of error.
 */
int writeFile(int fileDescriptor, void *buffer, int numBytes)
{
	return -1;
}


/*
 * @brief	Modifies the position of the seek pointer of a file.
 * @return	0 if succes, -1 otherwise.
 */
int lseekFile(int fileDescriptor, long offset, int whence)
{
	return -1;
}

/*
 * @brief 	Verifies the integrity of the file system metadata.
 * @return 	0 if the file system is correct, -1 if the file system is corrupted, -2 in case of error.
 */
int checkFS(void)
{
	return -2;
}

/*
 * @brief 	Verifies the integrity of a file.
 * @return 	0 if the file is correct, -1 if the file is corrupted, -2 in case of error.
 */
int checkFile(char *fileName)
{
	return -2;
}
