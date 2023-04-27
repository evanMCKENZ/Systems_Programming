#include "recordio.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>


/* Develop a test program called testio which includes recordio.h and 
 * is linked against recordio.o. This program should expect a single 
 * argument which supplies a file name. The program should rio_open 
 * the supplied argument and report any errors to the user. Upon a 
 * successful open it should execute a series of rio_read statements, 
 * read the file one record at a time and write each record to the 
 * standard output as shown below.
 *
 * Data File                          Index file
 * ---------------------------------------------------
 *  Systems                            0, 8
 *  programming is cool.               8, 20
 *
 * "make testio" will compile this souce into testio
 * To execute your program: ./testio <some_record_file_name>
 */
int main(int argc, char *argv[]){
	
        int fd;		//global file descriptor

        if((fd = rio_open(argv[1], O_RDWR | O_CREAT, 0600)) < 0)			//open both files
        {
		return -1;
                //printf("you made a mistake bucko\n");
        }

	
        char* buff;			//global buffer and lseek argument variables
       int e;
       int count = 0;

        printf("Data File Records\n");                          //standard output header
        printf("------------------------------------------------------------\n");

	while((e = rio_lseek(fd, count, SEEK_SET)) != -1)
	{
		buff = rio_read(fd, &e);

		if(strlen(buff) == 0)
		{
			return 0;
		}
		else
		{
                	printf("%s", buff);
                //int f = rio_lseek(fd, count, SEEK_SET);
                //buff = rio_read(fd, &f);
                	count = count + 1;
		}
	}

/*
        if((e = rio_lseek(fd, 0, SEEK_SET)) == -1)		//seek to the start of both files
        {
		return -1;
                //printf("you made a mistake bucko pt 2\n");
        }


	printf("Data File Records\n");				//standard output header
	printf("------------------------------------------------------------\n");
        
	buff = rio_read(fd, &e);			//read first record from data file
	//int position = 0;				//printed position
	int count = 1;					//current offset in index file
	while((strlen(buff) != 0))
	{
		        //buff[strlen(buff)] = '\0';
		
		printf("%s", buff);
		int f = rio_lseek(fd, count, SEEK_SET); 
		buff = rio_read(fd, &f);
		count = count + 1;
	}

	/*
        buff = rio_read(d, &e);
	buff[strlen(buff)] = '\0';
        //int length1 = (strlen(buff)-1);
        //char buffer1[length1-1];
        //strcpy(buffer1, buff);
	
	int length1 = strlen(buff)-1;

        printf("%s                  %d,%d\n", buff, e, length1);

	f = rio_lseek(d, 1, SEEK_SET);
        buff2 = rio_read(d, &f);
	
	//buff2[strlen(buff2)-1] = '\0';
        //int length2 = (strlen(buff2)-1);
        //char buffer2[length2 -1];
        //strcpy(buffer2, buff2);
	
	int length2 = strlen(buff2) -1;

        printf("%s                  %d,%d\n", buff2, length1, length2);

        int out;
        if( (out = rio_close(d)) == 0)
        {
		return -1;
                //printf("close worked\n");
        }
	*/
        return 0;

}
