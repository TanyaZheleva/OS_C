//Да се напише програма на C, която изпълнява команда ls с точно един аргумент.

#include <unistd.h>
#include <stdlib.h>
#include <err.h>

int main(int argc, char *argv[]){
	if(argc != 2){
		errx(1,"Invalid count of arguments");
	}

	if(execl("/bin/ls","ls",argv[1],(char*)NULL) == -1){
		err(2,"Failed to execl");
	}

	exit(0);
}
