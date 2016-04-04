#include <stdio.h>
#include <stdlib.h>

typedef enum { false, true } bool;

//global lookahead character
char look;

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
	snprintf(buffer, sizeof(buffer), "%s Expected.", string);
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

//initialize
void init() {
	getChar();
}

//main program 
int main() {
	init();
}