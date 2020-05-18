//Да се напише програма на С, която получава като параметър команда (без параметри) и при успешното ѝ изпълнение, извежда на стандартния изход името на командата.

#include <stdlib.h>
#include <err.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char* argv[]){
	if(argc!=2){
		errx(1,"Invalid count of argument");
	}
	int pid=fork();
	if(pid == -1){
		err(2,"Failed to fork()");
	}

	int status;
	if(pid ==  0){ //=> child process created; this way execl replaces the child process
		if(execlp(argv[1],argv[1],0) == -1){
			err(3,"Faield to execlp %s",argv[1]);
		}
		exit(22);	
	}
	else{  
		if (wait(&status) > 0) { //suspend process until a change in the state of the child process is detected and then proceed
            		//WIFEXITED=true -> child terminated normally
			//WEXITSTATUS->returns exit status of child -> if equal to 0 then we good
			if (WIFEXITED(status) == 1 && WEXITSTATUS(status) == 0)  
				printf("%s\n",argv[1]); 
		} 
        } 

	exit(0);
}
