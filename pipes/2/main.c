//Напишете програма на C, която демонстрира комуникация през pipe между parent и child процеси. Parent-ът трябва да изпраща стринга, получен като първи аргумент на командния ред към child-а, който да го отпечатва на стандартния изход.

#include <err.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
	if(argc != 2){
		errx(1,"Intended use: %s <string>",argv[0]);
	}

	int pipefd[2];
	if(pipe(pipefd) == -1){
		err(2,"Failed to open pipe");
	}
	
	int pid=fork();
	if(pid == -1 ){
		err(3,"Failed to fork()");
	}

	if(pid == 0){
	       close(pipefd[1]);
	       char buf;
	       while(read(pipefd[0],&buf,sizeof(buf)) == sizeof(buf)){
		       if(write(1,&buf,sizeof(buf)) != sizeof(buf)){
			       const int _errno=errno;
			       close(pipefd[0]);
			       errno=_errno;
			       err(4,"Child failed to write to stdout");
		       }
	       }
	       close(pipefd[0]);
	       exit(0);
	}

	close(pipefd[0]);
	size_t res;
	if((res=write(pipefd[1],argv[1],strlen(argv[1]))) != strlen(argv[1]) ){
		const int _errno=errno;
		close(pipefd[1]);
		errno=_errno;
		err(5,"Failed to write to pipe");
	}
	close(pipefd[1]);
	wait(NULL);
	exit(0);
}
