#include <stdio.h>
#include <stdlib.h> 
#include <ctype.h>
#import <string.h>

typedef enum { false, true } bool;

//global lookahead character
char look;
int lCount;

//read character from input stream
void getChar() {
	look = getchar();
}

//report error
void error(const char* message) {
	printf("\nError: %s.\n", message);
}

//report error and halt
void abrt(const char* message) {
	error(message);
	exit(1);
}

//report what was expected 
void expected(const char* string) {
	char buffer[1024];
	snprintf(buffer, sizeof(buffer), "%s Expected", string);
	abrt(buffer);
}

//match specific input character 
void match(char ch) {
	if (look == ch) {
		getChar();
	}
	else {
		char buffer[1024];
		snprintf(buffer, sizeof(buffer), "\"%c\"", ch);
		expected(buffer);
	}
}

//recognize alpha character 
bool isAlpha(char ch) {
	return isupper(toupper(ch));
}

//recognize decimal digit
bool isDigit(char ch) {
	return isdigit(ch);
}

//recognize an addop
bool isAddop(char ch) {
	return (ch == '+' || ch == '-');
}

//get an identifier
char getName() {
	if (!isAlpha(look)) {
		expected("Name");
	}
	char ret = toupper(look);
	getChar();
	return ret;
} 

//get a number
char getNum() {
	if (!isDigit(look)) {
		expected("Integer");
	}
	char ret = look;
	getChar();
	return ret;
}

//output string with a tab
void emit(const char* string) {
	printf("\t%s", string);
}

//output string with tab and newline
void emitLn(const char* string) {
	char buffer[1024];
	snprintf(buffer, sizeof(buffer), "%s\n", string);
	emit(buffer);
}

//recognize and translate an 'other'
void other() {
	//char c = getName();
	char buffer[1024];
	snprintf(buffer, sizeof(buffer), "%c", getName());
	emitLn(buffer);
}

//generate a unique label
const char* newLabel() {
	char string[1024];
	snprintf(string, sizeof(string), "L%i", lCount);
	lCount++;

	const char* lbl = string;
	return lbl;
}

//post label to output
void postLabel(const char* label) {
	printf("%s:\n", label);
}

void doIf();

//recognize and translate a statement block
void block() {
	while (look != 'e') {
		if (look == 'i') {
			doIf();
		}
		//else if (look == 'o') {
		else {
			other();
		}
	}
}

//parse and translate a boolean condition
//this version is a dummy
void condition() {
	emitLn("<condition>");
}

//recognize and translate an if construct
void doIf() {
	const char* string = newLabel();
	char bak[1024];
	strcpy(bak, string);

	printf("BEF string is %s bak is %s", string, bak);

	match('i');
	condition();

	char branch[1024];
	snprintf(branch, sizeof(branch), "BEQ %s", string);
	emitLn(branch);

	block();
	match('e');
	postLabel(bak);
}

//parse and translate a program
void program() {
	block();
	if (look != 'e') {
		expected("End");
	}
	emitLn("END");
}

//initialize
void init() {
	getChar();
	lCount = 0;
}

//main program 
int main() {
	init();
	program();
}