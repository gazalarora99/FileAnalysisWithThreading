#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

struct token_listNode{
  char* token_name;
  double pDistribution;
  struct token_listNode next_token;
};

struct file_listNode{
  char* filename;
  int num_tokens;
  struct file_listNode nextFile;
  struct token_listNode tokenList;
  
};

struct thread_arg{
struct file_listNode file_head;
pthread_mutex_t mut;
DIR open_dir;
};



/*function that takes thread_arg
as an argument and reads files from it
dirent current gets overwritten each time a 
new directory or file is found, we assume that
each directory contains only subdirectories and
text files, no other kind of files will be given to us
*/
void *directory_handler(void *arg){

//this dirent gets over written each time readdir is called
  struct dirent *current; 
  int files = 0;
  struct thread_arg* argument = (struct thread_arg*)arg;
  while(current=readdir((argument->open_dir)){
    files++;

    //skip over . and .. directories else we will have an infinite
    //number of directories during recursion
    if(current->d_name=='.' || current->d_name=='..'){
      continue;
    }

    //DT_DIR = 4 means current is a directory
    if(current->d_type==4){
      //recursion here along with creation of thread
    }
    //DT_REG = 8 means current is a file
    else if(current->d_type==8){
      //create struct for new file_listNode, then lock mutex, add struct to shared list, then unlock
      //minimum functionality inside the mutex lock so that if malloc fails
      //we don't encounter the fail within a lock
      struct file_listNode* new_node = malloc(sizeof(struct file_listNode)) ;
      new_node->filename = current->d_name;
      new_node->num_token = 0;
      new_node->tokenList = NULL;
      struct file_listNode* temp;

      //lock, adding to head of the list
      temp = argument->fileHead;
      argument->fileHead = new_node;
      new_node->nextFile = temp;
      //unlock
    }
    
    //skip over anything that is not a file or directory 
    else{
      continue;
    }
    
    printf("File %d's name: %s, Type: %d\n", files, current->d_name, current->d_type);
  }
  pthread_exit(NULL);
}


int main(int argc, char **argv)
{
  //check for valid number of arguments 
  if(argc!=2){
    puts("Invalid number of arguments");
    exit(0);
  }

  //opening the given directory read from argv[1]
    DIR *dirp;
    dirp = opendir(argv[1]);
    
    if(dirp == NULL)
    {
        perror("Not a directory");
        return(1);
    }

    //initializing pthread attributes
    pthread_attr_t attr;
    int err1 = pthread_attr_init(&attr);
    if(err1) {
      exit(EXIT_FAILURE);
    }

    //declared thread id
    pthread_t thread_id;
    int err = pthread_create(&thread_id, &attr, directory_handler, (void*)(dirp));
    if(err){
      exit(EXIT_FAILURE);
    }

    //joining the thread created with thread_id
    pthread_join(thread_id, NULL);

    //closing the open directory
    closedir(dirp);

    //colored printing on terminal
    printf("\033[0;31m");
    printf("This better be red\n");
    printf("\033[0m");
    return(0);
}

