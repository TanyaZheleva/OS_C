// Да се напише програма на С, която получава като параметър име на файл. Създава процес син, който записва стринга `foobar` във файла (ако не съществува, го създава, в противен случай го занулява), след което процеса родител прочита записаното във файла съдържание и го извежда на стандартния изход, добавяйки по един интервал между всеки два символа.
//the printing of pid's is to check if the conditions which process to do what are correctly executed

#include <err.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdio.h>

int main(int argc, char *argv[]){
	if(argc != 2){
		errx(1,"invalid count of arguments");
	}
	//printf("parent: %d\n",getpid());
	//printf("parent of parent: %d\n",getppid());
	int pid=fork();
	if(pid == -1){
		err(2,"Failed to fork()");
	}
	int status;
	size_t wsize;
	if(pid==0){ // created child process 
		//printf("child: %d\nparent: %d\n",getpid(),getppid());
                int fd=open(argv[1],O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR);
                if(fd < 0){
                        err(3,"Child process failed to open %s",argv[1]);
                }

                if((wsize=write(fd,"foobar\n",7)) != 7 ){
                        err(4,"Child process failed to write to %s",argv[1]);
                }
		close(fd);
	}
	else{
		if(wait(&status) > 0){
			if (WIFEXITED(status) == 1 && WEXITSTATUS(status) == 0){
				//printf("parent: %d\nparent of parent:%d\n",getpid(),getppid());
				int fd=open(argv[1],O_RDONLY);
				if(fd < 0){
					err(5,"Parent process failed to open %s",argv[1]);
				}
				char buff[7];
				if((wsize=read(fd,buff,sizeof(buff))) != 7 ){
					err(6,"Parent process failed to read %s",argv[1]);
				}
				close(fd);
			}
		}
	}

	exit(0);
}

