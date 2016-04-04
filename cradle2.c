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

void expression();

//parse and translate an identifier 
void ident() {
	char name = getName();
	char buffer[1024];
	if (look == '(') {
		match('(');
		match(')');
		snprintf(buffer, sizeof(buffer), "BL %c", name);
	}
	else {
		snprintf(buffer, sizeof(buffer), "MOV %c, R0", name);
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
		//as chars are ascii, converting them to ints makes them shifted 48 to the right
		//so, subtract the '0' char to account for the shift 
		int intVal = (int)getNum() - '0';
		snprintf(buffer, sizeof(buffer), "MOV #%i, R0", intVal);
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
	char name = getName();
	match('=');
	expression();
	
	char buffer[1024];
	snprintf(buffer, sizeof(buffer), "LEA %c, R0", name);
	emitLn(buffer);
}

//initialize
void init() {
	getChar();
}

//main program 
int main() {
	init();
	assignment();
	/*
	if (look != '\r') {
		expected("Newline");
	}
	*/
}