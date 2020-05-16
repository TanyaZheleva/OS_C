//Реализирайте команда wc, с един аргумент подаден като входен параметър
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>

int main( int argc, char *argv[]){
	int fd1;
	int w=0; //word
	int l=0; //lines
	int ch=0; //characters
	char c;
	if ( argc != 2 ){
		write (2,"err\n",4);
		exit (-1);
	}

	if ( (fd1=open(argv[1],O_RDONLY)) == -1 ){
		write (2, "File failed to open in read mode\n", 33);
		exit (-1);
	}

	while ( read(fd1,&c,1) ){
		if ( c == '\n' ){
			l++;
			w++;
		}
		if ( c == ' ' ){
			w++;
		}
		ch++;		
	}
	fprintf(stdout,"letters: %d\nwords: %d\ncharacters: %d\n",l,w,ch);
	close(fd1);
	exit(0);
}
