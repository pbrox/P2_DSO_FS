/*
 * OPERATING SYSTEMS DESING - 16/17
 *
 * @file 	test.c
 * @brief 	Implementation of the client test routines.
 * @date	01/03/2017
 */

#include <stdio.h>
#include <string.h>
#include "include/filesystem.h"


// Color definitions for asserts
#define ANSI_COLOR_RESET   "\x1b[0m"
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_BLUE   "\x1b[34m"

#define N_BLOCKS	25						// Number of blocks in the device
#define DEV_SIZE 	N_BLOCKS * BLOCK_SIZE	// Device size, in bytes


int main() {
	int ret;
	
	//Create disk
	ret = mkFS(DEV_SIZE);
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST mkFS 02", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST mkFS 02", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
 
	//Mount not possible when already mounted
	ret = mountFS(); 
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST mountFS 01", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST mountFS 01", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
	
	ret = mountFS(); 
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST mountFS 02", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST mountFS 02", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
	
	//Unmount not possible when already unmounted
	ret = unmountFS(); 
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST unmountFS 01", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST unmountFS 01", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
	
	ret = unmountFS(); 
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST unmountFS 02", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST unmountFS 02", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
	
	//Name of more than 32 bytes
	ret = createFile("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST createFile 01", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST createFile 01", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
	
	//Name of exactly 32 bytes
	ret = createFile("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST createFile 02", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST createFile 02", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
	
	//Creating an already existing file
	ret = createFile("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
	if(ret != 0) {	
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST createFile 03", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST createFile 03", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

	//Open a non-existing file
	ret = openFile("test.txt"); 
	if(ret < 0) {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST openFile 01", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST openFile 01", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
	
	//Open an already opened file + open a valid file (first case)
	ret = openFile("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"); 
	if(ret < 0) {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST openFile 02", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST openFile 02", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
	
	ret = openFile("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"); 
	if(ret < 0) {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST openFile 03", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST openFile 03", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
	
	//Unmount when there are opened files
	ret = unmountFS(); 
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST unmountFS 03", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST unmountFS 03", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
	
	//Mount again so that we can check the remaining test cases
	mountFS(); 
	
	//Remove a non-existing file
	ret = removeFile("test.txt"); 
	if(ret < 0) {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST removeFile 01", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST removeFile 01", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
	
	//Remove an opened file
	ret = removeFile("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"); 
	if(ret < 0) {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST removeFile 02", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST removeFile 02", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
	
	//Close a file with an invalid file descriptor
	ret = closeFile(-1); 
	if(ret < 0) {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST closeFile 01", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST closeFile 01", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
	
	//Close an already closed file
	ret = closeFile(0); 
	if(ret < 0) {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST closeFile 02", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST closeFile 02", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
	
	ret = closeFile(0); 
	if(ret < 0) {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST closeFile 03", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST closeFile 03", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
	
	//Open the file we will be using for the next test cases
	openFile("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"); 
	
	char aBuff[10479616];
	
	//Read 0 bytes
	ret = readFile(0, aBuff, 0);
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST readFile 01", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST readFile 01", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
	
	//Read a negative number of bytes
	ret = readFile(0, aBuff, -1);
	if(ret < 0) {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST readFile 02", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST readFile 02", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
	
	//Read a closed file
	closeFile(0); 
	
	ret = readFile(0, aBuff, 100);
	if(ret < 0) {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST readFile 03", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST readFile 03", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
	
	//Open again the file
	openFile("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"); 
	
	//Write 0 bytes
	ret = writeFile(0, aBuff, 0);
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST writeFile 01", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST writeFile 01", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
	
	//Write a negative number of bytes
	ret = writeFile(0, aBuff, -1);
	if(ret < 0) {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST writeFile 02", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST writeFile 02", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
	
	//Write a closed file
	closeFile(0); 
	
	//Write 0 bytes
	ret = writeFile(0, aBuff, 100);
	if(ret < 0) {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST writeFile 03", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST writeFile 03", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
	
	//Open again the file
	openFile("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"); 
	
	//Fill out the buffer
	int it = 0;
	while (it < 10479616){
		aBuff[it]= 'a';
		++it;
	}
	
	//Write on a newly created file + occupy all the available disk space + write a file of 1MB (max size)
	ret = writeFile(0, aBuff, 1048576);
	if(ret != 1048576) {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST writeFile 04", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST writeFile 04", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
	
	//Create a new file when all the disk's size is full
	ret = createFile("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaab");
	if(ret < 0) {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST createFile 04", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST createFile 04", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
	
	//Read more than the size of the file + check size
	ret = readFile(0, aBuff, 1048577);
	if(ret != 1048576) {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST readFile 04", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST readFile 04", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
	
	//Write when the disk is full
	ret = writeFile(0, aBuff, 1);
	if(ret < 0) {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST writeFile 05", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST writeFile 05", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
	
	//Non-existing whence
	ret = lseek(0, 0, 355);
	if(ret != 1) {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST lseek 01", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST lseek 01", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
	
	//Check CUR
	ret = lseek(0, 100, FS_SEEK_CUR);
	if(ret < 0) {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST lseek 02", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST lseek 02", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
	
	//Read when file pointer is not 0 and less than a block
	ret = readFile(0, aBuff, 100);
	if(ret != 100) {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST readFile 05", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "readFile 05", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
	
	//Read the whole file with several calls
	ret = readFile(0, aBuff, 1048476);
	if(ret != 11048476) {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST readFile 06", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "readFile 06", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
	
	removeFile("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
	createFile("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
	openFile("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
	
	lseek(0, 100, FS_SEEK_CUR);
	
	//Write with displaced file pointer
	ret = writeFile(0, aBuff, 100);
	if(ret != 100) {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST writeFile 06", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST writeFile 06", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
	
	//Check BEGIN
	ret = lseek(0, 0, FS_SEEK_BEGIN);
	if(ret < 0) {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST lseek 02", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST lseek 02", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
	
	//Check END
	ret = lseek(0, 0, FS_SEEK_END);
	if(ret < 0) {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST lseek 03", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST lseek 03", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
	
	//Check non-existing file
	ret = checkFile("a");
	if(ret < 0) {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST checkFile 01", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST checkFile 01", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
	
	//Check non-existing file
	ret = checkFile("a");
	if(ret < 0) {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST checkFile 01", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST checkFile 01", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

	return 0;
}
