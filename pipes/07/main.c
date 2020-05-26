//cut -d":" -f 7 /etc/passwd | sort | uniq -c | sort

#include <err.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc,char *argv[]){
	
	if(argc != 1){
		errx(1,"Intended use: %s",argv[0]);
	}

	int cut[2];
	int sort[2];
	int uniq[2];
	
	if(pipe(cut) == -1){
		err(2,"Failed to pipe cut");
	}

	const pid_t pid=fork();
	if(pid == -1){
		err(3,"Faield to fork()");
	}

	if(pid == 0){
		close(cut[0]);
		close(1);
		if(dup(cut[1]) == -1){
			close(cut[1]);
			err(4,"Failed to dub cut[1]");
		}
		if(execlp("cut","cut","-d:","-f7","/etc/passwd",0) == -1){
			close(cut[1]);
			err(5,"Failed to execlp cut");
		}
		close(cut[1]);
	}
	
	close(cut[1]);
	
	wait(NULL);
	if(pipe(sort) == -1){
		close(cut[0]);
		err(6,"Failed to pipe sort");
	}

	const pid_t pid2=fork();
	if(pid2 == -1){
		close(cut[0]);
		err(7,"Failed to fork().2");
	}
	if(pid2 == 0){
		close(1);
		close(sort[0]);
		if(dup(sort[1]) == -1){
			close(sort[1]);
			err(8,"Failed to dup sort");
		}

		close(0);
		if(dup(cut[0]) == -1){
			close(sort[1]);
			close(cut[0]);
			err(9,"Failed to dup cut[0]");
		}

		if(execlp("sort","sort",0) == -1){
			close(sort[1]);
			close(cut[0]);
			err(10,"Failed to execlp sort1");
		}

		close(cut[0]);
		close(sort[1]);
	}

	close(cut[0]);
	close(sort[1]);
	wait(NULL);

	if(pipe(uniq) == -1){
		close(sort[0]);
		err(11,"Failed to pipe uniq");
	}

	const pid_t pid3=fork();
	if(pid3 == -1){
		err(12,"Faield to ffork().3");
	}

	if(pid3 == 0){
		close(uniq[0]);
		close(0);
		if(dup(sort[0]) == -1){
			close(uniq[1]);
			close(sort[0]);
			err(13,"Failed to dup sort[0]");
		}
		close(1);
		if(dup(uniq[1]) == -1){
			close(sort[0]);
			close(uniq[1]);
			err(14,"Failed to dup uniq[1]");
		}

		if(execlp("uniq","uniq","-c",0) == -1){
			close(sort[0]);
			close(uniq[1]);
		}
		close(sort[0]);
		close(uniq[1]);
	}

	close(sort[0]);
	close(uniq[1]);
	
	wait(NULL);
	const pid_t pid4=fork();
	if(pid4 == -1){
		close(uniq[0]);
		err(15,"Faield to fork().4");
	}

	if(pid4 == 0){
		close(0);
		if(dup(uniq[0]) == -1){
			close(uniq[0]);
			err(16,"Failed to ddup uniq[0]");
		}
		if(execlp("sort","sort","-n",0) == -1){
			close(uniq[0]);
			err(17,"Faield to execlp sort2");
		}
		close(uniq[0]);
	}

	wait(NULL);
	close(uniq[0]);
	exit(0);
}
