/* @authors 
Gazal Arora and Charles Rodriguez */


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
//double probability_calc(char * token ,struct Tnode * L1, struct Tnode * L2); 

//void complete_output(struct thread_arg * input ); 


struct thread_node * ThreadList = NULL;  

int main(int argc, char * argv[]){ 

  
  if(argc!=2){
    printf("Error: Invalid number of arguments\n");
    return 0;
  }
	char * dir_handle  = argv[1]; 

	struct stat stats;
	stat(dir_handle,&stats);
	if(!(S_ISDIR(stats.st_mode))){
	  printf("Error: Given path is not a directory\n");
	  return 0;
	}
	if(strcmp(dir_handle, ".")==0 || strcmp(dir_handle, "..")==0 || strcmp(dir_handle, "./")==0 || strcmp(dir_handle, "../")==0){
	    printf("Error: Can't run current or parent directory, please use relative path or absolute path for another dirrectory\n");
	    return 0;
	  }

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
 
 	//printf("parent Id %ld\n",parent); 

	pthread_join(parent,NULL); 

	struct thread_node * ptr = ThreadList;
//	dir_handler((void *)dir_obj);
	
	while(ptr!=NULL){ 
		pthread_join( *(ptr->id),NULL);
		ptr = ptr->nextId;
	} 
//	dir_handler((void *)dir_obj);

	if(strcmp((dir_obj->list_head)->file_handle,"")==0){
	  printf("Empty directory, nothing added to shared structure\n");
	  pthread_mutex_destroy(&file_lock);
          pthread_attr_destroy(&threadAttr2);
	  return 0;
	}
	//sort data structure based on number of tokens, emit an error & stop if a single file is there
	//call to make mean token list
	//use mean list and each file's token list to compute each file's KLD

	if((dir_obj->list_head)->next_list==NULL){
	  printf("Single file in directory, can't compute Jensen Shannon distance\n");
	  pthread_mutex_destroy(&file_lock);
          pthread_attr_destroy(&threadAttr2);
	  return 0;
	}

	//	printLL(dir_obj);
	complete_output(dir_obj);
	//	printLL(dir_obj);
       	free_stuff(dir_obj);
	pthread_mutex_destroy(&file_lock); 	
	pthread_attr_destroy(&threadAttr2); 
	//	free_stuff(dir_obj);
	//	printf("wowza\n");
	return 1;
}

void free_stuff(struct thread_arg * arg){

  struct Lnode* list = arg->list_head;
  //  struct Lnode* tempList;
  //struct Tnode* tokens;
  //struct Tnode* temp;
  //  if(list==NULL) puts("empty list");
  
  while(list!=NULL){
    if(strcmp(list->file_handle, "") != 0) {
    free(list->file_handle);
    //    puts("file handle freed");
    }
    struct Tnode* tokens = list->token_list;
    struct Lnode* tempList = list;
  while(tokens!=NULL){
    if(strcmp(tokens->token, "") != 0){
      free(tokens->token);
      //puts("token freed");
    }
    struct Tnode* temp = tokens;
    tokens = tokens->next_token;
    free(temp);
    //if (temp==NULL) puts("freed temp tokens");
  }
  list = list->next_list;
  free(tempList);
  if(tempList==NULL){
    //puts("freed ttemp list");
  }
  }
  free(arg);
  //if(arg==NULL) puts("freed arg");
  //  printf("arg %s\n", arg->path);
}

void printLL(struct thread_arg * arg){
 // puts("in printLL");
  int files = 0;
	struct thread_arg * ptr = arg;
	if( ptr->list_head==NULL){ 
		printf("not good\n");
	}
	printf("\nPrinting filenames\n");
	while(ptr->list_head!=NULL){
	  files++;
	  printf("File number %d: %s, num tokens %d\n", files, (ptr->list_head)->file_handle, (ptr->list_head)->num_tokens);
          ptr->list_head = (ptr->list_head)->next_list;
	}
}

char* input(struct thread_arg * arg, int fd){
  if (fd>=0) {
    //printf("\nfile %s opened\n", arg->path);
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
    //printf("File's data\n%s", buf);
  }
  //  buf[size] = '\0';
  return buf;
}


void  addToList(struct thread_arg * arg, struct Lnode * newLnode, char* string){

if(arg->list_head == NULL){ 
 	return;
 }
 newLnode->file_handle=arg->path;
 newLnode->token_list = tokenize(string, newLnode->token_list);
 newLnode->next_list = NULL;
 newLnode->num_tokens = (newLnode->token_list)->tot;
 struct Lnode *H= arg->list_head;
 
 // if(H==NULL) { puts("yessss, head is null");}


 if(strcmp(H->file_handle, "")==0){
   H->file_handle = newLnode->file_handle;
   H->token_list = newLnode->token_list;
   H->next_list = newLnode->next_list;
   H->num_tokens = newLnode->num_tokens;
   //if(H->token_list!=NULL){
     //H->num_tokens = (H->token_list)->tot;
     // }
   //   printf("adding first node to file list, filename: %s\n", H->file_handle);
 }
 
 else {
    struct Lnode *ptr = H;
   //printf("temp node %s\n", temp->file_handle);
   //Head = newLnode;
   //printf("list head %s\n", Head->file_handle);
   struct Lnode *prev = H;
   struct Lnode *temp;
   //head of list
   if(newLnode->num_tokens < H->num_tokens){     
     temp = malloc(sizeof(struct Lnode));
     temp->file_handle = H->file_handle;
     temp->token_list = H->token_list;
     temp->next_list = H->next_list;
     temp->num_tokens = H->num_tokens;
   
     H->file_handle = newLnode->file_handle;
     H->token_list = newLnode->token_list;
     H->next_list = temp;
     H->num_tokens = newLnode->num_tokens;
     return;
   }

   while(ptr!=NULL){
     //middle of list
      if(newLnode->num_tokens < ptr->num_tokens){
	prev->next_list = newLnode;
	newLnode->next_list = ptr;
	return;
      }
      prev = ptr;
      ptr=ptr->next_list;
   }
   //end of list
   prev->next_list = newLnode;
   newLnode->next_list  = NULL;
 }
}
    /*
	 while(ptr->next_list!=NULL){	   
	   ptr = ptr->next_list;
	 }
    */
  	//	printf("is it true %ld",ptr->next_list);

//	if(ptr == NULL){ 
//		printf(" \t \t WOEAODASDFASF \n");
//	}
    /*
	 ptr->next_list = newLnode;
	 (ptr->next_list)->token_list =  tokenize(string, newLnode->token_list);
	 if((ptr->next_list)->token_list!=NULL){
	 (ptr->next_list)->num_tokens = ((ptr->next_list)->token_list)->tot;
    */	 //	 printf("added current item %s\n",(ptr->next_list)->file_handle);
   



double probability_calc( char * token ,struct Tnode * L1, struct Tnode * L2){ 


struct Tnode * ptr = L1; 
struct Tnode * ptr2 = L2; 

double p1 = 0.0; 
double p2 = 0.0; 


				while(ptr!= NULL){ 
					if(strcmp(ptr->token, token) == 0){ 
							p1 = ptr->prob;
							break;
					}
					ptr = ptr->next_token;
				}
				
				while(ptr2!= NULL){ 
					if(strcmp(ptr2->token, token) == 0){ 
							p2 = ptr2->prob;
							break;
					}
					ptr2 = ptr2->next_token;
				}


return (p1 + p2)/2;

}


struct Tnode * ordered_insert(struct Tnode * shared_struct,char * token, double prob){ 
/* insert LL nodes into the mean linked list*/	 

	if(shared_struct == NULL){ 
			shared_struct  = (struct Tnode *)malloc(sizeof(struct Tnode)); 
			shared_struct->token = token; 
			shared_struct->prob = prob; 
			shared_struct->next_token = NULL;
			return shared_struct;
	}

	struct Tnode * ptr = shared_struct; 

			if(strcmp(ptr->token,token)==0){ 
						return shared_struct;
			}
	

	while(ptr->next_token != NULL){ 
				if(strcmp(ptr->token,token)==0){ 
						return shared_struct;
				}	
				ptr = ptr->next_token;
	} 
		
	if(strcmp(ptr->token,token)==0){ 
						return shared_struct;
			}
	
	struct Tnode * new_node = (struct Tnode *)malloc(sizeof(struct Tnode));
	new_node->token = token; 
	new_node->next_token = NULL;
	ptr->next_token = new_node;
	//ptr->prob = prob; 
	new_node->prob = prob;
	return shared_struct;
}






void * file_handler(void * file_info){ 


struct thread_arg *arg  = (struct thread_arg *)file_info;
//printf("file name %s\n",arg->path);


struct Lnode *newLnode = (struct Lnode*)malloc(sizeof(struct Lnode));
//if(arg->head==NULL) { puts("yessss");} 
  char *string; 
  //struct Tnode *token_list_head;
  pthread_mutex_lock(arg->lock);
  int fd = open(arg->path, O_RDONLY);
  string = input(arg, fd);
  addToList(arg, newLnode, string);
  pthread_mutex_unlock(arg->lock);
  //tokenize(string, newLnode->token_list);
  //  token_list_head=tokenize(string);

//free(string);

//free(arg);

pthread_exit(0);

}


void printMeanList(struct Tnode* mean){
	struct Tnode * ptr  = mean;
  while(ptr!=NULL){
    printf("token name: %s, mean prob %lf\n", ptr->token, ptr->prob);
    ptr = ptr->next_token;
  }
  
}

void complete_output(struct thread_arg * input){ 

  printf("\nJensen-Shannon Distance for each pair of file in given directory:\n\n");
struct Lnode * ptr = input->list_head;
struct Lnode * ptr2 = NULL;
while(ptr->next_list!=NULL){ 
	ptr2 = ptr->next_list;
	if(strcmp((ptr->token_list)->token, "")==0)
                          {
                            printf("No tokens in \"%s\" to compare with any other file\n", ptr->file_handle);
			    ptr = ptr->next_list;
			    continue;
                          }

	
	while(ptr2!=NULL){ 
			
			struct Tnode * list_ptr = ptr->token_list;	
			struct Tnode * list_ptr2 = ptr2->token_list;
			
			struct Tnode * mean_list =  NULL;
			/*if(strcmp(list_ptr->token, "")==0)
			  {
			    printf("No tokens in \"%s\" to compare with any other file\"\n", ptr->file_handle);
			    break;
			    }*/
			if(strcmp(list_ptr2->token, "")==0)
                          {
			   printf("Can't compare tokens in \"%s\" with ZERO tokens in empty file \"%s\"\n", ptr->file_handle, ptr2->file_handle);
			   ptr2 = ptr2->next_list;			  
                           continue;
                          }
			mean_list = ordered_insert(mean_list ,list_ptr->token, list_ptr->prob);
			
			while(list_ptr!=NULL){ 
				mean_list = ordered_insert(mean_list,list_ptr->token,list_ptr->prob);
		//						printf("list 1 toks %s \n",list_ptr->token);
				list_ptr = list_ptr->next_token;
			}
			
			while(list_ptr2!=NULL){ 
				mean_list = ordered_insert(mean_list,list_ptr2->token,list_ptr2->prob);
			//		printf("list 2 toks %s \n",list_ptr2->token);
				list_ptr2 = list_ptr2->next_token;
			}
			
			list_ptr = ptr->token_list; 
			list_ptr2 = ptr2->token_list;
			struct Tnode * mean_list_ptr = mean_list; 
				
			while(mean_list_ptr!=NULL){ 
								
				mean_list_ptr-> prob = probability_calc( mean_list_ptr->token ,list_ptr, list_ptr2);
				
				//printf(" token (%s)  %lf \n",mean_list_ptr->token, mean_list_ptr->prob);
				mean_list_ptr = mean_list_ptr->next_token;
				
			} 
			//printMeanList(mean_list);
		kld(ptr, ptr2, mean_list);
		ptr2 = ptr2->next_list;
		//freeing the mean_list for this pair of files                                                                                                                                   
		struct Tnode* temp;
		while(mean_list!=NULL){
		  temp = mean_list;
		  mean_list = mean_list->next_token;
		  free(temp);
		  //printf("freed\n");
		}
	}
	ptr = ptr->next_list;
 
 }
 printf("\n");
}




void kld(struct Lnode* list1, struct Lnode *list2 , struct Tnode* mean_list){


struct Lnode * ptr = list1;
struct Lnode * ptr2 = list2;

struct Tnode * list_ptr = ptr->token_list;
struct Tnode * list_ptr2 = ptr2->token_list;
struct Tnode * mean_list_ptr = mean_list;
 
 double k1 = 0.0;
 double k2 = 0.0 ; 

 double jsd, x, m, log1, log2 = 0.0;
    while(list_ptr!=NULL){
        x = list_ptr->prob;
        mean_list_ptr = mean_list; // resets mean_list ptr to head of mean list after iteration
	while(mean_list_ptr!=NULL){
            if(strcmp(mean_list_ptr->token, list_ptr->token)==0){
                 m = mean_list_ptr->prob;
                 break;
	     }
             mean_list_ptr = mean_list_ptr->next_token;
	}
			log1 = log10((x/m));
	    k1 = k1 + (x * log1);
			list_ptr = list_ptr->next_token;
   }
	 

          mean_list_ptr = mean_list;

          while(list_ptr2!=NULL){
            x = list_ptr2->prob;
             mean_list_ptr = mean_list;
	       while(mean_list_ptr!=NULL){
                if(strcmp(mean_list_ptr->token, list_ptr2->token)==0){
                  m = mean_list_ptr->prob;
                  break;
		}
                mean_list_ptr = mean_list_ptr->next_token;
               }
	    log2 = log10((x/m));
	    k2 = k2 + (x * log2);
	    list_ptr2 = list_ptr2->next_token;
	  } 

	  jsd = ((k1+k2))/2 ;
	    
	  // printf("JSD for file %s, file %s is %lf\n", list1->file_handle, list2->file_handle, jsd);
	    if(jsd>=0.0 && jsd<=0.1){
	      printf("\033[0;31m"); //red
	      printf("%.6f \"%s\" and \"%s\"\n", jsd, list1->file_handle, list2->file_handle);
	    }
	    else if(jsd>0.1 && jsd<=0.15){
	      printf("\033[0;33m");//yellow
	      printf("%.6f \"%s\" and \"%s\"\n", jsd, list1->file_handle, list2->file_handle);
	     }
	    else if(jsd>0.15 && jsd<=0.2){
	      printf("\033[0;32m");//green
	      printf("%.6f \"%s\" and \"%s\"\n", jsd, list1->file_handle, list2->file_handle);
	      }
	    else if(jsd>0.2 && jsd<=0.25){
	      printf("\033[0;36m");//cyan
	      printf("%.6f \"%s\" and \"%s\"\n", jsd, list1->file_handle, list2->file_handle);
	     }
	    else if(jsd>0.25 && jsd<=0.3){
	      printf("\033[0;34m");//blue
	      printf("%.6f \"%s\" and \"%s\"\n", jsd, list1->file_handle, list2->file_handle);
	      }
	    else if(jsd>0.3){
	      printf("\033[0m"); //default
	      printf("%.6f \"%s\" and \"%s\"\n", jsd, list1->file_handle, list2->file_handle);
	      }
	    printf("\033[0m");

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
		//		printf("thread Id %ld \n",*(ptr->id));
		ptr = ptr->nextId;
	}
	// 	printf("thread count %d\n",i);
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
		
	  if(strcmp(file->d_name, ".")!=0 && strcmp(file->d_name, "..")!=0){
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
				//				printf("dir path %s \n",concat_path);
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
	//	free(arg);
//	print_thread_list(ThreadList);
//	return NULL ;
	pthread_exit(0);
}


