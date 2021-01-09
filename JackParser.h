#pragma once

#include <fstream>
#include <stack>
#include <string>


#include "JackTokenizer.h"
#include "JackSymbolTableManager.h"

class JackParser {

	JackTokenizer* tokenizer = nullptr;
	JackSymbolTabelManager* symbol_tbl_mgr;
	
	std::ofstream* out_file;
	std::stack<Token*> tokens_stack;
	Token* current_token = nullptr;
	int label_count = 0;

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