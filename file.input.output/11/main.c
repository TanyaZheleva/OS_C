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
		errx(1,"Only one argument\n");
	}
	int fd1;
	if ((fd1=open(argv[1],O_RDONLY)) == -1){
		err(2,"File failed to open in read mode\n");
	}
	struct stat st;
	if(stat(argv[1],&st) == -1){
		const int _errno = errno;
                close(fd1);
                errno=_errno;
                err(3,"Failed to stat\n");
	}
	if(st.st_size == 0){ //if source file is empty
		close(fd1);
		exit(0);
	}

	int fd2;
        if ((fd2=open("sorted_file", O_CREAT | O_WRONLY | O_TRUNC, S_IWUSR | S_IRUSR))== -1){
                const int _errno = errno;
                close(fd1);
                errno=_errno;
                err(4,"Failed to open file in write mode\n");
        }
	void *buff = malloc(st.st_size);
	if (!buff){
		const int _errno = errno;
                close(fd1);
                close(fd2);
                errno=_errno;
                err(5,"No memory\n");
	}
	if ( read(fd1,buff,st.st_size) != st.st_size ){
		const int _errno = errno;
		close(fd1);
                close(fd2);
		free(buff);
                errno=_errno;
                err(6,"Failed reading\n");
	}
	qsort(buff,st.st_size,1,cmp);
	if(write(fd2,buff,st.st_size) != st.st_size){
		const int _errno = errno;
                close(fd1);
                close(fd2);
                free(buff);
                errno=_errno;
                err(7,"Failed writing\n");
	}
	free(buff);
	close(fd1);
	close(fd2);
	exit (0);
}
