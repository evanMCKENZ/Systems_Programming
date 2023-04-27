//Author: Evan McKenzie
//Program: HMalloc Implementation
//Class: CS 3411
//Purpose: Rewrite the fundamental malloc() and free() calls with our own implementation

#include "hmalloc.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
/*You may include any other relevant headers here.*/


/*Add additional data structures and globals here as needed.*/
void *free_list = NULL;
int freelistSize = 0;

/* traverse
 * Start at the free list head, visit and print the length of each
 * area in the free pool. Each entry should be printed on a new line.
 */
void traverse()
{
	if(free_list != NULL)				//check that we do have entries to print
	{
		int linkervalue;			//link value of current node
		void* temp = free_list;		//pointer to freelist head
		int i = 0;			//local counter
		do{
			linkervalue = *(int*)(temp + 4);			//get link value of current node
			printf("Index:%d, Address: %08x, Length: %d\n", i, temp, *(int*)(temp));		//print formatted information about current node
                        temp = temp + *(int*)(temp + 4);		//move to the next node
			i++;		//increase our counter
		}while(linkervalue != 0);		//exit condition
	}

   /* Printing format:
	 * "Index: %d, Address: %08x, Length: %d\n"
	 *    -Index is the position in the free list for the current entry. 
	 *     0 for the head and so on
	 *    -Address is the pointer to the beginning of the area.
	 *    -Length is the length in bytes of the free area.
	 */
}

/* hmalloc
 * Allocation implementation.
 *    -will not allocate an initial pool from the system and will not 
 *     maintain a bin structure.
 *    -permitted to extend the program break by as many as user 
 *     requested bytes (plus length information).
 *    -keeps a single free list which will be a linked list of 
 *     previously allocated and freed memory areas.
 *    -traverses the free list to see if there is a previously freed
 *     memory area that is at least as big as bytes_to_allocate. If
 *     there is one, it is removed from the free list and returned 
 *     to the user.
 */
void *hmalloc(int bytes_to_allocate)
{
	if(free_list == NULL)			//check if freelist is empty
	{					//if it is, we need to call sbrk() to extend the program break and create more memory
		void *ptr = sbrk(bytes_to_allocate + 8);		//Add 8 bytes for the storage of information and metadata
		*(int*)(ptr) = bytes_to_allocate;		//store length in first word
		*(int*)(ptr + 4) = 0;				//store link value in second word
		return (ptr + 8);			//return pointer to the start of the allocated area
	}
	else		//if the freelist is not empty, we need to look for an element that call hold it 
	{
		void* temp = free_list;		//get pointer to freelist head
		void* next = NULL;			//create pointer to the next node
		int linkvalue;		//local link value container
		do		//do-while
		{
			int bytevalue = *(int*)(temp);			//set size of allocated space
			linkvalue = *(int*)(temp + 4);				//get link value of current node
			if( bytevalue >= bytes_to_allocate)		//size comparison
			{
				//int linkvalue = *(int*)(temp + 4);
				if(next != NULL)			//check if we have a next node in our freelist
				{
					*(int*)(next + 4) += linkvalue;				//add the link value of the current node to the next node in the freelist
				}								//this essentially frees the memory as there is no link value pointing to it
				else
				{
					free_list += linkvalue;
					if(linkvalue == 0)				//check if free list has only one element
					{
						free_list = NULL;			//when we remove our one element, traversal prints nothing
					}
				}
				//temp = temp + linkvalue				//addition for negative offset values
				*(int*)(temp) = bytes_to_allocate;
				return (temp + 8);				//return pointer to start of allocated area
			}
			/*else
			{
				temp += linker;
			}
			*/
			
			next = temp;		//previous node equal to current
			temp += linkvalue;		//move to next node

		}while( linkvalue != 0);
		
		if( *(int*)free_list >= bytes_to_allocate)				//if statement catches last element in free list
		{
			free_list = temp + 8;					//set new freelist head
			*(int*)(free_list) = bytes_to_allocate;				//set length of allocated area
			return temp;		//return freelist head
		}

		void* ptr = sbrk(bytes_to_allocate + 8);		//default case for when all elements in free list cannot hold new hmalloc call
		*(int*)(ptr) = bytes_to_allocate;		//set length
		*(int*)(ptr + 4) = 0;				//no offset as not freed
		return (ptr + 8);		//return new allocated area
	}
}

/* hcalloc
 * Performs the same function as hmalloc but also clears the allocated 
 * area by setting all bytes to 0.
 */
void *hcalloc(int bytes_to_allocate){
	
   void* ptr = hmalloc(bytes_to_allocate);		//call hmalloc to create the desired space, return the pointer to the start of the allocated area

   int i = 0;		//counter

   while(i < bytes_to_allocate)				//loop over every byte in the allocated area
   {
	   *(int*)(ptr + i) = 0;	   //set the byte at position ptr + i to 0, do this for all bytes
	   i = i + 1;
   }
   return ptr;		//return the same pointer we were given by hmalloc
}

/* hfree
 * Responsible for returning the area (pointed to by ptr) to the free
 * pool.
 *    -simply appends the returned area to the beginning of the single
 *     free list.
 */
void hfree(void *ptr)
{
    void* temp = free_list;			//temp variable of the previous freelist head
    if(free_list == NULL)		//if no entries
    {
        free_list = (ptr - 8);			//set new freelist head
        *(int*)(free_list + 4) = 0;	//set offset to 0, as this is the first entry
	freelistSize++;		//increase global counter
    }
    else
    {
        free_list = ptr - 8;		//create new freelist head
        *(int*)(free_list + 4) = temp - free_list;		//set link value of new head to the difference in distance between the old head and the new one
	freelistSize++;				//increase global size counter
    }
}

/* For the bonus credit implement hrealloc. You will need to add a prototype
 * to hmalloc.h for your function.*/

/*You may add additional functions as needed.*/
