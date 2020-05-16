//Реализирайте команда cp, работеща с два аргумента, подадени като входни параметри.

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <err.h>

int main(int argc, char *argv[]){
	if ( argc != 3 ){
		errx(1, "Need two files");
	}

	int fd1;
	if ( (fd1=open(argv[1],O_RDONLY)) == -1 ){
		err(2,"Failed to open file in read mode");
	}

	int fd2;
	if ( (fd2=open(argv[2],O_CREAT | O_WRONLY | O_TRUNC , S_IRUSR | S_IWUSR))== -1 ){
		err(3,"Failed to open in write mode");
		close (fd1);
	}
	
	char c[4096];
	ssize_t read_size;
	while ( (read_size=read(fd1,&c,sizeof(c))) > 0){
		if (write(fd2,&c,read_size)!=read_size){
			//int errno_save=errno;
			close (fd1);
			close(fd2);
			//errno=errno_save;
			err(4,"Failed writing");
		}
	}

	close(fd1);
	close(fd2);
}

