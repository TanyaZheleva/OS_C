/*Напишете програма, която приема точно 2 аргумента. Първият може да бъде само `--min`, `--max` или `--print` (вижте `man 3 strcmp`). Вторият аргумент е двоичен файл, в който има записани цели неотрицателни двубайтови числа (`uint16_t` - вижте `man stdint.h`). Ако първият аргумент е:
  - `--min` - програмата отпечатва кое е най-малкото число в двоичния файл.
  - `--max` - програмата отпечатва кое е най-голямото число в двоичния файл.
  - `--print` - програмата отпечатва на нов ред всяко число.*/

#include <err.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
int main(int argc, char *argv[]){
	if (argc != 3){
		errx(1,"%s requires 2 arguments.\n",argv[0]);
	}
	if ( strcmp(argv[1],"--min") != 0 &&strcmp(argv[1],"--max")!=0 && strcmp(argv[1],"--print")!=0){
		errx(2,"%s requires first argument to be --min,--max or --print.\n",argv[0]);
	}

	int fd=open(argv[2],O_RDONLY);
	if ( fd == -1){
		err(3,"File failed to open in read mode.\n");
	}

	char buff;
	short int max=30000;
	short int min=-30000;
	short int curr;

	while( read(fd,&buff,1) > 0){
		if ( buff == ':' ){
			for(int i=0;i<8;i++){
				if( read(fd,&buff,1) == -1){
					break;
				}
				if( read(fd,&curr,2) == 1){
					break;
				}
				if ( curr > max ){
					max=curr;
				}
				if( curr < min ){
					min=curr;
				}
				if( strcmp(argv[1],"--print") == 0){
					printf("%d\n",curr);
				}
			}
		}
	}
	close(fd);
	if( strcmp(argv[1],"--max") == 0){
		printf("%d\n",max);
	}
        if( strcmp(argv[1],"--min") == 0){
                printf("%d\n",min);
        }
}
