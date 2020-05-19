/* Да се напише програма на C, която изпълнява последователно подадените ѝ като параметри команди, като реализира следната функционалност постъпково:
        * `main cmd1 ... cmdN` Изпълнява всяка от командите в отделен дъщерен процес.
        * ... при което се запазва броя на изпълнените команди, които са дали грешка и броя на завършилите успешно.
*/

#include <err.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[]){
	
	int pid,status;
	for ( int i=1;i<argc;i++){
		pid=fork();
		if(pid ==0){
			if(execlp(argv[i],argv[i],0) == -1){
				warn("Failed to execute %s",argv[i]);
				exit(1);
			}
		}
		else{
			wait(&status);
		}
		
	}
	
	exit(0);
}
