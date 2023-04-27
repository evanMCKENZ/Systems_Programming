//Author: Evan McKenzie
//Class: CS 3411
//Project: 5
//Opened: December 7, 2021
//Last Edited: December 12, 2021

//Add any includes you require.

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <stdlib.h>
#include <signal.h>

/* main - implementation of filter
 * In this project, you will be developing a filter program that forks
 * a child and then executes a program passed as an argument and intercepts 
 * all output and input from that program. The syntax for the execution of 
 * the filter program is given by:
 * 
 * 	filter <program name> <arguments to program>
 *
 * The following commands must be implemented:
 * 	//           : Pass the character '/' as input to the program.
 * 	/i           : Go to input only mode.
 * 	/o           : Go to input/output mode.
 * 	/c           : Go to command mode.
 * 	/m <integer> : Set the max-text, maximum number of lines to be displayed.
 * 	/k <integer> : Send the child process indicated signal.
 *
 * See the spec for details.
 * 
 * After receiving each command, the program should output a prompt indicating 
 * the current mode and if there is more data to be displayed. 
 * The prompt syntax is :
 *
 * 	<pid> m <more> #
 *
 * where <pid> is the process id of the child process, m is the current mode (
 * i (input only), c (command), o(input/output)), optional <more> is the text "more" 
 * if there is data available to be displayed, and lastly the pound character.
 */

#define COMMAND 100
#define INPUT 200		//definitions for the various modes
#define OUTPUT 300

int mode = COMMAND;		//global variables
int MAXLINES = 20;

void chgMode(){			//change between modes
        char h;
        read(0, &h, 1);		//read command character
        if(h == 'i'){
                mode = INPUT;}
        else if(h == 'c'){
                mode = COMMAND;}		//swap to correct mode
        else if(h == 'o'){
                mode = OUTPUT;}
        read(0, &h, 1);		//clear newline from input stream
}
char*  readIn(int fd){		//method to read and parse input
	char* input = malloc(100);		//max characters plus one
	int index = 0;
	char in;
	read(0, &in, 1);
	if(in == '/'){		//check for command marker
		chgMode();
		input[index] = '/';	//change mode and return flag
		return input;
	}
	else{			//otherwise read normal input
		while(in != '\n'){
			input[index] = in;
			index++;		//increment index of input
			int checker;
			if( (checker = read(0, &in, 1)) == 0){		//make sure not end of input
			break;}
		}
	input[index] = '\0';		//add null terminator onto end of input
        return input;		//return input string
	}
}
void prtPrompt(pid_t pid, int bool){		//method to print prompt after each interaction
	char md;
	if(mode == COMMAND){
		md = 'c';
	}else if(mode == INPUT){		//find current mode
		md = 'i';
	}else{
		md = 'o';}
	if(bool == 1){
		printf("%d %c more #\n", pid, md);}	//if more input, use this prompt
	else{
		printf("%d %c #\n", pid, md);}	//if input less than MAXLINES, use this prompt
}
int main(int argc, char *argv[]){
	//Hint: use select() to determine when reads may execute without blocking.
	
	int inPipe[2];
	int outPipe[2];
	int errPipe[2];

	pipe(inPipe);		//create input pipe
	pipe(outPipe);		//create output pipe
	pipe(errPipe);		//create error pipe

	//child process
	pid_t cpid = fork();
	if(cpid == 0)			//create child process
	{
		dup2(inPipe[0], 0);		//dup and delete stdin for child
		close(inPipe[0]);
		close(inPipe[1]);
		dup2(outPipe[1], 1);		//dup and delete stdout for child
		close(outPipe[0]);
		close(outPipe[0]);
		dup2(errPipe[1], 2);		//dup and delete stderr for child
		close(errPipe[0]);
		close(errPipe[1]);
		char* argv2[argc-1];		//create new argv array for child
		argv2[0] = argv[1];			//set program name
		for(int i = 1; i < argc-2; i++)		//copy arguments
		{
			argv2[i] = argv[i+1];		//from old argv to new argv
		}
		const char* file = argv2[0];		//get file name from arguments
		int echeck = execvp(file, argv2);		//exec new process for child
		//if(echeck == -1){ printf("exec error\n");}	//err check on exec
		exit(0)
	}
	else{		//parent process
	fd_set fdst;
	close(inPipe[0]);		//close read end of input pipe
	close(outPipe[1]);		//close write end of output pipe
	close(errPipe[1]);		//close write end of err pipe
	//pid_t  ppid = getpid();		//parent process pid
	while(1){
		FD_ZERO(&fdst);		//zero out the fd set (make sure there are none inside)
		switch(mode){
			case INPUT:;		//input only mode
				FD_SET(0, &fdst);			//add stdin to the fd set
				//FD_SET(errPipe[0], &fdst);
				int check2 = select(errPipe[0] + 1, &fdst, NULL, NULL, NULL);		//watch the fd set for input
				if(check2 > 0){
					if(FD_ISSET(0, &fdst) > 0){		//if data exists in stdin
						char* inin;
						inin = readIn(0);		//call read function to read the input
						if(inin[0] != '/'){
							write(inPipe[1], inin, sizeof(inin));	//send input to child if it is not a command
						}
					}
				}
				prtPrompt(cpid, 0);		//print prompt for input
				break;
			case OUTPUT:;
				FD_SET(0, &fdst);		//add stdin to fd set
				FD_SET(outPipe[0], &fdst);		//add read end of output pipe to fd set
				int lines = 0;		//global lines printed
				int check3 = select(errPipe[1] + 1, &fdst, NULL, NULL, NULL);	//watch the fd set for input/output
				if(check3 > 0){
					if( (FD_ISSET(0, &fdst)) != 0){		//if input to stdin
						char* outin;
      						outin = readIn(0);		//read stdin
						write(inPipe[1], outin, sizeof(outin));		//write stdin to child proces
					}
					if(FD_ISSET(outPipe[0], &fdst) != 0){		//if output from child process
						char out;
						int e;
						while( (e = read(outPipe[0], &out, 1) != 0) && (lines < MAXLINES)){		//read from the output pipe
								if(out == '\n'){
									lines++;
								}
								write(1, &out, 1);
						}
					}
				}
				if(lines == MAXLINES){			//more input to process, print prompt with more
					prtPrompt(cpid, 1);
				}
				else{
					prtPrompt(cpid, 0);		//no more input, print appropriate prompt
				}
				break;
			case COMMAND:;
                                FD_SET(0, &fdst);		//add stdin to fd set
                                //FD_SET(errPipe[0], &fdst);
                                int check = select(errPipe[0] + 1, &fdst, NULL, NULL, NULL);			//watch fd set for input
                                if(check > 0){
                                        if(FD_ISSET(0, &fdst) != 0){		//if there is input on stdin
                                                char slash;
                                                read(0, &slash, 1);		//make sure the first char is a slash (denotes command)
                                                if(slash == '/'){
                                                        char comm;
                                                        read(0, &comm, 1);		//read command character
                                                        char dump;
                                                        read(0, &dump, 1);		//delete whitespace/newline from input
                                                        switch(comm){
                                                                case 'i':;
                                                                         mode = INPUT;		//switch to input mode
                                                                         break;
                                                                case 'o':;
                                                                         mode = OUTPUT;		//switch to output mode
                                                                         break;
                                                                case 'c':;
                                                                         mode = COMMAND;	//idk why youd want this one
                                                                         break;
                                                                case 'm':;
                                                                         char buffer[2];
                                                                         read(0, buffer, 2);
                                                                         MAXLINES = atoi(buffer);		//read new max lines value
                                                                         //printf("%d, maxlines\n", MAXLINES);
                                                                         char nl;
                                                                         read(0, &nl, 1);		//delete newline
                                                                         break;
                                                                case 'k':;
                                                                         char buffer2[2];
                                                                         read(0, buffer2, 2);
                                                                         int sig = atoi(buffer2);		//read signal for child process
                                                                         kill(cpid, sig);
                                                                         char nl2;
                                                                         read(0, &nl2, 1);		//clear newline
                                                                         break;
                                                        }
                                                }
                                                else{
                                                        printf("invalid command\n");		//invalid command if not above symbols
                                                }
                                        }
                                }
                                prtPrompt(cpid, 0);		//print prompt
                                break;
			default:;
				printf("invalid mode\n");		//invalid mode if not INPUT, OUTPUT, or COMMAND
				exit(0);		//exit process
		}
	}
	}
	return 0;	//need return from main
}
