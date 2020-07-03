/* напишете програма на С, която сортира файла f1-двоичен с uint32_t числа. Ограничения:
- числата биха могли да са макс 100 000 000 на брой;
- програмата трябва да работи на машина със същия endiance, както машината, която е създала файла;
- програмата трябва да работи на машина с 256 MB RAM и 8 GB свободно дисково пространство.
*/
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <err.h>
#include <errno.h>

int cmp(const void *a, const void *b){
	if( *((uint32_t*)a) > *((uint32_t*)b)){
		return 1;
	}
	else if( *((uint32_t*)a) < *((uint32_t*)b) ){
		return -1;
	}
	return 0;
}

int main(int argc, char *argv[]){
	if(argc != 2){
		errx(1,"Invalid count of arguments");
	}

	struct stat st;
	if(stat(argv[1],&st) == -1){
		err(2,"Failed to stat file");
	}

	if(st.st_size % sizeof(uint32_t) != 0){
		errx(3,"Invalid format of file");
	}

	uint32_t numel=st.st_size/sizeof(uint32_t);
	uint32_t half=numel/2;
	uint32_t *buff=malloc(half*sizeof(uint32_t));
	if(!buff){
		err(5,"Failed to malloc");
	}

	int fd=open(argv[1],O_RDWR);
	if(fd == -1){
		const int _errno=errno;
		free(buff);
		errno=_errno;
		err(4,"Failed to open file");
	}

	int t1=open("temp1",O_CREAT | O_TRUNC | O_RDWR, S_IRUSR | S_IWUSR);
	if(t1 == -1){
		const int _errno=errno;
		free(buff);
		close(fd);
		errno=_errno;
		err(5,"Faield to open temp file");
	}

	size_t res=read(fd,buff,half*sizeof(uint32_t));
	if(res != half*sizeof(uint32_t)){
		const int _errno=errno;
		free(buff);
		close(fd);
		errno=_errno;
		err(6,"failed to read");
	}
	//   where cnt   sizeof1        how
	qsort(buff,half,sizeof(uint32_t),cmp);
	res=write(t1,buff,half*sizeof(uint32_t));
	if(res != half*sizeof(uint32_t)){
		const int _errno=errno;
		close(fd);
		close(t1);
		free(buff);
		errno=_errno;
		err(7,"Failed to write");
	}

	free(buff);
	uint32_t rhalf=numel-half;
	uint32_t *rbuff=malloc(rhalf*sizeof(uint32_t));
	if(!rbuff){
		const int _errno=errno;
		close(t1);
		close(fd);
		errno=_errno;
		err(8,"Failed to malloc");
	}

	int t2=open("temp2",O_CREAT | O_TRUNC | O_RDWR, S_IRUSR | S_IWUSR);
	if(t2 == -1){
		const int _errno=errno;
		close(t1);
		close(fd);
		free(rbuff);
		errno=_errno;
		err(9,"Failed to open temp file2");
	}

	res=read(fd,rbuff,rhalf*sizeof(uint32_t));
	if(res!=rhalf*sizeof(uint32_t)){
		const int _errno=errno;
		close(fd);
		close(t1);
		close(t2);
		free(rbuff);
		errno=_errno;
		err(10,"Failed to read");
	}
	qsort(rbuff,rhalf,sizeof(uint32_t),cmp);
	res=write(t2,rbuff,rhalf*sizeof(uint32_t));
	if(res != rhalf*sizeof(uint32_t)){
		const int _errno=errno;
		close(t1);
		close(t2);
		close(fd);
		free(rbuff);
		errno=_errno;
		err(11,"Faield to wrrite");
	}
	free(rbuff);
	lseek(t1,0,SEEK_SET);
	lseek(t2,0,SEEK_SET);
	lseek(fd,0,SEEK_SET);
	uint32_t a;
	uint32_t b;

	while((read(t1,&a,sizeof(a)) == sizeof(a)) && (read(t2,&b,sizeof(b)) == sizeof(b))){
		if(a<=b){
			if(write(fd,&a,sizeof(a)) != sizeof(a)){
				const int _errno=errno;
				close(fd);
				close(t1);
				close(t2);
				errno=_errno;
				err(12,"Failed to write");
			}
			lseek(t2,-1*sizeof(b),SEEK_CUR);
		}
		else{
			if(write(fd,&b,sizeof(b)) != sizeof(b)){
				const int _errno=errno;
				close(fd);
				close(t1);
				close(t2);
				errno=_errno;
				err(13,"Failed to write");
			}
			lseek(t1,-1*sizeof(a),SEEK_CUR);
		}
	}

	while(read(t1,&a,sizeof(a)) == sizeof(a)){
		if(write(fd,&a,sizeof(a)) != sizeof(a)){
			const int _errno=errno;
			close(fd);
			close(t1);
			close(t2);
			errno=_errno;
			err(14,"Failed to write");
		}
	}
	while(read(t2,&b,sizeof(b)) == sizeof(b)){
		if(write(fd,&b,sizeof(b)) != sizeof(b)){
			const int _errno=errno;
			close(fd);
			close(t1);
			close(t2);
			errno=_errno;
			err(15,"Failed to write");
		}
	}

	close(fd);
	close(t1);
	close(t2);
	exit(0);
}
