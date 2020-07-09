/*реализирайте simple command prompt; в цикъл да се изпълнява слеедната поредица от действия:
- извежда промпт на стандартния изход
- прочите име на командата
- изпълнява без параметри посочената команда
! командите се търсят в /bin
! Въвеждането на командата exit се смята за край на програмата
*/
#include <err.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sys/wait.h>

int main(){
	const char *prompt="[tanya@tanya-80xr test]$";
	char buff[1<<8];
	
	while(1){
		
		//print prompt
		if((res=write(1,prompt,strlen(prompt))) != strlen(prompt)){
			err(2,"Failed writing prompt");
		}
		
		//read command
		ssize_t sz=read(0,&buff,sizeof(buff));
		if(sz == -1){
			err(3,"Failed reading from stdin");
		}
		buff[sz-1]='\0';
		
		//check if exit
		if(strcmp(buff,"exit") == 0){
			exit(0);
		}
		
		//exec command
		const pid_t pid=fork();
		if(pid == -1){
			err(4,"Failed to fork");
		}

		if(pid == 0){
			if(execlp(buff,buff,0) == -1){
				err(5,"Failed to execute command");
			}
		}
		wait(NULL);
	}
	exit(0);
}
