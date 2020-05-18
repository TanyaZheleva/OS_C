//Да се напише програма на C, която изпълнява команда sleep (за 60 секунди).

#include <stdlib.h>
#include <err.h>
#include <unistd.h>

int main(){
	
	if(execl("/bin/sleep","sleep","60",(char *)NULL) == -1){
		err(1,"Failed to execl");
	}
	exit(0);
}
