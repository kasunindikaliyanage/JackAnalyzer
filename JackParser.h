#pragma once

#include <fstream>
#include <stack>
#include <map>
#include <string_view>
#include <string>


#include "JackTokenizer.h"



struct Entry
{
	std::string name;
	std::string type;
	std::string kind;
	int index;
	
	const void print() const
	{
		std::cout << name << " " << type << " " << kind << " " << index << std::endl;
	}
};


class JackParser {

	std::map<std::string, Entry> class_symbol_table;
	std::map<std::string, Entry> subroutine_symbol_table;

	std::string current_class_name;
	
	//TODO it seems there is no need to keep global variables for below 4.
	int class_static_var_index = 0;
	int class_field_var_index = 0;
	
	int subroutine_arg_var_index = 0;
	int subroutine_local_var_index = 0;
 	
	JackTokenizer* tokenizer = nullptr;
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