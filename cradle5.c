#include <stdio.h>
#include <stdlib.h> 
#include <ctype.h>
#include <string.h>

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
void doWhile();
void doLoop();
void doRepeat();

//recognize and translate a statement block
void block() {
	while (look != 'e' && look != 'l' && look != 'u') {
		if (look == 'i') {
			doIf();
		}
		else if (look == 'w') {
			doWhile();
		}
		else if (look == 'p') {
			doLoop();
		}
		else if (look == 'r') {
			doRepeat();
		}
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
	const char* constLab1 = newLabel();
	char lab1[1024];
	strcpy(lab1, constLab1);
	
	//copy of lab1
	char* lab2 = malloc(sizeof(char) * strlen(lab1) + 1);
	strcpy(lab2, lab1);
	
	match('i');
	condition();
	
	char branch[1024];
	snprintf(branch, sizeof(branch), "BEQ %s", lab1);
	emitLn(branch);
	
	block();
	
	//handle 'else'
	if (look == 'l') {
		match('l');
		
		const char* tmp = newLabel();
		strcpy(lab2, tmp);
		
		char elseBranch[1024];
		snprintf(elseBranch, sizeof(elseBranch), "BRA %s", lab2);
		emitLn(elseBranch);
		
		postLabel(lab1);
		block();
	}
	
	match('e');
	postLabel(lab2);
}

//parse and translate a while statement 
void doWhile() {
	match('w');
	
	const char* constLab1 = newLabel();
	char lab1[1024];
	strcpy(lab1, constLab1);
	const char* constLab2 = newLabel();
	char lab2[1024];
	strcpy(lab2, constLab2);
	
	postLabel(lab1);
	condition();
	
	char beq[1024];
	snprintf(beq, sizeof(beq), "BEQ %s", lab2);
	emitLn(beq);
	
	block();
	
	match('e');
	
	char bra[1024];
	snprintf(bra, sizeof(bra), "BRA %s", lab1);
	emitLn(bra);
	
	postLabel(lab2);
}

//parse and translate a loop statement 
void doLoop() {
	match('p');
	
	const char* constLab = newLabel();
	char lab[1024];
	strcpy(lab, constLab);
	
	postLabel(lab);
	
	block();
	match('e');
	
	char bra[1024];
	snprintf(bra, sizeof(bra), "BRA %s", lab);
	emitLn(bra);
}

//parse and translate a repeat statement 
void doRepeat() {
	match('r');
	
	const char* constLab = newLabel();
	char lab[1024];
	strcpy(lab, constLab);
	
	postLab(lab);
	
	block();
	
	match('u');
	condition();
	
	char beq[1024];
	snprintf(beq, sizeof(beq), "BEQ %s", lab);
	emitLn(lab);
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