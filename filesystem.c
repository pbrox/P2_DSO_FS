/*
 * OPERATING SYSTEMS DESING - 16/17
 *
 * @file 	filesystem.c
 * @brief 	Implementation of the core file system funcionalities and auxiliary functions.
 * @date	01/03/2017
 */
//TO DO
// CRC
//ASK DAVID NAME_FILES AND  FIXXX!!!

#include "include/filesystem.h"		// Headers for the core functionality
#include "include/auxiliary.h"		// Headers for auxiliary functions
#include "include/metadata.h"		// Type and structure declaration of the file system
#include "include/crc.h"			// Headers for the CRC functionality


int ialloc(){

	for(int i = 0; i < mem_superblock.in_num; ++i){

		if(!bitmap_getbit(mem_superblock.in_map, i)){ //i.e the node is free
			//The first free inode
			bzero((mem_inodes+i),sizeof(inode)); //Clearing the inode (not done in free)
			bitmap_setbit(mem_superblock.in_map, i, 1); //Sets the inode as used
			return i; //Returns its id
		}
	}

	return -1; //Error in case of not found inode
}

int balloc(){

	char blank[BLOCK_SIZE] = {0}; //to 0 by default

	for(int i = 0; i < mem_superblock.bk_num; ++i){

		if(!bitmap_getbit(mem_superblock.bk_map, i)) { //The block is free

			if(bwrite("disk.dat", i, blank) < 0) return -1;
			bitmap_setbit(mem_superblock.bk_map, i, 1); //Sets it as used
			return i;

		}
	}

	return -1;
}

/* 
 * @brief 	Generates the proper file system structure in a storage device, as designed by the student.
 * @return 	0 if success, -1 otherwise.
 */
int mkFS(long deviceSize) 
{	
	//Falta corregir errores
	//Falta el  CRC

	if(deviceSize > MAX_CAPACITY || deviceSize < MIN_CAPACITY) return -1;

	//SuperBlock creation

	mem_superblock.magicNum = MAGIC_NUM;  //Setting magic number
	mem_superblock.bk_num = deviceSize/BLOCK_SIZE; //Setting the numBlocks
	mem_superblock.crc = 0; //by now 
	mem_superblock.in_num = NUM_INODES; //Setting the inodes
	mem_superblock.in_size = sizeof(inode);//setting the inode size
	//Regarding the blockmap, it is initiallized automaticaly to 0 as it is global
	bitmap_setbit(mem_superblock.bk_map,  0, 1); //Set first two bocks as used
	bitmap_setbit(mem_superblock.bk_map,  1, 1);

	//Padding is also automaticaly initiaized to 0's
	//Inodes Part 
	mem_inodes = (inode*)calloc(NUM_INODES, sizeof(inode)); //Set to 0's
	if(!mem_inodes) return -1;
	unmountFS(); //Write changes in the disk
	return 0;

}

/*
 * @brief 	Mounts a file system in the simulated device.
 * @return 	0 if success, -1 otherwise.
 */
int mountFS(void)
{	
	//Gets the superblock
	if(bread("disk.dat", 0, (char*)&mem_superblock) < 0) return -1;

	//Check magic number
	if(mem_superblock.magicNum != MAGIC_NUM) return -1;

	//Checks the number of inodes and alocates its memory
	if(mem_superblock.in_num != NUM_INODES) return -1;
	char raw[BLOCK_SIZE];
	//Gets inodes
	if(bread("disk.dat", 1, raw) < 0) return -1;

	//Allocates memory for those inodes
	mem_inodes = (inode *)calloc(mem_superblock.in_num, mem_superblock.in_size);
	if(!mem_inodes) return -1;

	//Copies inodes
	memcpy(mem_inodes, raw, mem_superblock.in_num*mem_superblock.in_size);

	//Allocates memory for the file table, sets its to 0 (calloc call)
	file_table = (openFile_table*)calloc(1,sizeof(openFile_table));
	if(!file_table) return -1;

	return 0;

}

/*
 * @brief 	Unmounts the file system from the simulated device.
 * @return 	0 if success, -1 otherwise.
 */
int unmountFS(void)
{
	//Writes superbock and inode block
	//Computes the CRC -- TO_DO
	//Write the two metadata bocks of the fiesystem

	//Checks open files and closes opened ones //DUDA QUITAR?
	for(int i = 0; i < mem_superblock.in_num;++i){
		if(bitmap_getbit(file_table->is_opened, i)){
			if(closeFile(i) < 0) return -1;
		}
	}

	//Writes SuperBock, no need padding because is exactly in memory 2048 bytes
	if(bwrite("disk.dat", 0, (char*)&mem_superblock) < 0) return -1;

	//Inodes part
	char aux_blk[BLOCK_SIZE] = {0}; //Implicity to 0's
	memcpy(aux_blk, mem_inodes, NUM_INODES*sizeof(inode)); //Writes exisixting inodes
	if(bwrite("disk.dat", 1, aux_blk) < 0) return -1;

	//Free used memory
	free(mem_inodes);
	free(file_table);

	return 0;
}

/*
 * @brief	Creates a new file, provided it it doesn't exist in the file system.
 * @return	0 if success, -1 if the file already exists, -2 in case of error.
 */
int createFile(char *fileName)
{
	if(strlen(fileName)>31) return -2; //If the file_name is bigger than 32 CHECKK!
	//Checks uniqueness
	for(int i = 0; i < mem_superblock.in_num; ++i){ //For each inode allocated we see if the name is the same
		if(bitmap_getbit(mem_superblock.in_map, i) && !strcmp(fileName, mem_inodes[i].name)) return -1;
	}
	int inode_id, indirect_id;
	//Allocates a new inode
	if((inode_id = ialloc()) < 0) return -2;
	//Allocates the indirect block
	if((indirect_id = balloc()) < 0 ){ //If cannot find a block
		ifree(inode_id); //Frees the allocated inode
		return -1;
	}

	//Fills the inode CRC not covered yet
	//mem_inodes[inode_id].crc = ...;
	//I think ialloc sets to 0 check
	//mem_inodes[inode_id].size = 0;
	mem_inodes[inode_id].indirect = indirect_id; //The block is set to 0
	//The indirect occupied blocks is not needed because it can be computed
	strcpy(mem_inodes[inode_id].name, fileName);
	return 0;
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
