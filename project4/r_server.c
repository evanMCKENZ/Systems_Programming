//Author: Evan McKenzie
//Class: CS 3411
//Project: 4
//Created: Nov 19, 2021
//Last Edited Nov 28, 2021

/* Add any includes needed*/
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>

/*Opcodes for RPC calls*/
#define open_call   1 
#define close_call  2
#define read_call   3
#define write_call  4
#define seek_call   5
#define pipe_call   6
#define dup2_call   7

/* In this project, we will develop a mini Remote Procedure Call (RPC) 
 * based system consisting of a em server and a client. Using the remote 
 * procedures supplied by the server our client program will be able to 
 * open files and perform computations on the server. 
 *
 * The server should open a socket and listen on an available port. The 
 * server program upon starting should print the port number it is using 
 * on the standard output 
 * (Print only the port number with no additional formatting. You may use printf \%d). 
 * This port number is then manually passed as a command line argument to 
 * the client to establish the connection. In order to implement the RPC, 
 * the server and the client should communicate through a TCP socket. It 
 * is allowed to fork a child for each open connection and delagate the 
 * handling to the child.
 */

/* main - server implementation
 */
int handleconnect(int fd);			//function prototype

int main(int argc, char *argv[]){
	
	struct sockaddr_in s1, s2;
	int conn, listener;
        socklen_t length;
	listener = socket(AF_INET, SOCK_STREAM, 0);		//listener fd

	bzero((char*) &s1, sizeof(s1));
	s1.sin_family = AF_INET;
	s1.sin_addr.s_addr = htonl(INADDR_ANY);
	s1.sin_port = htons(0);

	bind(listener, (struct sockaddr*) &s1, sizeof(s1));
	length = sizeof(s1);

	getsockname(listener, (struct sockaddr*) &s1, &length);

	printf("port num: %d\n", ntohs(s1.sin_port));		//only output of server

	listen(listener, 1);
	length = sizeof(s2);
	
	while(1)		//infinte loop to handle clients
	{
		conn = accept(listener, (struct sockaddr*) &s2, &length);
		if(fork() == 0)		//create child to handle messaging
		{
			handleconnect(conn);
			close(listener);
		}
		close(conn);		//close connection when formatting done
	}
	return 0;
}

char op;
int handleconnect(int fd)
{
	int check;
	while((check = read(fd, &op, 1)) > 0)
	{
		switch(op)
		{
			case 1:;		//open call
			        unsigned char* len =  malloc(2 * sizeof(char));		//input file name length
				read(fd, len, 2);
				int length = (len[0] << 8) | len[1];
				char* pathname = malloc(length + 1);		//file name
				read(fd, pathname, length);
				pathname[length] = '\0';
				unsigned char* flagval = malloc(4 * sizeof(char));		//open flag values
				read(fd, flagval, 4);
				int flags = (flagval[0] << 24) | (flagval[1] << 16) | (flagval[2] << 8) | flagval[3];
				unsigned char* modeval = malloc(4 * sizeof(char));		//open mode value
				read(fd, modeval, 4);
				int mode = (modeval[0] << 24) | (modeval[1] << 16) | (modeval[2] << 8) | modeval[3];
				int fdtoop = open(pathname, flags, mode);		//open remote file
				int ind1 = 0;
				char* retmsg1 = malloc(8 * sizeof(char));
				retmsg1[ind1++] = (fdtoop >> 24) & 0xFF;
				retmsg1[ind1++] = (fdtoop >> 16) & 0xFF;		//pack open call response
				retmsg1[ind1++] = (fdtoop >> 8) & 0xFF;
				retmsg1[ind1++] = fdtoop & 0xFF;
				retmsg1[ind1++] = (errno >> 24) & 0xFF;
                        	retmsg1[ind1++] = (errno >> 16) & 0xFF;			//pack errno value after open call
                        	retmsg1[ind1++] = (errno >> 8) & 0xFF;
                        	retmsg1[ind1++] = errno & 0xFF;
				write(fd, retmsg1, ind1);		//write response to socket
				free(len);
				free(pathname);
				free(flagval);
				free(modeval);
				free(retmsg1);
				break;
			case 2:;		//close call
				unsigned char* fileclose = malloc(4 * sizeof(char));		//file to close fd
				read(fd, fileclose, 4);
				int fdc = (fileclose[0] << 24) | (fileclose[1] << 16) | (fileclose[2] << 8) | fileclose[3];
				int rep = close(fdc);		//close remote file
				int ind2 = 0;
                        	char* retmsg2 = malloc(8 * sizeof(char));
                        	retmsg2[ind2++] = (rep >> 24) & 0xFF;
                       		retmsg2[ind2++] = (rep >> 16) & 0xFF;		//pack close call response
                        	retmsg2[ind2++] = (rep >> 8) & 0xFF;
                        	retmsg2[ind2++] = rep & 0xFF;
                        	retmsg2[ind2++] = (errno >> 24) & 0xFF;
                        	retmsg2[ind2++] = (errno >> 16) & 0xFF;			//pack errno value after close call
                        	retmsg2[ind2++] = (errno >> 8) & 0xFF;
                        	retmsg2[ind2++] = errno & 0xFF;
				write(fd, retmsg2, ind2);		//write response to socket
				free(fileclose);
				free(retmsg2);
				break;
			case 3:;		//read call
				unsigned char* fileread = malloc(4 * sizeof(char));		//file to read from fd
			        read(fd, fileread, 4);
				int fdtr = (fileread[0] << 24) | (fileread[1] << 16) | (fileread[2] << 8) | fileread[3];
				unsigned char* counter = malloc(4 * sizeof(char));		//number of characters to read
				read(fd, counter, 4);
				int count = (counter[0] << 24) | (counter[1] << 16) | (counter[2] << 8) | counter[3];
				char* msgread = malloc(count * sizeof(char));		//buffer to hold read characters
				int readr = read(fdtr, msgread, count);		//read from remote file
				int ind3 = 0;
				char* retmsg3 = malloc( (8*sizeof(char)) + (count*sizeof(char)));
                                retmsg3[ind3++] = (readr >> 24) & 0xFF;
                                retmsg3[ind3++] = (readr >> 16) & 0xFF;			//pack read response
                                retmsg3[ind3++] = (readr >> 8) & 0xFF;
                                retmsg3[ind3++] = readr & 0xFF;
                                retmsg3[ind3++] = (errno >> 24) & 0xFF;
                                retmsg3[ind3++] = (errno >> 16) & 0xFF;		//pack errno value after read call
                                retmsg3[ind3++] = (errno >> 8) & 0xFF;
                                retmsg3[ind3++] = errno & 0xFF;
				while(readr > 0)
				{
					retmsg3[ind3++] = *msgread;			//pack read message from file
					msgread++;
					readr = readr - 1;
				}
                                write(fd, retmsg3, ind3);		//write to socket
				free(fileread);
				free(counter);
				free(retmsg3);
				break;
			case 4:;		//write call
				unsigned char* filewrite = malloc(4);		//file to write to
			        read(fd, filewrite, 4);
				int fdtw = (filewrite[0] << 24) | (filewrite[1] << 16) | (filewrite[2] << 8) | filewrite[3];
				unsigned char* wcounter = malloc(4);		//number of characters to write
				read(fd, wcounter, 4);
				int wcount = (wcounter[0] << 24) | (wcounter[1] << 16) | (wcounter[2] << 8) | wcounter[3];
				unsigned char* msgtow = malloc(wcount);		//message to write
				read(fd, msgtow, wcount);
				int writer = write(fdtw, msgtow, wcount);		//write to remote file
				int ind4 = 0;
                                char* retmsg4 = malloc(8 * sizeof(char));
                                retmsg4[ind4++] = (writer >> 24) & 0xFF;
                                retmsg4[ind4++] = (writer >> 16) & 0xFF;		//pack write response
                                retmsg4[ind4++] = (writer >> 8) & 0xFF;
                                retmsg4[ind4++] = writer & 0xFF;
                                retmsg4[ind4++] = (errno >> 24) & 0xFF;
                                retmsg4[ind4++] = (errno >> 16) & 0xFF;		//pack errno value after write call
                                retmsg4[ind4++] = (errno >> 8) & 0xFF;
                                retmsg4[ind4++] = errno & 0xFF;
				write(fd, retmsg4, ind4);		//write to socket
				free(filewrite);
				free(wcounter);
				free(msgtow);
				free(retmsg4);
				break;
			case 5:;		//seek call
				unsigned char* fileseek = malloc(4 * sizeof(char));		//file to seek fd
			        read(fd, fileseek, 4);
				int fdts = (fileseek[0] << 24) | (fileseek[1] << 16) | (fileseek[2] << 8) | fileseek[3];
				unsigned char* offseta = malloc(4 * sizeof(char));		//offset from whence
				read(fd, offseta, 4);
				int offset = (offseta[0] << 24) | (offseta[1] << 16) | (offseta[2] << 8) | offseta[3];
				unsigned char* where = malloc(4 * sizeof(char));		//where to start offset
                                read(fd, where, 4);
                                int whence = (where[0] << 24) | (where[1] << 16) | (where[2] << 8) | where[3];
				int dest = lseek(fdts, offset, whence);			//seek remote file
				int ind5 = 0;
                        	char* retmsg5 = malloc(8 * sizeof(char));
                        	retmsg5[ind5++] = (dest >> 24) & 0xFF;
                        	retmsg5[ind5++] = (dest >> 16) & 0xFF;			//pack lseek response
                        	retmsg5[ind5++] = (dest >> 8) & 0xFF;
                        	retmsg5[ind5++] = dest & 0xFF;
                        	retmsg5[ind5++] = (errno >> 24) & 0xFF;
                        	retmsg5[ind5++] = (errno >> 16) & 0xFF;		//pack errno value after lseek call
                        	retmsg5[ind5++] = (errno >> 8) & 0xFF;
                        	retmsg5[ind5++] = errno & 0xFF;
                        	write(fd, retmsg5, ind5);		//write to socket
				free(fileseek);
				free(offseta);
				free(where);
				free(retmsg5);
				break;
			case 6:;		//pipe call
				int pipefd[2];
				int pret = pipe(pipefd);			//pipe remote files
				int ind6 = 0;
				char* retmsg6 = malloc(16 * sizeof(char));
				retmsg6[ind6++] = (pret >> 24) & 0xFF;
                                retmsg6[ind6++] = (pret >> 16) & 0xFF;		//pack pipe response
                                retmsg6[ind6++] = (pret >> 8) & 0xFF;
                                retmsg6[ind6++] = pret & 0xFF;
                                retmsg6[ind6++] = (errno >> 24) & 0xFF;
                                retmsg6[ind6++] = (errno >> 16) & 0xFF;		//pack errno value after pipe call
                                retmsg6[ind6++] = (errno >> 8) & 0xFF;
                                retmsg6[ind6++] = errno & 0xFF;
				retmsg6[ind6++] = (pipefd[0] >> 24) & 0xFF;
                                retmsg6[ind6++] = (pipefd[0] >> 16) & 0xFF;		//pack fd 0
                                retmsg6[ind6++] = (pipefd[0] >> 8) & 0xFF;
                                retmsg6[ind6++] = pipefd[0] & 0xFF;
                                retmsg6[ind6++] = (pipefd[1] >> 24) & 0xFF;
                                retmsg6[ind6++] = (pipefd[1] >> 16) & 0xFF;		//pack fd 1
                                retmsg6[ind6++] = (pipefd[1] >> 8) & 0xFF;
                                retmsg6[ind6++] = pipefd[1] & 0xFF;
				write(fd, retmsg6, ind6);	//write to socket
				free(retmsg6);
				break;
			case 7:;		//dup2 call
				unsigned char* old = malloc(4 * sizeof(char));		//old fd
			        read(fd, old, 4);
				int oldfd = (old[0] << 24) | (old[1] << 16) | (old[2] << 8) | old[3];
				unsigned char* new = malloc(4 * sizeof(char));		//new fd
				read(fd, new, 4);
				int newfd = (new[0] << 24) | (new[1] << 16) | (new[2] << 8) | new[3];
				int retfd = dup2(oldfd, newfd);			//dup the old fd to the new fd
				int ind7 = 0;
                        	char* retmsg7 = malloc(8 * sizeof(char));
                        	retmsg7[ind7++] = (retfd >> 24) & 0xFF;
                        	retmsg7[ind7++] = (retfd >> 16) & 0xFF;		//pack the dup2 response
                        	retmsg7[ind7++] = (retfd >> 8) & 0xFF;
                        	retmsg7[ind7++] = retfd & 0xFF;
                        	retmsg7[ind7++] = (errno >> 24) & 0xFF;
                        	retmsg7[ind7++] = (errno >> 16) & 0xFF;		//pack errno value after dup2 call
                        	retmsg7[ind7++] = (errno >> 8) & 0xFF;
                        	retmsg7[ind7++] = errno & 0xFF;
                        	write(fd, retmsg7, ind7);
				free(old);
				free(new);
				free(retmsg7);
				break;
			default:;
				printf("invalid opcode!\n");		//if opcode no 1-7 print error message and exit
				exit(0);
		}
	}
	//return 0;		//return to main (child process done)
	exit(0);
}
