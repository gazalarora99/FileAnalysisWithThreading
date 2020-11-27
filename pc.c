#include<stdlib.h>
#include<stdio.h>
#include<pthread.h>
#include<string.h>
#include<sys/types.h>
#include<dirent.h>

void * dir_handler(void * dir_info); 
void * file_handler(void * file_info);
struct Tnode * ordered_insert(struct Tnode * shared_struct,char * token, double prob); 
  

struct thread_arg{ // this argument stuct is passed to pthread_create so our routine has access to the right arguments 
	pthread_mutex_t * lock;
	char * path;	
}; 

struct thread_node{ 
	pthread_t * id; // thread id 
	struct thread_node * nextId;
};

struct thread_node * id_list = NULL;  

struct thread_node * idIns(pthread_t * id, struct thread_node * list);
void print_Id_list(struct thread_node * list);

struct Tnode{  // token node for linked list of tokens
char * token;
double prob; 
struct Tnode * next_token;
};


struct Lnode{ // list nodes for list of linked list
char * file_handle;
struct Tnode ** token_list;
struct Lnode * next_list;
};



int main(int argc, char * argv[]){ 


	char * dir_handle  = argv[1]; 



	struct thread_arg * dir_obj = (struct thread_arg *)malloc(sizeof(struct thread_arg));  
	pthread_mutex_t file_lock; // for file sync to work we need to make sure the lock is initialized outside of function 
	pthread_mutex_init(&file_lock,NULL);
	
	dir_obj->path = dir_handle; 
	dir_obj->lock = &file_lock;
/*	
	pthread_attr_t	threadAttr2;		
	pthread_attr_init(&threadAttr2);

	pthread_t parent;
	
	pthread_create(&parent,&threadAttr2,dir_handler, (void *)dir_obj);
 
 	printf("parent Id %ld\n",parent); 

	 
	

	pthread_join(parent,NULL);
*/	
	struct thread_node * ptr = id_list;
	dir_handler((void *)dir_obj);
	
	while(ptr!=NULL){ 
		pthread_join( *(ptr->id),NULL);
		ptr = ptr->nextId;
	} 
	//pthread_exit(0);
//	dir_handler((void *)dir_obj);
	
	pthread_mutex_destroy(&file_lock); 	
//	pthread_attr_destroy(&threadAttr2); 

	return 1;
}



struct Tnode * ordered_insert(struct Tnode * shared_struct,char * token, double prob){ 
/* insert nodes in alphabetical order 
need to free this all later since this memory is dynamically allocated
We want to make sure that the inserted tokens are unique and not duplicates
*/	
		if(shared_struct == NULL){ 
						struct Tnode * head  =  (struct Tnode *)malloc(sizeof(struct Tnode)); 
						head->token = token;
						head->prob = prob;
						head->next_token = NULL;
						return head;
				} 
				else{ 
						struct Tnode * ptr = shared_struct;
						int flag = 0;
						
						if( *(ptr->token) > *token){ 
							struct Tnode * new_head = (struct Tnode *)malloc(sizeof(struct Tnode)); 
							new_head->token = token; 
							new_head->prob = prob;
							new_head->next_token = ptr; 
							return ptr;
						} 

						while(ptr->next_token != NULL){ 
								
								if(*(ptr->next_token->token) > *token){ 
											flag = 1;
											break;
											
									}
								
								ptr = ptr->next_token;
						}

						
						if(flag){
							struct Tnode * new_node = (struct Tnode *)malloc(sizeof(struct Tnode));
							new_node->token = token;
							new_node->prob = prob;
							
							struct Tnode * hold; 

							hold = ptr->next_token;
							ptr->next_token = new_node;
							new_node->next_token = hold;
							return shared_struct;
						} 
						else{ 
								struct Tnode * new_node = (struct Tnode *)malloc(sizeof(struct Tnode));			
								new_node->token = token; 
								new_node->prob = prob; 
								ptr->next_token = new_node; 
								return shared_struct;
						}


				}
				






}






void * file_handler(void * file_info){ 


struct thread_arg *arg  = (struct thread_arg *)file_info;

		
	pthread_mutex_lock(arg->lock);
		
	int i =0; 
	for(i;i<5;i++){ 
		printf("%d\n",i);
	} 
	printf("%s\n",arg->path);
	
	pthread_mutex_unlock(arg->lock);
	


	free(arg);
	pthread_exit(0);

}


struct thread_node * idIns(pthread_t * id, struct thread_node * list){ 
	/*  
		We are going to make a global list of all created threads in dir handler
	*/
	if(list == NULL){ 
			list = (struct thread_node *)malloc(sizeof(struct thread_node ));  
			list->id = id; 
			list->nextId = NULL; 
			return list;
	}
	

 printf("we went here\n");
 
 struct thread_node * ptr = list;
 	while(ptr->nextId!=NULL){ 
		ptr = ptr->nextId;
	} 

	struct thread_node * newId = (struct thread_node *)malloc(sizeof(struct thread_node ));  
	newId->id = id; 
	newId->nextId = NULL;
	
	ptr->nextId = newId;
	
	return list;

}

void print_Id_list(struct thread_node * list){ 


struct thread_node * ptr = list;
	int i =0;
	while(ptr!=NULL){ 
		i++;
		printf("thread Id %ld \n",*(ptr->id));
		ptr = ptr->nextId;
	}
 	printf("thread count %d\n",i);
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
				printf("dir path %s \n",concat_path);
				/*
				 	above two for loops concatenate new address on  
					decided to do concat myself so that I can keep track of the addresses that I am
					dynamically allocating
				 */
				 
				struct thread_arg * sub_dir_arg = (struct thread_arg *)malloc(sizeof(struct thread_arg)); 
				sub_dir_arg->path = concat_path;
				sub_dir_arg->lock = arg->lock;
				
				pthread_t  *ptr = (pthread_t * )malloc(sizeof(pthread_t));
				pthread_create(ptr, &threadAttr, dir_handler, (void *)(sub_dir_arg) );	
				
				id_list = idIns(ptr,id_list); 

				printf("thread  name %ld \n",*ptr);
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
				pthread_t * ptr = (pthread_t * )malloc(sizeof(pthread_t));
				
				pthread_create(ptr, &threadAttr, file_handler, (void *)(sub_dir_arg) );
				id_list = idIns(ptr,id_list); 
				
				printf("thread 2 name %ld \n",*ptr); 
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
//	pthread_join(thread,NULL);
//	pthread_join(thread2,NULL);
	// pthread_attr_destroy(&threadAttr);
	free(arg);
	print_Id_list(id_list);
	return NULL ;
//	pthread_exit(0);
}


