//Author: Evan McKenzie
//Class: CS 3411
//Project: 4
//Created: Nov 21, 2021
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
 * rclient2 should open a local file as output and a remote file as input. 
 * It should seek the remote file to position 10 and copy the rest 
 * to the local file.
 */
int entry(int argc, char* argv[]){
	//you will call the r_ functions here.
        //char* input = argv[0];
        //char* output = argv[1];
        //open local output
        int lfd = open(argv[1], O_CREAT | O_WRONLY | O_TRUNC, 0666);
        //open remote input
        int rfd = r_open(argv[0], O_RDONLY, 0666);
	r_lseek(rfd, 10, SEEK_SET);			//seek remote file (skip first 10 characters)

	unsigned char* lch = malloc(1);		//single character buffer
        int r;
        while((r = r_read(rfd, lch, 1)) > 0)		//read from remote file
        {
                write(lfd, lch, 1);		//write to local file
        }

	free(lch);
        int lcl = close(lfd);			//close local file
        int rcl = r_close(rfd);			//close remote file
        if(lcl == rcl)
        {
                return 0;       //success
        }
        else
        {
                return -1;      //fail
        }
}
