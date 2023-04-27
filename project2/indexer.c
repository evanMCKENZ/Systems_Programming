#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
/* You must use this struct when constructing records. It is
 * expected that all binary data in the index file will follow
 * this format. Do not modify the struct.
 */
struct record_descriptor
{
	int position; //byte offset relative to the beginning of the data file
	int length;   //length of record in bytes
};

/* main - indexer
 * Develop a single standalone program called indexer which creates an 
 * index file for a text file whose title is supplied as an argument to 
 * the program. This program should read the text file from beginning 
 * till end, find the beginning and ending of each line (ending with 
 * the newline character), create a descriptor and write the descriptor 
 * to the created index file. The program should not modify the text 
 * file which is supplied as an argument.
 * 
 * "make indexer" will compile this source into indexer
 * To run your program: ./indexer <some_text_file>
 */
int main(int argc, char *argv[]){
	/* The index file for record i/o is named by following the 
	 * convention .rinx.<data file name> (i.e., if the data file
	 * is named myfile.txt, its index will be .rinx.myfile.txt). 
	 * This convention conveniently hides the index files while
	 * permitting easy access to them based on the data file name. 
	 */
	char str[] = ".rinx.";
	char* buffer = malloc(6 + strlen(argv[1]));
        strcpy(buffer, str);
        strcat(buffer, argv[1]);


	int index = open(buffer, O_RDWR | O_CREAT, 0600);
	int count;
	int size;

	int data = open(argv[1], O_RDONLY, 0600);
	char ch;
	int offset = 0;
	int endcount = 0;

	int seek1 = lseek(index, 0, SEEK_SET);
	int seek2 = lseek(data, 0, SEEK_SET);

	if( (seek1 == -1) | (seek2 == -1) )
	{
		return -1;
	}
	while((count = read(data, &ch, 1)) >= 0)
	{
		if((ch == '\n'))
		{
			struct record_descriptor new;
			new.length = size;
			new.position = offset;
			printf("Here are the values for that entry - Length %d, Offset %d \n", size, offset);
			write(index, &new, sizeof(struct record_descriptor));
			printf("%d, %d\n", new.position, new.length);
			offset = offset + size;
			size = 0;
		}
		else if((count == 0) & (endcount == 0))
		{
			struct record_descriptor new;
                        new.length = size;
                        new.position = offset;
                        printf("Here are the values for that entry - Length %d, Offset %d \n", size, offset);
                        write(index, &new, sizeof(struct record_descriptor));
                        printf("%d, %d\n", new.position, new.length);
                        offset = offset + size;
                        size = 0;
			endcount = endcount + 1;
		        close(data);
		        close(index);
        		return 0;
		}
		else
		{
			size += 1;
		}
	}

	close(data);
	close(index);
	return 0;
}
