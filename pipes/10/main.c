/*Напишете програма-наблюдател P, която изпълнява друга програма Q и я рестартира. когато Q завърши изпънението си. 
 - Параметри: 
	 - праг за продължителност в секунди = 1-9
	 - Q
	 - параметри на Q - незадължителни
 - Алгоритъм на P:
 	 - стартира Q с подадените параметри
	 - изчаква я да завъши изпълнението си
	 - записва в текстов файл run.log един ред с три полета(цели числа разделени с интервали):
	 	- момент на стартиране на Q(Unix time)
		- момент на завършване на Q(Unix time)
		- код за грешка, с който Q е завършила(exit code)
	 - проверява дали е изпълнено условието за спиране и ако не е стартира наново Q
 - Условие за спиране:
	 - кодът за грешка на Q е бил различен от 0
	 - разликата между момента на завършване и момента на стартиране на Q е била по-малка от подадения като първи параметър на P праг
 Ако и 2те условия са изпълнени при 2 последователни извиквания на Q, то P спира цикъла и сам завършва изпълнението си.
*/
#define _POSIX_C_SOURCE 200809L
#include <err.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char *argv[]){
	
	//validations
	if(argc < 3){
		errx(1,"Invalid count of arguments");
	}

	if(strlen(argv[1]) != 1){
		errx(2,"Invalid length of time argument");
	}
	
	//get arguments for time and for command
	int length=*argv[1]-'0';
	if(length < 1 || length > 9){
		errx(3,"Invalid time argument");
	}
	char *args[12];
	int i=2;
	while(i<argc){
		args[i-2]=argv[i];
		i++;
	}
	args[i-2]='\0';
	
	//open run.log
	int fd=open("run.log",O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR);
	if(fd == -1){
		err(4,"Failed to open run.log");
	}
	
	//loop to execute
	int count=0;//0->sucess
		   //1->one unsucessfull execution of Q
		   //2->two unsucessfull executions of Q -> terminate program
	
	while(cnt<2){
		//write starting time
		time_t start=time(NULL);
		dprintf(fd,"%ld",start);
		
		//execute Q
		const pid_t pid=fork();
		int status;
		if(pid < 0){
			err(5,"Failed to fork");
		}
		if(pid==0){
			if(execvp(args[0],args) == -1){
				err(6,"Failed to execute command");
			}
		}
		//wait and write
		wait(&status);
		time_t end=time(NULL);
		dprintf(fd," %ld",end);
		dprintf(fd," %d\n",status);
		time_t diff=end-start;
		//checkint to terminate
		if(status!=0 && (int)diff < length){
			cnt++;
		}
		
		else{
			cnt=0;
		}
	}
	exit(0);
}
