#include<stdlib.h>
#include<stdio.h>
#include<pthread.h>


void * p_t( void *  nothing );


struct thread_arg{ // this argument stuct is passed to pthread_create so our routine has access to the right arguments 
pthread_mutex_t * lock;
int * input;
};

void * p_t(void * nothing){ // takes in struct

int i =1;

struct thread_arg * arg = (struct thread_arg *)(nothing); 

int result = pthread_mutex_lock(arg->lock);
printf("did lock work %d \n",result);
for(i;i<=10;i++){ 
	//printf("Thread number %d count %d\n", *((int *)(nothing)),i);
	 printf("Thread number %d count %d\n", *(arg->input),i);
}

pthread_mutex_unlock(arg->lock);
free(nothing);
pthread_exit(0);

} 



int main(int argc, char * argv[]){ 


pthread_attr_t  threadAttr; // we can use this one struct to create all our threads
pthread_attr_init(&threadAttr); 

pthread_mutex_t stableThread;

pthread_mutex_init(&stableThread,NULL);

pthread_t arr[5];
int i =0; 
int values[] = {1,2,3,4,5};
int * input;
	for(i;i<5;i++){
		//input = &values[i];
		struct thread_arg * arg = (struct thread_arg * )malloc(sizeof(struct thread_arg)); 
		arg->lock = &stableThread; 
		arg->input = &values[i];
		//pthread_create(&arr[i], &threadAttr, p_t, (void *)( input) );	
			int output = pthread_create(&arr[i], &threadAttr, p_t, (void *)(arg) );
			if(output!=0){ 
				break;
			}
			
			} 

	pthread_attr_destroy(&threadAttr);
	int j =0; 
	for(j;j<5;j++){ 
		pthread_join(arr[j],NULL);
	} 
	/* 
	if we destory mutex before joining the threads we will run the chance of having a lock failure 
	becuase the mutex lock will be destoryed while the thread is running. So when that thread finishes and another
	tries to lock the poitner to mutex lock will just be to null
	*/
	pthread_mutex_destroy(&stableThread);// destory sync mechanism after all threads haved been joined


return 1;
}
