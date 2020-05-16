/* приема два параметъра (./main input.bin output.bin), двоични файлове, състоящи се от uint16_t числа
 - input.bin може да съдържа макс 65535 числа (=> KB)
 - output.bin се създава и съдържа числата от input.bin, сортирани във възходящ ред
 - endianness-ът на машината, създала файла input.bin е същия като на текущата машина
!- да се използват не повече от 256 KB RAM & 2 MB дисково ппространство 
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <err.h>
#include <errno.h>
#include <stdlib.h>
#include <stdint.h>

int cmp(const void *a, const void *b){
	if(*((uint16_t*)a) > *((uint16_t*)b)){
		return 1;
	}
	else if(*((uint16_t*)a) < *((uint16_t*)b)){
		return -1;
	}
	return 0;
}

int main(int argc,char* argv[]){
	if(argc != 3){
		errx(1,"Invalid count of arguments");
	}

	struct stat st;
	if(stat(argv[1],&st) != 0){
		err(2,"Failed to stat %s",argv[1]);
	}
	off_t size=st.st_size;
	if(size % sizeof(uint16_t) != 0){
		err(3,"%s doesnt contain uint16_t numbers",argv[1]);
	}
	uint32_t cel=size/sizeof(uint16_t); //count elements
	
	int fd1=open(argv[1],O_RDONLY);
	if( fd1 < 0){
		err(4,"Failed to open %s", argv[1]);
	}

	uint32_t lc=cel/2;//first half of elements
	void *buff=malloc(lc*sizeof(uint16_t));
	if(!buff){
		const int _errno=errno;
		close(fd1);
		errno=_errno;
		err(5,"Failed to allocate memory");
	}	
	
	size_t res=read(fd1,buff,lc*sizeof(uint16_t));
	if(res != lc*sizeof(uint16_t)){
		const int _errno=errno;
		close(fd1);
		free(buff);
		errno=_errno;
		err(6,"Failed to read from %s",argv[1]);
	}

	qsort(buff,lc,sizeof(uint16_t),cmp);
	int fdt1=open("temp_file_1",O_CREAT | O_TRUNC | O_RDWR, S_IRUSR | S_IWUSR);
	if(fdt1<0){
		const int _errno=errno;
		close(fd1);
		free(buff);
		errno=_errno;
		err(7,"Failed to open temp file 1");
	}

	res=write(fdt1,buff,lc*sizeof(uint16_t));
	if(res != lc*sizeof(uint16_t)){
		const int _errno=errno;
                close(fd1);
		close(fdt1);
                free(buff);
                errno=_errno;
                err(8,"Failed to write to temp file 1");
        }
	free(buff);
	
	uint32_t rc=cel-lc;
	void *rbuff=malloc(rc*sizeof(uint16_t));
        if(!rbuff){
                const int _errno=errno;
                close(fd1);
                errno=_errno;
                err(9,"Failed to allocate memory");
        }

        res=read(fd1,rbuff,rc*sizeof(uint16_t));
        if(res != rc*sizeof(uint16_t)){
                const int _errno=errno;
                close(fd1);
                free(rbuff);
                errno=_errno;
                err(10,"Failed to read from %s",argv[1]);
        }

        qsort(rbuff,rc,sizeof(uint16_t),cmp);
        int fdt2=open("temp_file_2",O_CREAT | O_TRUNC | O_RDWR, S_IRUSR | S_IWUSR);
        if(fdt2<0){
                const int _errno=errno;
                close(fd1);
		close(fdt1);
                free(rbuff);
                errno=_errno;
                err(11,"Failed to open temp file 1");
        }

        res=write(fdt2,rbuff,rc*sizeof(uint16_t));
        if(res != rc*sizeof(uint16_t)){
                const int _errno=errno;
                close(fd1);
		close(fdt1);
                close(fdt2);
                free(rbuff);
                errno=_errno;
                err(12,"Failed to write to temp file 1");
        }
	free(rbuff);
	close(fd1);
	
	int fd2=open(argv[2],O_CREAT | O_TRUNC | O_RDWR, S_IRUSR | S_IWUSR);
        if(fd2<0){
                const int _errno=errno;
                close(fdt1);
		close(fdt2);
                errno=_errno;
                err(13,"Failed to open %s",argv[2]);
        }

	lseek(fdt1,0,SEEK_SET);
	lseek(fdt2,0,SEEK_SET);
	uint16_t lnum;
	uint16_t rnum;
	while(read(fdt1,&lnum,sizeof(lnum)) > 0 && read(fdt2,&rnum,sizeof(rnum)) > 0){
		if(lnum<rnum){
			if(write(fd2,&lnum,sizeof(lnum)) != sizeof(lnum)){
				const int _errno=errno;
				close(fdt1);
				close(fdt2);
				close(fd2);
				errno=_errno;
				err(14,"Failed to write to %s", argv[2]);
			}
			lseek(fdt2,-1*sizeof(uint16_t),SEEK_CUR);
		}
		else if (lnum >=rnum){
			if(write(fd2,&rnum,sizeof(rnum)) != sizeof(rnum)){
                                const int _errno=errno;
                                close(fdt1);
                                close(fdt2);
                                close(fd2);
                                errno=_errno;
                                err(15,"Failed to write to %s", argv[2]);
                        }
                        lseek(fdt1,-1*sizeof(uint16_t),SEEK_CUR);
                }
	}

	while(read(fdt1,&lnum,sizeof(lnum)) > 0){
		 if(write(fd2,&lnum,sizeof(lnum)) != sizeof(lnum)){
	                 const int _errno=errno;
                         close(fdt1);
        	         close(fdt2);
                         close(fd2);
                         errno=_errno;
                         err(16,"Failed to write to %s", argv[2]);
		}
	}

	 while(read(fdt2,&rnum,sizeof(rnum)) > 0){
                 if(write(fd2,&rnum,sizeof(rnum)) != sizeof(rnum)){
                         const int _errno=errno;
                         close(fdt1);
                         close(fdt2);
                         close(fd2);
                         errno=_errno;
                         err(17,"Failed to write to %s", argv[2]);
		}
        }

	close(fdt1);
	close(fdt2);
	close(fd2);
	exit(0);
}

