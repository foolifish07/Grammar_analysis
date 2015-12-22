#include <iostream>
#include <string>
#include <string.h>
#include <cstdio>
#include <map>
#include <vector>

using namespace std;
const int maxn = 105;

int digit[maxn] , latinm;
char latin[maxn];
char op[] = {
	'[',
	']',
	'#',
	'(',
	')',
	',',
	'-',
	'*',
	'/',
	'%',
	'!',
	'=',
	'<',
	'>' 
};
int kn = 11;
string keyword[] = {
	"break" , 
	"continue" ,
	"return" , 
	"char" , 
	"int" , 
	"double" , 
	"if" , 
	"else" , 
	"for" , 
	"while" , 
	"do" 
};

void fordigit(string nt){
	for(int i=0;i<10;i++)
		printf("\t| %d %s\n" , i , nt.c_str() );
}
void forlatin(string nt){
	for(int i=0;i<26;i++){
		printf("\t| %d %s\n" , i+'a' , nt.c_str() );
		printf("\t| %d %s\n" , i+'A' , nt.c_str() );
	}
}
void putend(){
	printf("\t;\n");
}

int main(){

	for(int i=0;i<=9;i++) digit[i] = i;
	for(char i='a';i<='z';i++) latin[i-'a'] = i;
	for(char i='A';i<='Z';i++) latin[i-'A'+'z'] = i;

	printf("idstring\n");
	printf("\t: _ idstring\n");
	fordigit("idstring"); forlatin("idstring");
	putend();

	printf("identifier\n");
	for(int i=0;i<26;i++){
		printf("\t| %d idstring\n" , i+'a');
		printf("\t| %d idstring\n" , i+'A');
	}
	
/*

digit :  0  | 1  | 2  | 3  | 4  | 5  | 6  | 7  | 8  | 9   ;
latin :  a  | b  | c  | d  | e  | f  | g  | h  | i  | j  | k  | l  | m  | n  | o  | p  | q  | r  | s  | t  | u  | v  | w  | x  | y  | z  | A  | B  | C  | D  | E  | F  | G  | H  | I  | J  | K  | L  | M  | N  | O  | P  | Q  | R  | S  | T  | U  | V  | W  | X  | Y  | Z   ;

operator
	:	[
	| 	]
	| 	#
	| 	(
	| 	)
	| 	, 	
 	|	+
	| 	-
	| 	*
	| 	/
	| 	%
	| 	!
	| 	=
	| 	<
	| 	>
	;

limiter	
	: 	{
 	|	}
	| 	;
	;

idstring 
	: digit idstring
	| latin idstring
	| _ idstring
	;
identifier
	: latin string
	;

number
	: digit number
	: digit
	;

keyword
	:	break
	| 	continue
	| 	return
	|	char
	| 	int
	| 	double
	| 	if
	| 	else
	| 	for
	| 	while
	| 	do
	;

*/
	
	return 0;
}