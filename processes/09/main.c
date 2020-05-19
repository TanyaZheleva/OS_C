/*Да се напише програма на C, която създава файл в текущата директория и генерира два процесa, които записват низовете `foo` и `bar` в създадения файл.
        * Програмата не гарантира последователното записване на низове.
        * Променете програмата така, че да записва низовете последователно, като първия е `foo`.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <err.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(){
	printf("0.parent of parent: %d\nparent: %d\n",getppid(),getpid());
	int fd=open("file",O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR);
	if(fd < 0){
		err(1,"Failed to create file in current directory");
	}
	//close(fd);

	int pid=fork();
	int status;
	if(pid==0){
		printf("1.parent: %d\nfirst child: %d\n",getppid(),getpid());	
		if(write(fd,"foo\n",4) != 4){
			err(3,"First Child failed to write to file");
		}
	}
	else{
		if(wait(&status) > 0){
			if(WIFEXITED(status) == 1 && WEXITSTATUS(status)==0){
				int _pid=fork();
				int _status;
				if(_pid == 0){
					printf("2.parent: %d\nsecond child: %d\n",getppid(),getpid());
					if(write(fd,"bar\n",4) != 4 ){
						err(5,"Second child failed to write to file");
					}
				}
				else{
					if(wait(&_status) > 0){
						if(WIFEXITED(_status) == 1 && WEXITSTATUS(_status) == 0){
							close(fd);
							printf("4.(0).parent of parent: %d\nparent: %d\n",getppid(),getpid());
						}
					}
				}
			}
		}
	}


	exit(0);
}
