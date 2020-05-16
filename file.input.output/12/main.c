/*бинарниите файлове f1 съдържа двойки от uint32_t числа
в f3 запишете числата в интервалите от тези числа от f2
за двойка 19000,10 -> 19000,19001,...,19009
*/

#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <err.h>
int main(){
	int fd1;
	int fd2;
	int fd3;

        struct pair_t{
        uint32_t x; //start
        uint32_t y; //count
        };
	
	off_t f1_size;
	off_t f2_size;
	
	struct stat st;	
	struct pair_t p;	

	fd1=open("f1",O_RDONLY);
	if ( fd1 < 0){
		err(1,"cant open f1");
	}

	fd2=open("f2",O_RDONLY);
	if(fd2 < 0){
		const int _errno=errno;
		close(fd1);
		errno=_errno;
		err(2,"cant open f2");
	}

	if(fstat(fd1,&st) < 0){
        	close(fd1);
		close(fd2);
		err(4,"cant stat f1");
        }
        f1_size=st.st_size;

        if(fstat(fd2,&st) < 0){
		close(fd1);
		close(fd2);
		err(5,"cant stat f2");
        }
        f2_size=st.st_size;

	if(f1_size%sizeof(p)!=0){
                errx(6,"File f1 isnt exact count pairs");
        }
        if(f2_size%sizeof(uint32_t)!=0){
                errx(7,"File f2 isnt exact count uint32_t numbers");
        }

	fd3=open("f3",O_CREAT | O_RDWR | O_TRUNC , S_IRUSR | S_IWUSR);
	if(fd3 < 0){ 
		const int _errno=errno;
		close(fd1);
		close(fd2);
		errno=_errno;
		err(3,"cant open f3");
	}
	
	struct pair_t pair;

	while (read(fd1,&pair,sizeof(pair)) > 0){
		off_t check = lseek(fd2,(pair.x -1)*sizeof(uint32_t),SEEK_SET);	
		if(check < 0 ){
			const int _errno=errno;
			close(fd1);
			close(fd2);
			close(fd3);
			errno=_errno;
			err(7,"not enough numbers in f2");
		}

		uint32_t store;
		uint32_t number_count = 0;
		size_t length=0;
		while(read(fd2,&store,sizeof(store))){
			length=write(fd3,&store,sizeof(store));
			if(length != sizeof(store)){
				const int _errno=errno;
				close(fd1);
				close(fd2);
				close(fd3);
				errno=_errno;
				err(8,"failed writing");
			}
			number_count++;
			if (number_count == pair.y){
				break;
			}
		}
	}



	close(fd1);
	close(fd2);
	close(fd3);
}
