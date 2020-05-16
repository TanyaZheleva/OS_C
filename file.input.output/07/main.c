//Реализирайте команда cp, работеща с произволен брой подадени входни параметри.
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <err.h>

int main(int argc, char *argv[]){
	int i=1;
	int fdsrc;
	int fddest;
	int index_dest=argc-1;
	char c [4096];
	ssize_t read_size;
	if ((fddest=open(argv[index_dest],O_CREAT | O_WRONLY | O_TRUNC , S_IRUSR | S_IWUSR)) == -1){
		err(1,"Failed to open destination file in write mode\n");
	}

	while ( i < index_dest ){
		if( (fdsrc=open(argv[i],O_RDONLY)) == -1){
			err(2,"File failed to open in read mode\n");
		}

		while( (read_size=read(fdsrc,&c, sizeof(c))) > 0){
			if( write(fddest,&c,read_size) != read_size){
				close(fdsrc);
				err(3,"File failed to write\n");
			}
		close(fdsrc);
		i++;
		}
	}

	close(fddest);
	exit(0);
}
