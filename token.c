/*#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
*/
#include "tokenizer.h"
/*struct Tnode{  // token node for linked list of tokens                                                                                                                                          
char * token;
double prob;
  int occur;
struct Tnode * next_token;
};

struct Tnode* head;
int Delimiter_present(char c);
char* createSubstring(char* string, int beginIndex, int endIndex);
char* isWord(char* string);
void printLinkedList(struct Tnode* tokenList);
void addToTokens(struct Tnode* nw, struct Tnode* tokenList);
void tokenize(char* string, struct Tnode* tokenList);
*/

//struct Tnode* head; // my edit

int Delimiter_present(char c){
  
  //if((c==' ') || (c=='\n') || (c=='\t') || (c=='\v') || (c=='\r') || (c=='\f')){
  if((c==' ') || (c=='\n') || (c=='-')){
    return 1;
  }
  return 0;
}

char* createSubstring(char* string, int beginIndex, int endIndex){

int substring_size = endIndex - beginIndex + 1;	
char *result = malloc(sizeof(char)*(substring_size)+1); 

int j = 0;
for(int i = beginIndex; i<=endIndex; i++){
result[j] = string[i];
j++;	
}

//printf("Substring: %s\n", result);
return result;
}

  char* isWord(char* string){
    int nonWords=0, i = 0, j = 0;
    
    while(string[i]!='\0'){
      if(!isalpha(string[i])){
	  nonWords++;
	}
      i++;
	}
      i = 0;
      int len = (strlen(string))-nonWords;
      char* word = malloc(sizeof(char)*(len+1));

      while(string[i]!='\0'){
	if(isalpha(string[i])){
          word[j] = string[i];
	  j++;
	  }
	  i++;
      }
      word[len] = '\0';
	return word;
   }
  
struct Tnode* tokenize(char* string, struct Tnode* tokenList){
  //  struct Token* head = NULL;
  //  tokenList = head;
  //  struct Tnode* head;
  //head = tokenList;
  //  if(head==NULL) puts("head is null");
  if(tokenList==NULL) puts ("tokenList is null");
  tokenList = malloc(sizeof(struct Tnode));
  tokenList->token="";
  tokenList->next_token=NULL;
  tokenList->occur = 0;
  tokenList->prob = 0.0;
  int i = 0, j = 0, hyphens = 0;
  char * substring;
     char * words;
     //     head = NULL;
     char* hyphenToken = (char*)(malloc(sizeof(char)*2));

     hyphenToken[0] = '-';
     hyphenToken[1] = '\0';
     //counting hyphens and will skip them later
     while(string[i]!='\0'){
       if(string[i]=='-'){
	 hyphens++;
       }
       i++;
     }
     if(hyphens>0){
     struct Tnode* newHyphen = malloc(sizeof(struct Tnode));
        newHyphen->token = hyphenToken;
        newHyphen->next_token = NULL;
        newHyphen->occur = hyphens;
	newHyphen->prob = 0.0;
        addToTokens(newHyphen,tokenList);
     }
     
     i=0;
     while(string[i]!='\0'){

       if(Delimiter_present(string[i])){
	//make a token of previous values
	if(i!=0){
	substring = createSubstring(string, j, i-1);
	words = isWord(substring);
	if(strcmp(words,"")!=0){
	struct Tnode* newT = malloc(sizeof(struct Tnode));
	newT->token = words;
	newT->next_token = NULL;
	newT->occur=1;
	newT->prob=0.0;
	//	printf("going to add %s\n", newT->token);
	addToTokens(newT,tokenList);
	//	puts("back from add, going next iter");
	}
	}
	j=i+1;
	i=i+1;
	continue;
       }
      i++; 
     }

    //last token
        substring = createSubstring(string, j, i-1);
        words = isWord(substring);
	if(strcmp(words,"")!=0){
        struct Tnode* newTok = malloc(sizeof(struct Tnode));
	newTok->token = words;
        newTok->next_token = NULL;
	newTok->occur=1;
        newTok->prob=0.0;
	//printf("going tto add %s\n", newTok->token);
        addToTokens(newTok,tokenList);
	}
    
    free(substring);
    printf("printing tokens\n");
    printLinkedList(tokenList);
    return tokenList;
}

void addToTokens(struct Tnode* nw, struct Tnode* tokenList){
    //    struct Token* head;
    //head = *top;
  // puts("adding tokens");
    //    if(nw!=NULL) puts ("nw is not null");
    // if(nw->token==NULL) puts("don't like newStr"); 
  //int total=hyp_occur;
  char* newStr = nw->token;
  //if(newStr==NULL) puts("don't like newStr");
  for(int i = 0; i < strlen(newStr); i++){
    newStr[i] = tolower(newStr[i]);
  }
  //  printf("new string %s\n", newStr);
  struct Tnode* prev;
  struct Tnode* curr;

  if(strcmp(tokenList->token, "")==0){
      //head = nw;
      tokenList->token = nw->token;
      tokenList->occur = nw->occur;
      tokenList->next_token = nw->next_token;
      tokenList->prob = nw->prob;
			//      total= total+head->occur;
      //  if(head==NULL) puts("head still null");
      //printf("first token %s\n", head->token);
    }
    else{
      curr = tokenList; // Charles edits below did the following edits below
			
			if(strcmp(curr->token,nw->token)==0){ 
				curr->occur++;
				return;
			} 

     	while(curr->next_token!=NULL){
					if(strcmp(curr->next_token->token, nw->token)==0){
					  curr->next_token->occur++;
	 							return;
					}
					
					if(strcmp(curr->next_token->token, nw->token)>0){
								break;
							}
		   		
					curr = curr->next_token;
			 } 
					nw->next_token = curr->next_token; 
					curr->next_token = nw;
					
      
			//printf("added %s\n", newStr);
      //puts("nw's next is NULL");
      //printf("Last token %s\n", (prev->next_token)->token);
    }
    //    printf("printing\n");
    //printLinkedList(tokenList);
}

void printLinkedList(struct Tnode* tokenList){
  int total = 0;
 // tokenList = head;
  struct Tnode* prev = NULL;
      struct Tnode* curr = tokenList;
  if(tokenList==NULL){
    puts("empty list");
  }
while(curr!=NULL){
  total+=curr->occur;
        prev=curr;
        curr = curr->next_token;
	//        printf("curr token %s, occur %d\n", prev->token, prev->occur);
      }
 printf("total tokens %d\n", total);
 probability(tokenList, total);
}

void probability(struct Tnode* tokenList, int total){
  double oc = 0.0;
  if(tokenList==NULL){
    puts("no tokens in this file");
  }
  struct Tnode* curr = tokenList;
  while(curr!=NULL){
  oc = (double) curr->occur;
  curr->prob = (oc/total);
  printf("curr token %s, occur %d, prob %.3f\n", curr->token, curr->occur, curr->prob);
  curr = curr->next_token;
  }
  puts("End of prob list for this file");
}
/*
  int main(){
    //    head = malloc(sizeof(struct Token));
    //    head->token_string=NULL;
    // head->next = NULL;
    tokenize(" -happy can't\ngo  -- yum lamb happy happy LAmbi JB  lambi lamb-yummy---yum-jb", NULL);
    //printLinkedList();
    return 0;
  }
*/
