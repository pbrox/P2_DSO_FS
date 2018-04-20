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

int nametoi(char * name){

	for(int i = 0; i < mem_superblock.in_num; ++i){ //For each inode allocated we see if the name is the same
		//If the inode exists and the name is the same, returns the index (the inode number)
		if(bitmap_getbit(mem_superblock.in_map, i) && strcmp(name, mem_inodes[i].name) == 0) return i;
	}

	return -1; //If no inode is found returns error 
}

//Frees an inode
int ifree(int inode){
	//If the inode was not allocated or is not a valid one return error
	if(inode < 0 || inode > 40 || !bitmap_getbit(mem_superblock.in_map, inode)) return -1;
	bitmap_setbit(mem_superblock.in_map, inode, 0); //Sets the inode to not used
	return 0;
}

//Frees a block
int bfree(int block){

	if(block < 0 || block > mem_superblock.bk_num || !bitmap_getbit(mem_superblock.bk_map, block)) return -1;
	bitmap_setbit(mem_superblock.bk_map, block, 0); //Sets the block to not used

	return 0;

}

 //Return the number of blocks needed to fit the given size
int numblocks(int size){
	return (size/BLOCK_SIZE + ((size % BLOCK_SIZE == 0) ? 0 : 1));
	//It is the number of blocks that are totally used and 1 more if there's still size to put
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
	if(nametoi(fileName) != -1) return -1; //The file already exist if nametoi finds it, if it returns error the file didn't exist
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
	int inode_t = nametoi(fileName);
	if(inode_t < 0) return -1; //The file does not exist, return error

	//Checks if the file is opened, if so, closes it to avoid problems
	if(bitmap_getbit(file_table->is_opened,inode_t)) closeFile(inode_t);

	//Gets the number of blocks
	int num_blk = numblocks(mem_inodes[inode_t].size);

	//Reads the indirect block
	int indirect[BLOCK_SIZE/sizeof(uint32_t)];
	if(bread("disk.dat", mem_inodes[inode_t].indirect, (char*)indirect) < 0) return -2;

	//Frees all the blocks
	for(int i = 0; i < num_blk; ++i) if(bfree(indirect[i]) < 0) return -1; //If any error return it
		//Behaviour to DEFINEE!

	//Frees indirect block
	if(bfree( mem_inodes[inode_t].indirect) < 0) return -1; //Same as above

	//Frees inode
	if(ifree(inode_t) < 0) return -1; //Undefined also

	return 0;
}

/*
 * @brief	Opens an existing file.
 * @return	The file descriptor if possible, -1 if file does not exist, -2 in case of error..
 */
int openFile(char *fileName)
{	
	int inode_p = nametoi(fileName);//Gets the inode associated to that name
	
	if(inode_p < 0) return -1; //The file does not exist
	//Searchs the file inside allocated inodes
	if(bitmap_getbit(file_table->is_opened, inode_p)) return -2; //The file is already opened
	bitmap_setbit(file_table->is_opened,inode_p,1); //Set the file as opened
	file_table->file_pos[inode_p] = 0; //Sets the file pointer to 0
	
	return inode_p;
}

/*
 * @brief	Closes a file.
 * @return	0 if success, -1 otherwise.
 */
int closeFile(int fileDescriptor)
{	
	if(fileDescriptor < 0 || fileDescriptor > 40) return -1;
	//Duda con esto
	if(!bitmap_getbit(file_table->is_opened, fileDescriptor)) return -1; //The file is already closed
	bitmap_setbit(file_table->is_opened,fileDescriptor,0); //Set the file as closed
	//No need to put the pointer to 0 because it is initializated as 0
	return 0;
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

	if(fileDescriptor < 0 || fileDescriptor > 40) return -1;
	//First, we have to check if the file to be written is opened or not.
	if(!bitmap_getbit(filetable->is_opened, fileDescriptor)) return -1;

	int seek_ptr = file_table.file_pos[fileDescriptor];

	//Gets the number of blocks
	//int num_blk = numblocks(mem_inodes[inode_t].size);

	int a = mem_inodes[fileDescriptor].size - seek_ptr;
	int blocksOffset = BLOCK_SIZE - a;
	int nBytes = numBytes - blocksOffset;

	//Gets the number of blocks
	int num_blk = numblocks(nBytes);

	int blk;
	if(nBytes != 0){
		int blk = balloc();
	}



	int indirect[BLOCK_SIZE/sizeof(uint32_t)];
	if(bread("disk.dat", mem_inodes[fileDescriptor].indirect, (char*)indirect) < 0) return -1;

	//If the file is open, then we have to now if the current file is empty or not (ESTO NO ESTA HECHO, DE MOMENTO SOLO LEE Y COPIA)
	char aux_blk[BLOCK_SIZE] = {0};
	if(bread("disk.dat", indirect[seek_ptr/BLOCK_SIZE], aux_blk) < 0) return -1;


	for(int i = seek_ptr; i < BLOCK_SIZE; ++i){
		
		memcpy(aux_blk[i], buffer, numBytes);
	}
	
	//If a new block is to be used it needs to be allocated
	if(seek_ptr%BLOCK_SIZE != 0){
		

	}

	for(int i = seek_ptr; i < numBytes; ++i){

	}

	if(bwrite("disk.dat", seek_ptr/BLOCK_SIZE, aux_blk) < 0) return -1;


	//TODO A PARTIR DE AQUI SOBRA
	//Asumo que existe una correspondencia 1:1 entre inode y fileDescriptor
	if(bread("disk.dat", , aux_blk)){
		for(int i = 0; i < numBytes; ++i){

		}
	} 

	fwrite(buffer,1,sizeof(buffer),pFile);
	return -1;
}


/*
 * @brief	Modifies the position of the seek pointer of a file.
 * @return	0 if succes, -1 otherwise.
 */
int lseekFile(int fileDescriptor, long offset, int whence)
{
	//Error if invalid file descriptor or the file is closed
	if(fileDescriptor < 0 || fileDescriptor > 40 || !bitmap_getbit(file_table->is_opened,fileDescriptor) ) return -1;

	switch(fileDescriptor){
		
		case FS_SEEK_BEGIN:
			filetable.file_pos[fileDescriptor] = 0; //If seek beguinputs the pointer to 0 
			break;
		case FS_SEEK_END:
			filetable.file_pos[fileDescriptor] = mem_inodes[fileDescriptor].size; //If seek to end, sets at the size of the file (which in fact is the place to write
			break;
		case FS_SEEK_CUR:
			int aux_p = filetable.file_pos[fileDescriptor] + offset; //Gets the new pointer
			if(aux_p < 0 || aux_p > mem_inodes[fileDescriptor].size) return -1; //If the position is not valid returns error
			filetable.file_pos[fileDescriptor] = aux_p; //Sets the pointer
			break;
		default:
			return -1; //if option is not correct return error

	}

	return 0; //Return success

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
