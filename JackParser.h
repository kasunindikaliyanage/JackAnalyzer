#pragma once

#include <fstream>
#include <stack>

#include "JackTokenizer.h"

class JackParser {
	
	JackTokenizer* tokenizer = nullptr;
	std::ofstream* out_file;
	std::stack<Token*> tokens_stack;
	Token* current_token = nullptr;

	Token* getNextToken();
	
public:
	JackParser( const char* fileName);

	~JackParser();

	void compileClass();

	void compileClassVarDec();

	void compileSubroutineDec();

	void compileParameterList();

	void compileSubroutineBody();

	void compileVarDec();

	void compileStatements();

	void compileLet();

	void compileIf();

	void compileWhile();

	void compileDo();

	void compileReturn();

	void compileExpressionList();

	void compileExpression();

	void compileTerm();
};