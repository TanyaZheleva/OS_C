/*Напишете програма, която приема точно 2 аргумента. Първият може да бъде само `--min`, `--max` или `--print` (вижте `man 3 strcmp`). Вторият аргумент е двоичен файл, в който има записани цели неотрицателни двубайтови числа (`uint16_t` - вижте `man stdint.h`). Ако първият аргумент е:
  - `--min` - програмата отпечатва кое е най-малкото число в двоичния файл.
  - `--max` - програмата отпечатва кое е най-голямото число в двоичния файл.
  - `--print` - програмата отпечатва на нов ред всяко число.*/
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <err.h>
#include <errno.h>
#include <stdio.h>

int main(int argc, char *argv[]){
	if(argc != 3){
		errx(1,"Invalid count of arguments");
	}

	if( strcmp(argv[1],"--min")!=0 && strcmp(argv[1],"--max")!=0 && strcmp(argv[1],"--print")!=0){
		errx(2,"Invalid format of first argument - accepted are:--min,--max,--print");
	}
	
	struct stat st;
	if(stat(argv[2],&st) == -1){
		errx(3,"Can't stat file");
	}

	if(st.st_size%sizeof(uint16_t)!=0){
		errx(4,"Invalid format of binary file");
	}
	
	int fd=open(argv[2],O_RDONLY);

	if(fd == -1){
		err(5,"Can't open binary file for reading");
	}

	uint16_t min;
	uint16_t max;
	uint16_t curr;
	while(read(fd,&curr,2) == 2){
				if(curr > max){
					max=curr;
				}
				if(curr < min){
					min=curr;
				}
				if(strcmp(argv[1],"--print") == 0){
					printf("%d\n",curr);
				}
	}
	close(fd);
	
	if(strcmp(argv[1],"--min")==0){
		printf("%d\n",min);
	}
	if(strcmp(argv[1],"--max")==0){
		printf("%d\n",max);
	}
	exit(0);
}
