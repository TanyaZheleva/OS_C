/*2 параметъра
 ./main input.bin output.bin
  input.bin и output.bin са двоични файлове с  uint32_t числа
  input.bin може да има макс 4194304 числа
  output.bin се създава и съдържа числата от input.bin сортирани във възходящ ред
 забележки:
  endianness-ът на машината създала input.bin е същия като на текущата машина
  програмата може да използва не повече от  9 MB RAM и 64 MB дисково пространство
*/

/*
  input.bin може да е макс 16 MB -> разбиваме  input.bin на две и всяка част ще е макс 8 МВ
  записваме всяка сортирана част във временен файл и тях сливаме в output.bin чрез merge стъпката на merge sort
  input.bin + output.bin + temp1 + temp2 = 16 + 16 + 8 + 8 = 48 MB < 64 MB //worst case
*/

#include <err.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>

int cmp(const void *a, const void *b){
	if(*((uint32_t*)a) > *((uint32_t*)b)){
		return 1;
	}
	else if ( *((uint32_t*)a) < *((uint32_t*)b) ){
		return -1;
	}
	return 0;
}

int main(int argc, char *argv[]){
	if(argc != 3){
		errx(1,"Invalid count of arguments");
	}

	struct stat st;
	if(stat(argv[1],&st) < 0){
		err(2,"Failed to stat %s",argv[1]);
	}

	off_t size=st.st_size;
	if(size%sizeof(uint32_t) != 0){
		errx(3,"%s doesn't consist exclusively uint32_t numbers", argv[1]);
	}
	uint32_t numel=size/sizeof(uint32_t);
	int fd1=open(argv[1],O_RDONLY);
	if(fd1 < 0){
		err(4,"Failed to open %s", argv[1]);
	}
	int lhalf=numel/2;
	void *buff=malloc(lhalf*sizeof(uint32_t));
	if(!buff){
		const int _errno=errno;
		close(fd1);
		errno=_errno;
		err(5,"Failed to alocate memory");
	}
	size_t res=read(fd1,buff,lhalf*sizeof(uint32_t));
	if(res != lhalf*sizeof(uint32_t)){
		const int _errno=errno;
		close(fd1);
		free(buff);
		errno=_errno;
		err(6,"Failed reading %s", argv[1]);
	}
	qsort(buff,lhalf,sizeof(uint32_t),cmp);
	int fdt1=open("temp_file_1.bin",O_CREAT | O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR);
	if(fdt1<0){
		const int _errno=errno;
		close(fd1);
		free(buff);
		errno=_errno;
		err(7,"Failed to open temp file 1");
	}

	res=write(fdt1,&buff,lhalf*sizeof(uint32_t));
	if(res != lhalf*sizeof(uint32_t) ){
		const int _errno=errno;
                free(buff);
		close(fd1);
		close(fdt1);
                errno=_errno;
                err(8,"Failed to write to temp file 1");
        }

	free(buff);
	
	int rhalf=numel-lhalf;
	void *buffer=malloc(rhalf*sizeof(uint32_t));
	if(!buffer){
		const int _errno=errno;
		close(fd1);
		close(fdt1);
		errno=_errno;
		err(9,"Failed to alocate memory");
	}

	res=read(fd1,buffer,rhalf*sizeof(uint32_t));
	if(res != rhalf*sizeof(uint32_t)){
		const int _errno=errno;
		close(fd1);
		close(fdt1);
		free(buffer);
		errno=_errno;
		err(10,"Failed to read from %s", argv[1]);
	}
	qsort(buffer,rhalf, sizeof(uint32_t),cmp);
	int fdt2=open("temp_file_2.bin",O_CREAT | O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR);
	if(fdt2 < 0){
		const int _errno=errno;
		close(fd1);
		close(fdt1);
		free(buffer);
		errno=_errno;
		err(11, "Failed to open temp file 2");
	}
	
	res=write(fdt2,buffer,rhalf*sizeof(uint32_t));
	if(res != rhalf*sizeof(uint32_t)){
		const int _errno=errno;
		close(fd1);
		close(fdt1);
		close(fdt2);
		free(buffer);
		errno=_errno;
		err(12,"Failed to write to temp file 2");
	}
	close(fd1);
	int fd2=open(argv[2], O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR);
	if(fd2 < 0){
		const int _errno=errno;
		close(fdt1);
		close(fdt2);
		free(buffer);
		errno=_errno;
		err(13,"Failed to open %s", argv[2]);
	}
	free(buffer);

	uint32_t a;//read from temp file 1
	uint32_t b;//read from temp file 2
	lseek(fdt1,0,SEEK_SET);
	lseek(fdt2,0,SEEK_SET);
	while(read(fdt1,&a,sizeof(a)) == sizeof(a) && read(fdt2,&b,sizeof(b)) == sizeof(b)){
		if( a < b ){
			if(write(fd2,&a,sizeof(a)) != sizeof(a)){
				const int _errno=errno;
				close(fdt1);
				close(fdt2);
				close(fd2);
				errno=_errno;
				err(14,"Failed to write to %s", argv[2]);
			}
			lseek(fdt2,-1*sizeof(b),SEEK_CUR);
		}
		else{
			if(write(fd2,&b,sizeof(b)) != sizeof(b)){
				const int _errno=errno;
				close(fdt1);
				close(fdt2);
				close(fd2);
				errno=_errno;
				err(15,"Failed to write to %s",argv[2]);
			}
			lseek(fdt1,-1*sizeof(a),SEEK_CUR);
		}
	}

	while(read(fdt1,&a,sizeof(a)) == sizeof(a)){
		if(write(fd2,&a,sizeof(a)) != sizeof(a)){
			const int _errno=errno;
			close(fdt1);
			close(fdt2);
			close(fd2);
			errno=_errno;
			err(16,"Failed to write to %s", argv[2]);
		}
	}

	while(read(fdt2,&b,sizeof(b)) == sizeof(b)){
		if(write(fd2,&b,sizeof(b)) != sizeof(b)){
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
