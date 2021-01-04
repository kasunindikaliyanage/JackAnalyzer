#pragma once

#include "JackTokenizer.h"

class JackParser {
	
	JackTokenizer* tokenizer = nullptr;

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

	void compileType();
};