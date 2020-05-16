/* напишете програма на С, която сортира файла f1-двоичен с uint32_t числа. Ограничения:
- числата биха могли да са макс 100 000 000 на брой;
- програмата трябва да работи на машина със същия endiance, както машината, която е създала файла;
- програмата трябва да работи на машина с 256 MB RAM и 8 GB свободно дисково пространство.
*/

#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdint.h>
#include <err.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>

int cmp(const void *a, const void* b){
//  return *((uint32_t*)a) - *((uint32_t*)b);
    if ( *((uint32_t*)a) > *((uint32_t*)b) ) {
        return 1;
    } else if ( *((uint32_t*)a) < *((uint32_t*)b) ) {
        return -1;
    } 
    return 0;
}

int main()
{
    struct stat st; 
    if(stat("f1",&st) < 0){
        err(1,"failed stat");
    }

    if ( st.st_size % sizeof(uint32_t) != 0){
        errx(2,"wrong count"); 
    }
    
    uint32_t numel=st.st_size / sizeof(uint32_t);
    
    //left(first half of file)

    uint32_t half=numel/2;
    uint32_t *p=malloc(half*sizeof(uint32_t));
    if (!p){
        err(3,"failed malloc");
    }


    int fd1=open("f1",O_RDONLY);
    if( fd1 == -1){
	const int _errno=errno;
	free(p);
	errno=_errno;
        err(4,"failed open f1");
    }

    int t1=open("temp1", O_CREAT | O_RDWR | O_TRUNC , S_IRUSR | S_IWUSR);
    if(t1 == -1){
        const int _errno=errno;
        free(p);
        close(fd1);
        errno=_errno;
        err(5,"failed to open temp1");
    }
    
    //read the wlole first half of f1
    size_t res = read(fd1,p,half*sizeof(uint32_t));
    if (res != half*sizeof(uint32_t)){
        const int _errno=errno;
        free(p);
        close(fd1);
        close(t1);
        errno=_errno;
        err(6,"failed to read f1");
    }

    //sort it
    qsort(p,half,sizeof(uint32_t),cmp);
    //dump the sorted half in t1
    res=write(t1,p,half*sizeof(uint32_t));
    if(res != half*sizeof(uint32_t)){
        const int _errno=errno;
        free(p);
        close(fd1);
        close(t1);
        errno=_errno;
        err(7,"failed to write t1");
    }

//  the same for the rest of f1 ( right side)
    free(p);
    uint32_t rhalf=numel-half;
    uint32_t *rp=malloc(rhalf*sizeof(uint32_t));
    if(!rp){
        const int _errno=errno;
        close(fd1);
        errno=_errno;
        err(8,"failed malloc");
    }

    int t2=open("temp2", O_CREAT | O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR);
    if( t2 < 0){
        const int _errno=errno;
        close(fd1);
        free(rp);
        errno=_errno;
        err(9,"failed to opem temp2");
    }
    res=read(fd1,rp,rhalf*sizeof(uint32_t));
    if( res != rhalf*sizeof(uint32_t)){
        const int _errno=errno;
        close(fd1);
        close(t2);
        free(rp);
        errno=_errno;
        err(10,"failed to read f1");
    }

    qsort(rp,rhalf,sizeof(uint32_t),cmp);

    res=write(t2,rp,rhalf*sizeof(uint32_t));
    if( res != rhalf*sizeof(uint32_t)){
        const int _errno=errno;
        close(fd1);
        close(t2);
        free(rp);
        errno=_errno;
        err(11,"failed to write temp2");
    }

    free(rp);
    close(fd1);
//return at the start of both soreted parts
    lseek(t1,0,SEEK_SET);
    lseek(t2,0,SEEK_SET);

    //open fd for the file with the final result
    int fd2=open("f2",O_CREAT | O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR);
    if(fd2 < 0){
        const int _errno=errno;
        close(t1);
        close(t2);
        errno=_errno;
        err(12,"failed to open f2");
    }

    uint32_t a;
    uint32_t b;
    size_t d1 = 0;
    size_t d2 = 0;
    size_t d3 = 0;
// merge the two halves in f2
// read from both and write down the smaller number
// return with lseek at the previous position to read the number that wasn't written down in f2
    while ((read(t1, &a, sizeof(a)) == sizeof(a)) && (read(t2, &b, sizeof(b)) == sizeof(b))) {
        if ( a <= b) {
            write(fd2, &a, sizeof(a));
            lseek(t2,-1*sizeof(b),SEEK_CUR);
        } else {
            write(fd2, &b, sizeof(b));
            lseek(t1,-1*sizeof(a),SEEK_CUR);
        }
        d1++;
    }

//write down the rest of t1 if there is something unwritten left
    while (read(t1, &a, sizeof(a)) == sizeof(a)) {
            write(fd2, &a, sizeof(a));
        d2++;
    }

//same with t2
    while (read(t2, &b, sizeof(b)) == sizeof(b)) {
            write(fd2, &b, sizeof(b));
        d3++;
    }

    close(t1);
    close(t2);
    close(fd2);
    exit(0);
}
