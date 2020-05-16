/* три параметъра - имена на двойчни файлове ( f1.bin f2.bin patch.bin)
 f1.bin f2.bin - двоични файлове с uint8_t числа; f1 e оигиналния файл, а f2 е модифицирано копие
 patch.bin - двоичен фаил от наредени тройки:
 -отместване uint16_t - спрямо началото в f1.bin/f2.bin
 -оригинален байт uint8_t - на тази позиция в f1.bin
 -нов байт uint8_t - на тази позиция в f2.bin
 Създаваме файла patch.bin спрямо f1.bin и f2.bin, като описва само разликите в двата файла
 Ако дадено отместване съществува само в единия файл - прекратяваме работа
*/

#include <err.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main( int argc, char *argv[]){
	if( argc != 4){
		errx(1,"Invalid count of arguments");
	}

	struct stat st;
	if (stat(argv[1],&st) < 0){
		err(2,"failed to stat file %s", argv[1]);
	}
	off_t size1=st.st_size;
	if ( size1 % sizeof(uint8_t) != 0){
                err(3,"File isnt exact count items");
        }
	uint32_t count1 = size1 / sizeof(uint8_t);

	if (stat(argv[2],&st) < 0){
                err(4,"failed to stat file %s", argv[2]);
        }
        off_t size2=st.st_size;
	if ( size2 % sizeof(uint8_t) != 0){
                err(5,"File isnt exact count items");
        }
        uint32_t count2 = size2 / sizeof(uint8_t);
	
	//check count of elements 
	if(count1 != count2){
		errx(6,"Different lengths of files");
	}

	//open files
	int fd1=open(argv[1],O_RDONLY);
	if( fd1 < 0 ){
		err(7,"failed to open file %s",argv[1]);
	}
	int fd2=open(argv[2],O_RDONLY);
	if(fd2 < 0 ){
		const int _errno=errno;
		close(fd1);
		errno=_errno;
		err(8, "Failed to open file %s", argv[2]);
	}
	int fd3=open(argv[3],O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR);
	if(fd3 < 0){
		const int _errno=errno;
		close(fd1);
		close(fd2);
		errno=_errno;		
		err(9,"Failed to open file %s", argv[3]);
	}

	//read and write down all found differences in the appropriate format
	struct triple{
		uint16_t pos;
		uint8_t b1;
		uint8_t b2;
	};

	uint32_t counter=0;
	char c1;
	char c2;
	struct triple diff;
	while (counter<count1){
		if(read(fd1,&c1,sizeof(c1)) < 0){
		       const int _errno=errno;
		       close(fd1);
		       close(fd2);
		       close(fd3);
		       errno=_errno;
		       err(10,"Failed to read from %s", argv[1]);
		}
		if(read(fd2,&c2,sizeof(c2)) < 0){
                       const int _errno=errno;
                       close(fd1);
                       close(fd2);
                       close(fd3);
                       errno=_errno;
                       err(11,"Failed to read from %s", argv[2]);
                }
		if(c1 != c2){
			diff.pos=counter;
			diff.b1=c1;
			diff.b2=c2;
			if(write(fd3,&diff,sizeof(diff)) < 0){
				const int _errno=errno;
				close(fd1);
				close(fd2);
				close(fd3);
				errno=_errno;
				err(12,"Failed to write in %s", argv[3]);
			}
		}
		counter++;
	}
	close(fd1);
	close(fd2);
	close(fd3);
	exit(0);
}
