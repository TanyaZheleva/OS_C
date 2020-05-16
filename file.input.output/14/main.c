/*приема 4 параметъра f1.dat f1.idx f2.dat f2.idx - имена на двоични файлове. ф1- входен комплект(съществува); ф2 - изходен комплект(да се създаде);
 .dat - DAT файлове с uint8_t числа
 .idx - двоичнифайлове с наредени тройки :
 -отместване uint16_t - показва позицията на първия байт от даден низ спрямо началото на файла
 -дължина uint8_t - дължината на низа
 -запазен uint8_t - не се ползва.
 Тези тройки дефинират поредици от байтове от съответния DAT файл.
Създаване на изходен комплект:
- да се копират само низовете от входния комплект, които започват с главна латинска буква.
- ако файловете са неконсистентни по някакъв начин програмата прекратява изпълнението си.
! - DAT файла съдържа текстови данни с ASCII кодова таблица.
*/

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <err.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char* argv[]){
	
	if ( argc != 5){
		errx(1,"Incompatable count of arguments");
	}
//check if file contains only uint8_t numbers
	struct stat st;
	if ( stat(argv[1],&st) < 0){
		err(2,"cant stat first file");
	}
	
	off_t f1_size=st.st_size;
	if ( f1_size % sizeof(uint8_t) != 0){
		err(3,"File isnt exact count items");
	}
// structure that holds the defining items of the IDX files
	struct triple{
		uint16_t pos;
		uint8_t len;
		uint8_t space;
	};
	struct triple a;
//checking second file 
	if ( stat(argv[2],&st) < 0){
                err(4,"cant stat second file");
        }

        off_t f2_size=st.st_size;
        if ( f2_size % sizeof(a) != 0){
                err(5,"File isnt exact count triples");
        }
	uint32_t count_triples=f2_size/sizeof(a); // count of triples in the second file(e.g. f1.idx)
//opening the files for reading and writing accordingly
	int fd1=open(argv[1],O_RDONLY);
	if(fd1<0){
		err(6,"Fail to open first file");
	}

	int fd2=open(argv[2],O_RDONLY);
	if(fd2<0){
		const int _errno=errno;
		close(fd1);
		errno=_errno;
		err(7,"Fail to open second file");
	}

	int fd3=open(argv[3],O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR);
	if(fd3<0){
		const int _errno=errno;
		close(fd1);
		close(fd2);
		errno=_errno;
		err(8,"Fail to open third file");
	}

	int fd4=open(argv[4],O_CREAT | O_WRONLY | O_TRUNC , S_IRUSR | S_IWUSR);
        if( fd4 < 0){
		const int _errno=errno;
		close(fd1);
		close(fd2);
		close(fd3);
		errno=_errno;
                err(9,"Fail to open fourth file");
        }

//while there are still untead triples-read triples one by one
	struct triple one;//to read from the incoming files
	uint32_t count_read_triples=0;
	uint8_t len_read=0;
	char c;
	uint8_t prev_len=0;
	while(read(fd2,&one,sizeof(one)) > 0 ){
		lseek(fd1,one.pos,SEEK_SET);//move to pos, given at one
		
		//check if the first letter is a capital letter
		if(read(fd1,&c,sizeof(c)) < 0){
			const int _errno=errno;
                        close(fd1);
                        close(fd2);
                        close(fd3);
                        close(fd4);
                        errno=_errno;
                	err(10,"Fail read from 1st file");
                }

		if ( c >= 0x41 && c <= 0x5A ){
			//can write down in f2.dat and f2.idx
			lseek(fd1,-1,SEEK_CUR); //move back 1, to read the 1st letter again
			len_read=0;
			while(len_read < one.len){
				if(read(fd1,&c,sizeof(c)) < 0){
					const int _errno=errno;
					close(fd1);
					close(fd2);
					close(fd3);
					close(fd4);
					errno=_errno;
					err(11,"Fail read from 1st file");
				}
			
				if(write(fd3,&c,sizeof(c)) < 0){
					const int _errno=errno;
	                                close(fd1);
	                                close(fd2);
	                                close(fd3);
	                                close(fd4);
	                                errno=_errno;
	                                err(12,"Fail write in 3rd file");
				}
				len_read++;
			}
			// add triple to 4th file
			one.pos=prev_len; //update pos in f2.idx appropriately
			prev_len=one.len;//update length for next iteration
			//write in f2.idx
			if(write(fd4,&one,sizeof(one)) != sizeof(one)){
				const int _errno=errno;
                                close(fd1);
                                close(fd2);
                                close(fd3);
                                close(fd4);
                                errno=_errno;
                                err(13,"Fail write in 4th file");
			}
		}
			
		count_read_triples++;
		if(count_read_triples == count_triples){
			break;
		}
				
	}	
		
//close all open fd-s and exit
	close(fd1);
	close(fd2);
	close(fd3);
	close(fd4);
	exit(0);
}
