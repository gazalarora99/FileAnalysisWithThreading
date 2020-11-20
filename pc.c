#include<stdlib.h>
#include<stdio.h>
#include<pthread.h>
#include<string.h>
#include<sys/types.h>
#include<dirent.h>

void * dir_handler(void * dir_info); 

void * file_handler(void * file_obj);

struct thread_arg{ // this argument stuct is passed to pthread_create so our routine has access to the right arguments 
	pthread_mutex_t * lock;
	char * path;	
};

int main(int argc, char * argv[]){ 


	char * dir_handle  = argv[1]; 



	struct thread_arg * dir_obj = (struct thread_arg *)malloc(sizeof(struct thread_arg));  

	dir_obj->path = dir_handle; 
	dir_obj->lock = NULL;
	
	
	dir_handler((void *)dir_obj);

	return 1;
}







void * dir_handler(void * dir_info){ 

	struct thread_arg * arg = (struct thread_arg *)dir_info ;

	printf("the dir name %s",arg->path);
	DIR * ptr = opendir(arg->path); 

	struct dirent * file  = readdir(ptr); 
	
	int safety  = 0; 
	long int loc = 0;
	
	pthread_attr_t	threadAttr;		
	pthread_attr_init(&threadAttr); 
	pthread_t thread;	


	while(file != NULL){
			if(file->d_type == DT_DIR){ // checks if current object is a directory
			
				
				arg->path = strcat(file->d_name, arg->path); // gets the name of current dir object and appends it to the path of current obj
				
				pthread_create(&thread, &threadAttr, dir_handler, (void *)(arg) );
				
				pthread_join(thread, NULL); 
				pthread_attr_destroy(&threadAttr);
			} 
		else if(file->d_type == DT_REG){ // checks if current object is a file
				printf("%s\n",file->d_name);
			
		 } 

		loc = telldir(ptr); // gets updated position to move next object
			
			
			if(loc == -1){ 
				break;
			}
		
		seekdir(ptr,loc); //moves pointer to next object
		
			
		file = readdir(ptr); 
	
	}


	closedir(ptr);
	
	pthread_exit(0);
	
}


