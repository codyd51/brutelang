#include <iostream>
#include <cstdio>
#include <map>
#include <string>
#include <memory>
#include <vector>
#include <utility>
#include <stdlib.h>

using namespace std;

enum TokenType {
	tok_eof = -1,

	tok_def = -2,
	tok_return = -3,
	tok_if = -4,
	tok_while = -5,

	tok_identifier = -6,
	tok_number = -7
};

struct Token {
	TokenType type;
	int val;
	string rawStrVal;
	string strVal;
	double numVal;
};

static Token gettok() {
	Token token = Token();

	static int lastchar = ' ';

	//skip whitespace
	while (isspace(lastchar)) {
		lastchar = getchar();
	}

	//identifier
	if (isalpha(lastchar)) {
		token.strVal = lastchar;
		//build up identifier
		while (isalnum(lastchar = getchar())) {
			token.strVal += lastchar;
		}

		token.rawStrVal = token.strVal;

		//check for keywords
		if (token.strVal == "func") {
			token.type = tok_def;
			return token;
		}
		else if (token.strVal == "return") {
			token.type = tok_return;
			return token;
		}
		else if (token.strVal == "if") {
			token.type = tok_if;
			return token;
		}
		else if (token.strVal == "while") {
			token.type = tok_while;
			return token;
		}
		token.type = tok_identifier;
		return token;
	}

	//number
	else if (isdigit(lastchar) || lastchar == '.') {
		string numStr;
		//build up number
		do {
			numStr += lastchar;
			lastchar = getchar();
		} while (isdigit(lastchar) || lastchar == '.');

		token.rawStrVal = numStr;

		token.numVal = strtod(numStr.c_str(), 0);
		token.type = tok_number;
		return token;
	}

	//comments
	else if (lastchar == '#') {
		//comment until end of line
		do {
			lastchar = getchar();
		} while (lastchar != EOF && lastchar != '\n' && lastchar != '\r');
	}

	//check for end of file
	else if (lastchar == EOF) {
		token.type = tok_eof;
		return token;
	}

	//otherwise, just return the character as its ascii value
	int currchar = lastchar;
	lastchar = getchar();
	token.val = currchar;
	token.rawStrVal = string(1, currchar);
	return token;
}

static Token curTok;
static Token getToken() {
	Token next = gettok();
	//cout << "Got token: " << next.rawStrVal << endl;
	curTok = next;
	return curTok;
}

void expected(string ch) {
	cout << "Expected " << ch << ".";
	exit(0);
}

void match(string ch) {
	//cout << "Matching " << curTok.rawStrVal << " to " << ch << "\n";
	if (curTok.rawStrVal == ch) {
		getToken();
	}
	else {
		expected(ch);
	}
}











class Node {
public:
	//virtual void prettyPrint() = 0;
	virtual void prettyPrint(int tabCount) {
		cout << endl;
		for (int i = 0; i < tabCount; i++) {
			cout << "\t";
		}
	}
};

//identifier ::= 'A-Z'
class Identifier : public Node {
public:
	string name;

	Identifier(string name) : name(name) {}

	void prettyPrint(int tabCount) {
		Node::prettyPrint(tabCount);

		cout << "[IDENTIFIER " << name << "]";
	}
};

//number ::= '0-9'
class Number : public Node {
public:
	double value;

	Number(double value) : value(value) {}

	void prettyPrint(int tabCount) {
		//cout << "pretty printing number" << endl;
		Node::prettyPrint(tabCount);

		cout << "[NUMBER " << value << "]";
	}
};

//prototype ::= <identifier> '(' [<identifier>] ')'
class Prototype : public Node {
public:
	unique_ptr<Identifier> fnName;
	vector<unique_ptr<Identifier> > args;

	Prototype(unique_ptr<Identifier> name, vector<unique_ptr<Identifier> > args) : fnName(move(name)), args(move(args)) {}

	void prettyPrint(int tabCount) {
		Node::prettyPrint(tabCount);

		cout << "[PROTOTYPE ";
		Node::prettyPrint(tabCount+1);
		cout << "[NAME ";
		fnName->prettyPrint(tabCount+2);
		cout << "]";

		//cout << endl;

		for (auto const& identifier : args) {
			Node::prettyPrint(tabCount+1);
			cout << "[ARG ";
			identifier->prettyPrint(tabCount+2);
			//cout << "]" << endl;
		}
		cout << "]";
	}
};
//factor ::= <identifier> | <number> | <prototype>
class Factor : public Node {
public:
	unique_ptr<Node> node;

	Factor(unique_ptr<Identifier> identifier) : node(move(identifier)) {}
	Factor(unique_ptr<Number> number) : node(move(number)) {}
	Factor(unique_ptr<Prototype> prototype) : node(move(prototype)) {}

	//Factor(Node* node) : node(node) {
		//TODO check type of node passed to factor
		//expected("Identifier, number, or prototype");
	//}

	void prettyPrint(int tabCount) {
		Node::prettyPrint(tabCount);

		cout << "[FACTOR ";
		node->prettyPrint(tabCount+1);
		cout << "]";
	}
};

//termop ::= '*' | '/'
class TermOp : public Node {
public:
	string name;

	TermOp(string name) : name(name) {
		if (name != "*" && name != "/") {
			expected("* or /");
		}
	}

	void prettyPrint(int tabCount) {
		Node::prettyPrint(tabCount);

		cout << "[TERMOP " << name << "]";
	}
};

//term ::= <factor> [<termop> <term>]
class Term : public Node {
public:
	unique_ptr<Factor> lhs;
	unique_ptr<TermOp> op;
	unique_ptr<Term> rhs;

	Term(unique_ptr<Factor> lhs) : lhs(move(lhs)) {}
	Term(unique_ptr<Factor> lhs, unique_ptr<TermOp> op, unique_ptr<Term> rhs) : lhs(move(lhs)), op(move(op)), rhs(move(rhs)) {}

	void prettyPrint(int tabCount) {
		Node::prettyPrint(tabCount);

		cout << "[TERM ";
		lhs->prettyPrint(tabCount+1);
		if (op) {
			op->prettyPrint(tabCount+1);
			rhs->prettyPrint(tabCount+1);
		}
		cout << "]";
	}
};

//exprop ::= '+' | '-'
class ExprOp : public Node {
public:
	string name;

	ExprOp(string name) : name(name) {
		if (name != "+" && name != "-") {
			expected("+ or -");
		}
	}

	void prettyPrint(int tabCount) {
		Node::prettyPrint(tabCount);

		cout << "[EXPROP " << name << "]";
	}
};

//expression ::= <term> [<exprop> <expression>]
class Expression : public Node {
public:
	unique_ptr<Term> lhs;
	unique_ptr<ExprOp> op;
	unique_ptr<Expression> rhs;

	Expression(unique_ptr<Term> lhs) : lhs(move(lhs)) {}
	Expression(unique_ptr<Term> lhs, unique_ptr<ExprOp> op, unique_ptr<Expression> rhs) : lhs(move(lhs)), op(move(op)), rhs(move(rhs)) {}

	void prettyPrint(int tabCount) {
		Node::prettyPrint(tabCount);

		cout << "[EXPRESSION ";
		lhs->prettyPrint(tabCount+1);
		if (op) {
			op->prettyPrint(tabCount+1);
			rhs->prettyPrint(tabCount+1);
		}
		cout << "]";
	}
};

//assignment ::= <identifier> '=' <expression>
class Assignment : public Node {
public:
	unique_ptr<Identifier> lhs;
	unique_ptr<Expression> rhs;

	Assignment(unique_ptr<Identifier> lhs, unique_ptr<Expression> rhs) : lhs(move(lhs)), rhs(move(rhs)) {}

	void prettyPrint(int tabCount) {
		Node::prettyPrint(tabCount);

		cout << "[ASSIGNMENT ";
		lhs->prettyPrint(tabCount+1);

		Node::prettyPrint(tabCount+1);
		cout << "[ASSIGNOP =]";

		rhs->prettyPrint(tabCount+1);
		cout << "]";
	}
};

//condition ::= TODO flesh out
class Condition : public Node {
public:
	Condition() {}

	void prettyPrint(int tabCount) {
		Node::prettyPrint(tabCount);

		cout << "[CONDITION ]";
	}
};

//if ::= 'if' '(' <condition> ')' '{' [<statement>] '}'
class Statement;
class If : public Node {
public:
	unique_ptr<Condition> condition;
	vector<unique_ptr<Node> > statementList;

	If(unique_ptr<Condition> condition, vector<unique_ptr<Node> > statementList) : condition(move(condition)), statementList(move(statementList)) {}

	void prettyPrint(int tabCount) {
		Node::prettyPrint(tabCount);

		cout << "[IF ";
		condition->prettyPrint(tabCount+1);
		for (auto const& statement : statementList) {
			statement->prettyPrint(tabCount+1);
		}
		cout << "]";
	}
};

//while ::= 'while' '(' <condition> ')' '{' [<statement>] '}'
class While : public Node {
public:
	unique_ptr<Condition> condition;
	vector<unique_ptr<Node> > statementList;

	While(unique_ptr<Condition> condition, vector<unique_ptr<Node> > statementList) : condition(move(condition)), statementList(move(statementList)) {}

	void prettyPrint(int tabCount) {
		Node::prettyPrint(tabCount);

		cout << "[WHILE ";
		condition->prettyPrint(tabCount+1);
		for (auto const& statement : statementList) {
			statement->prettyPrint(tabCount+1);
		}
		cout << "]";
	}
};

//statement ::= <assignment> | <prototype> | <if> | <while> ';'
class Statement : public Node {
public:
	unique_ptr<Node> node;

	Statement(unique_ptr<Assignment> assignment) : node(move(assignment)) {}
	Statement(unique_ptr<Prototype> proto) : node(move(proto)) {}
	Statement(unique_ptr<If> ifStmt) : node(move(ifStmt)) {}
	Statement(unique_ptr<While> whileStmt) : node(move(whileStmt)) {}

	void prettyPrint(int tabCount) {
		Node::prettyPrint(tabCount);

		cout << "[STATEMENT ";
		node->prettyPrint(tabCount+1);
		cout << "]";
	}
};

//function ::= 'func' <prototype> '{' [<statement>] '}'
class Function : public Node {
public:
	unique_ptr<Prototype> proto;
	vector<unique_ptr<Statement> > statementList;

	Function(unique_ptr<Prototype> proto, vector<unique_ptr<Statement> > statementList) : proto(move(proto)), statementList(move(statementList)) {}

	void prettyPrint(int tabCount) {
		Node::prettyPrint(tabCount);

		cout << "[FUNCTION ";
		proto->prettyPrint(tabCount+1);
		for (auto const& statement : statementList) {
			statement->prettyPrint(tabCount+1);
		}
		cout << "]";
	}
};

static unique_ptr<Identifier> parseIdentifier() {
	string name = curTok.strVal;
	getToken();
	return unique_ptr<Identifier>(new Identifier(name));
}

static unique_ptr<Number> parseNumber() {
	double val = curTok.numVal;
	getToken();
	return unique_ptr<Number>(new Number(val));
}
//prototype ::= <identifier> '(' [<identifier>] ')'
static unique_ptr<Prototype> parsePrototype() {
	unique_ptr<Identifier> ident (parseIdentifier());

	match("(");

	vector<unique_ptr<Identifier> > args;
	while (curTok.type == tok_identifier) {
		args.push_back(move(parseIdentifier()));
	}

	match(")");

	return unique_ptr<Prototype>(new Prototype(move(ident), move(args)));
}

//factor ::= <identifier> | <number> | <prototype>
static unique_ptr<Factor> parseFactor() {
	if (curTok.type == tok_number) {
		return unique_ptr<Factor>(new Factor(unique_ptr<Number>(parseNumber())));
	}

	string name = curTok.rawStrVal;
	unique_ptr<Identifier> ident (new Identifier(name));

	getToken();

	if (curTok.rawStrVal == "(") {
		match("(");

		vector<unique_ptr<Identifier> > args;
		while (curTok.type == tok_identifier) {
			args.push_back(move(parseIdentifier()));
		}

		match(")");

		unique_ptr<Prototype> proto (new Prototype(move(ident), move(args)));

		return unique_ptr<Factor>(new Factor(move(proto)));
	}
	return unique_ptr<Factor>(new Factor(move(ident)));
}

static bool hasTermOp() {
	string op = curTok.rawStrVal;
	return (op == "*" || op == "/");
}

//termop ::= '*' | '/'
static unique_ptr<TermOp> parseTermOp() {
	string op = curTok.rawStrVal;
	if (!hasTermOp()) {
		expected("* or /");
	}
	getToken();
	return unique_ptr<TermOp>(new TermOp(op));
}

//term ::= <factor> [<termop> <term>]
static unique_ptr<Term> parseTerm() {
	unique_ptr<Factor> lhs = parseFactor();
	if (hasTermOp()) {
		unique_ptr<TermOp> op = parseTermOp();	
		unique_ptr<Term> rhs = parseTerm();
		return unique_ptr<Term>(new Term(move(lhs), move(op), move(rhs)));
	}
	return unique_ptr<Term>(new Term(move(lhs)));
}

static bool hasExprOp() {
	string op = curTok.rawStrVal;
	return (op == "+" || op == "-");
}

//exprop ::= '+' | '-'
static unique_ptr<ExprOp> parseExprOp() {
	string op = curTok.rawStrVal;
	if (!hasExprOp()) {
		expected("+ or -");
	}
	getToken();
	return unique_ptr<ExprOp>(new ExprOp(op));
}

//expression ::= <term> [<exprop> <expression>]
static unique_ptr<Expression> parseExpression() {
	unique_ptr<Term> lhs = parseTerm();
	if (hasExprOp()) {
		unique_ptr<ExprOp> op = parseExprOp();
		unique_ptr<Expression> rhs = parseExpression();
		return unique_ptr<Expression>(new Expression(move(lhs), move(op), move(rhs)));
	}
	return unique_ptr<Expression>(new Expression(move(lhs)));	
}

//assignment ::= <identifier> '=' <expression>
static unique_ptr<Assignment> parseAssignment() {
	unique_ptr<Identifier> lhs = parseIdentifier();
	match("=");
	unique_ptr<Expression> rhs = parseExpression();
	return unique_ptr<Assignment>(new Assignment(move(lhs), move(rhs)));
}

static unique_ptr<Condition> parseCondition() {
	getToken();
	return unique_ptr<Condition>(new Condition());
}

static unique_ptr<Statement> parseStatement();

//if ::= 'if' '(' <condition> ')' '{' [<statement>] '}'
static unique_ptr<If> parseIf() {
	match("if");
	match("(");
	unique_ptr<Condition> condition (parseCondition());
	match(")");
	match("{");

	vector<unique_ptr<Node> > statementList;
	while (curTok.rawStrVal != "}") {
		statementList.push_back(parseStatement());
	}

	match("}");

	return unique_ptr<If>(new If(move(condition), move(statementList)));
}

//while ::= 'while' '(' <condition> ')' '{' [<statement>] '}'
static unique_ptr<While> parseWhile() {
	match("while");
	match("(");
	unique_ptr<Condition> condition (parseCondition());
	match(")");
	match("{");

	vector<unique_ptr<Node> > statementList;
	while (curTok.rawStrVal != "}") {
		statementList.push_back(parseStatement());
	}

	match("}");

	return unique_ptr<While>(new While(move(condition), move(statementList)));
}

//statement ::= <assignment> | <prototype> | <if> | <while>
static unique_ptr<Statement> parseStatement() {
	//if
	if (curTok.type == tok_if) {
		return unique_ptr<Statement>(new Statement(parseIf()));
	}
	//while
	else if (curTok.type == tok_while) {
		return unique_ptr<Statement>(new Statement(parseWhile()));
	}

	unique_ptr<Identifier> ident (new Identifier(curTok.strVal));
	getToken();

	//assignment
	if (curTok.rawStrVal == "=") {
		match("=");

		unique_ptr<Expression> rhs (parseExpression());
		unique_ptr<Assignment> assignment (new Assignment(move(ident), move(rhs)));

		match(";");

		return unique_ptr<Statement>(new Statement(move(assignment)));
	}

	//prototype
	match("(");

	vector<unique_ptr<Identifier> > args;
	while (curTok.type == tok_identifier) {
		args.push_back(move(parseIdentifier()));
	}

	match(")");

	unique_ptr<Prototype> proto (new Prototype(move(ident), move(args)));

	match(";");

	return unique_ptr<Statement>(new Statement(move(proto)));		
}

//function ::= 'func' <prototype> '{' [<statement>] '}'
static unique_ptr<Function> parseFunction() {
	match("func");

	unique_ptr<Prototype> proto (parsePrototype());

	match("{");


	vector<unique_ptr<Statement> > statementList;
	while (curTok.rawStrVal != "}") {
		statementList.push_back(move(parseStatement()));
	}

	match("}");

	return unique_ptr<Function>(new Function(move(proto), move(statementList)));
}

int main() {
	cout << "ready> ";
	getToken();

	vector<Token> tokens;
	vector<unique_ptr<Node> > ast;

	do {
		tokens.push_back(curTok);

		if (curTok.type != tok_eof) {
			unique_ptr<Node> ident (parseFunction());

			ast.push_back(move(ident));
		}

		if (curTok.rawStrVal == "E") {
			break;
		}
	} while (curTok.rawStrVal != "E");

	for (auto tok : tokens) {
		//cout << "[Token " << tok.rawStrVal << "]" << endl;
	}

	for (auto const& node : ast) {
		node->prettyPrint(0);
		//cout << typeid(node).name() << endl;
	}

	return 0;
}














