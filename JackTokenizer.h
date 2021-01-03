#pragma once

#include <fstream>
#include <iostream>
#include <string>

enum TokenTypes {
	INVALID,
	KEYWORD,
	SYMBOL,
	IDENTIFIER,
	INT_CONST,
	STRING_CONST,
	EOFILE
};

enum KeyWords {
	CLASS,
	METHOD,
	FUNCTION,
	CONSTRUCTOR,
	INT,
	BOOLEAN,
	CHAR,
	VOID,
	VAR,
	STATIC,
	FIELD,
	LET,
	DO,
	IF,
	ELSE,
	WHILE,
	RETURN,
	TRUE,
	FALSE,
	NULLX,
	THIS
};

class JackTokenizer {

	int line_number = 0;
	std::ifstream input_stream;

	TokenTypes currenct_token_type = TokenTypes::INVALID;
	std::string parsed_str;
	int parsed_value = 0;

public:
	JackTokenizer(const char* fileName);

	bool hasMoreTokens();

	void advance();

	TokenTypes tokenType();

	KeyWords keyWord();

	char symbol();

	std::string indentifier();

	int intVal();

	std::string stringVal();
};