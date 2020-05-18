/* Да се напише програма на С, която получава като параметри три команди (без параметри), изпълнява ги последователно, като изчаква края на всяка и извежда на стандартния изход номера на завършилия процес, както и неговия код на завършване.
*/

#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[]){

	if(argc != 4){
		errx(1,"Invalid count of arguments");
	}
	int status;
	int pid;
	for( int i=1;i<argc;i++){
		pid=fork();
		if(pid == -1){
			err(2,"Faield to fork ");
		}

		if(pid==0){
			if(execlp(argv[i],argv[i],0) == -1){
				err(3,"Failed to execl");
			}
		}
		else{
			if(wait(&status) > 0){
				if (WIFEXITED(status) == 1 && WEXITSTATUS(status) == 0){
					printf("%d  : %d\n",pid,status);
				}
			}
		}
	}
	exit(0);
}

