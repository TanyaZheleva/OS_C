/*Приема 0 или 1 параметера - име на команда;  ако са 0 се приема default echo,иначе името да е до 4 символа;
 - чете низове от stdin като разделители са интервал и нов ред(0x20, 0x0A), ако някой низ е по дълъг от 4 символа програмата терминира
 - низовете се приемат като параметри за командата
 - комадата да се изпълни колкото пъти е нужно, за да обработи входа
 - може да проема максимум два параметъра на изпулнение
 */

#include <stdlib.h>
#include <stdio.h>
#include <err.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void execute( char *comm, char *arg){
	const pid_t pid=fork();
	if(pid == -1){
		err(4,"Faield to fork()");
	}
	if(pid == 0){
		if(execlp(comm,comm,arg,0) == -1){
			err(5,"Failed to execlp");
		}
	}
	wait(NULL);
}

int main(int argc, char *argv[]){

	char *command="echo";
	if( argc > 2 ){
		errx(1,"Invalid arguments");
	}

	if( argc == 2){
		if(strlen(argv[1]) > 4){
			errx(2,"Invalid command");
		}
		command=argv[1];
	}
	char buff;
	char argument[4];
	int i=0; // 0<=i<=3
	while(read(0,&buff,1) == 1){
		if(buff == 0x20 || buff == 0x0A){
		
			i=0;
			execute(command,argument);
		}
		else{	
			argument[i]=buff;
			i++;
		}
		if(i > 4){
			errx(3,"Argument is too long");
		}
	}	
	
	exit(0);
}
