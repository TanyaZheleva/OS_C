// Да се напише програма на C, която получава като параметри от команден ред две команди (без параметри). Изпълнява първата. Ако тя е завършила успешно изпълнява втората. Ако не, завършва с код -1.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <err.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char *argv[]){
	if(argc != 3 ){
		errx(1,"Invalid count of arguments");
	}
	int pid=fork();
	int status;
	if(pid == 0){
		if(execlp(argv[1],argv[1],0) == -1){
			err(-1,"Faied to execute first command");
		}
	}
	else{
		if(wait(&status)>0){
			if(WIFEXITED(status) == 1 && WEXITSTATUS(status) == 0){
				if(execlp(argv[2],argv[2],0) == -1){
					err(3,"Failed to execute second command");
				}
			}
		}
	}
	exit(0);
}
