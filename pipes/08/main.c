/*Приема 0 или 1 параметера - име на команда;  ако са 0 се приема default echo,иначе името да е до 4 символа;
 - чете низове от stdin като разделители са интервал и нов ред(0x20, 0x0A), ако някой низ е по дълъг от 4 символа програмата терминира
 - низовете се приемат като параметри за командата
 - комадата да се изпълни колкото пъти е нужно, за да обработи входа
 - може да проема максимум два параметъра на изпулнение
 */

#include <err.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdio.h>

void execute(char *command,char *arg1,char *arg2){
	const pid_t pid=fork();
	if(pid == -1){
		err(4,"Fail to fork");
	}
	if(pid == 0){
		if(strcmp(arg2,"fail")==0){
			if(execlp(command,command,arg1,0) == -1){
				err(5,"Fail to exec");
			}
		}
		else{
			if(execlp(command,command,arg1,arg2,0) == -1){
				err(6,"Fail to exec 2");
			}
		}
	}
	wait(NULL);
}

int main(int argc, char *argv[]){

	char *comm="echo";
	if(argc > 2){
		errx(1,"Invalid cnt arguments");
	}

	if(argc == 2){
		if(strlen(argv[1]) > 4){
			errx(2,"Invalid length of command");
		}
		comm=argv[1];
	}
	
	char buff;
	char arg1[5];
	char arg2[5];
	int cnt=0;
	int i=0;
	int j=0;
	while(read(0,&buff,1) == 1){
		if(buff==0x20 || buff == 0x0A){
			if(i!=0){
				arg1[i]='\0';
				i=0;
			}
			if(j!=0){
				arg2[j]='\0';
				j=0;
			}
			cnt++;
			if(cnt==2){
				execute(comm,arg1,arg2);
				cnt=0;

			}
		}
		else{
			if(cnt==0){
				arg1[i]=buff;
				i++;
			}
			else if(cnt==1){
				arg2[j]=buff;
				j++;
			}
		}
		if(i > 4 || j > 4){
			errx(3,"Invalid length of argument");
		}
	}
	if(cnt!=0){
		execute(comm,arg1,"fail");
	}
	exit(0);
}
