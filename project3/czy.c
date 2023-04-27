//Author: Evan McKenzie
//Class: CS 3411
//Created: Oct 22, 2021
//Last Edited: Oct 31, 2021

#include "bitsy.h" //This header includes prototypes for the proposed bit abstractions
/*Include any additional headers you require*/
#include <stdio.h>
/*You may use any global variables/structures that you like*/

unsigned short array[8] = {-1, -1, -1, -1, -1, -1, -1, -1};


/* main - czy compression implementation
 * Develop a program called czy which compresses the data stream directed 
 * at its standard input and writes the compressed stream to its standard 
 * output.
 *
 * The compression algorithm reads the input one symbol (i.e., byte) at a 
 * time and compares it with each of the 8 bytes previously seen.  It also 
 * checks to see if the following n characters are the same as the current 
 * symbol.  If the byte has been previously seen, it outputs the position of 
 * the previous byte relative to the current position as well as the number 
 * of times it occurs using the format described below. Otherwise, the symbol 
 * is output as is by prefixing it with a binary one.
 *
 * To compile czy: make czy
 * To execute: ./czy < somefile.txt > somefile.encoded
 */
int main(int argc, char *argv[]){
	//The implementation of your encoder should go here.
	
	//It is recommeded that you implement the bit abstractions in bitsy.c and
	//utilize them to implement your encoder. 
	//If so, do NOT call read/write here. Instead rely exclusively on 
	//readBit, readByte, writeBit, writeByte, and flushWriteBuffer.
	unsigned short current;		//current byte/char			
	unsigned short next;		//future byte
	current = readByte(); 		//get first char from input
	if(current == 277){			//check for EOF (empty file)
		return 0;	//no buffer to flush on first run
	}
	else{
		writeBit(1);		//infrequent encoding here
		writeByte(current);		//write current byte
		array[0] = current;		//insert into previous array
	}
	int flagVal = 0;
	while(current != 277){			//while we are not at the end of file
		int position = 0;		//position var (if needed)
		int recent = 0;			//recent boolean var
		if(flagVal == 0){		//if we need to read another character (on infrequent runs only)
			current = readByte();		//read next char
			if(current == 277){
				flushWriteBuffer();		//check for EOF
				return 0;
			}
		}
		flagVal = 0;		//set default flag value
		for(int i = 7; i >= 0; i=i-1){		//loop over previous chars to see if this is a recent character
			if(current == array[i]){		//check for equality
				position = i;		//position will be most recent encounter
				recent = 1;		//set bool value
			}
		}
		if(recent == 0){		//infrequent encoding here
                                writeBit(1);		//write initial 1
                                writeByte(current);		//can use shortcut to write all of character
                                for(int l = 7; l >= 0; l=l-1){
                                        array[l] = array[l-1];		//shift all previous chars over 1
                                }
                                array[0] = current;		//insert new char to position 0
		}
		else{
				next = readByte();		//if recent, we need to check forward as well
				if(next != current){			//recent, nonrepeating encoding here
					writeBit(0);		//write initial 0
					writeBit(0);		//write initial 0
					for(int j = 2; j >= 0; j = j -1){
						writeBit( ((position >> j) & 0x1) );		//write pos 3 bit number
					}
					for(int a = 7; a >= 0; a=a-1){
						array[a] = array[a-1];		//shift all entries in array over 1
					}
					flagVal = 1;		//maintain flag value
					array[0] = current;		//insert current to start of array
					current = next;		//reset byte value
				}
				else{					//recent, repeating encoding here
					int count = -1;
					while((next == current) && (count < 7)){
						count = count + 1;			//increment number of consec. chars
						next = readByte();		//get next byte in input
					}
					writeBit(0);		//write initial 0
                                        writeBit(1);			//write initial 1
					for(int b = 2; b >= 0; b = b-1){
                                                writeBit( ((position >> b) & 0x1) );		//write pos 3 bit number
                                        }
					for(int n = 2; n >= 0; n=n-1){
						writeBit( ((count >> n) & 0x1) );		//write count 3 bit number
					}
					while(count >= -1){
						for(int o = 7; o >= 0; o=o-1){
							array[o] = array[o-1];		//shift all entries count number of times
						}
						array[0] = current;		//insert current to position 0
						count = count -1;		//decrease count
					}
					flagVal = 1;		//maintain flag value
                                        current = next;		//reset byte value				
				}
		}
	}	
	flushWriteBuffer();			//final flush of all unwritten bits when at EOF
	return 0; //exit status. success=0, error=-1
}
