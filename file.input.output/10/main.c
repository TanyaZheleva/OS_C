/*Напишете програма, която приема точно 2 аргумента. Първият може да бъде само `--min`, `--max` или `--print` (вижте `man 3 strcmp`). Вторият аргумент е двоичен файл, в който има записани цели неотрицателни двубайтови числа (`uint16_t` - вижте `man stdint.h`). Ако първият аргумент е:
  - `--min` - програмата отпечатва кой е най-малкия байт.
  - `--max` - програмата отпечатва кой е най-големия байт.
  - `--print` - програмата отпечатва на нов ред всеки байт.*/
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
		errx(1,"Fail");
	}

	if( strcmp(argv[1],"--min")!=0 && strcmp(argv[1],"--max")!=0 && strcmp(argv[1],"--print")!=0){
		errx(2,"Fail");
	}
	
	struct stat st;
	if(stat(argv[2],&st) == -1){
		errx(3,"Fail");
	}

	if(st.st_size%sizeof(uint16_t)!=0){
		errx(4,"Fail");
	}
	
	int fd=open(argv[2],O_RDONLY);

	if(fd == -1){
		err(5,"Fail");
	}

	uint8_t min;
	uint8_t max;
	uint8_t curr;
	while(read(fd,&curr,1) == 1){
				if(curr > max){
					max=curr;
				}
				if(curr < min){
					min=curr;
				}
				if(strcmp(argv[1],"--print") == 0){
					printf("%x\n",curr);
				}
	}
	close(fd);
	
	if(strcmp(argv[1],"--min")==0){
		printf("%x\n",min);
	}
	if(strcmp(argv[1],"--max")==0){
		printf("%x\n",max);
	}
	exit(0);
}
