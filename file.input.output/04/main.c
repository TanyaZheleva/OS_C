//Реализирайте команда swap, разменяща съдържанието на два файла, подадени като входни параметри.
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <err.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

int main (int argc, char *argv[]){
	if ( argc != 3 ){
		errx(1,"err\n");
	}

	int fd1;
	int fd2;
	int fd3;
	
	if ( (fd1=open(argv[1],O_RDWR)) == -1){
		err(2,"File %s failed to open in read mode\n",argv[1]);
	}

	if ((fd2=open(argv[2],O_RDWR)) == -1 ){
		close(fd1);
		err(3,"File %s failed to open in read mode\n",argv[2]);
	}

	if (( fd3=open("my_temp_file",O_CREAT | O_RDWR | O_TRUNC, S_IWUSR | S_IRUSR)) == -1 ){
		close (fd1);
		close (fd2);
 		err (4, "File %s failed to open in read mode\n","my_temp_file");	
	}
	
	char c [4096];
	ssize_t read_size;

	while ( (read_size=read ( fd1,&c,sizeof(c))) > 0){
		if ( (write(fd3,&c,read_size)) != read_size){
			close(fd1);
			close(fd2);
			close(fd3);
			err(5,"Error while writing\n");
		}
	}
	
	lseek(fd1,0,SEEK_SET);

	while ((read_size=read(fd2,&c,sizeof(c))) > 0){
		if (write(fd1,&c,read_size) != read_size){
                        close(fd1);
                        close(fd2);
                        close(fd3);
                        err(5,"Error while writing\n");
                }
        }
	
	lseek(fd2,0,SEEK_SET);
	lseek(fd3,0,SEEK_SET);

	while (( read_size=read(fd3,&c,sizeof(c))) > 0){
		if (write(fd2,&c,read_size)!=read_size){
			close(fd1);
			close(fd2);
			close(fd3);
			err(5,"Error while writing\n");
		}
	}

	close(fd1);
	close(fd2);
	close(fd3);

	exit(0);
}
