/* приема като параметър име на директория и извежда името на най-скоро променения по съдържание файл в нея включително и нейните поддиректории.
 Използва външни shell команди през pipe()
commmands:
find <directory> -type f -printf "%T@ %p\n" | sort -n | tail -1 | cut -d" " -f2
*/
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <err.h>
#include <errno.h>

int main(int argc, char *argv[]){
	if(argc != 2){
		errx(1,"Intented use: %s <command>",argv[0]);
	}

	int find[2],sort[2],tail[2];

	if(pipe(find) == -1){
		err(2,"Failed to pipe find");
	}

	const pid_t pid1=fork();
	if(pid1 == -1){
		const int _errno=errno;
		close(find[0]);
		close(find[1]);
		errno=_errno;
		err(3,"Faield to fork()");
	}

	if(pid1 == 0){
		close(1);
		if(dup(find[1])==-1){
			const int _errno=errno;
			close(find[0]);
			close(find[1]);
			errno=_errno;
			err(4,"Faield to dup");
		}
		close(find[0]);
		if(execlp("find","find",argv[1],"-type", "f","-printf", "%T@ %p\n",0) == -1){
			const int _errno=errno;
			close(find[1]);
			errno=_errno;
			err(4,"Faield to execlp find");
		}
		close(find[1]);
	}

	wait(NULL);
	close(find[1]);
	if(pipe(sort) == -1){
		const int _errno=errno;
		close(find[0]);
		errno=_errno;
		err(5,"Faield to pipe sort");
	}

	const pid_t pid2=fork();
	if(pid2 == -1){
		const int _errno=errno;
		close(find[0]);
		close(sort[0]);
		close(sort[1]);
		errno=_errno;
		err(6,"Faild to fork().2");
	}

	if(pid2 == 0){
		close(0);
		if(dup(find[0]) == -1){
			const int _errno=errno;
			close(find[0]);
			close(sort[0]);
			close(sort[1]);
			errno=_errno;
			err(7,"Faield to dup");
		}
		close(1);
		if(dup(sort[1]) == -1){
                        const int _errno=errno;
                        close(find[0]);
                        close(sort[0]);
                        close(sort[1]);
                        errno=_errno;
                        err(7,"Faield to dup");
                }
		close(sort[0]);
		if(execlp("sort","sort","-n",0) == -1){
			const int _errno = errno;
			close(find[0]);
			close(sort[1]);
			errno=_errno;
			err(7,"Faield to execlp sort");
		}
		close(find[0]);
		close(sort[1]);
	}

	wait(NULL);
	close(find[0]);
	close(sort[1]);
	
	if(pipe(tail) == -1){
		const int _errno=errno;
		close(sort[0]);
		errno=_errno;
		err(8,"Failed to pipe tail");
	}

	const pid_t pid3=fork();
	if(pid3 == -1){
		const int _errno=errno;
		close(sort[0]);
		close(tail[0]);
		close(tail[1]);
		errno=_errno;
		err(9,"Failed to fork().3");
	}

	if(pid3 == 0){
		close(0);
		if(dup(sort[0]) == -1){
                        const int _errno=errno;
                        close(find[0]);
                        close(sort[0]);
                        close(sort[1]);
                        errno=_errno;
                        err(10,"Faield to dup");
                }

		close(1);
		if(dup(tail[1]) == -1){
                        const int _errno=errno;
                        close(find[0]);
                        close(sort[0]);
                        close(sort[1]);
                        errno=_errno;
                        err(10,"Faield to dup");
                }

		close(tail[0]);
		if(execlp("tail","tail","-n1",0) == -1){
			const int _errno=errno;
			close(sort[0]);
			close(tail[1]);
			errno=_errno;
			err(10,"Failed to execlp sort");
		}
		close(tail[1]);
		close(sort[0]);
	}

	wait(NULL);
	close(sort[0]);
	close(tail[1]);
	

	const pid_t pid4=fork();
	if(pid4 == -1){
		const int _errno=errno;
		close(tail[0]);
		errno=_errno;
		err(12,"Failed to fork().4");
	}

	if(pid4 == 0){
		close(0);
		if(dup(tail[0]) == -1){
                        const int _errno=errno;
                        close(find[0]);
                        close(sort[0]);
                        close(sort[1]);
                        errno=_errno;
                        err(13,"Faield to dup");
                }

		if(execlp("cut","cut","-d"," ", "-f2",0) == -1){
			const int _errno=errno;
			close(tail[0]);
			errno=_errno;
			err(13,"Failed to execlp cut");
		}
		close(tail[0]);
	}	
	
	wait(NULL);
	close(tail[0]);
	exit(0);
}

