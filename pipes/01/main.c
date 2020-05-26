/* Да се напише програма на C, която приема аргумент - име на файл. Програмата да:
        * записва във файла 'fo'
        * създава child процес, който записва 'bar\n'
        * parent-а, след като изчака child процеса, записва 'o\n'
*/

#include <unistd.h>
#include <stdlib.h>
#include <err.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char *argv[]){
	if(argc != 2){
		errx(1,"Invalid count of arguments");
	}
	int fd=open(argv[1],O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR);
	if(fd < 0){
		err(2,"Failed to open %s", argv[1]);
	}

	if(write(fd,"fo",2) != 2){
		const int _errno=errno;
		close(fd);
		errno=_errno;
		err(3,"Failed to wirte 'fo' from parent process");
	}
	
	int pid=fork();
	int status;
	if(pid == -1){
		const int _errno=errno;
		close(fd);
		errno=_errno;
		err(4,"Failed to fork()");
	}
	if(pid==0){
		if(write(fd,"bar",3) != 3){
			const int _errno=errno;
			close(fd);
			errno=_errno;
			err(5,"Failed to write 'bar' from child process");
		}
		exit(0);
	}

	if(wait(&status) == -1 ){
		const int _errno=errno;
		close(fd);
		errno=_errno;
		err(6,"Could not wait for child");
	}
	if(WIFEXITED(status) != 1){
		const int _errno=errno;
                close(fd);
                errno=_errno;
                err(7,"Child didn't terminate properly");
	}

	if( WEXITSTATUS(status) != 0){
		const int _errno=errno;
                close(fd);
                errno=_errno;
                err(8,"Child exit code not 0");
        }

	if(write(fd,"o\n",2) != 2){
		const int _errno=errno;
		close(fd);
		errno=_errno;
		err(6,"Failed to write 'o' from parent");
	}
	close(fd);
	exit(0);
}
