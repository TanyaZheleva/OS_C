//приема име на текстови файл като параметър, извежда съдържанието му сортирано чрез pipe()( sort | cat )

#include <stdlib.h>
#include <unistd.h>
#include <err.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <string.h>

int main( int argc, char *argv[]){

	if(argc != 2){
		errx(1,"Intenede use: %s <filename of a text file>", argv[0]);
	}

	int pipefd[2];
	if(pipe(pipefd) == -1 ){
		err(2,"Failed to create a pipe");
	}

	const pid_t pid=fork();
	if(pid == -1){
		err(3,"Failed to fork()");
	}

	if(pid == 0){
		close(0);
		dup(pipefd[0]);
		if(execlp("sort","sort",argv[1],0) == -1){
			err(4,"Failed to execlp sort");
		}		
	}
	
	
	
	const pid_t pid2=fork();
	if(pid2 == -1){
		err(5,"Failed to fork()");
	}

	if( pid2 == 0){
		close(0);
		dup(pipefd[0]);
		if(execlp("cat","cat",0) == -1){
			err(6,"Failed to execlp cat");
		}
	}

	close(pipefd[0]);
	close(pipefd[1]);
	wait(NULL);
	exit(0);
}
