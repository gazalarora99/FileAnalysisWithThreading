#include<stdlib.h>
#include<stdio.h>
#include<pthread.h>
#include<string.h>
#include<sys/types.h>
#include<dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
//#include <tokenizer.h>
struct Tnode{  // token node for linked list of tokens                                                                                                                                   
char * token;
double prob;
struct Tnode * next_token;
};

struct Lnode{ // list nodes for list of linked list                                                                                                                                      
char * file_handle;
int num_tokens;
struct Tnode * token_list;
struct Lnode * next_list;
};

//struct Lnode * head;

struct thread_arg{ // this argument stuct is passed to pthread_create so our routine has access to the right arguments 
	pthread_mutex_t * lock;
	char * path;
  struct Lnode *head;
}; 

void * dir_handler(void * dir_info);
void * file_handler(void * file_info);
char * input(struct thread_arg * arg, int fd);
void addToList(struct thread_arg * arg, struct Lnode * Head);
void printLL(struct thread_arg * arg);



int main(int argc, char * argv[]){ 


	char * dir_handle  = argv[1]; 
  
	
	struct thread_arg * dir_obj = (struct thread_arg *)malloc(sizeof(struct thread_arg));  
	
	pthread_mutex_t file_lock;
	pthread_mutex_init(&file_lock,NULL);
	
	dir_obj->path = dir_handle; 
	dir_obj->lock = &file_lock;
	dir_obj->head = (struct Lnode *)malloc(sizeof(struct Lnode));
	dir_handler((void *)dir_obj);
	printf("whoops");	
	printLL(dir_obj);
	free(dir_obj);
	return 1;
}


void printLL(struct thread_arg * arg){
 // puts("in printLL");
	if( arg->head==NULL){ 
		printf("not good\n");
	}
	while(arg->head!=NULL){
    printf("current filename: %s\n", (arg->head)->file_handle);
    arg->head = (arg->head)->next_list;
  }
}

char* input(struct thread_arg * arg, int fd){
  if (fd>=0) {
    printf("file %s opened\n", arg->path);
  }
  off_t off = lseek(fd, 0, SEEK_END);
  printf("size of file %llu\n", (long long int) off);
  char *buf = malloc(sizeof(char) * ((int)off));
  int size = (int) off;
  off= lseek(fd,0,SEEK_SET);
  printf("new offset %llu\n", (long long int) off);
  int bytes, pos;
  while((bytes=read(fd,buf,size)) > 0){
    printf("read %d bytes\n", bytes);
    printf("%s\n", buf);
  }
  return buf;
}

void  addToList(struct thread_arg * arg, struct Lnode * newLnode){
 newLnode->file_handle=arg->path;
 newLnode->token_list = NULL;
 newLnode->num_tokens = 0;
 struct Lnode *Head = arg->head;
 if(Head==NULL) { puts("yessss, head is null");}

 if(Head!=NULL){
    struct Lnode *temp = Head;
   //printf("temp node %s\n", temp->file_handle);
   //Head = newLnode;
   //printf("list head %s\n", Head->file_handle);
    struct Lnode *prev = NULL;
   while(temp!=NULL){
     prev = temp;
     temp = temp->next_list;
   }
   prev->next_list = newLnode;
   printf("previous item %s, current item %s\n",prev->file_handle, (prev->next_list)->file_handle);
   //   printf("adding to head of the non-empty list, filename: %s\n", (arg->list_head)->file_handle);                                                                                   
 }
 else{
   //   newLnode->next_list= NULL;                                                                                                                                                       
   //   if(newLnode==NULL) { puts("yessss");}                                                                                                                                            
   Head = newLnode;
   //   Head->next_list = NULL;                                                                                                                                                          
   printf("adding first node to file list, filename: %s\n", Head->file_handle);
 }
 //return Head->token_list;
  
}



void * file_handler(void * file_info){ 

struct thread_arg *arg  = (struct thread_arg *)file_info;
struct Lnode *newLnode = (struct Lnode*)malloc(sizeof(struct Lnode));
//if(arg->head==NULL) { puts("yessss");} 
  char *string; 
  struct Tnode *token_list_head;
  pthread_mutex_lock(arg->lock);
  addToList(arg, newLnode);
  int fd = open(arg->path, O_RDONLY);
  string = input(arg, fd);
  pthread_mutex_unlock(arg->lock);
  
	//  token_list_head=tokenize(string);
free(string);
free(arg);
//free(newLnode);
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
	//	struct thread_arg * sub_dir_arg2 = (struct thread_arg *)malloc(sizeof(struct thread_arg));
	//struct thread_arg * sub_dir_arg1 = (struct thread_arg *)malloc(sizeof(struct thread_arg));
	
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
				 

				/*
				 	above two for loops concatenate new address on  
					decided to do concat myself so that I can keep track of the addresses that I am
					dynamically allocating
				 */
				 
				struct thread_arg * sub_dir_arg = (struct thread_arg *)malloc(sizeof(struct thread_arg)); 
				sub_dir_arg->path = concat_path; 
				sub_dir_arg->lock = arg->lock;
				sub_dir_arg->head = arg->head;
				pthread_create(&thread, &threadAttr, dir_handler, (void *)(sub_dir_arg) );	
				
				
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
				
			/*	
				 	above two for loops concatenate new address on  
					decided to do concat myself so that I can keep track of the addresses that I am
					dynamically allocating
				*/ 
				 
				struct thread_arg * sub_dir_arg = (struct thread_arg *)malloc(sizeof(struct thread_arg)); 
				sub_dir_arg->path = concat_path2;
				sub_dir_arg->lock = arg->lock;	
				sub_dir_arg->head = arg->head;
				pthread_create(&thread2, &threadAttr, file_handler, (void *)(sub_dir_arg) );
					
			
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
	return NULL;
}


