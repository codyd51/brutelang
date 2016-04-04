#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum { false, true } bool;

//helper to concat strings 
char* stradd(const char* a, const char* b) {
	size_t len = strlen(a) + strlen(b);
	char* ret = (char*)malloc(len * sizeof(char) + 1);
	*ret = '\0';
	return strcat(strcat(ret, a), b);
}

//global lookahead character
char look;

//recognize white space
bool isWhite(char ch) {
	return (ch == ' ' || ch == '\t');
}

//recognize alpha character 
bool isAlpha(char ch) {
	return isupper(toupper(ch));
}

//recognize decimal digit
bool isDigit(char ch) {
	return isdigit(ch);
}

//recognize an alphanumeric
bool isAlNum(char ch) {
	return (isAlpha(ch) || isDigit(ch));
}

//recognize an addop
bool isAddop(char ch) {
	return (ch == '+' || ch == '-');
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
	snprintf(buffer, sizeof(buffer), "%s Expected", string);
	abrt(buffer);
}

//skip leading white space
void skipWhite() {
	while (isWhite(look)) {
		getChar();
	}
}

//match specific input character 
void match(char ch) {
	if (look != ch) {
		char buffer[1024];
		snprintf(buffer, sizeof(buffer), "\"%c\"", ch);
		expected(buffer);
	}
	else {
		getChar();
		skipWhite();
	}
}

//get an identifier
const char* getName() {
	const char* token = "";
	if (!isAlpha(look)) {
		expected("Name");
	}
	while (isAlNum(look)) {
		token = stradd(token, &look);
		
		getChar();
	}
	skipWhite();
	return token;
} 

//get a number
const char* getNum() {
	const char* value = "";
	
	if (!isDigit(look)) {
		expected("Integer");
	}
	while (isDigit(look)) {
		value = stradd(value, &look);
		
		getChar();
	}
	skipWhite();
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

void expression();

//parse and translate an identifier 
void ident() {
	const char* name = getName();
	char buffer[1024];
	if (look == '(') {
		match('(');
		match(')');
		snprintf(buffer, sizeof(buffer), "BL %s", name);
	}
	else {
		snprintf(buffer, sizeof(buffer), "MOV %s, R0", name);
	}
	emitLn(buffer);
}

//parse and translate a math factor
void factor() {
	if (look == '(') {
		match('(');
		expression();
		match(')');
	}
	else if (isAlpha(look)) {
		ident();
	}
	else {
		char buffer[1024];
		snprintf(buffer, sizeof(buffer), "MOV #%s, R0", getNum());
		emitLn(buffer);
	}
}

//recognize and translate a multiply
void multiply() {
	match('*');
	factor();
	emitLn("POP {R3}");
	emitLn("MULS R3, R0");
}

//parse and translate a divide 
void divide() {
	match('/');
	factor();
	emitLn("POP {R3}");
	emitLn("MOV R3, R0");
}

//parse and translate a term 
void term() {
	factor();
	while (look == '*' || look == '/') {
		emitLn("PUSH {R0}");
		if (look == '*') {
			multiply();
		}
		else if (look == '/') {
			divide();
		}
	}
}

//recognize and translate an add 
void add() {
	match('+');
	term();
	emitLn("POP {R3}");
	emitLn("ADD R0, R3");
}

//recognize and translate a subtract 
void subtract() {
	match('-');
	term();
	emitLn("POP {R3}");
	emitLn("SUB R0, R3");
	emitLn("NEG R0");
}

//parse and translate an expression 
void expression() {
	if (isAddop(look)) {
		emitLn("CLR R0");
	}
	else {
		term();
	}
	while (isAddop(look)) {
		emitLn("PUSH {R0}");
		if (look == '+') {
			add();
		}
		else if (look == '-') {
			subtract();
		}
	}
}

//parse and translate an assignment statement
void assignment() {
	const char* name = getName();
	
	match('=');
	expression();
	
	char buffer[1024];
	snprintf(buffer, sizeof(buffer), "LEA %s, R0", name);
	emitLn(buffer);
}

//initialize
void init() {
	getChar();
	skipWhite();
}

//main program 
int main() {
	init();
	assignment();
	if (look != '\r') {
		expected("Newline");
	}
}