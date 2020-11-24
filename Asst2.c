#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <fcntl.h>

void *file_handler(void *argument);
void *directory_handler(void *argument);

struct token_listNode{
  char* token_name;
  double pDistribution;
  struct token_listNode *next_token;
};

struct file_listNode{
  char* filename;
  int num_tokens;
  struct file_listNode *nextFile;
  struct token_listNode *tokenList;
  
};

struct thread_arg{
struct file_listNode *file_head;
pthread_mutex_t mut;
char* path;
};



/*function that takes thread_arg
as an argument and reads files from it
dirent current gets overwritten each time a 
new directory or file is found, we assume that
each directory contains only subdirectories and
text files, no other kind of files will be given to us
*/
void *directory_handler(void *argument){

  struct thread_arg* arg = (struct thread_arg*)argument;
  DIR *dirp = opendir(arg->path);
  if(dirp == NULL)
    {
        perror("Not a directory");
        exit(EXIT_FAILURE);
    }
  struct dirent *current; //this dirent gets over written each time readdir is called
  int files = 0;
  char* sub_dir; 
  
    if(dirp == NULL)
    {
        perror("Not a directory");
        exit(EXIT_FAILURE);
    }

    pthread_attr_t threadAttr, file_attr;		
    pthread_attr_init(&threadAttr);
    pthread_attr_init(&file_attr);
    pthread_t thread, file_thread;
	
	
    while(current=readdir((dirp))){
    files++;

    //skip over . and .. directories else we will have an infinite
    //number of directories during recursion
    if(strcmp(current->d_name, ".")==0 || strcmp(current->d_name, "..")==0){
      continue;
    }

    //DT_DIR = 4 means current is a directory
    if(current->d_type==DT_DIR){
      //recursion here along with creation of thread
      //if we hit a subdirectory, we set sub_dir with d_name as this
      //is the next directory we pass as argument to new thread using struct thread_arg
      // sub_dir = current->d_name; //you may want to concatenate this with argument->dir/
      pthread_mutex_lock(&(arg->mut));
      char * old_path = (char *)malloc(sizeof(char)*strlen(arg->path)); 
      int k;

      for(k=0; k<strlen(arg->path); k++){
	*(old_path+k) = *(arg->path+k);
      }

      arg->path = strcat(arg->path,"/");
      arg->path = strcat(arg->path,current->d_name);
      pthread_create(&thread, &threadAttr, directory_handler, (void *)(arg));
       pthread_mutex_unlock(&(arg->mut));
      //      pthread_create(&thread, &threadAttr, directory_handler, (void *)(arg));
      printf("current path %s\n",old_path);
      pthread_join(thread, NULL);
       pthread_mutex_lock(&(arg->mut));
      arg->path = old_path; // roll back file path after thread has finished exploring previous dir
      pthread_mutex_unlock(&(arg->mut));
      pthread_attr_destroy(&threadAttr);
      
    }
    /*
    //DT_REG = 8 means current is a file
    else if(current->d_type==DT_REG){
      pthread_mutex_lock(&(arg->mut));
      char * old_path = (char *)malloc(sizeof(char)*strlen(arg->path));
      int k;

      for(k=0; k<strlen(arg->path); k++){
        *(old_path+k) = *(arg->path+k);
      }

      arg->path = strcat(arg->path,"/");
      arg->path = strcat(arg->path,current->d_name);
      pthread_mutex_unlock(&(arg->mut));
      pthread_create(&file_thread, &file_attr, file_handler, (void *)(arg));
      printf("current path %s\n",old_path);
      pthread_join(file_thread, NULL);
      pthread_mutex_lock(&(arg->mut));
      arg->path = old_path; // roll back file path after thread has finished exploring previous dir                                                                                      
      pthread_mutex_unlock(&(arg->mut));
      pthread_attr_destroy(&file_attr);
    }
    */
    //skip over anything that is not a file or directory 
    else{
      continue;
    }
    
    printf("File %d's name: %s, Type: %d\n", files, current->d_name, current->d_type);
    }
    printf("File %d's name: %s, Type: %d\n", files, current->d_name, current->d_type);
    //closing the open directory                                                                                                                                                         
    closedir(dirp);
    pthread_exit(NULL);
}


void *file_handler(void* argument){

  struct thread_arg* arg = (struct thread_arg*)argument;

   struct file_listNode* temp;
      struct file_listNode* new_node = malloc(sizeof(struct file_listNode)) ;
      new_node->num_tokens = 0;
      new_node->tokenList = NULL;

      pthread_mutex_lock(&(arg->mut));
      new_node->filename = arg->path;
      temp = arg->file_head;
      arg->file_head = new_node;
      new_node->nextFile = temp;
      pthread_mutex_unlock(&(arg->mut));

      int fd = open(new_node->filename, O_RDONLY);

      if(fd<=0){
	puts("Not a file");
	exit(EXIT_FAILURE);
      }

      off_t off = lseek(fd,0,SEEK_END);
      printf("size of this file %llu", (long long int)off);
      
      close(fd);
      pthread_exit(NULL);
  
}
  
int main(int argc, char **argv)
{
  //check for valid number of arguments 
  if(argc!=2){
    puts("Invalid number of arguments");
    exit(0);
  }
  char* dir_handle  = argv[1]; 
  struct thread_arg *args = (struct thread_arg*) malloc (sizeof(struct thread_arg));
  args->path = dir_handle;
  args->file_head = NULL;
  pthread_mutex_init(&args->mut, NULL);

  directory_handler((void*)args);
  
  //colored printing on terminal
    printf("\033[0;31m");
    printf("This better be red\n");
    printf("\033[0m");
    return(0);
}

