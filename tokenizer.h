#include <stdio.h>
#include <stdlib.h>
#include<pthread.h>
#include<string.h>
#include<sys/types.h>
#include<dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
//#include "tokenizer.h"

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



//#include "pc2.h"
//#include <pc2.c>

/*
struct Token{
char *token_string;
enum token_type token_type;
char *optional_c_operator_type;
struct Token *next;
int alreadyPrinted;
};
*/

enum token_type {Integer, Word, Hexadecimal, Octal, FloatingPoint, COperator, CKeyword, SingleQuote, DoubleQuote};

struct Token{
char *token_string;
enum token_type token_type;
char *optional_c_operator_type;
struct Token *next;
int alreadyPrinted;
};


char* getTokenTypeFromEnum(enum token_type tt);
int Delimiter_present(char c);
struct Token* addTokentoLinkedList(struct Token* token);
struct Token* createToken(char *token_string, enum token_type tt);
void freeLinkedList(struct Token* head);
int isWord(char *string);
int isHex(char* string);
int isPossibleHex(char *string);
int isOctal(char *string);
int isFloat(char *string);
int isPossibleFloat(char *string);
int isInt(char *string);
char* isC_Operator(char *string);
int isCKeyword(char* string);
char* createSubstring(char* string, int beginIndex, int endIndex);
struct Token* add_viable_tokens_to_linkedlist(char* currentstring);
char* printToken(struct Token* token, struct Tnode* tokenList);
int getTokenInQuotes(char* string, int begin, char quote);
int skipComments(char* string, int c1, int c2, char slash, char slash_or_star);
int Invalid_Token(char c, int substring_index, char* input);
void tokenize(char* string, struct Tnode *tokenList);
void addToken(char* newStr, struct Tnode* Tlist);
