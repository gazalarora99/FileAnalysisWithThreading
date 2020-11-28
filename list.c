/*
#include<stdlib.h>
#include<stdio.h>
#include<pthread.h>
#include<string.h>
#include<sys/types.h>
#include<dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h> 
*/

#include "tokenizer.h"

/*
struct Tnode{  // token node for linked list of tokens                                                                                                                                                                                        
char * token;
double prob;
  int occur;
  struct Tnode * next_token;
};


struct Lnode{ // list nodes for list of linked list                                                                                                                                                                                           
char * file_handle;
int num_tokens;
struct Tnode * token_list;
struct Lnode * next_list;
};  

struct thread_arg{ // this argument stuct is passed to pthread_create so our routine has access to the right arguments 
	pthread_mutex_t * lock;
	char * path;	
        struct Lnode *list_head;
}; 

struct thread_node{ 
	pthread_t * id; // thread id 
	struct thread_node * nextId;
};



//global thread id list   

struct thread_node * id_list = NULL;


void * dir_handler(void * dir_info);
void * file_handler(void * file_info);
struct Tnode * ordered_insert(struct Tnode * shared_struct,char * token, double prob);
char * input(struct thread_arg * arg, int fd);
void addToList(struct thread_arg * arg, struct Lnode * Head, char * string);
void printLL(struct thread_arg * arg);
struct thread_node * idIns(pthread_t * id, struct thread_node * list);
void print_Id_list(struct thread_node * list);
*/

struct thread_node * ThreadList = NULL;  

int main(int argc, char * argv[]){ 


	char * dir_handle  = argv[1]; 



	struct thread_arg * dir_obj = (struct thread_arg *)malloc(sizeof(struct thread_arg));  
	pthread_mutex_t file_lock; // for file sync to work we need to make sure the lock is initialized outside of function 
	pthread_mutex_init(&file_lock,NULL);
	
	dir_obj->path = dir_handle; 
	dir_obj->lock = &file_lock;
	dir_obj->list_head = (struct Lnode *)malloc(sizeof(struct Lnode));
	(dir_obj->list_head)->file_handle = "";
	(dir_obj->list_head)->num_tokens=0;
	(dir_obj->list_head)->token_list = NULL;
	(dir_obj->list_head)->next_list = NULL;

	
	pthread_attr_t	threadAttr2;		
	pthread_attr_init(&threadAttr2);

	pthread_t parent;
	
	pthread_create(&parent,&threadAttr2,dir_handler, (void *)dir_obj);
 
 	printf("parent Id %ld\n",parent); 

	pthread_join(parent,NULL); 

	struct thread_node * ptr = ThreadList;
//	dir_handler((void *)dir_obj);
	
	while(ptr!=NULL){ 
		pthread_join( *(ptr->id),NULL);
		ptr = ptr->nextId;
	} 
//	dir_handler((void *)dir_obj);

        printLL(dir_obj);
	pthread_mutex_destroy(&file_lock); 	
	pthread_attr_destroy(&threadAttr2); 
	printf("wowza\n");
	return 1;
}


void printLL(struct thread_arg * arg){
 // puts("in printLL");
	struct thread_arg * ptr = arg;
	if( ptr->list_head==NULL){ 
		printf("not good\n");
	}
	while(ptr->list_head!=NULL){
    printf("current filename: %s\n", (ptr->list_head)->file_handle);
    ptr->list_head = (ptr->list_head)->next_list;
  }
}

char* input(struct thread_arg * arg, int fd){
  if (fd>=0) {
    printf("file %s opened\n", arg->path);
  }
  off_t off = lseek(fd, 0, SEEK_END);
  //printf("size of file %llu\n", (long long int) off);
  // char *buf = malloc(sizeof(char) * ((int)off + 1));
  int size = (int) off;
  char *buf = malloc(sizeof(char) * (size + 1));
  off= lseek(fd,0,SEEK_SET);
  //printf("new offset %llu\n", (long long int) off);
  int bytes, pos;
  buf[size] = '\0';
  while((bytes=read(fd,buf,size)) > 0){
    //printf("read %d bytes\n", bytes);
    printf("%s\n", buf);
  }
  //  buf[size] = '\0';
  return buf;
}


void  addToList(struct thread_arg * arg, struct Lnode * newLnode, char* string){

if(arg->list_head == NULL){ 
 	return;
}
 newLnode->file_handle=arg->path;
 newLnode->token_list = NULL;
 newLnode->next_list = NULL;
 newLnode->num_tokens = 0;
 struct Lnode *H= arg->list_head;
 
 if(H==NULL) { puts("yessss, head is null");}

 if(strcmp(H->file_handle, "")==0){
   H->file_handle = newLnode->file_handle;
   H->token_list = newLnode->token_list;
   H->next_list = newLnode->next_list;
   H->num_tokens = newLnode->num_tokens;
   printf("adding first node to file list, filename: %s\n", H->file_handle);
 }
 else {
    struct Lnode *ptr = H;
   //printf("temp node %s\n", temp->file_handle);
   //Head = newLnode;
   //printf("list head %s\n", Head->file_handle);
   // struct Lnode *prev = NULL;
   
	 
	 while(ptr->next_list!=NULL){
         ptr = ptr->next_list;
	 }
  	//	printf("is it true %ld",ptr->next_list);

//	if(ptr == NULL){ 
//		printf(" \t \t WOEAODASDFASF \n");
//	}
	 ptr->next_list = newLnode;
	 //	 (ptr->next_list)->token_list =  tokenize(string, newLnode->token_list);
	 
	printf("added current item %s\n",(ptr->next_list)->file_handle);
   //   printf("adding to head of the non-empty list, filename: %s\n", (arg->list_head)->file_handle);                                                                                   
 	return;
 }  
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
//printf("file name %s\n",arg->path);


struct Lnode *newLnode = (struct Lnode*)malloc(sizeof(struct Lnode));
//if(arg->head==NULL) { puts("yessss");} 
  char *string; 
  //  struct Tnode *token_list_head;
  pthread_mutex_lock(arg->lock);
  int fd = open(arg->path, O_RDONLY);
  string = input(arg, fd);
  addToList(arg, newLnode, string);
  pthread_mutex_unlock(arg->lock);
  //tokenize(string, newLnode->token_list);
  //  token_list_head=tokenize(string);

//free(string);

free(arg);

pthread_exit(0);

}


  /*
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
  */



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
	

// printf("we went here\n");
 
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

void print_thread_list(struct thread_node * list){ 


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
				sub_dir_arg->list_head = arg->list_head;
				pthread_t  *ptr = (pthread_t * )malloc(sizeof(pthread_t));
				pthread_create(ptr, &threadAttr, dir_handler, (void *)(sub_dir_arg) );	
				
				ThreadList = idIns(ptr,ThreadList); 

		//		printf("thread  name %ld \n",*ptr);
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
				sub_dir_arg->list_head = arg->list_head;
				pthread_t * ptr = (pthread_t * )malloc(sizeof(pthread_t));
				
				pthread_create(ptr, &threadAttr, file_handler, (void *)(sub_dir_arg) );
				ThreadList = idIns(ptr,ThreadList); 
				
			//	printf("thread 2 name %ld \n",*ptr); 
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
//	print_thread_list(ThreadList);
//	return NULL ;
	pthread_exit(0);
}

