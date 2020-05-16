/*подобие на командата tr; изпълнява следните функционалности:
- чете от stdin и пише на stdout
- общ вид на изпълнение : ./main [OPTION] SET1 [SET2]
- OPTION може да е -d или -s или да липсва
- SET1 и SET2 са низове, знаците на които нямат специални значения (literal strings); ако има SET2, той трябва да е със същата дължина като SET1
- при подадена опция -d се трие всяко срещане на символ от SET1 от stdin; не е необходим SET2
- при подадена опция -s всяка поредица  от символи от SET1 във входа се премахва и се оставя само едно; не е необходим SET2
- при липса на опция всеки символ от SET1 се заменя със съотвеетващия му символ от SET2 във входа
-пример-
$ echo asdf | ./main -d 'd123' | ./main 'sm' 'fa' | ./main -s 'f'
af
*/

#include <err.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char *argv[]){

	if(argc != 3){
		errx(1,"Invalid count of arguments");
	}

	char buff;
	char check;
	int size=strlen(argv[2]);
	if(strcmp(argv[1],"-d") == 0){
		while(read(0,&buff,sizeof(buff)) > 0){
			check=1;
			for(int i=0; i<size; i++){
				if(buff == argv[2][i]){
					check=0;
					break;
				}
			}
			if(check==1){
				if(write(1,&buff,sizeof(buff)) < 0){
					err(2,"Failed writing to stdout");
				}
			}
		}
	}

	else if(strcmp(argv[1],"-s") == 0){
		char save;
		check=0;
		while(read(0,&buff,sizeof(buff)) > 0){
			for(int i=0; i<size; i++){
				if(check == 1 && buff != save){
					write(1,&save,sizeof(save));
					check=0;
				}
				if(buff == argv[2][i]){
					save=buff;
					check=1;
				}
			}
			if(check == 0){	
				if(write(1,&buff,sizeof(buff)) < 0 ){
					err(3,"Failed writing");
				}
			}					
		}
	}

	else{
		int size2=strlen(argv[2]);
		if ( size==size2){
			while(read(0,&buff,sizeof(buff)) > 0){
				for(int i=0;i<size;i++){
					if(buff==argv[1][i]){
						buff=argv[2][i];
						break;
					}
				}
				if(write(1,&buff,sizeof(buff)) < 0){
					err(4,"Failed writing");
				}
			}
		}
		else{
			errx(6,"str1 and str2 have to have the same length");
		}
		exit(123);
	}
	exit(0);
}
