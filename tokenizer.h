#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
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
void printToken(struct Token* token);
int getTokenInQuotes(char* string, int begin, char quote);
int skipComments(char* string, int c1, int c2, char slash, char slash_or_star);
int Invalid_Token(char c, int substring_index, char* input);
void tokenize(char* string, struct Tnode *tokenList);

