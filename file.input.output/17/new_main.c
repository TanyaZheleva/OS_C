#include <err.h>
#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int main(int argc, char *argv[]){
	
	if(argc != 4){
		errx(1,"Invalid count of arguments");
	}

	struct stat st;
	if(stat(argv[1],&st) == -1){
		err(2,"FAiled to stat");
	}

	int sz1=st.st_size;
	struct triple{
		uint16_t pos;
		uint8_t old;
		uint8_t new;
	};

	struct triple buff;
	if(sz1 % sizeof(buff) != 0){
		errx(3,"Invalid format of first file");
	}

	if(stat(argv[2],&st) == -1){
		err(4,"Failed stat");
	}

	int sz2=st.st_size;
	if(sz2 % sizeof(uint8_t) != 0){
		errx(5,"Invalid format of second file");
	}
	int maxpos=sz2/sizeof(uint8_t);
	maxpos--;
	int fd1=open(argv[1],O_RDONLY);
	if(fd1 == -1){
		err(6,"FAiled open 1");
	}

	int fd2=open(argv[2],O_RDONLY);
	if(fd2 == -1){
		err(7,"Failed open 2");
	}

	int fd3=open(argv[3],O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR);
	if(fd3 == -1){
		err(8,"Failed to open 3");
	}
	
	int pos=0;
	char c;
	while(read(fd1,&buff,sizeof(buff)) == sizeof(buff)){
		if(buff.pos<=maxpos){
			
			while(pos<buff.pos){
				if(read(fd2,&c,sizeof(c)) != sizeof(c)){
					err(9,"FaIELD READ");
				}
				if(write(fd3,&c,sizeof(c)) != sizeof(c)){
					err(10,"FaILED write");
				}
				pos++;
			}
			if(read(fd2,&c,sizeof(c)) != sizeof(c)){
				err(11,"FAil");
			}
			c=buff.new;
			if(write(fd3,&c,sizeof(c)) != sizeof(c)){
				err(12,"FAIl");
			}
			pos++;
		}
		else{
			errx(13,"Invalid pos");
		}
	}
	
	while(read(fd2,&c,sizeof(c)) == sizeof(c)){
		if(write(fd3,&c,sizeof(c)) != sizeof(c)){
			err(14,"FAield write");
		}
	}	
	close(fd1);
	close(fd2);
	close(fd3);
	exit(0);
}
