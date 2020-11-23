#include<stdlib.h>
#include<stdio.h>
#include<pthread.h>
#include<string.h>
#include<sys/types.h>
#include<dirent.h>

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

	dir_obj->path = dir_handle; 
	dir_obj->lock = NULL;
	
	
	dir_handler((void *)dir_obj);

	return 1;
}





void * file_handler(void * file_info){ 


struct thread_arg *arg  = (struct thread_arg *)file_info;


//pthread_mutex_lock(arg->lock);
printf("a\n");
// token function call
// call math functions  

//pthread_mutex_unlock(arg->lock);

pthread_exit(0);
}





void * dir_handler(void * dir_info){ 

	struct thread_arg * arg = (struct thread_arg *)dir_info ;

	printf("the dir name %s \n",arg->path);
	DIR * ptr = opendir(arg->path); 
	//printf("isnull %d",ptr == NULL);
	struct dirent * file  = readdir(ptr); 
	
	int safety  = 0; 
	long int loc = 0;
	
	pthread_attr_t	threadAttr;		
	pthread_attr_init(&threadAttr); 
	pthread_t thread;	// handles calls to dir handler 

	pthread_t thread2; // handles calls to file handler
	
	pthread_mutex_t file_lock;
	pthread_mutex_init(&file_lock,NULL);
	while(file != NULL){
		
		if(file->d_name[0]!='.'){
			if(file->d_type == DT_DIR){ // checks if current object is a directory
				char * hold  = arg->path;
				/*
				int arg_p = strlen(arg->path); 
				int argFile_d = strlen(file->d_name);

				char * old_path = (char *)malloc(sizeof(char) * (arg_p + argFile_d) ); // malloc space for new concat path
				*(old_path+arg_p+argFile_d) = '\0';
				
				int k=0;  

				for(k;k<arg_p;k++){ 
					*(old_path+k) = *(arg->path +k);
				}  
				
				*(old_path + arg_p ) = '/'; 
				k=0;
				for(k;k<=argFile_d;k++){ 
					*(old_path+k+arg_p+1) = *(file->d_name +k);
				}  
*/
				// above two for loops concatenate new address on
				 /* 
					decided to do concat myself so that I can keep track of the addresses that I am
					dynamically allocating
				 */
					arg->path = file->d_name;
				 
				pthread_create(&thread, &threadAttr, dir_handler, (void *)(arg) );	
				
//				printf("hold path %s\n",hold);

			//	pthread_join(thread, NULL); 
				
			//	free(old_path); // once we have used this new path we free the malloc memory
				
				arg->path = hold;// we then resume use of the old path
				
				//pthread_attr_destroy(&threadAttr);
			} 
		else if(file->d_type == DT_REG){ // checks if current object is a file
			/*	
				char * hold2  = arg->path;
				int arg_p2 = strlen(arg->path); 
				int argFile_d2 = strlen(file->d_name);

				char * old_path2 = (char *)malloc(sizeof(char) * (arg_p2 + argFile_d2) ); // stores original path before updating
				*(old_path2+arg_p2+argFile_d2) = '\0';
				int k=0;  

				for(k; k <arg_p2;k++){ 
					*(old_path2 + k) = *(arg->path +k);
				}  
				
				*(old_path2 + arg_p2 ) = '/'; 
				
				k=0;
				for(k;k<=argFile_d2;k++){ 
					*(old_path2 +k+arg_p2+1) = *(file->d_name +k);
				}  
				printf("current file path %s\n",old_path2);
			
				// above two for loops concatenate new address on
				  
				//	decided to do concat myself so that I can keep track of the addresses that I am
				//	dynamically allocating
				 
					arg->path = old_path2;
				 
//				pthread_create(&thread2, &threadAttr, file_handler, (void *)(arg) );	
				
//				printf("hold path %s\n",hold);

	//			pthread_join(thread2, NULL); 
				
				free(old_path2); 
				
				arg->path = hold2;
					
				*/	
					
					
					printf("%s\n",file->d_name);
			
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
	pthread_attr_destroy(&threadAttr);
	pthread_join(thread,NULL);
	pthread_exit(0);
	
}


