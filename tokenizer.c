
/*
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <pc2.c>
*/
#include "tokenizer.h"
/*
struct Token{
char *token_string;
enum token_type token_type;
char *optional_c_operator_type;
struct Token *next;
int alreadyPrinted;
};

enum token_type {Integer, Word, Hexadecimal, Octal, FloatingPoint, COperator, CKeyword, SingleQuote, DoubleQuote};
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

*/


/*Function takes a token_type enum
 *and returns a pointer to the string that it represents
 */
//enum token_type {Integer, Word, Hexadecimal, Octal, FloatingPoint, COperator, CKeyword, SingleQuote, DoubleQuote};
char* getTokenTypeFromEnum(enum token_type tt){

char *tokentype = malloc(sizeof(char)*30);

switch(tt){
	case 0: tokentype = "decimal integer"; break;
	case 1: tokentype = "word"; break;
	case 2: tokentype = "hexadecimal integer"; break;
	case 3: tokentype = "octal integer"; break;
	case 4: tokentype = "floating point"; break;
	case 5: tokentype = "C operator"; break;
	case 6: tokentype = "C keyword"; break;
	case 7: tokentype = "single quote"; break;
	case 8: tokentype = "double quote"; break;
	default: return tokentype;
} 
return tokentype;
}


/*
struct Token{
char *token_string;
enum token_type token_type;
char *optional_c_operator_type;
struct Token *next;
int alreadyPrinted;
};
*/

struct Token* head = NULL;

//Function to check if delimiter is present,
//return 1 for true and 0 for false
int Delimiter_present(char c){
  
  //if((c==' ') || (c=='\n') || (c=='\t') || (c=='\v') || (c=='\r') || (c=='\f')){
if((c==' ') || (c=='\n')){
    return 1;
  }
  return 0;
}

//Function takes a pointer to a struct token,
//adds this token to the front of the linked list, 
//and returns a pointer to the head
struct Token* addTokentoLinkedList(struct Token* token){	
if (head!=NULL){	
head->next=token;
}
head = token;
return head;
}


//Function takes a token string and token type
//and return a pointer to a newly malloc'ed struct token 
struct Token* createToken(char *token_string, enum token_type tt){
struct Token *token = malloc(sizeof(struct Token));
token->token_string = token_string;
token->token_type = tt;
 token->alreadyPrinted=0;
return token;
}


/**Function frees entire linked list
 *given a pointer to the head
 *
*/
void freeLinkedList(struct Token* head){

struct Token* current;
current = head;

while((current->next)!=NULL){
current = current->next;
free(head);
head = current;
}
free(head);
	
}



/*This function, given a string
checks if its a word that is,
it starts with an alphabet 
and is allowed to have alphanumeric characters
following the first alphabet
return 1 for true, 0 for false */
int isWord(char *string){
  int k=1,j=1,i = 1;
  /*    if(string[0]=='-'){
    while(string[j]!='\0'){
      if(string[j]!='-'){
	//not hyphen word
	k=0;
	break;
      }
      j++;
    }
    if(k==1){
      return 1;
    }
  }
  */
  //checks if first character is an alphabet
  if(isalpha(string[0])==0){
  return 0;
  }

  
  while(string[i]!='\0'){
    /*    if(string[(strlen(string))-2]=='\'') {
      i++;
      continue;
      }
    //checks if the following character are alphanumeric
    //if a non-alphanumeric character is found then loop terminates to end the token there
    */if(isalnum(string[i])==0){
      //printf("end of word");
      return 0;
    }
    i++;
  }
  return 1;
}

/*This function, given a string
check if the string as it stands a hexadecimal integer
Returns 1 for true, 0 for false*/
int isHex(char* string){
 
if(strlen(string)<=2){
return 0;
}

//if first character is not zero then return false 
  if((string[0]-'0')!= 0){
    return 0;
  }
  int c = string[1];
  //if second character is not 'x' then return false                                                                                                                                     
  if(c!=88 && c!=120){
    return 0;
  }
  //check if following characters are alphanumeric                                                                                                                                       
  int i = 2;
  while(string[i]!='\0'){
     //if its not an alphabet from a-f & A-F then check its a digit, if not then return false                                                                                        
    if(isxdigit(string[i])==0){
      // printf("end of hex\n");
      return 0;
    }
  
    i++;
  }
  return 1;
}


/*This function, given a string
check if it could possibly be a hex
returns 1 for true and 0 for false*/
int isPossibleHex(char *string){
  //if first character is not zero then return false                                                                                                                                     
  if((string[0]-'0')!= 0){
    return 0;
  }
  int c = string[1];
  //if second character is not 'x' then return false                                                                                                                                     
  if(c!=88 && c!=120){
    return 0;
  }
  //check if following characters are alphanumeric                                                                                                                                       
  int i = 2;
  while(string[i]!='\0'){
     //if its not an alphabet from a-f & A-F then check its a digit, if not then return false                                                                                
    if(isxdigit(string[i])==0){
      //      printf("end of hex\n");
      return 0;
    }
  
    i++;
  }
  return 1;
}

/*Given a string, this function determines
if its an Octal integer 
It returns 1 for true and 0 for false*/
int isOctal(char *string){
  int i = 1;

  if((string[0]-'0')!= 0){
    return 0;
  }

while (string[i]!='\0'){
//converts char to int                                                      
  int c = string[i]-'0';
//reached a character not between 0 and 7
  if (c<0 || c>7){
   return 0;
  }
  i++;
 }
 return 1;
}

/*Given a string,
 * Function will return true (1) or false(0)
 *if the string is currently, as it stands, a float 
 *ie 436.1 or 117.2
 */
int isFloat(char *string){

int decimal_count=0;
int i = 0;
int decimalPosition;
int e = 0;	
	while (string[i]!='\0'){
		
		char c = string[i]-'0';
	     
		//If character has a decimal, must only have one                                                                                       
                //And must not be at first or last index                                                                                               
                if ((string[i]=='.')&&(i!=strlen(string)-1) &&(i!=0)){
                decimal_count++;
		decimalPosition=i;
                }
		
		//if character is not an integer or . return false
		if((string[i]!='.') && ((c<0) || c>9)){

		  //if character is not a decimal and not an integer
		  //then it is allowed to be e or E and also allowed to be + or - if occurring right after e/E
		  if (e==0 && decimalPosition!=(i-1) && decimal_count==1 && (string[i]==69 || string[i]==101)){
                  e=i;
		  i++;
		  continue;
		  }
		  else if(e==(i-1) && decimal_count==1 && (string[i]==45 || string[i]==43)){
                  i++;
                  continue;
		  }

		  //not an e, E, decimal, integer then return 0
		  return 0;
		}
        
	i++;
	}

if (decimal_count!=1){
return 0;
}
return 1;
}


/**Function is given a string
 * and returns a boolean if the string could possibly
 * be a float on the next iteration
 *ie 436. or 117.
 */
int isPossibleFloat(char *string){
int i = 0;
int t = 0;
 int s = 0;
 int e=0;
 int negative=0;

 //just added this line for compilation - compiler says negative is initialized but never used
 //and i dont see when you do anything with it other than set it to 0 or 1
 if (negative){

}

 // int k = (int)strlen(string);
 for(int j = 0;  j < strlen(string); j++){
   if(string[j]=='.' && j!=0){
     s=1;
   }
 }
 if(s==0){
   return 0;
 }
while (string[i]!='\0'){
  //converts char to int
  int c = string[i]-'0';
  //reached a non int                                                                                                                   
  if (c<0 || c>9){
    if(i>0 && string[i]==46 && t==0){
      t=1;
      
    }
    else if( e==0 && t==1 && (string[i]==69 || string[i]==101)){
      e=i;
      // printf("found e");
      //      continue;
    }
    else if(e==(i-1) && t==1 && (string[i]==45 || string[i]==43)){
      negative=1;
      // printf("found e negative/positive");
    }
    else {
      return 0;
    }
  }
  i++;
}

  return 1;
}


//Function iterates through string to determine              
//whether it is a decimal integer
//Returns 0 for false or 1 for true                                                                                                                                                      
int isInt(char *string){
int i = 0;
 if(isOctal(string)){
   return 1;
 }
while (string[i]!='\0'){
//converts char to int                                                                                                                                                                   
int c = string[i]-'0';
//reached a non int                                                                                                                                                                      
if (c<0 || c>9){
  return 0;
}
i++;
}

return 1;
}

/**This function takes a char array, 
 * and returns a pointer to a newly malloced char array
 *this char array will contain an empty string, if the input is not a C operator
 or it will return a pointer to a string which describes the type of c operator it is
 */

char* isC_Operator(char *string){

char *c_op = malloc(sizeof(char)*50);	

//struct C_Operator *c_operator_struct = malloc(sizeof(struct C_Operator));
//c_operator_struct->operator_length=0;
//c_operator_struct->operator_name="";

c_op = "";
/*
int i = 0;	
//char c = string[0];

//printf("ignore: %s\n", c_op);
//printf("%c\n", c);

//if the string is length one, choose from possible operators
if (strlen(string)==1){

//c_operator_struct->operator_length=1;
switch(string[i]) {
  	case '(':
		//puts("found left parenthesis");
		c_op = "left parenthesis";	
		return c_op;
		//c_operator_struct->operator_name = "left parenthesis";
		//return c_operator_struct;
        case ')':
		c_op = "right parenthesis";
		return c_op;
        case '[': 
		c_op = "left bracket";
		return c_op;
	case ']':
		c_op = "right bracket";
		return c_op;
	case '.':
		c_op = "structure member";
		return c_op;
	case ',':
		c_op = "comma";
		return c_op;
	case '!':
		c_op = "negate";
		return c_op;
	case '~':
		c_op = "1s complement";
		return c_op;
	case '^':
		c_op = "bitwise XOR";
		return c_op;
	case '|':
		c_op = "bitwise OR";
		return c_op;
	case '+':
		c_op = "addition";
		return c_op;
	case '/':
		c_op = "division";
		return c_op;
	case '?':
		c_op = "conditional true";
		return c_op;
	case ':':   		
		c_op = "conditional false";
		return c_op;
	case '<':
		c_op = "less than test";
		return c_op;
	case '>':
		c_op = "greater than test";
		return c_op;
	case '=':
		c_op = "assignment";
		return c_op;
	case '&':
		c_op = "AND/address operator";
		return c_op;
  	case '-':
		c_op = "hyphen";
		return c_op;
			case '*':
		c_op = "multiply/dereference operator";
		return c_op;
        case '%':
                c_op="mod";
                return c_op;
        default:
		//c_operator_struct->operator_length=0;
		return c_op;
		//return c_operator_struct;
	}


}

if (strlen(string)==2){

	
//c_operator_struct->operator_length=2;

char c1 = string[0];
char c2 = string[1];


if ((c1 =='-') && (c2 == '>')){	
//c_operator_struct->operator_name = "structure pointer";
//return c_operator_struct;
c_op = "structure pointer";
return c_op;
}

if ((c1 =='>') && (c2 == '>')){
c_op = "shift right";
return c_op;
}

if ((c1 =='<') && (c2 == '<')){
c_op = "shift left";
return c_op;
}
if ((c1 =='+') && (c2 == '+')){
c_op = "increment";
return c_op;
}
if ((c1 =='-') && (c2 == '-')){
c_op = "hyphens";
return c_op;
}

if ((c1 =='|') && (c2 == '|')){
c_op = "logical OR";
return c_op;
}
if ((c1 =='&') && (c2 == '&')){
c_op = "logical AND";
return c_op;
}
if ((c1 =='=') && (c2 == '=')){
c_op = "equality test";
return c_op;
}
if ((c1 =='!') && (c2 == '=')){
c_op = "inequality test";
return c_op;
}
if ((c1 =='<') && (c2 == '=')){
c_op = "less than or equal test";
return c_op;
}
if ((c1 =='>') && (c2 == '=')){
c_op = "greater than or equal test";
return c_op;
}
if ((c1 =='+') && (c2 == '=')){
c_op = "plus equals";
return c_op;
}
if ((c1 =='-') && (c2 == '=')){
c_op = "minus equals";
return c_op;
}
if ((c1 =='*') && (c2 == '=')){
c_op = "times equals";
return c_op;
}
if ((c1 =='/') && (c2 == '=')){
c_op = "divide equals";
return c_op;
}
if ((c1 =='%') && (c2 == '=')){
c_op = "mod equals";
return c_op;
}
if ((c1 =='&') && (c2 == '=')){	
c_op = "bitwise AND equals";
return c_op;
}
if ((c1 =='^') && (c2 == '=')){
c_op = "bitwise XOR equals";
return c_op;
}
if ((c1 =='|') && (c2 == '=')){
c_op = "bitwise OR equals";
return c_op;
}
//c_operator_struct->operator_length=0;
return c_op;
}


//Strings length 3
if (strlen(string)==3){

//c_operator_struct->operator_length=3;
	
char c1 = string[0];
char c2 = string[1];
char c3 = string[2];
	
if ((c1 =='-') && (c2 == '-') && (c3=='-')) {
c_op = "hyphens";
return c_op;
}

if ((c1 =='<') && (c2 == '<')&& (c3=='=')){
c_op = "shift left equals";
return c_op;
}

}

if(strlen(string)==(6)){

if(strcmp(string, "sizeof")==0){
c_op = "sizeof";
return c_op;	
}

}
  //c_operator_struct->operator_length=0;
  //return c_operator_struct;
*/	return c_op;
}

int isCKeyword(char* string){

int string_length = strlen(string);
	
if(string_length==2){
if((strcmp(string, "if")==0) || (strcmp(string, "do")==0)){
return 1;
}

return 0;
}	

if(string_length==3){
if((strcmp(string, "int")==0) || (strcmp(string, "for")==0)){
return 1;
}
return 0;
}

if(string_length==4){

if((strcmp(string, "auto")==0) || (strcmp(string, "long")==0) || (strcmp(string, "void")==0) || (strcmp(string, "else")==0)
|| (strcmp(string, "case")==0) || (strcmp(string, "enum")==0) || (strcmp(string, "goto")==0) || (strcmp(string, "char")==0)){
return 1;
}
return 0;
}

if(string_length==5){

if((strcmp(string, "const")==0) || (strcmp(string, "short")==0) || (strcmp(string, "float")==0)
|| (strcmp(string, "break")==0) || (strcmp(string, "union")==0) || (strcmp(string, "while")==0)){
return 1;
}
return 0;	
}

if(string_length==6){

if((strcmp(string, "double")==0) || (strcmp(string, "signed")==0) || (strcmp(string, "switch")==0)
|| (strcmp(string, "return")==0) || (strcmp(string, "static")==0) || (strcmp(string, "extern")==0)
|| (strcmp(string, "struct")==0)){
return 1;
}
return 0;
}

if(string_length==7){

if((strcmp(string, "default")==0) || (strcmp(string, "typedef")==0)){
return 1;
}
return 0;
}

if(string_length==8){

if((strcmp(string, "unsigned")==0) || (strcmp(string, "continue")==0) || (strcmp(string, "register")==0)
|| (strcmp(string, "volatile")==0)
){
return 1;
}
return 0;
}

return 0;
}

/**This function returns a pointer to a newly malloc'ed char array
 *Given a string, and the beginning and ending indexes you want a substring of
 */
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


/**This function takes a string and checks to see what tokens it qualifies for, if any
 *Will add these tokens to a linked list, from the head
 */
struct Token* add_viable_tokens_to_linkedlist(char* currentstring){

//Determine what type of token the substring is
//and create a struct token (linked list node) for it, and add to front of linked list
//If multiple types are possible, head will point to the node that was last added
//And that is the only one we need
//Current precedence is order of if statements - word,int,float,hex,octal,c operator
if (isWord(currentstring)){
//puts("found word");
struct Token *word_token = createToken(currentstring, Word);
head = addTokentoLinkedList(word_token);
 }

if(isCKeyword(currentstring)){
//puts("found C Keyword");
struct Token *c_keyword_token = createToken(currentstring, CKeyword);
head = addTokentoLinkedList(c_keyword_token);
}

if(isInt(currentstring)){
//puts("found int");
struct Token *int_token = createToken(currentstring, Integer);
head = addTokentoLinkedList(int_token);
}
if(isFloat(currentstring)){
//puts("found float");
struct Token *float_token = createToken(currentstring, FloatingPoint);
head = addTokentoLinkedList(float_token);
}
if(isHex(currentstring)){
//puts("found hex");
struct Token *hex_token = createToken(currentstring, Hexadecimal);
head = addTokentoLinkedList(hex_token);
}
if (isOctal(currentstring)){
//puts("found octal");
struct Token *octal_token = createToken(currentstring, Octal);
head = addTokentoLinkedList(octal_token);
}

//pointer that points to the string returned by isC_Operator
char *str = isC_Operator(currentstring);

//If the string returned is not empty, we have found a string operator
if(strlen(str)!=0){
//printf("found C Operator: %s\n", str);
//coperator = 1;

struct Token *c_operator_token = createToken(currentstring, COperator);
//c_operator_token->optional_c_operator_type=c_operator_struct->operator_name;
c_operator_token->optional_c_operator_type=str;
head = addTokentoLinkedList(c_operator_token);
}

return head;
}


/*This function will print the string and type
 *of a token, given a pointer to it
 *for null token, will return without printing
 */
char* printToken(struct Token* token){
if(token==NULL){
return NULL;
}
//prin
/*ting a c operator token type
if ((token->optional_c_operator_type)!=NULL){
printf("%s: ", token->optional_c_operator_type);
printf("\"%s\"\n", token->token_string);
}*/

if (strcmp((getTokenTypeFromEnum(token->token_type)) , "word")==0){
//print any other token type	
printf("%s: ", getTokenTypeFromEnum(token->token_type));
printf("\"%s\"\n", token->token_string);
 return token->token_string;
}
 return NULL;
//printing token string
//printf("\"%s\"\n", token->token_string);
//printf("token length, i: %i\n", i);
}


/*This function will return an integer which 
  gives the index of the first character 
  after a double or single quote pair
  and tokenizes the string that was
  in between the quotes pair*/
int getTokenInQuotes(char* string, int begin, char quote){
  int i=begin+1;
  int end;
  char* token;
  
  while((string[i]!=quote)){
    i++;
  }
  end=i;

  token = createSubstring(string,begin,end);
  //tokenizing string in between quotations depending on type of quotation
  if(quote=='\''){
  struct Token *single_quote_token = createToken(token,SingleQuote);
  head = addTokentoLinkedList(single_quote_token);
  }
  if(quote=='\"'){
  struct Token *double_quote_token = createToken(token,DoubleQuote);
  head = addTokentoLinkedList(double_quote_token);
  }
  return (end+1);
}

/*This function will skip C comments and 
  returns the index of the first character
  after the end of the comment*/
int skipComments(char* string, int c1, int c2, char slash, char slash_or_star){
  int i = c2+1;
  int end;
  //type of comment /*comment*/
  if(slash_or_star == '*'){
    while(string[i]!='*' && string[i+1]!='/'){
      i++;
    }
    end=i+2;
  }
  //type of comment //comment with
  //delimiter as end of string or new line character
  else if (slash_or_star=='/'){
    while(string[i]!='\0'){
      if(string[i]=='\n'){
	return i+1;
      }
      i++;
    }
    end=-1;
    }
  return end;
}


/*This function takes a char, the input string and current index to check if
  that character is a valid token input, if not return 0, else returns 1*/ 
int Invalid_Token(char c, int substring_index, char* input){
  char* currentItem;
  currentItem = createSubstring(input, substring_index, substring_index);
  if(strlen(currentItem)!=1 || currentItem[0]!=c){
    // puts("not working");
  }
  char* str = isC_Operator(currentItem);
  if(isWord(currentItem) || isCKeyword(currentItem) || isInt(currentItem) || isFloat(currentItem) || isHex(currentItem) || isOctal(currentItem) || strlen(str)!=0 ){
    //if(isWord(currentItem) || strlen(str)>=0 ){
  //    puts("found type");
    return 1;
  }
  //  puts("found invalid token");
  return 0;
}


void addToken(char* newStr, struct Tnode *Tlist){

  struct Tnode *newTnode = (struct Tnode) malloc(sizeof(struct Tnode));
  newTnode->token = newStr;
  newTnode->occur = 1;
  newTnode->next_token = NULL;

  struct Tnode *prev = NULL, curr = Tlist;

  while(curr!=NULL){
    prev = curr;
    curr = curr->next_token;
  }
    
  
}


//int main(int argc, char** argv){
void tokenize(char* string, struct Tnode *tokenList){
  /*if (argc!=2){
  puts("Invalid number of arguments");
  return 1;
  }*/

//////TO TEST DIFFERENT INPUT STRING, COMMENT OUT THE FOLLOWING 3 LINES
//Copies string from argv[1] into inputstring
int size_input_string = strlen(string);
char *inputString = malloc(sizeof(char)*(size_input_string)+1);
strncpy(inputString, string, size_input_string+1);
/////

////////TO TEST DIFFERENT INPUTSTRINGS, UNCOMMENT THIS SINGLE LINE WITH YOUR INPUT
// char inputString[] = ' here "1 is not 2" nor ".0812 either" ';
///////

 if(strlen(inputString)==0){
   puts("Empty string input, no tokens");
   return;
 }

//i iterates through input string char by char
//beginIndex is the beginning of the current substring we are examing 
//beginIndex will advance to beginIndex = i when a token is found, to examine new substring
int i = 0;
int beginSubstringIndex = 0;
char* currentstring;
//iterate through input string char by char until the null terminator
//currentstring = createSubstring(inputString, beginSubstringIndex, i);
 int quote_present = 0;
 char * newToken = (char*) malloc(sizeof(char));
while(inputString[i]!='\0'){
  //struct Token* oldhead = head;
  //check for comments and function call to skip them
  /*
  if((inputString[i]=='/' && inputString[i+1]=='*') || (inputString[i]=='/' && inputString[i+1]=='/')){
    if(head!=NULL) {
      if(head->alreadyPrinted==0){
        printToken(head);
        head->alreadyPrinted=1;
      }
    }
    
    beginSubstringIndex = skipComments(inputString, i, i+1, inputString[i], inputString[i+1]);
    i = beginSubstringIndex;
    if(i==-1) {
    return ;
    }
    else{
      continue;
    }
    }                       
  
  if((inputString[i]=='\"' || inputString[i]=='\'')){ 
    if(quote_present%2==0){
    //must print previous token before updating
      //      puts("printing oldhead before quotes token");


      if(head!=NULL){
	if(head->alreadyPrinted==0){
	printToken(head);
	head->alreadyPrinted=1;
      }
      }
    beginSubstringIndex = getTokenInQuotes(inputString, i, inputString[i]);
    i = beginSubstringIndex;

    //print quote token
    printToken(head);
    head->alreadyPrinted=1;
    // puts("quotes detected");

    //need to skip over rest of code, where it checks oldhead etc
     //    i++;
    quote_present++;
    //printf("value of quotes: %d\n", quote_present);
    continue;
    }
    else{
      quote_present++;
    }
  }
  */
 currentstring = createSubstring(inputString, beginSubstringIndex, i);
 
  if(Delimiter_present(inputString[i])==1){
    //  puts("delimiter");
    /*if(head!=NULL){
        if(head->alreadyPrinted==0){
        printToken(head);
        head->alreadyPrinted=1;
      }
     }*/
  i++;
  beginSubstringIndex++;
  continue;
  }

  //checks if a character is a non-token and skips it
  if(Invalid_Token(inputString[i], i,inputString)==0){
      if(head!=NULL){
        if(head->alreadyPrinted==0){
        newToken = printToken(head);
	addToken(newToken, tokenList);
        head->alreadyPrinted=1;
      }
     }
      // printf("non token index at %d\n", i);
       beginSubstringIndex=i+1;
    i+=1;
    //printf("restart at %d\n", i);
    
     
    continue;
    }

//save old head pointer
struct Token* oldhead = head;

//Searches for any possible tokens for current substring
//and adds these tokens to a linked list and updates head
head = add_viable_tokens_to_linkedlist(currentstring);

//reached the end of the string
//if(argv[1][i+1]=='\0'){	  
//puts("reached the end of the string, tokenizing rest of the token");
//printTokenFromLinkedListHead();
//return 0;
//}

//The head returned is the same as before
//therefore no new tokens have been found in this iteration
//One of two things may happen
//1 - We have reached a substring that is not currently a token, but may be if given one more iteration (ie 123.)
//2 - The largest token is the previously found token

if (oldhead==head){
  //puts("reached untokenizeable token");

//Case 1 Check - Next token may qualify

//Check for case 1 - float
if (isPossibleFloat(currentstring) && (inputString[i+1]!='\0')){
  //puts("can possibly be float - checking next index..");

char *possible_float = malloc(sizeof(char)*(i-beginSubstringIndex+2));
possible_float = createSubstring(inputString, beginSubstringIndex, i+1);

if (isFloat(possible_float)){
  //puts("next token will be float, continue iterations as normal");
i++;
free(possible_float);
continue;
}
else{
  //puts("next value will not be a float");
}
free(possible_float);
}

//Check for case 1 - hex
if (isPossibleHex(currentstring) && (inputString[i+1]!='\0')){
  //puts("can possibly be hex - checking next index..");

char *possible_hex = malloc(sizeof(char)*(i-beginSubstringIndex+2));
possible_hex = createSubstring(inputString, beginSubstringIndex, i+1);

if (isHex(possible_hex)){
  //puts("next token will be hex, continue iterations as normal");
i++;
free(possible_hex);
continue;
}
else{
  //puts("next token will not be hex");
}
free(possible_hex);
}

//Case 2 - Tokenize previous token
//puts("Reached a non token, tokenizing previous token");


 if(head!=NULL){
 if(head->alreadyPrinted==0){
 newToken = printToken(head);
 addToken(newToken, tokenList);
 head->alreadyPrinted=1;
 }
 }
 
//resets begin substring pointer to first char after the token
beginSubstringIndex=i;
continue;
}

i++;
}


// puts("printing last head");

 if(head!=NULL){
   if(head->alreadyPrinted==0){
   newToken =  printToken(head);
   addToken(newToken, tokenList);
   head->alreadyPrinted=1;
   }
 }

//free(inputString);
free(currentstring);
freeLinkedList(head);	

}
