//Author: Evan McKenzie
//Class: CS 3411
//Project: Record I/O
//Date Created: 10/9/21
//Last Edit: 10/17/21


#include "recordio.h"
#include <sys/stat.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

/* You must use this struct when constructing records. It is
 * expected that all binary data in the index file will follow
 * this format. Do not modify the struct.
 */
struct record_descriptor
{
	int position; //byte offset relative to the beginning of the data file
	int length;   //length of record in bytes
};

/* rio_open
 * Open data file and index file. If create is requested, open both 
 * the data file and the index file with O_CREAT. If O_RDONLY or
 * O_RDWR is requested, make sure that the index file is present and 
 * return an error otherwise. On success the data file's descriptor
 * should be returned and the file descriptor for the index file
 * must be maintained within the abstraction.
 */
int rio_open(const char *pathname, int flags, mode_t mode){
	
	int data;			//data file descriptor
	int index;			//index file descriptor
	
	char str[] = ".rinx.";

	char* buffer = malloc(strlen(str) + strlen(pathname));
	strcpy(buffer, str);
	strcat(buffer, pathname);


	if((flags & O_CREAT) == O_CREAT)		//if we need to create these files
	{
		data = open(pathname, O_CREAT|O_RDWR, mode);
		index = open(buffer, O_CREAT|O_RDWR,  mode);			//call create to make the data and index file
	}
	else if(((flags & O_RDONLY) == O_RDONLY) | ((flags & O_RDWR) == O_RDWR))		//else if the files already exit
	{
		data = open(pathname, flags, mode);
		if((index = open(buffer, flags, mode)) < 0)		//open the index and data files
		{
			return -1;
		}
	}

	if((data == -1) | (index == -1))		//eror checking
	{
		return -1;
	}

	int fd = (data << 16)|(index);			//pack both file descriptors into a single integer

	return fd; 		//return packed integer
}

/* rio_read
 * Allocate a buffer large enough to hold the requested record, read 
 * the record into the buffer and return the pointer to the allocated 
 * area. The I/O result should be returned through the return_value 
 * argument. On success this will be the number of bytes read into
 * the allocated buffer. If any system call returns an error, this
 * should be communicated to the caller through return_value.
 */
void *rio_read(int fd, int *return_value){

	int data = (fd >> 16) & 0xFFFF;		//unpack the file descriptors
	int index = fd & 0xFFFF;

	struct record_descriptor temp;		//temp struct object
	int wc;
	//printf("one\n");
        if((wc = read(index, &temp, sizeof(struct record_descriptor)) < 0))		//read current entry from index file
	{
		return (void*)-1;
	}
	//printf("length: %d\n", temp.length);
	//printf("position: %d\n", temp.position);
	//printf("error: %d\n", errno);
	int numofbytes = temp.length;			//get the length of the current record

	void* buffer = malloc(numofbytes+1);		//allocate space for the given record

	read(data, buffer, numofbytes+1);	//read into buffer
	//printf("%d reader\n", reader);
	return buffer;		//return character string from data file
}

/* rio_write
 * Write a new record. If appending to the file, create a record_descriptor 
 * and fill in the values. Write the record_descriptor to the index file and 
 * the supplied data to the data file for the requested length. If updating 
 * an existing record, read the record_descriptor, check to see if the new 
 * record fits in the allocated area and rewrite. Return an error otherwise.
 */
int rio_write(int fd, const void*buf, int count){

	int data = (fd >> 16) & 0xFFFF;				//unpack the file descriptors
	int index = fd & 0xFFFF;

	struct record_descriptor current;			//temp struct object

	int write1;		//return value variable
	
	int indexnext = read(index, &current, sizeof(struct record_descriptor));		//read current entry from index file
	if(indexnext == -1)
	{
		return -1;
	}

	int one = lseek(data, 0, SEEK_CUR);				//get bytes to current pointer
	int two = lseek(data, 0, SEEK_END);

	if(one == two)			//if these two are equal, we are already at the EOF
	{
		int seek;
		seek = lseek(data, 0, SEEK_END);			//set current I/O op to end of file

		if(seek != -1)
		{
			current.length = count;		//creat new record descriptor
			current.position = seek;		//set offset position

			write1 = write(data, buf, count);			//write to the data file with the given string
			write(index, &current, sizeof(struct record_descriptor));	//write the created file descriptor to the index file
		}
		else
		{
			return -1;
		}
	}
	else			//we are overwriting a record
	{
		if(current.length >= count)			//check whether our new record will fit
		{
			int overwrite = current.length;
			current.length = count;			//if it will, reset the length value

			int dataoffset = lseek(data, current.position, SEEK_SET);		//seek the data file to the record we just adjusted
		        write1 = write(data, buf, overwrite);			//overwrite the old record with the new

			if( (dataoffset == -1) )
			{
				return -1;
			}	
		}
		else			//error handling
		{
			return -1;
		}
	}

	return write1;			//return the number of bytes written
}

/* rio_lseek
 * Seek both files (data and index files) to the beginning of the requested 
 * record so that the next I/O is performed at the requested position. The
 * offset argument is in terms of records not bytes (relative to whence).
 * whence assumes the same values as lseek whence argument.
 *
 */
int rio_lseek(int fd, int offset, int whence){
	
	int data = (fd >> 16) & 0xFFFF;				//unpack file descriptors
        int index = fd & 0xFFFF;

	int one = lseek(index, (offset* 8), whence);			//go to requested offset in index file
	//int one = lseek(index, offset, whence);
	//printf("%d one\n", one);
	struct record_descriptor temp;			//temp struct object

	int two = read(index, &temp, (1* sizeof(struct record_descriptor)));		//read struct object from index file
	//int two = read(index, &temp, 1);

	//printf("%d position\n", temp.position);
	//printf("%d length\n", temp.length);
	
	int three = lseek(data, temp.position, SEEK_SET);				//pointer to data offset
	                //printf("%d error\n", errno);

	one = lseek(index, (offset*8), whence);			//reset back to the before the read

	if( (one == -1) | (two == -1) | (three == -1))
	{
		//printf("%d error\n", errno);					//erro checking
		//printf("failed\n");
		return -1;
	}

	return one; 			//return index offset in terms of record
}

/* rio_close
 * Close both files. Even though a single integer is passed as an argument, 
 * your abstraction must close the other file as well. It is suggested 
 * in rio_open that you return the descriptor obtained by opening the data file 
 * to the user and keep the index file descriptor number in the 
 * abstraction and associate them. You may also shift and pack them together 
 * into a single integer.
 */
int rio_close(int fd){
	
	int data = (fd >> 16) & 0xFFFF;			//unpack the file descriptors
	int index = fd & 0xFFFF;

	if( close(data) < 0)		//close data file
	{
		return -1;
	}
	if( close(index) < 0)		//close index file
	{
		return -1;
	}
	return 0; 
}
