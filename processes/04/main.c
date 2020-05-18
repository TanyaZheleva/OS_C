// Да се напише програма на C, която създава процес дете и демонстрира принцина на конкурентност при процесите.

#include <sys/types.h>
#include <unistd.h>
#include <err.h>
#include <stdlib.h>
#include <stdio.h>

int main(){
	
	int n=10;
	if(fork() > 0){
		for (int i=0; i < n; i++){
			printf("father\n");
		}
	}
	
	else{
		for(int i=0; i < n; i++){
			printf("child\n");
		}
	}
	exit(0);
}
