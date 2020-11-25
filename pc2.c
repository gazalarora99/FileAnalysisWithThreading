#include<stdlib.h>
#include<stdio.h>
#include<pthread.h>
#include<string.h>
#include<sys/types.h>
#include<dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>

void * dir_handler(void * dir_info); 
void * file_handler(void * file_info);


struct thread_arg{ // this argument stuct is passed to pthread_create so our routine has access to the right arguments 
	pthread_mutex_t * lock;
	char * path;	
}; 




struct Tnode{  // token node for linked list of tokens
char * token;
double prob; 
struct node * next_token;
};


struct Lnode{ // list nodes for list of linked list
char * file_handle;
struct Tnode * token_list;
struct Lnode * next_list;
};





int main(int argc, char * argv[]){ 


	char * dir_handle  = argv[1]; 


	struct thread_arg * dir_obj = (struct thread_arg *)malloc(sizeof(struct thread_arg));  
	pthread_mutex_t file_lock;
	pthread_mutex_init(&file_lock,NULL);
	
	dir_obj->path = dir_handle; 
	dir_obj->lock = &file_lock;
	
	
	dir_handler((void *)dir_obj);

	return 1;
}





void * file_handler(void * file_info){ 


struct thread_arg *arg  = (struct thread_arg *)file_info;


pthread_mutex_lock(arg->lock);
//printf("%s\n",arg->path);
//printf("a\n");
// token function call
// call math functions  
  int fd;
  //  while(pthread_mutex_trylock(arg->lock)!=0){
    //waiting
  // puts("waiting");
  // }
  fd = open(arg->path, O_RDONLY);
  if (fd>=0) {
    printf("file %s opened\n", arg->path);
  }
  //  pthread_mutex_unlock(arg->lock); 
  off_t off = lseek(fd, 0, SEEK_END);
  printf("size of file %llu\n", (long long int) off);
  char *buf = malloc(sizeof(char) * ((int)off));
  int size = (int) off;
  off= lseek(fd,0,SEEK_SET);
  printf("new offset %llu\n", (long long int) off);
  int bytes, pos;
  //  bytes=read(fd,buf,(int) size);                                                                                                                   
  //printf("read %d bytes\n", bytes);                                                                                                                  
  while((bytes=read(fd,buf,size)) > 0){
    printf("read %d bytes\n", bytes);
    //for(pos=0; pos<bytes; pos++){
      //if(isalpha(buf[pos])){
      //printf("%c\n", buf[pos]);
      //}
      //}
    printf("%s\n", buf);
  }


 
pthread_mutex_unlock(arg->lock);
free(arg);
pthread_exit(0);
}





void * dir_handler(void * dir_info){ 

	struct thread_arg * arg = (struct thread_arg *)dir_info ;

	//printf("the dir name %s \n",arg->path);
	
	DIR * ptr = opendir(arg->path); 
	struct dirent * file  = readdir(ptr); 
	
	int safety  = 0; 
	long int loc = 0;
	
	pthread_attr_t	threadAttr;		
	pthread_attr_init(&threadAttr); 
	pthread_t thread;	// handles calls to dir handler 

	pthread_t thread2; // handles calls to file handler
	
//	pthread_mutex_t file_lock;
//	pthread_mutex_init(&file_lock,NULL);
	while(file != NULL){
		
		if(file->d_name[0]!='.'){
			if(file->d_type == DT_DIR){ // checks if current object is a directory
				
				int arg_p = strlen(arg->path); 
				int argFile_d = strlen(file->d_name);

				char * concat_path = (char *)malloc(sizeof(char) * (arg_p + argFile_d +2) ); // malloc space for new concat path
				*(concat_path+arg_p+argFile_d+2) = '\0';
				
				int k=0;  

				for(k;k<arg_p;k++){ 
					*(concat_path+k) = *(arg->path +k);
				}  
				
				*(concat_path + arg_p ) = '/'; 
				k=0;
				
				for(k;k<=argFile_d;k++){ 
					*(concat_path+k+arg_p+1) = *(file->d_name +k);
				}   
				 

				//char * concat_path = strcat()
			//	printf("dir path %s \n",concat_path);
				/*
				 	above two for loops concatenate new address on  
					decided to do concat myself so that I can keep track of the addresses that I am
					dynamically allocating
				 */
				 
				struct thread_arg * sub_dir_arg = (struct thread_arg *)malloc(sizeof(struct thread_arg)); 
				sub_dir_arg->path = concat_path; 
				sub_dir_arg->lock = arg->lock;
				pthread_create(&thread, &threadAttr, dir_handler, (void *)(sub_dir_arg) );	
				
//				printf("hold path %s\n",hold);
				
			} 
		else if(file->d_type == DT_REG){ // checks if current object is a file
				
				
				int arg_p2 = strlen(arg->path); 
				int argFile_d2 = strlen(file->d_name);

				char * concat_path2 = (char *)malloc(sizeof(char) * (arg_p2 + argFile_d2 +2) ); // malloc space for new concat path
				*(concat_path2 + arg_p2 + argFile_d2+2) = '\0';
				
				int k=0;  

				for(k;k<arg_p2;k++){ 
					*(concat_path2+k) = *(arg->path +k);
				}  
				
				*(concat_path2 + arg_p2 ) = '/'; 
				k=0;
				
				for(k;k<=argFile_d2;k++){ 
					*(concat_path2+k+arg_p2+1) = *(file->d_name +k);
				}   
				
//				printf("file path %s \n",concat_path2);
			/*	
				 	above two for loops concatenate new address on  
					decided to do concat myself so that I can keep track of the addresses that I am
					dynamically allocating
				*/ 
				 
				struct thread_arg * sub_dir_arg = (struct thread_arg *)malloc(sizeof(struct thread_arg)); 
				sub_dir_arg->path = concat_path2;
				sub_dir_arg->lock = arg->lock;	
				pthread_create(&thread2, &threadAttr, file_handler, (void *)(sub_dir_arg) );
					
				//printf("%s\n",file->d_name);
			
		 } 
	}
		loc = telldir(ptr); // gets updated position to move next object
			
			
			if(loc == -1){ 
				break;
			}
		
		seekdir(ptr,loc); //moves pointer to next object
		
			
		file = readdir(ptr); 
	
	}

	closedir(ptr);
	pthread_join(thread,NULL);
	pthread_join(thread2,NULL);
	pthread_attr_destroy(&threadAttr);
	free(arg);
	pthread_exit(0);
}


