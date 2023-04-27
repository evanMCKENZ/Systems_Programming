//Author: Evan McKenzie
//Class: CS 3411
//Project: 4
//Created: Nov 20, 2021
//Last Edited: Nov 28, 2021

#include "r_client.h"
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
/* Add any includes needed*/

/* entry
 * This is essentially the "main" for any user program linked with
 * r_client. Main in r_client will establish the connection with the
 * server then call entry. From entry, we can implement the desired 
 * user program which may call any of the r_ RPC functions.
 *
 * rclient1 remotely opens an output file, locally opens an input file, 
 * copies the input file to the output file and closes both files.
 */
int entry(int argc, char* argv[]){
	//you will call the r_ functions here.
	char* input = argv[0];
	char* output = argv[1];
	//open remote output
	int rfd = r_open(output, O_CREAT | O_WRONLY | O_TRUNC, 0666);
	//open local input
	int lfd = open(input, O_RDONLY, 0666);
	
	unsigned char* lch = malloc(1);		//buffer for one character at a time
	int r;
	while((r = read(lfd, lch, 1)) > 0)		//local read
	{
		r_write(rfd, lch, 1);			//remote write
	}
	int lcl = close(lfd);
	int rcl = r_close(rfd);
	if(lcl == rcl)
	{
		return 0; 	//success
	}
	else
	{
		return -1;	//fail
	}
}
