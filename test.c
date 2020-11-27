#include<sys/types.h>
#include<sys/stat.h>
#include<dirent.h>
#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<unistd.h>
int main(int argc, char * argv[]){ 

 char arr[240];
 
 char * dir_name = getcwd(arr,240);
    
		printf("%s",dir_name);
	
 DIR * ptr = opendir(dir_name); 

struct dirent * file  = readdir(ptr); 
//off_t out  =  lseek(int fd, off_t offset, int whence);
	int safety  = 0; 
	long int loc = 0;
	struct stat info; 

	while(file != NULL){
			if(file->d_type == DT_DIR){ 

			printf("./%s\n",file->d_name);

		} 
		else if(file->d_type == DT_REG){
				printf("%s\n",file->d_name); 
     }
		loc = telldir(ptr);
			
			
			if(loc == -1){ 
				break;
			}
		
		seekdir(ptr,loc);
		
		file = readdir(ptr);
	
	}

closedir(ptr);

return 1;
}
