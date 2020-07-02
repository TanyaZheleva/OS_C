//сортирай бинарен файл подаден като аргумент
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <err.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

int cmp(const void *a, const void *b){
	return *((unsigned char*)a) - *((unsigned char*) b);
}

int main(int argc, char *argv[]){
	
	if( argc !=2 ){
		errx(1,"Only one argument");
	}
	
	struct stat st;
	if(stat(argv[1],&st) == -1){
                err(2,"Failed to stat\n");
	}
	if(st.st_size == 0){
		exit(0);
	}
	
	int fd;
	if ((fd=open(argv[1],O_RDWR)) == -1){
		err(3,"File failed to open");
	}
	
	void *buff = malloc(st.st_size);
	if (!buff){
		const int _errno = errno;
                close(fd1);
                close(fd2);
                errno=_errno;
                err(4,"No memory\n");
	}
	if ( read(fd,buff,st.st_size) != st.st_size ){
		const int _errno = errno;
		close(fd);
		free(buff);
                errno=_errno;
                err(5,"Failed reading\n");
	}
	qsort(buff,st.st_size,1,cmp);
	if(write(fd,buff,st.st_size) != st.st_size){
		const int _errno = errno;
                close(fd);
                free(buff);
                errno=_errno;
                err(6,"Failed writing\n");
	}
	free(buff);
	close(fd);
	exit (0);
}
