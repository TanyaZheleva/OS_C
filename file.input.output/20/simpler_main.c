#include <stdio.h>
#include <err.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int validateNumbers(char *str){
	int l=1;
	int r=9;
	if(strlen(str) == 1){
		int curr=str[0]-'0';
		if(curr>=l && curr<=r){
			return 1;
		}
	}
	if(strlen(str) == 3){
		if(str[1] == '-'){
			int a=str[0] - '0';
			int b=str[2] - '0';
			if(a>=l && a<=r && b>=l && b<=r && a<=b){
				return 1;
			}
		}
	}
	return 0;
}

void cutChars(char *nums){
	int from=nums[0]-'0';
	int to=0;
	if(strlen(nums)==3){
		to=nums[2]-'0';
	}
	char c;
	int cnt=1;
	
	while(cnt<from){
		if(read(0,&c,1) != 0){
			exit(0);
		}
		cnt++;
	}

	while(read(0,&c,1) == 1){
		write(1,&c,1);
		if(to!=0){
			if(from==to){
				break;
			}
			from++;	
		}
		else{
			break;
		}	
	}
}

void cutFields(char d,char *nums){
	int from=nums[0]-'0';
	int to=0;
	if(strlen(nums)==3){
		to=nums[2]-'0';
	}
	char c;
	int cnt=1;
	while(cnt<from){
		if(read(0,&c,1) != 1){
			exit(0);
		}
		if(c == d){
			cnt++;
		}
	}

	while(read(0,&c,1) == 1){
		if(c==d){
			if(to!=0){
				if(from==to){
					break;
				}
				from++;	
			}
			else{
				break;
			}
		}
		else{
			write(1,&c,1);
		}	
	}

}

int main(int argc, char *argv[]){
	if(argc < 3 || argc > 5){
		errx(1,"Invalid count of arguments");
	}
	
	if(strcmp(argv[1],"-c") == 0){
		if( validateNumbers(argv[2]) == 0){
			errx(2,"Invalid bounds");
		}
		cutChars(argv[2]);
		exit(0);
	}

	else if(strcmp(argv[1],"-d") == 0){
		if(strcmp(argv[3],"-f") != 0){
			errx(3,"Invalid options");
		}
		if(validateNumbers(argv[4]) != 1){
			errx(4,"Invalid bounds");
		}
		cutFields(argv[2][0],argv[4]);
		exit(0);
	}
	
	err(5,"Invalid option");

}
