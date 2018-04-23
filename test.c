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

#define N_BLOCKS	27						// Number of blocks in the device
#define DEV_SIZE 	N_BLOCKS * BLOCK_SIZE	// Device size, in bytes


int main() {
	int ret;

	///////




	ret = mkFS(DEV_SIZE);
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST mkFS ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST mkFS ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

	///////

	ret = mountFS(); 
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST mountFS ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST mountFS ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

	///////

	ret = createFile("test.txt");
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST createFile ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST createFile ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

	/////// 
	ret = openFile("test.txt"); 
	if(ret < 0) {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST createFile ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST createFile ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

	

	char wrBff[]={'a'};
	int bytes3 = writeFile(ret, wrBff, 0);
	printf("%d\n", bytes3);

	char wrBff2[51300]={0};
	 bytes3 = writeFile(ret, wrBff2, 51300);
	 printf("%d\n", bytes3);
/*
	///////
	//Escribe hasta "uta cursus" -->1965
	char * to_write_bff = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Morbi vitae porta nisi. Etiam urna enim, placerat quis tellus quis, consectetur accumsan ligula. Maecenas ultrices orci id turpis tempus fringilla. Pellentesque viverra tincidunt turpis, non consequat mi laoreet ultrices. Cras massa elit, iaculis a augue et, egestas mattis tellus. Etiam eleifend lectus eu diam iaculis, ut lobortis mi vehicula. Nam ut lacinia justo. Mauris ac massa ex. Phasellus faucibus tristique neque, vel fermentum augue dignissim eu. Integer non erat venenatis, rhoncus dui vitae, convallis neque. In egestas aliquet nisl, eu pellentesque dolor mollis vitae.\nNulla consectetur, metus ac elementum rhoncus, metus purus eleifend libero, eu iaculis enim magna id lorem. Donec eget laoreet ex. Vestibulum vitae leo magna. Pellentesque eget neque dui. Donec neque metus, blandit ut dignissim non, semper vitae libero. Aenean accumsan egestas magna. Ut a cursus metus, eget convallis ex. Mauris commodo mauris sed lorem cursus, vitae tristique metus. P2:Lorem ipsum dolor sit amet, consectetur adipiscing elit. Morbi vitae porta nisi. Etiam urna enim, placerat quis tellus quis, consectetur accumsan ligula. Maecenas ultrices orci id turpis tempus fringilla. Pellentesque viverra tincidunt turpis, non consequat mi laoreet ultrices. Cras massa elit, iaculis a augue et, egestas mattis tellus. Etiam eleifend lectus eu diam iaculis, ut lobortis mi vehicula. Nam ut lacinia justo. Mauris ac massa ex. Phasellus faucibus tristique neque, vel fermentum augue dignissim eu. Integer non erat venenatis, rhoncus dui vitae, convallis neque. In egestas aliquet nisl, eu pellentesque dolor mollis vitae.\nNulla consectetur, metus ac elementum rhoncus, metus purus eleifend libero, eu iaculis enim magna id lorem. Donec eget laoreet ex. Vestibulum vitae leo magna. Pellentesque eget neque dui. Donec neque metus, blandit ut dignissim non, semper vitae libero. Aenean accumsan egestas magna. Ut a cursus";
	int bytes = writeFile(ret, to_write_bff, strlen(to_write_bff));
	printf("%d %zu\n", bytes, strlen(to_write_bff));

	/////// 
	int rit = lseekFile(ret,0,FS_SEEK_BEGIN); 
	if(rit != 0) { 
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST createFile ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1; 
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST createFile ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
	char readBff[5000]; 
	bytes = readFile(ret,readBff,5000);
	printf("%d\n%.*s\n", bytes, bytes, readBff); 

	///////
    bytes = writeFile(ret, to_write_bff, strlen(to_write_bff));
	printf("W_%d %zu\n", bytes, strlen(to_write_bff));
	///////
	/////// 
	rit = lseekFile(ret,0,FS_SEEK_BEGIN); 
	if(rit != 0) {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST createFile ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1; 
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST createFile ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
	char readBff2[11000]; 
	bytes = readFile(ret,readBff2,11000);
	printf("B_%d\n %.*s\n", bytes, bytes, readBff2); 
	
	 rit = lseekFile( ret, -50 ,FS_SEEK_CUR);
	char * to_write_bff_aux = "!!!345678901234567890123456!!!";
	bytes = writeFile(ret, to_write_bff_aux, strlen(to_write_bff_aux));
	 rit = lseekFile( ret, -50 ,FS_SEEK_BEGIN);
	bytes = readFile(ret,readBff2,11000);
	printf("B_%d\n %.*s\n", bytes, bytes, readBff2); 

	*//*//escribir 0
	bytes = writeFile(ret, to_write_bff, 0);
	rit = lseekFile(ret,0,FS_SEEK_BEGIN); 
	bytes = readFile(ret,readBff2,11000);
	printf("B_%d\n %.*s\n", bytes, bytes, readBff2);

	ret = createFile("kaka.txt");
	ret = openFile("kaka.txt");
	char leBff[1047576]={0};
	bytes = writeFile(ret, leBff, 1047576);
	printf(" 21bytes: %d\n",bytes);
	char oneBff[]={'a'};
	int aux = 0;
	do{
		bytes = writeFile(ret, oneBff, 1);
		aux += bytes;
		printf("%d\n bytes: %d\n",aux,bytes);
	}while(bytes!=0);
	printf("%d\n",aux );

	rit = lseekFile(ret,0,FS_SEEK_BEGIN); 
	char oneBff2[1050000];
	bytes = readFile(ret , oneBff2, 1050000);
	printf("!!%d\n%.*s",bytes, bytes, oneBff2 + 1047576);
	*/


	///////
	int rot = closeFile(ret);
	if(rot != 0) {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST createFile ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST createFile ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
	//////

	ret = unmountFS();
	if(ret != 0) {
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST unmountFS ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST unmountFS ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

	/////// 


	return 0;
}
