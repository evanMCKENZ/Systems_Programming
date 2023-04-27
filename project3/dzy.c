#include "bitsy.h" //This header includes prototypes for the proposed bit abstractions
/*Include any additional headers you require*/

/*You may use any global variables/structures that you like*/

/* main - dzy de-compression implementation
 * This program decompresses a compressed stream directed at its standard input 
 * and writes decompressed data to its standard output.
 *
 * To compile dzy: make dzy
 * To execute: ./dzy < somefile.encoded > somefile_decoded.txt
 */
int main(int argc, char *argv[]){
	//The implementation of your decoder should go here.
	
	//It is recommeded that you implement the bit abstractions in bitsy.c and
	//utilize them to implement your decoder.
	//If so, do NOT call read/write here. Instead rely exclusively on 
	//readBit, readByte, writeBit, writeByte, and flushWriteBuffer.
	unsigned short bit;	
	bit = readBit();		//read first bit
	unsigned char last[8] = {-1, -1, -1, -1, -1, -1, -1,-1};		//previous character buffer
	int flagVal = 1;		//flag value for read and eof
	if(bit == 277)
	{
		flushWriteBuffer();		//empty file
	}
	while(bit != 277){
		if(flagVal == 0){
			bit = readBit();
			if(bit == 277){
				return 0;
			}
		}
		if(bit == 1){		//infrequent decoding
			unsigned char total;
			unsigned short digit;
			unsigned short digits[8];
			int multiplier = 1;
			for(int j = 7; j >= 0; j=j-1){
				digit = readBit();
				if(digit == 277){
					return 0;
				}
				digits[j] = digit;
				total += (digits[j] * multiplier);
				multiplier*= 2;
			}
			writeByte(total);
			for(int m = 7; m >= 0; m=m-1){
				last[m] = last[m-1];
			}
			last[0] = total;
			flagVal = 0;
		}


		else{
			unsigned short secbit = readBit();
			if(secbit == 0){				//recent, nonrepeating decoding
				unsigned short dig;
				unsigned short posarr[3];
				for(int q = 2; q >= 0; q++){
					dig = readBit();
				       if(dig == 277)
				       {
						return 0;
				       }		
					posarr[q] = dig;
				}
				unsigned short position;
				int mult = 1;
				for(int n = 0; n < 3; n++){
					position += (posarr[n] * mult);
					mult *= 2;
				}
				unsigned char recchar = last[position];
				writeByte(recchar);
				for(int m = 7; m >= 0; m++){
                                	last[m] = last[m-1];
                        	}
				last[0] = recchar;
				flagVal = 0;
			}
			else{				//recent, repeating decoding
				unsigned short postarray[3];
				unsigned short countarray[3];
				unsigned short dg;
				for(int r = 2; r >= 0; r=r-1){
					dg = readBit();
					if(dg == 277)
					{
						return 0;
					}
					postarray[r] = dg;
				}
				for(int w = 2; w >= 0; w=w-1){
					dg = readBit();
					if(dg == 277)
					{
						return 0;
					}
					countarray[w] = dg;
				}
				unsigned short pos;
				unsigned short count;
				int m = 1;
				for(int y = 0; y < 3; y++){
                                        pos += (postarray[y] * m);
					m *= 2;
                                }
				m = 1;
				for(int z = 0; z < 3; z++){
                                        count += (countarray[z] * m);
					m *= 2;
                                }
				unsigned char repchar = last[pos];
				writeByte(repchar);
				for(int a = 0; a < count; a++){
					writeByte(repchar);
				}
				while( count >= 0 )
				{
					for(int c = 7; c >= 0; c=c-1)
					{
						last[c] = last[c-1];
					}
					count = count -1;
					last[0] = repchar;
				}
				last[0] = repchar;
				flagVal = 0;
			}
		}
	}
	flushWriteBuffer();
	return 0; //exit status. success=0, error=-1
}
