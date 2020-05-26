//Напишете програма на C, която демонстрира употребата на dup/dup2 между parent и child процеси. Parent-ът трябва да изпраща стринга, получен като първи аргумент на командния ред към child-а, където той да може да се чете от stdin. Child процесът да изпълнява`wc -c`.

#include <err.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

int main(int argc, char *argv[]){
	
	if(argc != 2 ){
		errx(1,"Intended use: %s <string>", argv[0]);
	}

	int pipefd[2];
	if(pipe(pipefd) == -1){
		err(2,"Failed to create a pipe");
	}

	const pid_t pid = fork();
	if(pid == -1){
		err(3,"Failed to fork()");
	}

	if (pid == 0){
		close(pipefd[1]);
		close(0);
		dup(pipefd[0]);
		if(execlp("wc","wc","-c",0) == -1){
			err(4,"Failed to execlp");
		}
	}
	
	
	close(pipefd[0]);
	size_t res;
	if((res=write(pipefd[1],argv[1],strlen(argv[1]))) != strlen(argv[1])){
		const int _errno=errno;
		close(pipefd[1]);
		errno=_errno;
	}
	
	close(pipefd[1]);
	wait(NULL);
	exit(0);
}
