// Да се напише програма на C, която получава като параметри от команден ред две команди (без параметри) и име на файл в текущата директория. Ако файлът не съществува, го създава. Програмата изпълнява командите последователно, по реда на подаването им. Ако първата команда завърши успешно, програмата добавя нейното име към съдържанието на файла, подаден като команден параметър. Ако командата завърши неуспешно, програмата уведомява потребителя чрез подходящо съобщение.

#include <err.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

int main(int argc, char *argv[]){

	if(argc != 4){
		err(1,"Invalid count of arguments");
	}

	int fd = open(argv[argc-1],O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR);
	int pid,status;

	for(int i=1;i<argc-1;i++){
		pid=fork();
		if(pid == -1){
			err(2,"Faield to fork()");
		}
		if(pid == 0){
			if(execlp(argv[i],argv[i],0) == -1){
				warn("Faield to execute %s", argv[i]);
				exit(222);
			}
		}
		else{
			if(wait(&status) > 0){
				if(WIFEXITED(status) == 1 && WEXITSTATUS(status) == 0){
					size_t res;
					if( (res=write(fd,argv[i],strlen(argv[i]))) != strlen(argv[i])){
						err(3,"failed to write to file");
					}
				}
			}
		}
	}

	close(fd);
	exit(0);
}
