#pragma once


class JackParser {

public:
	JackParser( char* fileName);

	void compileClass();

	void compilerClassVarDec();

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