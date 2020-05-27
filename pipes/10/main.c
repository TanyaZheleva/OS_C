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

#define _POSIX_C_SOURCE 200809L //for dprintf
#include <err.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>
int diff(time_t end, time_t start){

printf("start:%ld\n",start);
printf("end:%ld\n",end);
printf("pinters:%d\n",(int)end - (int)start);

printf("end-start:%ld\n",end-start);
return 0;
}

int main(int argc, char *argv[]){
	//validate arguments
	if(argc < 3){
		errx(1,"Invalid arguments");
	}
	if(strlen(argv[1]) != 1){
		errx(2,"Invalid time length");
	}
	
	int length=*argv[1]-'0';	
	if(length < 1 && length > 9){
		errx(3,"Invalid time");
	}
	
	//open fd for file run.log
	int fd=open("run.log", O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR);
	if(fd < 0){
		err(4,"Failed to opend fd for run.log");
	}
	int count=0;//0->sucess
		   //1->one unsucessfull execution of Q
		   //2->two unsucessfull executions of Q -> terminate program
	while(1){
		//write starting time 
		time_t start=time(NULL);
		dprintf(fd,"%ld ",start);
		
		//execute Q 
		const pid_t pid=fork();
		int status;
		if(pid == -1){
			err(5,"Failed to fork process");
		}
		if(pid == 0){
			if(execlp(argv[2],argv[2],0) == -1){
				err(6,"Faield to execlp");
			}
		}
		wait(&status);
		//write end time and status
		time_t end=time(NULL);
		dprintf(fd,"%ld ",end);
		dprintf(fd,"%d\n",0);
		//check if the conditions to terminate the program are fulffilled twice in a row
		if(status != 0 && diff(end,start) != (*argv[1]-'0')){
			count++;
		}
		//if not in a row start over the counting
		else{
			count=0;
		}
		//if yes exit the program
		if(count>=2){
			break;
		}
	}

	exit(0);
}
