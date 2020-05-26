/*реализирайте simple command prompt; в цикъл да се изпълнява слеедната поредица от действия:
- извежда промпт на стандартния изход
- прочите име на командата
- изпълнява без параметри посочената команда
! командите се търсят в /bin
! Въвеждането на командата exit се смята за край на програмата
*/
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <err.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
int main(){
	 
	const char *prompt="tanya@tanya:~$";
	int move=0;
	char buff;
	int fd=open("temp_file",O_CREAT | O_TRUNC | O_RDWR, S_IRUSR | S_IWUSR);
	while (1){
		//print prompt
		if(write(1,prompt,14) != 14){
			err(1,"Failed to write prompt to stdout");
		}
		
		//read command ant write it to buffer
		while(read(0,&buff,1) == 1){
			if(buff=='\n'){
				lseek(fd,(-1)*move,SEEK_CUR);
				char *buffer=malloc(move);
				if(read(fd,buffer,move)!=move){
					err(7,"Failed reading");
				}
				//if the command enterd was exit -> exit
				if(strcmp(buffer,"exit") == 0){
					free(buffer);
					close(fd);
					exit(123);
				}

				//execute command	
				const pid_t pid=fork();
				if(pid == -1){
					err(2,"Failed to fork()");
				}
				if(pid == 0){//created child process
					if(execlp(buffer,buffer,0) == -1){
						err(3,"Failed to execlp");
					}
				}
				//wait for child prrocess to end
				wait(NULL);
				//prepare fo next cycle	
				free(buffer);
				move=0;
				break;
			}
			
			else{
				write(fd,&buff,1);
				move++;
			}
		}

	/*	int pipefd[2];
		if(pipe(pipefd) == -1){
			err(2,"Failed to create pipe");
		}
		
		char buff;

		const pid_t pid=fork();
		if(pid == -1){
			err(3,"Failed to fork()");
		}
		if(pid == 0){
			//close(0);
			//dup(pipefd[0]);
			close(1);
			dup(pipefd[1]);
			while(read(0,&buff,1) == 1){
				
		}
		
		//close(1);
		//dup(piefd[1]);
		while(read(0,&buff,1) == 1){
			if(write(pipefd[1],&buff,1) != 1){
				err(5,"Failed to write to pipe");
			}
		}
		*/
	}
}
