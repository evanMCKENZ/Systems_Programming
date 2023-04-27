#include "bitsy.h"
#include <unistd.h>
#include <stdio.h>
/* Add any global variables or structures you need.*/

int counter = -01;

/* readByte
 * Abstraction to read a byte.
 * Relys on readBit.
 */
unsigned short readByte(){ 
	/* This function should not call read() directly.
	 * If we are buffering data in readBit, we dont want to skip over
	 * that data by calling read again. Instead, call readBit and 
	 * construct a byte one bit at a type. This also allows us to read
	 * 8 bits that are not necessarily byte alligned. */

	//Do NOT call read here. Instead rely on readBit.
	
	//I suggest returning a unique value greater than the max byte value
	//to communicate end of file. We need this since 0 is a valid byte value
	//so we need another way to communicate eof. These functions are typed
	//as short to allow us to return a value greater than the max byte value.
	
	unsigned char returner = 0;
        unsigned short bit;

        for(int i = 7; i >= 0; i = i-1)
        {
                bit = readBit();
                if(bit != 277)
                {
                        returner = returner | (bit << i);
                }
                else{
                        return 277;
                }
        }
        return returner;
}

unsigned char ch = 0;
/* readBit
 * Abstraction to read a bit.
 * */
unsigned short readBit(){
	/* This function is responsible for reading the next bit on the
	 * input stream from stdin (fd = 0). To accomplish this, keep a 
	 * byte sized buffer. Each time read bit is called, use bitwise
	 * operations to extract the next bit and return it to the caller.
	 * Once the entire buffered byte has been read the next byte from 
	 * the file. Once eof is reached, return a unique value > 255
	 *
	 */	

	//You will need to call read here

	//I suggest returning a unique value greater than the max byte value
	//to communicate end of file. We need this since 0 is a valid byte value
	//so we need another way to communicate eof. These functions are typed
	//as short to allow us to return a value greater than the max byte value.
	
        unsigned short bit;
	int numBits = 0;
	if(counter == -1)
	{
		counter = 7;
		numBits = read(0, &ch, 1);
		if(numBits == 0)
		{
			return 277;
		}
	}
	bit = (ch >> counter) & 0x1;
	counter = counter - 1;

	return bit;
}

unsigned char buffer2;
int bitdown = 7;

/* writeByte
 * Abstraction to write a byte.
 */
void writeByte(unsigned char byt){
	/* Use writeBit to write each bit of byt one at a time. Using writeBit
	 * abstracts away byte boundaries in the output.*/

	//Do NOT call write, instead utilize writeBit()

	for(int i = 7; i >= 0; i = i -1)
	{
		writeBit((byt >> i) & 0x1);
	}
}

/* writeBit
 * Abstraction to write a single bit.
 */
void writeBit(unsigned char bit){
	/* Keep a byte sized buffer. Each time this function is called, insert the 
	 * new bit into the buffer. Once 8 bits are buffered, write the full byte
	 * to stdout (fd=1).
	 */
	//You will need to call write here eventually.
	buffer2 = (bit << bitdown) | buffer2;
	bitdown = bitdown - 1;
	if(bitdown == -1)
	{
		write(1, &buffer2, 1);
		bitdown = 7;
		buffer2 = 0;
	}
}

/* flushWriteBuffer
 * Helper to write out remaining contents of a buffer after padding empty bits
 * with 1s.
 */
void flushWriteBuffer(){
	/* This will be utilized when finishing your encoding. It may be that some bits
	 * are still buffered and have not been written to stdout. Call this function 
	 * which should do the following: Determine if any buffered bits have yet to be 
	 * written. Pad remaining bits in the byte with 1s. Write byte to stdout
	 */
	int numtowr = bitdown;
	if(numtowr != 7)
	{
		while(numtowr >= 0)
		{
			writeBit(1);
			numtowr = numtowr - 1;
		}
	}
		
}
