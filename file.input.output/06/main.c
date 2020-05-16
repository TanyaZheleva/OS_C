//Реализирайте команда cat, работеща с произволен брой подадени входни параметри.
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <err.h>
#include <unistd.h>
#include <stdlib.h>
int main(int argc, char *argv[]){
	int fd;
	int i=1;
	char c[4096];
	ssize_t read_size;
	while ( i < argc ){
		if ((fd=open(argv[i],O_RDONLY)) == -1 ){
			err(i,"Failed to open file in read mode\n");
		}
		while((read_size=read(fd,&c,sizeof(c))) > 0){
			if (write(1,&c,read_size) !=read_size){
				close(fd);
				err(-1,"Failed to write on stdout\n");	
			}

		}
		i++;
		close(fd);
	}
	exit(0);
}
