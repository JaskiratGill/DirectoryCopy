#include "common.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>

void cf(char* s, char* d);
void copyDirectory(const char *s, char* d);

void usage(){
    fprintf(stderr, "Usage: cpr srcdir dstdir\n");
    exit(1);
}

int
main(int argc, char *argv[])
{
    if (argc != 3) 
        usage();
    

    struct stat fileInfo;
    stat(argv[1], &fileInfo);

    if (S_ISREG(fileInfo.st_mode))
        cf(argv[1],argv[2]);  
  
    else 
        copyDirectory(argv[1], argv[2]);  
       
    return 0;
}

void copyDirectory(const char *s, char* d){
   
    DIR*  directoryPtr = opendir(s);
    struct dirent *direntp;
    
    if(mkdir(d, 0777) != 0){
        syserror(mkdir, d);
    }

    while((direntp = readdir(directoryPtr)) != NULL){
       
        char destPathname[strlen(d) + strlen(direntp -> d_name) + 2];
        strcpy(destPathname, d);
        strcat(destPathname, "/");
        
        char srcPathname[strlen(s) + strlen(direntp -> d_name) + 2];
        strcpy(srcPathname, s);
        strcat(srcPathname, "/");
        
        //Append the new path name
        strcat(destPathname, direntp->d_name);
        strcat(srcPathname, direntp->d_name);
       
        struct stat infoStat;
   
        if(stat(srcPathname, &infoStat) != 0){
             syserror(stat, srcPathname);
        }
        
        if (S_ISREG(infoStat.st_mode)){
            cf(srcPathname, destPathname);
        }

        else if(S_ISDIR(infoStat.st_mode) && (strcmp(direntp->d_name, ".") != 0) && (strcmp(direntp->d_name, "..") != 0)){
            
                copyDirectory(srcPathname, destPathname);
                chmod(destPathname, infoStat.st_mode);
        }     
    }
    
    closedir(directoryPtr);
}

void cf(char *s, char *d){
    
    int fd = open(s,O_RDONLY);
    
    if(fd <0)
        syserror(open,s);
    char buf[4096];
    int newFile = creat(d,0777);
    
    if(newFile < 0)
        syserror(creat,d);
   
    int ret = read(fd, buf,4096);
    while(ret != 0){
     
        if(write(newFile,buf,ret) <= 0)
            syserror(write, d);
          ret = read(fd, buf,4096);
    }
    
    struct stat fileInfo;
    stat(s, &fileInfo);
    chmod(d, fileInfo.st_mode);


    close(fd);
    close(newFile);
}

         
