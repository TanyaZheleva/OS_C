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
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void delete(char *string){
	int cnt=strlen(string);
	int index=0;
	char c;
	int found=0;
	while(read(0,&c,1) == 1){
		while(index<cnt){
			if(c == string[index]){
				found=1;
				break;
			}
			index++;
		}
		if(found == 0){
			if(write(1,&c,sizeof(c)) != 1){
				err(13,"Fail writing to stdout");
			}
		}
		found=0;
		index=0;
	}
}

//tr squeezes only 1 symbol,not a sequence, meaning if $echo"affgfgfggff" | tr -s "fg", the res is "afgfgfgf"
void squeeze(char *string){
	int cnt = strlen(string);
	char save;
	int check=0;
	char c;
	while(read(0,&c,sizeof(c)) == 1){
		for(int i=0;i<cnt;i++){
			if(check==1 && c != save){
				if(write(1,&save,sizeof(save)) != 1){
					err(12,"Fail writing to stdout");
				}
				check=0;
			}

			if(c == string[i]){
				save=c;
				check=1;
			}
		}

		if(check == 0){
			if(write(1,&c,sizeof(c)) != 1){
				err(11,"Fail writing to stdout");
			}
		}
	}
}

void replace(char *str1, char *str2){
	if(strlen(str1) != strlen(str2)){
		errx(2,"Different lengths of str1 and str2");
	}
	int cnt=strlen(str1);
	int i=0;
	char c;
	int found=0;
	while(read(0,&c,sizeof(c)) == 1){
		while(i<cnt){
			if(c==str1[i]){
				found=1;
				break;
			}
			i++;
		}
		if(found==1){
			c=str2[i];
		}
		if(write(1,&c,sizeof(c)) != 1){
			err(10,"Fail writing to stdout");
		}
		i=0;
		found=0;
	}
}

int main(int argc, char *argv[]){
	if(argc != 3){
		errx(1, "Invalid count of arguments");
	}

	if(strcmp(argv[1],"-d") == 0){
		delete(argv[2]);
		exit(0);
	}

	if(strcmp(argv[1],"-s") == 0){
		squeeze(argv[2]);
		exit(0);
	}

	replace(argv[1],argv[2]);
	exit(0);
}
