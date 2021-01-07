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

enum class KeyWords {
	INVALID,
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

struct Token {
	TokenTypes type;
	std::string str;
	int value =  -1;
	KeyWords keyword = KeyWords::INVALID;

	Token(TokenTypes _type, std::string _str): type(_type), str(_str)
	{}

	Token(TokenTypes _type, std::string _str, int _value) : type(_type), str(_str), value(_value)
	{}

	Token(TokenTypes _type, std::string _str, KeyWords _keyword) : type(_type), str(_str), keyword(_keyword)
	{}
};

class JackTokenizer {

	int line_number = 0;
	std::ifstream input_stream;

	//TokenTypes currenct_token_type = TokenTypes::INVALID;
	std::string parsed_str;
	Token* token = nullptr;
	//int parsed_value = 0;

public:
	JackTokenizer(const char* fileName);

	bool hasMoreTokens();

	Token* advance();

	//Token* tokenType();

	//KeyWords keyWord();

	int lineNumber();

	//char symbol();

	//std::string indentifier();

	//int intVal();

	//std::string stringVal();
};