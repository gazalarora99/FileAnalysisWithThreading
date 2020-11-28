#include<stdlib.h>
#include<stdio.h>
#include<pthread.h>
#include<string.h>
#include<sys/types.h>
#include<dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include "tokenizer.h"

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

//struct Lnode * head;                                                                                                                                                                                                                        

struct thread_arg{ // this argument stuct is passed to pthread_create so our routine has access to the right arguments                                                                                                                        
        pthread_mutex_t * lock;
        char * path;
  struct Lnode *list_head;
};

void * dir_handler(void * dir_info);
void * file_handler(void * file_info);
char * input(struct thread_arg * arg, int fd);
void addToList(struct thread_arg * arg, struct Lnode * H,char* string);
void printLL(struct thread_arg * arg);
