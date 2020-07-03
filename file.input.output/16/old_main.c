/*имплементирайте командата cat, която приема опционален параметър за опция -n и опционално неоределен брой имена на файлове;
-n номерира всеко ред като започва от 1
извежда на STDOUT 
ако няма имена на файлове се приема STDIN
име на файл - се приема за STDIN
при липса на подадена опция просто се извежда съдържанието на файловете последователно
*/

#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

int main(int argc, char *argv[]){
	//if no arguments are given or the only argument is '-' -> read stdin and print on stdout
	if(argc==1 || (argc == 2 && strcmp(argv[1], "-") == 0)){
		char c;
		while(read(0,&c,sizeof(c)) > 0){
			if(write(1,&c,sizeof(c)) != sizeof(c)){
				err(1,"Failed to write to stdout");
			}
		}
		exit(0);
	}
	
	//if the only argument is -n -> read stdin, numerate line and print on stdout
	if(argc == 2 && strcmp(argv[1], "-n") == 0){
		char c;
		int newline=1;
		int line=1;//count lines
		while(read(0,&c,sizeof(c)) > 0){
			if(newline == 1){
				setbuf(stdout,NULL);
				fprintf(stdout,"%02d",line);
				newline=0;
			}
			if(c == '\n'){
				newline=1;
				line++;
			}
			if(write(1,&c,sizeof(c)) != sizeof(c)){
				err(2,"Failed to write to stdout");
			}
		}
		exit(0);
	}
	
	//if there is  one argument (and its not -n) or more 
	//check for numbeerring of lines
	const char n = (strcmp(argv[1],"-n") == 0) ? 2 : 1; // if -n is set n is 2 because the first argument is -n
	int rd;//read from
	int numline=1;//count lines
	int wd =1;//write to
	char c;//buffer
	int check=1;//in case -n is set; to find newlines
	for (int i=n;i<argc;i++){
		if(strcmp(argv[i],"-") == 0){
			rd=0;//read from stdin
			wd=1;//write to stdout
		}
		else{
			rd=open(argv[i],O_RDONLY);
			if(rd<0){
				err(1,"File %s failed to open",argv[i]);
			}
		}

		if (n==1){
			while(read(rd,&c,sizeof(c)) > 0){
				if(write(wd,&c,sizeof(c)) < 0){
					const int _errno=errno;
					close(rd);
					close(wd);
					errno=_errno;
					err(2,"Failed to write");
				}
			}
			close(rd);
			close(wd);
		}
		else{//print with numbering
			while(read(rd,&c,sizeof(c)) > 0){
				if(check == 1){
					setbuf(stdout,NULL);
					fprintf(stdout,"%02d",numline);
					check=0;
				}

				if( c == '\n' ){
					check=1;
					numline++;
				}

				if(write(wd,&c,sizeof(c)) != sizeof(c)){
					const int _errno=errno;
					close(rd);
					errno=_errno;
					err(5, "Failed writing to stdout");
				}
			}
		}
	}
	
	exit(0);
}
