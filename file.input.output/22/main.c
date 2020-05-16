/* приема 1 параметър - име на двоичен файл(с инф. за потребителите, които са влизали в системата), който се състои отнаредени петорки:
 - UID - uint32_t
 - запазено 1 - uint16_t
 - запазено 2 - uint16_t
 - време 1 - uint32_t - време на започване на сесията(UNIX time)
 - време 2 - uint32_t - време на завършване на сесията(UNIX time)
 Да се изведат на stdout UID и продължителността на най-дългата сесия, за потребители, които са имали сесия(сесии),за която квадрата на продулжителността й е по-голям от дисперсията на продължителността на всички сесии във файла.
 ! файла има не повече от 16384 записа и в тях се срещат не повече от 2048 различни UID
*/

#include <err.h>
#include <errno.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>

struct five{
	uint32_t uid;
        uint16_t a;
        uint16_t b;
        uint32_t start;
        uint32_t stop;
};

void merge(struct five *arr, uint32_t  l, uint32_t m,uint32_t r){ 
	uint32_t i, j, k; 
	uint32_t n1 = m - l + 1; 
        uint32_t n2 =  r - m; 
  
        //create temp arrays
        struct five *L=(struct five*)malloc(n1*sizeof(struct five));
	struct five *R=(struct five*)malloc(n2*sizeof(struct five));
  
        //copy data to temp arrays L[] and R[]
        for (i = 0; i < n1; i++){
        	L[i] = arr[l + i];
	}	
        for (j = 0; j < n2; j++){ 
        	R[j] = arr[m + 1+ j];
	}	
  
    	//merge the temp arrays back into arr[l..r]
    	i = 0; // initial index of first subarray 
    	j = 0; // initial index of second subarray 
    	k = l; // initial index of merged subarray 
    	while (i < n1 && j < n2){ 
        	if (L[i].uid <= R[j].uid){ 
            		arr[k] = L[i]; 
            		i++; 
        	} 
        	else{ 
            		arr[k] = R[j]; 
            		j++; 
        	} 
        	k++; 
    	} 
  
    	//copy the remaining elements of L[], if any
    	while (i < n1){ 
        	arr[k] = L[i]; 
        	i++; 
        	k++; 
    	} 
  	
	//same with R[]
    	while (j < n2){ 
        arr[k] = R[j]; 
        j++; 
        k++; 
    	} 
} 
  
void mergeSort(struct five *arr, uint32_t l, uint32_t r){ 
	if (l < r){ 
        	uint32_t m = l+(r-l)/2; 
		mergeSort(arr, l, m); 
        	mergeSort(arr, m+1, r); 
		merge(arr, l, m, r); 
    	} 
} 

int main( int argc, char *argv[]){
	
	if(argc != 2){
		errx(1,"Invalid count of arguments");
	}
	struct stat st;
	if(stat(argv[1],&st) < 0){
		err(2,"Failed to stat file");
	}

	off_t size=st.st_size;
	struct five buff;

	if(size%sizeof(buff) != 0){
		errx(3,"Invalid content of file");
	}

	uint32_t gcount=size/sizeof(buff);
		
	int fd1=open(argv[1],O_RDONLY);
	if(fd1<0){
		err(4,"Failed to open file");
	}
	
	//get average length of a session
	uint64_t diff; //buff.start-buff.stop
	uint64_t sum=0;
	uint64_t avg; //average of lengths of all sessions

	while(read(fd1,&buff,sizeof(buff)) == sizeof(buff)){
		diff=buff.stop - buff.start;
		sum+=diff;
	}
	avg=sum/gcount;
	
	//get dispersion of sessions
	lseek(fd1,0,SEEK_SET);
	sum=0;
	uint64_t square;
	while(read(fd1,&buff,sizeof(buff)) == sizeof(buff)){
                diff=buff.stop - buff.start - avg;
                square=diff*diff;
		sum+=square;
        }
	uint64_t dispersion=sum/gcount;
	//sort file by uid's so they are consecutive
	//start reading struct by struct from file and print to stdout the longest session of each uid if it's > dispersion

	lseek(fd1,0,SEEK_SET);
	struct five *array=(struct five*)malloc(gcount*sizeof(buff));
	uint32_t index=0;
	while(read(fd1,&buff,sizeof(buff)) == sizeof(buff)){
		array[index]=buff;
		index++;
	}
	
	//sort array into sorted using merge sort
	mergeSort(array,0,gcount-1);
	
	index=0;
	uint32_t old_uid;
	uint32_t old_length;
	square = 0;
	//set first values of old_uid and old_length that are printable(length^2 > dispersion)
	while(square<dispersion && index<gcount){
		old_uid=array[index].uid;
		old_length=array[index].stop-array[index].start;
		square = old_length*old_length;
		index++;
	}
	//now we know that old_length and old_uid != NULL
	for(;index<gcount;index++){
		diff=array[index].stop-array[index].start;
		square=diff*diff;
		if(square>dispersion){
			if(array[index].uid==old_uid && diff > old_length ){
				old_length=diff;
			}
			else if(array[index].uid != old_uid){
				setbuf(stdout,NULL);
				fprintf(stdout,"%d %d\n",old_uid,old_length);
				old_uid=array[index].uid;
				old_length=diff;
			}
		}
	}
	free(array);
	close(fd1);

	exit(0);
}
