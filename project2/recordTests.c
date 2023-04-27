#include "recordio.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

/* You can use this main function to implement tests that utilize
 * the recordio functions as you implement. This file will not be
 * graded. 
 * "make recordio" will compile the recordio library into a .o file,
 * but it will also produce an executable named recordio which uses
 * the main function declared here.
 */
int main(int argc, char *argv[]){
	int d;

	if((d = rio_open(argv[1], O_RDWR | O_CREAT, 0600)) < 0)
	{
		printf("you made a mistake bucko\n");
	}
	else
	{
		printf("open worked\n");
	}

	char* buff;
	int e;

	
	if((e = rio_lseek(d, 0, SEEK_SET)) == -1)
	{
		printf("you made a mistake bucko pt 2\n");
	}
	else
	{
		printf("lseek worked\n");
	}
	
	printf("%d \n", e);
	buff = rio_read(d, &e);
	
	int length1 = strlen(buff);
        char buffer1[length1 -1];
        strcpy(buffer1, buff);

	printf("%s\n", buffer1);

	buff = rio_read(d, &e);

	int length2 = strlen(buff);
	char buffer2[length2 -1];
	strcpy(buffer2, buff);
	
	printf("%s\n", buffer2);
	
	int out;
	if( (out = rio_close(d)) == 0)
	{
		printf("close worked\n");
	}
	return 0;
}
