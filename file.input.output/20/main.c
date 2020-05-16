/* Имплементирайте командата cut, но само следните функционалност:
 - чете от stdin и пише на stdout
 - ако е подадена опцията -c следващия аргумент е едноцифрено число(n) или две едноцифрени числа с тире между тях(n-m)
	- ако е едно - извежда n-я символ от реда
	- ако са 2 - извежда от n-ти до m-ти символ
- ако е подаде опцията -d 
	- втория параметър е стринг, а първия символ от него се използва за разделител между полетата
	- третия параметър трябва да е -f
	- четвъртия параметър едно едноцифрено число(n) или две едноцифрени числа с тире между тях(n-m)
		 - ако е едно - извежда n-тото поле
	         - ако са 2 - извежда от n-тото до m-тото поле
- ако някой ред няма достатъчно символи/полета, извежда колкото има или празен ред
*/

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

//validate that 0<=n<=9 
int valnumber(char a){
	int l=1;
	int r=9;
	int val=a - '0';//conv char to corresponding int
	if( val >=l && val <=r){
		return 1;
	}
	return 0;
}

//check if the argument is of type n or n-m
//return 1 if n or n-m
//return 0 if neither
int valinput(char *a){
	if(strlen(a) == 1){
		if(valnumber(a[0]) == 1){
			return 1;
		}
	}
	else if(strlen(a) == 3){
		if((valnumber(a[0]) == 1) && (valnumber(a[2]) == 1) && (a[1]- '-' == 0)){
			return 1;
		}
	}
	return 0;
}

void cread(char *a){
	int start;
	int end;
	int check=0;//only one character to print
	start=a[0] - '0';
	end=start;
	if(strlen(a) == 3){
		end=a[2] - '0';
		check=1;//many
	}
	char c;
	int counter=1;
	while (start <= end ){
		if(read(0,&c,sizeof(c)) != sizeof(c)){
			err(3,"Failed reading from stdin");
		}
		if(counter==start){
			if(write(1,&c,sizeof(c)) != sizeof(c)){
				err(4,"Failed to write to stdout");
			}

			if(check==0){
				setbuf(stdout,NULL);
				fprintf(stdout,"\n");
				break;
			}
			else if(check==1){
				start++;
			}
		}
		counter++;
	}
	if(check == 1){
		setbuf(stdout,NULL);
		fprintf(stdout,"\n");
	}
}

void dread(char* d,char* a){
	char delimeter=d[0]; //get delimeter
	int start=a[0] - '0'; // get number of starting field
	int end=start; //if type of call is "-d <string> -f n", number of ending field is same as starting
	int check=0; //type of call is "-d <string> -f n"
	if(strlen(a) == 3){ 
		end=a[2] - '0'; //set the number of the given ending field
		check=1; //type of call is "-d <string> -f n-m"
	}
	char c;
	int field=1; 
	while(start <= end){
		size_t res=read(0,&c,sizeof(c));
		//if reached the end of the input, stop	
		if(c == '\n'){
			setbuf(stdout,NULL);
			fprintf(stdout,"\n");
			break;
		}
		if(res != sizeof(c)){
			err(5,"Failed reading from stdin");
		}
		
		if(field == start){
			if(check == 0 && c==delimeter){//dont print delimeter & print a new line
				setbuf(stdout,NULL);
                                fprintf(stdout,"\n");
                                break;
                        }
			
			if(check == 1 && start==end && c==delimeter){//same but if this is the delimeter affter the last field to be printed
				setbuf(stdout,NULL);
                                fprintf(stdout,"\n");
                                break;
                        }

			if(write(1,&c,sizeof(c)) != sizeof(c)){
				err(6,"Failed writing to stdout");
			}
		}
		
		if(c==delimeter){
			if(check == 1 && field==start){
				start++;
			}
			field ++;
		}
	}
}

int main(int argc,char *argv[]){

	if( argc == 3 && strcmp(argv[1],"-c") == 0){
		if(valinput(argv[2])==1 ){
			cread(argv[2]);
		}
		else {
			errx(2,"%s is not a valid argument for %s",argv[2],argv[1]);
		}

	}

	else if(argc == 5 && strcmp(argv[1],"-d") == 0){
		if(valinput(argv[4]) == 1 ){
			dread(argv[2],argv[4]);
		}
	}

	else{
		errx(1,"Invalid count of arguments");
	}
	exit(0);
}
