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
	if(argc==1){
		//char buf [BUFSIZ];
                //setvbuf(stdin,NULL,_IOLBF,sizeof(buf));
                //fflush(stdin);
                //setvbuf(stdout,NULL,_IONBF,sizeof(buf));
		printf("in the first if\n");
		exit(0);
	}
	//if there are any arguments
	//code to number the lines
	if( strcmp(argv[1],"-n") == 0 ){	
		for( int i=2; i<=argc; i++){
			if( strcmp(argv[i], "-")){//add numbering
				printf("in the if with numbering\n");
				//char buf [BUFSIZ];
				//setvbuf(stdin,buf,_IOFBF,sizeof(buf));
			}
			else{//add numbering of lines
				printf("in the else with numbering\n");
				int fd=open(argv[i],O_RDONLY);
				if(fd < 0 ){
					err(1,"Failed to open file %s for reading",argv[i]);
				}
				
				char c[4096];
				ssize_t rdsz;
				while( (rdsz=read(fd,&c,sizeof(c))) > 0){
					if( write(1,&c,rdsz) != rdsz){
						const int _errno=errno;
						close(fd);
						errno=_errno;
						err(2,"Failed to read file %s",argv[i]);
					}
				}
				close(fd);				       
			}
		}
	}

	//without numbering the lines
	else{
		for( int i=1; i<=argc; i++){
                        if( strcmp(argv[i], "-") == 0){
				printf("in the if without numbering\n");
                                //char buf [BUFSIZ];
				//setvbuf(stdin,buf,_IOFBF,sizeof(buf));
				//printf("%s",buf);
				//setvbuf(stdout,buf,_IONBF,sizeof(buf));
                                //if(setvbuf(stdin,buf,_IOFBF,sizeof(buf)) != 0 ){
				//	err(5,"failed setvbuf");
				//}
				//setvbuf(stdout,buf,_IONBF,sizeof(buf));
                        }
                        else{
				printf("in the else without bufferring\n");
                                int fd=open(argv[i],O_RDONLY);
                                if(fd < 0 ){
                                        err(1,"Failed to open file %s for reading",argv[i]);
                                }
                                
                                char c;
                                while( read(fd,&c,sizeof(c)) > 0){
                                        if( write(1,&c,1) != 1){
                                                const int _errno=errno;
						close(fd);
                                                errno=_errno;
                                                err(2,"Failed to write file %s",argv[i]);
                                        }                 
                                }
                                close(fd);
                        }
                }
	}	

	exit(0);
}
