/* приема 3 параметъра - patch.bin f1.bin f2.bin, двойчни файлове
f1.bin f2.bin - състоящи се от uint8_t числа
patch.bin - състои се от наредени тройки:
- отместване uint16_t
- оригинален байт uint8_t
- нов байт uint8_t

по patch.bin от f1.bin да се създаде f2.bin
ако не е описана промяна по patch.bin байта се записва директно
ако е дадено отместване, на което не съществува байт в f1.bin се ппрекратява изпълнението 
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <err.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[]){
	if(argc != 4){
		errx(1,"Invalid count of arguments");
	}

	struct stat st;
	if(stat(argv[1],&st) < 0){
		err(2,"Failed to stat file %s", argv[1]);
	}
	off_t fsize=st.st_size;
	
	if(fsize%sizeof(uint8_t) != 0){
		err(3,"Invalid format of file %s", argv[1]);
	}

	if(stat(argv[2],&st) <0){
		err(4,"Failed to stat file %s",argv[2]);
	}
	off_t psize=st.st_size;
	
	struct triple{
		uint16_t pos;
		uint8_t og;
		uint8_t new;
	};

	struct triple a;
	if(psize%sizeof(a) != 0){
                err(5,"Invalid format of file %s", argv[2]);
        }
	uint32_t count_triples=psize/sizeof(a);

	int fd1=open(argv[1],O_RDONLY);
	if(fd1 < 0){
		err(6,"Failed to open %s", argv[1]);
	}

	int fdp=open(argv[2],O_RDONLY);
	if(fdp < 0){
		const int _errno=errno;
		close(fd1);
		errno=_errno;
		err(7,"Failed to open %s", argv[2]);
	}

	lseek(fdp,((count_triples -1)*sizeof(a)),SEEK_SET);
	if(read(fdp,&a,sizeof(a)) < 0){
		const int _errno=errno;
                close(fd1);
		close(fdp);
                errno=_errno;
                err(8,"Failed to read");
        }
	
	if(a.pos>= fsize){
                const int _errno=errno;
                close(fd1);
                close(fdp);
                errno=_errno;
                err(9,"%s is too short",argv[1]);
        }
	lseek(fdp,0,SEEK_SET);
	int fd2=open(argv[3],O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR);
	if(fd2 < 0){
		const int _errno=errno;
                close(fd1);
                close(fdp);
                errno=_errno;
                err(10,"Failed to open %s",argv[3]);
        }

	uint16_t curpos=0;
	uint8_t c;
	while(read(fdp,&a,sizeof(a)) > 0){
		while(curpos<a.pos){
			if(read(fd1,&c,sizeof(c)) < 0){
				const int _errno=errno;
                                close(fd1);
                                close(fdp);
                                close(fd2);
                                errno=_errno;
                                err(11,"Failed to read file %s", argv[1]);
                        }
			if(write(fd2,&c,sizeof(c)) < 0){
				const int _errno=errno;
				close(fd1);
				close(fdp);
				close(fd2);
				errno=_errno;
				err(12,"Failed to write to file %s", argv[3]);
			}
			curpos++;
		}
		
		if(curpos==a.pos){
			if(read(fd1,&c,sizeof(c)) < 0){
                                const int _errno=errno;
                                close(fd1);
                                close(fdp);
                                close(fd2);
                                errno=_errno;
                                err(11,"Failed to read file %s", argv[1]);
                        }
			if(c==a.og){
				c=a.new;
				if(write(fd2,&c,sizeof(c)) < 0){
         	                       const int _errno=errno;
                	                close(fd1);
                        	        close(fdp);
                        	        close(fd2);
                        	        errno=_errno;
                        	        err(13,"Failed to write to file %s", argv[3]);
				}
			}
		}
		curpos++;
	}
	
	while(read(fd1,&c,sizeof(c)) > 0){
		if(write(fd2,&c,sizeof(c)) < 0){
			const int _errno=errno;
			close(fd1);
			close(fd2);
			close(fdp);
			errno=_errno;
			err(14,"Failed to write to file %s", argv[3]);
		}
	}


	close(fd1);
	close(fdp);
	exit(0);
}
