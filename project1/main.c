//Author: Evan McKenzie
//Program: Redefined Main for Hmalloc
//Class: CS 3411

#include "hmalloc.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
/*You may include any other relevant headers here.*/


/*	main()
 *	Use this function to develop tests for hmalloc. You should not 
 *	implement any of the hmalloc functionality here. That should be
 *	done in hmalloc.c
 *	This file will not be graded. When grading I will replace main 
 *	with my own implementation for testing.*/
int main(int argc, char *argv[]){
	
	void* ptr = hmalloc(32);				//save the return of hmalloc, as this is our key to reference that area again
	
	void* ptr2 = hmalloc(40);
	
	void *ptr3 = hmalloc(33);

	void* ptr5 = hmalloc(10);

	hfree(ptr);			//free the pointer to the 32 space
			
	hfree(ptr2);			//free the pointer to the 40 space

	hfree(ptr5);			//free the pointer to the 10 space

	//hfree(ptr2);

	//hfree(ptr3);
	
	void* ptr4 = hmalloc(16);			//call malloc, this will take 40 out of the freelist and replace the allocated space with 16

	hfree(ptr3);			//free the pointer to the 33 space

	hfree(ptr4);			//free the pointer to the new 16 space

	traverse();			//traverse in this instance should print 16, 33, 10, and 32 IN THAT ORDER

	return 1;
}
