/*имплементирайте командата cat, която приема опционален параметър за опция -n и опционално неоределен брой имена на файлове;
-n номерира всеко ред като започва от 1
извежда на STDOUT 
ако няма имена на файлове се приема STDIN
име на файл - се приема за STDIN
при липса на подадена опция просто се извежда съдържанието на файловете последователно
*/
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <err.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
	int counting=0;
	int i=1;
	
	if(argc != 1 && strcmp(argv[1],"-n") == 0){
		counting=1;
		i++;
	}
	
	while(i<argc || argc==1 || (argc==2 && counting == 1) ){
		int fd=0;
		
		if(argc != 1 && ((counting==1 && argc!=2) || counting == 0 ) &&  strcmp(argv[i],"-") != 0){
			fd=open(argv[i],O_RDONLY);
			if(fd == -1){
				err(10,"Fail open");
			}
		}
		char c;
		if(counting == 1){
			int lines=1;
			int prnl=1;
			while(read(fd,&c,sizeof(c)) == sizeof(c)){
				if(prnl == 1){
					setbuf(stdout,NULL);
					fprintf(stdout,"%02d",lines);
					lines++;
					prnl=0;	
				}					
				if(write(1,&c,sizeof(c)) != sizeof(c)){
					err(i*3,"Fail wrritii");
				}
				if(c=='\n'){
					prnl=1;
				}
			}
		}
		else{
			while(read(fd,&c,sizeof(c)) == sizeof(c)){
				if(write(1,&c,sizeof(c))!=sizeof(c)){
					err(i*2,"Failed to write");
				}
			}
		}

		if(fd != 0){
			close(fd);
		}

		i++;
	}
	exit(0);
}
