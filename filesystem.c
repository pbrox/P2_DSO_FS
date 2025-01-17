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

//Alocates an inode
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


	for(int i = 2; i < mem_superblock.bk_num; ++i){

		if(!bitmap_getbit(mem_superblock.bk_map, i)) { //The block is free

			if(bwrite("disk.dat", i, blank) < 0) return -1;
			bitmap_setbit(mem_superblock.bk_map, i, 1); //Sets it as used
			return i;

		}
	}

	return -1; //Cannot find a free block
} 
 
int nametoi(char * name){

	//As strncmp will not check it we have to check this before 
	if(strlen(name) > MAX_NAME) return -1;

	for(int i = 0; i < mem_superblock.in_num; ++i){ //For each inode allocated we see if the name is the same
		//If the inode exists and the name is the same, returns the index (the inode number)
		if(bitmap_getbit(mem_superblock.in_map, i) && strncmp(mem_inodes[i].name, name , MAX_NAME) == 0) return i;
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

//Computes the 32 CRC of the blocks which blk_id is inside of bk_array and bk array
uint32_t blk_32CRC(int * bk_array, int n_blk, int * err){

	//To store the results
	uint32_t current;
	char aux_blks[(n_blk+1)*BLOCK_SIZE];
	char aux_blk[BLOCK_SIZE];
 
	//getting blocks
	for(int i = 0; i < n_blk; ++i){  

		if(bread("disk.dat", bk_array[i] ,aux_blk) < 0){
			*err = -1;
			return 0;
		} 

		memcpy(aux_blks+(i*BLOCK_SIZE), aux_blk, BLOCK_SIZE);

	}
	//Adding indirect
	memcpy(aux_blks+(n_blk*BLOCK_SIZE), (char*)bk_array, BLOCK_SIZE);

	//Computing CRC
 	current = CRC32((const unsigned char*)aux_blks, BLOCK_SIZE*n_blk, 0);
	*err = 0; 
	return current;
}

/* 
 * @brief 	Generates the proper file system structure in a storage device, as designed by the student.
 * @return 	0 if success, -1 otherwise.
 */
int mkFS(long deviceSize) 
{	
	//If it is  mounted  or the capacity is out the limits fails
	if(is_mounted) return 0;

	if(deviceSize > MAX_CAPACITY || deviceSize < MIN_CAPACITY) return -1;

	//SuperBlock creation

	mem_superblock.magicNum = MAGIC_NUM;  //Setting magic number
	mem_superblock.bk_num = deviceSize/BLOCK_SIZE; //Setting the numBlocks
	mem_superblock.in_num = NUM_INODES; //Setting the inodes
	mem_superblock.in_size = sizeof(inode);//setting the inode size
	//Regarding the blockmap and inmap, it is initiallized automaticaly to 0 as it is global
	bitmap_setbit(mem_superblock.bk_map,  0, 1); //Set first two bocks as used
	bitmap_setbit(mem_superblock.bk_map,  1, 1);
	//Padding is also automaticaly initiaized to 0's

	char aux_blank_inodes[BLOCK_SIZE] = {0}; 
	
	//Writes SuperBock, no need padding because is exactly in memory 2048 bytes
	if(bwrite("disk.dat", 0, (char*)&mem_superblock) < 0) return -1;

	//Inodes Part, write a 0's block, no inode is created
	if(bwrite("disk.dat", 1, aux_blank_inodes) < 0) return -1; //Write the whole block as 0's

	return 0;
  
}

/*
 * @brief 	Mounts a file system in the simulated device.
 * @return 	0 if success, -1 otherwise.
 */
int mountFS(void)
{	
	//If it is already mounted error
	if(is_mounted) return -1;

	//Reads the superblock
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
	is_mounted = 1;

	return 0;

}

/*
 * @brief 	Unmounts the file system from the simulated device.
 * @return 	0 if success, -1 otherwise.
 */
int unmountFS(void)
{
	//Writes superbock and inode block
	//Write the two metadata bocks of the fiesystem
	//Error if the file is already mounted
	if(!is_mounted) return -1;
	//Checks open files and closes opened ones
	for(int i = 0; i < mem_superblock.in_num;++i){
		if(bitmap_getbit(file_table->is_opened, i)){
			if(closeFile(i) < 0) return -1;
		} 
	}

	//Compute inodes block
	char aux_blk[BLOCK_SIZE] = {0}; //Implicity to 0's
	memcpy(aux_blk, mem_inodes, NUM_INODES*sizeof(inode)); //Writes exisixting inodes

	//Writes SuperBock, no need padding because is exactly in memory 2048 bytes
	if(bwrite("disk.dat", 0, (char*)&mem_superblock) < 0) return -1;

	//Inodes  writing part
	if(bwrite("disk.dat", 1, aux_blk) < 0) return -1;

	//Free used memory
	free(mem_inodes);
	free(file_table);
	is_mounted = 0;

	return 0;
}

/*
 * @brief	Creates a new file, provided it it doesn't exist in the file system.
 * @return	0 if success, -1 if the file already exists, -2 in case of error.
 */
int createFile(char *fileName)
{	
	//If aready mounted returns error
	if(!is_mounted) return -1;
	if(strlen(fileName)>MAX_NAME) return -2; //If the file_name is bigger than 32 
	//Checks uniqueness
	if(nametoi(fileName) != -1) return -1; //The file already exist if nametoi finds it, if it returns error the file didn't exist
	int inode_id, indirect_id;
	//Allocates a new inode
	if((inode_id = ialloc()) < 0) return -2;
	//Allocates the indirect block
	if((indirect_id = balloc()) < 0 ){ //If cannot find a block
		ifree(inode_id); //Frees the allocated inode
		return -2;
	}

	//Fills the inode CRC
	//As it is new it does not have a CRC associated to each block, balloc has set blocks to 0 before opening, so CRC can be computed with a blank block
	int indirect_b[BLOCK_SIZE/sizeof(uint32_t)];
	int err;
	if(bread("disk.dat", indirect_id, (char*)indirect_b) < 0 )return -1;
	mem_inodes[inode_id].crc = blk_32CRC(indirect_b,0,&err);
	if(err < 0) return -1;
	mem_inodes[inode_id].indirect = indirect_id; //The block is set to 0
	//The indirect occupied blocks is not needed because it can be computed
	strncpy(mem_inodes[inode_id].name, fileName, MAX_NAME);
	return 0;
}

/*
 * @brief	Deletes a file, provided it exists in the file system.
 * @return	0 if success, -1 if the file does not exist, -2 in case of error..
 */ 
int removeFile(char *fileName)
{	
	//If the file system is not mounted error
	if(!is_mounted) return -1;
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
	for(int i = 0; i < num_blk; ++i) if(bfree(indirect[i]) < 0) return -2; //If any error return it

	//Frees indirect block
	if(bfree( mem_inodes[inode_t].indirect) < 0) return -2; //Same as above

	//Frees inode
	if(ifree(inode_t) < 0) return -2;

	return 0;
}

/*
 * @brief	Opens an existing file.
 * @return	The file descriptor if possible, -1 if file does not exist, -2 in case of error..
 */
int openFile(char *fileName)
{	
	//If the file is not mounted return error
	if(!is_mounted) return -1;
	int inode_p = nametoi(fileName);//Gets the inode associated to that name
	
	if(inode_p < 0) return -1; //The file does not exist

	//Checks the integrity of the file
	int check = checkFile(fileName);
	if(check == -1){
		printf("[ERROR] The file you're trying to open is corrupted. Aborting.\n");
		return -2;
	}
	if(check == -2){
		printf("[ERROR] The file you're trying cannot be checked. Aborting. Please try again.\n");
		return -2;
	}

	//Searchs the file inside allocated inodes
	if(bitmap_getbit(file_table->is_opened, inode_p)) return -2; //The file is already opened
	bitmap_setbit(file_table->is_opened,inode_p,1); //Set the file as opened
	file_table->file_pos[inode_p] = 0; //Sets the file pointer to 0
	
	//Returns the file descriptor
	return inode_p;
}

/*
 * @brief	Closes a file.
 * @return	0 if success, -1 otherwise.
 */
int closeFile(int fileDescriptor)
{	
	//If the system is not mounted error
	if(!is_mounted) return -1;
	if(fileDescriptor < 0 || fileDescriptor > 40) return -1;
	if(!bitmap_getbit(file_table->is_opened, fileDescriptor)) return -1; //The file is already closed
	//No need to put the pointer to 0 because it is initializated as 0
	int indirect[BLOCK_SIZE/sizeof(uint32_t)];
	//Getting the indirect block
	if(bread("disk.dat", mem_inodes[fileDescriptor].indirect, (char*) indirect) < 0) return -1;
	int err;
	//Updating CRC
	uint32_t _crc = blk_32CRC(indirect, numblocks(mem_inodes[fileDescriptor].size), &err);
	if(err < 0) return -1;
	
	mem_inodes[fileDescriptor].crc = _crc;

	bitmap_setbit(file_table->is_opened,fileDescriptor,0); //Set the file as closed 
	return 0;
} 

/*
 * @brief	Reads a number of bytes from a file and stores them in a buffer.
 * @return	Number of bytes properly read, -1 in case of error.
 */
/*
 * @brief	Reads a number of bytes from a file and stores them in a buffer.
 * @return	Number of bytes properly read, -1 in case of error.
 */ 
int readFile(int fileDescriptor, void *buffer, int numBytes)
{
	if(fileDescriptor < 0 || fileDescriptor > 40) return -1; //Check if fileDescriptor is valid
	if(!is_mounted) return -1; //If not mounted error
	if(!bitmap_getbit(file_table->is_opened,fileDescriptor)) return -1; //Check if file is opened

	if(numBytes < 0) return -1;//Check valid number of bytes

 
	//Prepare structures for reading
	//If size to read is greater than size of the file, just read until the end of the file
	int to_read, read_bytes = 0, to_copy;
	if(file_table->file_pos[fileDescriptor] + numBytes > mem_inodes[fileDescriptor].size) to_read = mem_inodes[fileDescriptor].size - file_table->file_pos[fileDescriptor];
	else to_read = numBytes; //Otherwise, read all the bytes

	//Check if there are bytes to read
	if(to_read == 0) return 0; //This is not strictly needed but avoids creating inecesary structures 

	//Read inode indirect block
	int indirect[BLOCK_SIZE/sizeof(uint32_t)];
	if(bread("disk.dat", mem_inodes[fileDescriptor].indirect ,(char*)indirect) < 0) return -1;

	//Get the starting position
	int st_bk = file_table->file_pos[fileDescriptor]/BLOCK_SIZE;
	int offset = file_table->file_pos[fileDescriptor] % BLOCK_SIZE;

	//Read the first block
	char aux_blk[BLOCK_SIZE] =  {0};
	if(bread("disk.dat", indirect[st_bk], aux_blk)< 0) return 0; //0 Bytes read

	//Copy bytes to buffer and update variable
	if(to_read + offset > BLOCK_SIZE) to_copy = BLOCK_SIZE - offset;
	else to_copy = to_read;

	memcpy(buffer, (aux_blk+offset), to_copy);
	read_bytes += to_copy; 
	to_read -= to_copy;

	//Copy the rest of the blocks (if needed)
	while(to_read > 0){

		//Computing remaining bytes of THE NEXT BLOCK
		if(to_read > BLOCK_SIZE) to_copy = BLOCK_SIZE;
		else to_copy = to_read;

		//Reading next block
		if(bread("disk.dat", indirect[++st_bk], aux_blk) < 0) break; //Stop prepaturely the loop in case of error to update variables until this point

		//Copy memory
		memcpy(((char*)buffer + read_bytes), aux_blk, to_copy);

		//Update variables
		read_bytes += to_copy;
		to_read -= to_copy;

	}

	//Update seek pointer
	file_table->file_pos[fileDescriptor] += read_bytes;
	//Return read bytes
	return read_bytes;
}
/*
 * @brief	Writes a number of bytes from a buffer and into a file.
 * @return	Number of bytes properly written, -1 in case of error.
 */
int writeFile(int fileDescriptor, void *buffer, int numBytes)
{	
	if(!is_mounted) return -1; //If not mounted error
	//Check file descriptor 
	if(fileDescriptor < 0 || fileDescriptor > 40) return -1;
	//First, we have to check if the file to be written is opened or not.
	if(!bitmap_getbit(file_table->is_opened, fileDescriptor)) return -1;

	if(numBytes < 0) return -1;//Check valid number of bytes

	//Now we prepare parameters in order to write in the current file pointer

	int file_blks = numblocks(mem_inodes[fileDescriptor].size);

	//Get the starting position
	int st_bk = file_table->file_pos[fileDescriptor]/BLOCK_SIZE;
	int offset = file_table->file_pos[fileDescriptor] % BLOCK_SIZE;

	int to_write, written_bytes = 0, to_copy;
	if(file_table->file_pos[fileDescriptor] + numBytes > MAX_FILE_SIZE) to_write = mem_inodes[fileDescriptor].size - file_table->file_pos[fileDescriptor];
	else to_write = numBytes; //Otherwise, read all the bytes

	if(to_write == 0) return 0;

	//end of parameters preparation

	//Read inode indirect block
	int indirect[BLOCK_SIZE/sizeof(uint32_t)];
	if(bread("disk.dat", mem_inodes[fileDescriptor].indirect ,(char*)indirect) < 0) return -1;

	//Read the first block
	char aux_blk[BLOCK_SIZE] =  {0};

	//Copy bytes to buffer and update variable
	if(to_write + offset > BLOCK_SIZE) to_copy = BLOCK_SIZE - offset;
	else to_copy = to_write;

	//Gets the iteration block
	int it_blk;
	if(st_bk >= file_blks){ //If need allocation it does it
		if((it_blk = balloc()) < 0 ) return 0;
		indirect[st_bk] = it_blk;
	}
	else 
	{ 
		it_blk = indirect[st_bk]; //If it exists it reads it to write above
		if(bread("disk.dat", it_blk, aux_blk)< 0) return 0;
	}

	//Copies the number ob bytes to the block and writes it
	memcpy(aux_blk+offset, buffer, to_copy);
	if(bwrite("disk.dat", it_blk, aux_blk) < 0) {
		if(it_blk >= file_blks) bfree(it_blk);
		return 0;
	}
	//Updates variables
	written_bytes += to_copy;
	to_write -= to_copy;

	//If need to write more
	while( to_write > 0){
 
		//Copy bytes to buffer and update variable
		if(to_write  > BLOCK_SIZE) to_copy = BLOCK_SIZE;
		else to_copy = to_write;

		int it_blk;
		if(++st_bk >= file_blks){ //Check if the block exists, if no allocate, else read the block
			if((it_blk = balloc()) < 0 ) break;
			indirect[st_bk] = it_blk;
			bzero(aux_blk, BLOCK_SIZE);
		} 
		else 
		{
			it_blk = indirect[st_bk];
			if(bread("disk.dat", it_blk, aux_blk)< 0) break;
		} 

		//Copy the bytes to the block
		memcpy(aux_blk, (char*)(buffer) + written_bytes, to_copy);
		if(bwrite("disk.dat", it_blk, aux_blk) < 0) {
			if(it_blk >= file_blks) bfree(it_blk);
			break;
		}
		//Update variables
		written_bytes += to_copy;
		to_write -= to_copy;
	} 

	//Compute new metadata and writes the new indirect block
	int aux_size = file_table->file_pos[fileDescriptor] + written_bytes - mem_inodes[fileDescriptor].size;
	mem_inodes[fileDescriptor].size = (aux_size > 0)? aux_size + mem_inodes[fileDescriptor].size : mem_inodes[fileDescriptor].size;

	file_table->file_pos[fileDescriptor] += written_bytes;
	if(bwrite("disk.dat", mem_inodes[fileDescriptor].indirect ,(char*)indirect) < 0) return -1;
	return written_bytes;
}
  

/*
 * @brief	Modifies the position of the seek pointer of a file.
 * @return	0 if succes, -1 otherwise.
 */ 
int lseekFile(int fileDescriptor, long offset, int whence)
{	
	if(!is_mounted) return -1;
	//Error if invalid file descriptor or the file is closed
	if(fileDescriptor < 0 || fileDescriptor > 40 || !bitmap_getbit(file_table->is_opened,fileDescriptor) ) return -1;
	int aux_p;
	switch(whence){
		
		case FS_SEEK_BEGIN:
			file_table->file_pos[fileDescriptor] = 0; //If seek beguinputs the pointer to 0 
			break;
		case FS_SEEK_END:
			file_table->file_pos[fileDescriptor] = mem_inodes[fileDescriptor].size; //If seek to end, sets at the size of the file (which in fact is the place to write
			break;
		case FS_SEEK_CUR: 
			aux_p = file_table->file_pos[fileDescriptor] + offset; //Gets the new pointer
			if(aux_p < 0 || aux_p > mem_inodes[fileDescriptor].size) return -1; //If the position is not valid returns error
			file_table->file_pos[fileDescriptor] = aux_p; //Sets the pointer
			break;
		default:
			return -1; //if option is not correct return error

	}

	return 0; //Return success

}
 
 
/*
 * @brief 	Verifies the integrity of a file.
 * @return 	0 if the file is correct, -1 if the file is corrupted, -2 in case of error.
 */
int checkFile(char *fileName) 
{
	if(!is_mounted) return -2;
	int inode_id = nametoi(fileName);
	if(inode_id < 0 || bitmap_getbit(file_table->is_opened, inode_id)) return -2; //If it does not exist or it is opened, return error

	//Read indirect from inode
	int indirect[BLOCK_SIZE/sizeof(uint32_t)];
	if(bread("disk.dat", mem_inodes[inode_id].indirect,(char*)indirect) < 0) return -2; //Return error if indirect block cannot be read

	//compute crc of blocks
	int err;
	uint32_t b_crc = blk_32CRC(indirect, numblocks(mem_inodes[inode_id].size), &err);
	if(err < 0) return -2; //Return error computing CRC

	//Comparing CRC
	if(b_crc == mem_inodes[inode_id].crc) return 0; //File not corrupted
	else return -1; //File corrupted
}

