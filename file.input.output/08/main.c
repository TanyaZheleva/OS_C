//Koпирайте файл /etc/passwd в текущата ви работна директория и променете разделителят на копирания файл от ":", на "?"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <err.h>
#include <stdlib.h>

int main(int argc, char* argv[]){
	
	if ( argc != 1 ){
		errx(1,"%s requires no arguments.\n",argv[0]);
	}

	int fd1=open("./passwd",O_RDWR);
	if ( fd1 == -1){
		err(2,"Failed to open passwd in read mode.\n");
	}
	
	int fd2=open("temp_file", O_CREAT | O_RDWR | O_TRUNC , S_IRUSR | S_IWUSR);
	if ( fd2 == -1){
               close(fd1);
	       err(3,"Failed to open temporary file in write mode.\n");
        }

	char c;	
	while (read(fd1,&c,1) > 0){
		if ( c == ':' ){
			c='?';
		}

		if ( write(fd2,&c,1) == -1 ){
			close(fd1);
			close(fd2);
			err(4,"Failed writing.\n");
		}
	}
//new file is in tempfile
//moving it into the original file
	
	lseek(fd1,0,SEEK_SET);
	lseek(fd2,0,SEEK_SET);

	while (read(fd2,&c,1) > 0){
		if ( write(fd1,&c,1) == -1 ){
			close(fd1);
			close(fd2);
			err(5,"Failed writing in original file.\n");
		}
	}

	close(fd1);
	close(fd2);
	exit(0);
}
