//Реализирайте команда cp, работеща с произволен брой подадени входни параметри.
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <err.h>
#include <errno.h>
#include <string.h>

void cp_to_file(char *src_filename,char *dest_filename){
	int src_fd = open(src_filename,O_RDONLY);
	if( src_fd == -1){
		err(2,"cant open src file %s",src_filename);
	}

	int dest_fd=open(dest_filename, O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR);

	if(dest_fd == -1){
		err(3, "Failed to open dest file %s",dest_filename);
	}

	char buff[4096];
	ssize_t rdsz=0;
	while((rdsz=read(src_fd,&buff,sizeof(buff))) > 0){
		if(write(dest_fd,&buff,rdsz) != rdsz){
			int _errno=errno;
			close(src_fd);
			close(dest_fd);
			errno=_errno;
			err(4,"Failed writing");
		}
	}
	close(src_fd);
	close(dest_fd);
}

void cp_to_dir(char *src_filename, char *dest_dirname){
	char * dest_filename=malloc(strlen(dest_dirname)+1+strlen(src_filename)+1);
	
	strcpy(dest_filename,dest_dirname);
	strcat(dest_filename,"/");
	strcat(dest_filename,src_filename);
	
	cp_to_file(src_filename, dest_filename);	
	free(dest_filename);
}

int main(int argc, char *argv[]){
	if(argc < 3){
		errx(1,"Invalid count arguments");
	}

	struct stat st;
	if(stat(argv[argc-1],&st) == -1){
		err(2,"Cannot stat file %s", argv[argc-1]);
	}
	if(!S_ISDIR(st.st_mode)){
		if(argc != 3){
			errx(4,"%s is not a directory",argv[argc-1]);
		}
		cp_to_file(argv[1],argv[2]);
		exit(0);
	}

	int cnt=1;
	while(cnt<argc-1){
		cp_to_dir(argv[cnt],argv[argc-1]);
		cnt++;
	}
	exit(0);
}
