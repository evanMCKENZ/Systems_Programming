//Author: Evan McKenzie
//Class: CS 3411
//Project: 4
//Created: Nov 20, 2021
//Last Edited: Nov 29, 2021
#include "r_client.h"

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
#include <netdb.h>

/*Opcodes for RPC calls*/
#define open_call   1 
#define close_call  2
#define read_call   3
#define write_call  4
#define seek_call   5
#define pipe_call   6
#define dup2_call   7

int entry(int argc, char *argv[]);

/* main - entry point for client applications.
 *
 * You are expected to develop a client program which will provide an 
 * environment into which we can plug an application and execute it 
 * using the remote versions of the supported calls. The client program 
 * therefore should expect a <hostname> <portnumber> pair as its first 
 * two arguments and attempt to connect the server. Once it connects, it
 * should call the user program which has a function called entry 
 * analogous to the main program in an ordinary C program. The entry 
 * routine should have the argv and argc arguments and return an integer 
 * value, just like the ordinary main. The client program should strip 
 * off the first two arguments, create a new argv array and call the entry procedure. 
 * Finally, when entry exits, the return value should be returned from the 
 * main procedure of the client.
 */
int socketfd;
int main(int argc, char *argv[]){
	char* args[2];
	args[0] = argv[3];			//strip command line args to get file names for entry()
	args[1] = argv[4];
	int numargs = argc - 3;
	char* hostname = argv[1];
	int portnum = atoi(argv[2]);
	struct sockaddr_in remote;
	struct hostent *h;
	socketfd = socket(AF_INET, SOCK_STREAM, 0);
	bzero((char*)&remote, sizeof(remote));
	remote.sin_family = AF_INET;
	h = gethostbyname(hostname);
	bcopy((char*)h->h_addr, (char*)&remote.sin_addr, h->h_length);
	remote.sin_port = htons(portnum);

	connect(socketfd, (struct sockaddr*)&remote, sizeof(remote));			//connect to socket
	//you will need to call entry()
	int eret = entry(numargs, args);
	return eret;
}

/* r_open
 * remote open
 */
int r_open(const char *pathname, int flags, int mode){
	int L;
	char * msg;
	const char * p;
	int in_msg;
	int in_err;
	int u_l;
	p=pathname;
	while(*p)
	{	
		p++;
	}
	u_l = p-pathname;
	L = 	1 + 				// this is the opcode
		2 + u_l + 			// 2-byte length field followed by the pathname.
		sizeof(flags) + 		// int bytes for flags.
		sizeof(mode); 			// int bytes for mode.
	msg = malloc(L);
	L=0;
	msg[L++] = 1; 				// this is the code for open.
	msg[L++] = (u_l >> 8) & 0xFF; 		// this is the length.
	msg[L++] = (u_l) & 0xFF;

	for (int i=0; i < u_l; i++)
	{
		msg[L++]=pathname[i]; 			// put the pathname.
	}		
	msg[L++] = (flags >> 24) & 0xFF; 	// put the flags.
	msg[L++] = (flags >> 16) & 0xFF;
	msg[L++] = (flags >> 8) & 0xFF;
	msg[L++] = (flags ) & 0xFF;
	msg[L++] = (mode >> 24) & 0xFF; 	// put the mode.
	msg[L++] = (mode >> 16) & 0xFF;
	msg[L++] = (mode >> 8) & 0xFF;
	msg[L++] = (mode ) & 0xff;
	write(socketfd,msg,L);
	unsigned char* retmsg = malloc(8 * sizeof(char));
	read(socketfd, retmsg, 8);
	in_msg = (retmsg[0] << 24) | (retmsg[1] << 16) | (retmsg[2] << 8) | retmsg[3];
	in_err = (retmsg[4] << 24) | (retmsg[5] << 16) | (retmsg[6] << 8) | retmsg[7];
	errno = in_err;
	free(msg);
	free(retmsg);
	return in_msg;
}

/* r_close
 * remote close
 */
int r_close(int fd){
	int K;
        int in_msg2;
        int in_err2;
	
	K = 1 + sizeof(fd);
	char* msg2 = malloc(K);
	K = 0;
	msg2[K++] = 2;
	msg2[K++] = (fd >> 24) & 0xFF;        // put the file descriptor.
        msg2[K++] = (fd >> 16) & 0xFF;
        msg2[K++] = (fd >> 8) & 0xFF;
        msg2[K++] = (fd) & 0xFF;
	write(socketfd, msg2, K);
	unsigned char* retmsg2 = malloc(8 * sizeof(char));
	read(socketfd, retmsg2, 8);
	in_msg2 = (retmsg2[0] << 24) | (retmsg2[1] << 16) | (retmsg2[2] << 8) | retmsg2[3];
        in_err2 = (retmsg2[4] << 24) | (retmsg2[5] << 16) | (retmsg2[6] << 8) | retmsg2[7];
        errno = in_err2;
        free(msg2);
	free(retmsg2);
        return in_msg2;
}

/* r_read
 * remote read
 */
int r_read(int fd, void *buf, int count){
	int M;
	int in_msg3, in_err3;

	M = 1 + sizeof(fd) + sizeof(count);
	unsigned char* msg3 =  malloc(M);
	M = 0;
	msg3[M++] = 3;
        msg3[M++] = (fd >> 24) & 0xFF;        // put the file descriptor.
        msg3[M++] = (fd >> 16) & 0xFF;
        msg3[M++] = (fd >> 8) & 0xFF;
        msg3[M++] = (fd) & 0xFF;
	msg3[M++] = (count >> 24) & 0xFF;        // put the count
        msg3[M++] = (count >> 16) & 0xFF;
        msg3[M++] = (count >> 8) & 0xFF;
        msg3[M++] = (count) & 0xFF;
	write(socketfd, msg3, M);
	unsigned char* retmsg3 = malloc(8 * sizeof(char));
	read(socketfd, retmsg3, 8);
	in_msg3 = (retmsg3[0] << 24) | (retmsg3[1] << 16) | (retmsg3[2] << 8) | retmsg3[3];
	in_err3 = (retmsg3[4] << 24) | (retmsg3[5] << 16) | (retmsg3[6] << 8) | retmsg3[7];
	read(socketfd, buf, in_msg3);
	errno = in_err3;
	free(msg3);
	free(retmsg3);
	return in_msg3;
}

/* r_write
 * remote write
 */
int r_write(int fd, const void *buf, int count){
	int N;
	int in_msg4, in_err4;
	N = 1 + sizeof(fd) + sizeof(count) + count;
	unsigned char* msg4 = malloc(N);
	N = 0;
	msg4[N++] = 4;
        msg4[N++] = (fd >> 24) & 0xFF;        // put the file descriptor.
        msg4[N++] = (fd >> 16) & 0xFF;
        msg4[N++] = (fd >> 8) & 0xFF;
        msg4[N++] = (fd) & 0xFF;
        msg4[N++] = (count >> 24) & 0xFF;        // put the count
        msg4[N++] = (count >> 16) & 0xFF;
        msg4[N++] = (count >> 8) & 0xFF;
        msg4[N++] = (count) & 0xFF;
	const char* buff = buf;
	for(int j = 0; j < count; j++)
	{
		msg4[N++] = buff[j];
	}
	write(socketfd, msg4, N);
	unsigned char* retmsg4 = malloc(8);
	read(socketfd, retmsg4, 8);
	in_msg4 = (retmsg4[0] << 24) | (retmsg4[1] << 16) | (retmsg4[2] << 8) | retmsg4[3];
        in_err4 = (retmsg4[4] << 24) | (retmsg4[5] << 16) | (retmsg4[6] << 8) | retmsg4[7];
        errno = in_err4;
        free(msg4);
	free(retmsg4);
        return in_msg4;
}

/* r_lseek
 * remote seek
 */
int r_lseek(int fd, int offset, int whence){
	int P;
	int in_msg5, in_err5;
	P = 1 + sizeof(fd) + sizeof(offset) + sizeof(whence);
	char* msg5 = malloc(P);
	P = 0;
	msg5[P++] = 5;
	msg5[P++] = (fd >> 24) & 0xFF;        // put the file descriptor.
        msg5[P++] = (fd >> 16) & 0xFF;
        msg5[P++] = (fd >> 8) & 0xFF;
        msg5[P++] = (fd) & 0xFF;
        msg5[P++] = (offset >> 24) & 0xFF;        // put the offset
        msg5[P++] = (offset >> 16) & 0xFF;
        msg5[P++] = (offset >> 8) & 0xFF;
        msg5[P++] = (offset) & 0xFF;
	msg5[P++] = (whence >> 24) & 0xFF;        // put the whence
        msg5[P++] = (whence >> 16) & 0xFF;
        msg5[P++] = (whence >> 8) & 0xFF;
        msg5[P++] = (whence) & 0xFF;
	write(socketfd, msg5, P);
	char* retmsg5 = malloc(8 * sizeof(char));
	read(socketfd, retmsg5, 8);
	in_msg5 = (retmsg5[0] << 24) | (retmsg5[1] << 16) | (retmsg5[2] << 8) | retmsg5[3];
        in_err5 = (retmsg5[4] << 24) | (retmsg5[5] << 16) | (retmsg5[6] << 8) | retmsg5[7];
        errno = in_err5;
        free(msg5);
	free(retmsg5);
        return in_msg5;
}

/* r_pipe
 * remote pipe
 */
int r_pipe(int pipefd[2]){
	int Q;
	int in_msg6, in_err6;
	Q = 1;
	char* msg6 = malloc(Q);
	Q = 0;
	msg6[Q++] = 6;
	write(socketfd, msg6, Q);
	char* retmsg6 = malloc(16 * sizeof(char));
	read(socketfd, retmsg6, 16);
	in_msg6 = (retmsg6[0] << 24) | (retmsg6[1] << 16) | (retmsg6[2] << 8) | retmsg6[3];
        in_err6 = (retmsg6[4] << 24) | (retmsg6[5] << 16) | (retmsg6[6] << 8) | retmsg6[7];
	int fd1 = (retmsg6[8] << 24) | (retmsg6[9] << 16) | (retmsg6[10] << 8) | retmsg6[11];
        int fd2 = (retmsg6[12] << 24) | (retmsg6[13] << 16) | (retmsg6[14] << 8) | retmsg6[15];
	errno = in_err6;
	pipefd[0] = fd1;
	pipefd[1] = fd2;
        free(msg6);
	free(retmsg6);
        return in_msg6;
}

/* r_dup2
 * remote dup2
 */
int r_dup2(int oldfd, int newfd){
	int R;
	int in_msg7, in_err7;
	R = 1 + sizeof(oldfd) + sizeof(newfd);
	char* msg7 = malloc(R);
	R = 0;
	msg7[R++] = 7;
	msg7[R++] = (oldfd >> 24) & 0xFF;        // put the oldfd
        msg7[R++] = (oldfd >> 16) & 0xFF;
        msg7[R++] = (oldfd >> 8) & 0xFF;
        msg7[R++] = (oldfd) & 0xFF;
        msg7[R++] = (newfd >> 24) & 0xFF;        // put the newfd
        msg7[R++] = (newfd >> 16) & 0xFF;
        msg7[R++] = (newfd >> 8) & 0xFF;
        msg7[R++] = (newfd) & 0xFF;
	write(socketfd, msg7, R);
	char* retmsg7 = malloc(8 * sizeof(char));
	read(socketfd, retmsg7, 8);
	in_msg7 = (retmsg7[0] << 24) | (retmsg7[1] << 16) | (retmsg7[2] << 8) | retmsg7[3];
        in_err7 = (retmsg7[4] << 24) | (retmsg7[5] << 16) | (retmsg7[6] << 8) | retmsg7[7];
        errno = in_err7;
        free(msg7);
	free(retmsg7);
        return in_msg7;
}
