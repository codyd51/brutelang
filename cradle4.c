#include <stdio.h>
#include <stdlib.h>

typedef enum { false, true } bool;

//global lookahead character
char look;
char keys[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
int values[];

void initTable() {
	char i;
	for (int i = 0; i < strlen(keys); i++) {
		values[i] = 0;
	}
}

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

//recognize and skip over a newline
void newLine() {
	if (look == '\r') {
		getChar();
		if (look == '\n') {
			getChar();
		}
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
int getNum() {
	int value = 0;
	if (!isDigit(look)) {
		expected("Integer");
	}
	while (isDigit(look)) {
		value = 10 * value + ((int)(look - '0'));
		getChar();
	}
	return value;
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

//parse and translate a math factor 
int factor() {
	int ret;
	if (look == '(') {
		match('(');
		ret = expression();
		match(')');
	}
	else if (isAlpha(look)) {
		ret = values[getName()];
	}
	else {
		ret = getNum();
	}
	return ret;
}

//parse and translate a math term
int term() {
	int value = factor();
	while (look == '*' || look == '/') {
		if (look == '*') {
			match('*');
			value = value * factor();
		}
		if (look == '/') {
			match('/');
			value = value / factor();
		}
	}
	return value;
}

//parse and translate an expression
int expression() {
	int value;
	if (isAddop(look)) {
		value = 0;
	}
	else {
		value = term();
	}
	while (isAddop(look)) {
		if (look == '+') {
			match('+');
			value = value + term();
		}
		else if (look == '-') {
			match('-');
			value = value - term();
		}
	}
	return value;
}

//parse and translate an assignment statement
void assignment() {
	char name = getName();
	match('=');
	values[name] = expression();
}

//initialize
void init() {
	getChar();
}

//main program 
int main() {
	init();
	while (look != ';') {
		assignment();
		newLine();
	}
}