#include "JackParser.h"

//TODO add const char for all the characters

JackParser::JackParser(const char* fileName)
{
	tokenizer = new JackTokenizer(fileName);
}

JackParser::~JackParser()
{
	delete tokenizer;
}

static void getNextToken( JackTokenizer* _tokenizer)
{
	if (_tokenizer->hasMoreTokens())
	{
		_tokenizer->advance();
		return;
	}

	std::cout << "No more tokens \n";
	EXIT_FAILURE;
}


void JackParser::compileClass()
{
	getNextToken(tokenizer);
	if (tokenizer->tokenType() != TokenTypes::KEYWORD && tokenizer->keyWord() != KeyWords::CLASS)
	{
		std::printf("Unexpected class definition. class keyword expected \n");
	}

	getNextToken(tokenizer);
	if (tokenizer->tokenType() != TokenTypes::IDENTIFIER)
	{
		std::printf("Unexpected class definition. class name expected \n");
	}

	getNextToken(tokenizer);
	if (tokenizer->tokenType() != TokenTypes::SYMBOL && tokenizer->symbol() != '{')
	{
		std::printf("Unexpected class definition. { expected \n");
	}

	getNextToken(tokenizer);

	while (true)
	{
		if (tokenizer->tokenType() != TokenTypes::KEYWORD
			|| (tokenizer->keyWord() != KeyWords::STATIC && tokenizer->keyWord() != KeyWords::FIELD))
		{
			break;
		}

		//line start with "static" or "field"
		compileClassVarDec();
		getNextToken(tokenizer);
	}

	while (true)
	{
		if ( tokenizer->tokenType() != TokenTypes::KEYWORD
			|| ( tokenizer->keyWord() != KeyWords::CONSTRUCTOR && tokenizer->keyWord() != KeyWords::FUNCTION && tokenizer->keyWord() != KeyWords::METHOD ) )
		{
			break;
		}
		compileSubroutineDec();
		getNextToken(tokenizer);
	}

	if (tokenizer->tokenType() != TokenTypes::SYMBOL || tokenizer->symbol() != '}')
	{
		std::printf("Unexpected class definition. } expected \n");
		return;
	}

	std::printf("Compilation Success \n");
}


void JackParser::compileClassVarDec()
{
	getNextToken(tokenizer);
	if ( !((tokenizer->tokenType() == TokenTypes::KEYWORD 
				&& ((tokenizer->keyWord() == KeyWords::BOOLEAN) || (tokenizer->keyWord() == KeyWords::INT) || (tokenizer->keyWord() == KeyWords::CHAR)))
			||(tokenizer->tokenType() == TokenTypes::IDENTIFIER)))
	{
		std::printf("Unexpected var declaration. Type expected \n");
		return;
	}

	getNextToken(tokenizer);
	while (true)
	{
		if (tokenizer->tokenType() != TokenTypes::IDENTIFIER)
		{
			std::printf("Unexpected var declaration. Identifier expected \n");
			return;
		}

		getNextToken(tokenizer);
		if (tokenizer->tokenType() != TokenTypes::SYMBOL || tokenizer->symbol() != ',')
		{
			break;
		}
	}

	// Match ';' at the end of var definitions
	if (tokenizer->tokenType() != TokenTypes::SYMBOL || tokenizer->symbol() != ';')
	{
		std::printf("Unexpected var declaration. Should end with ; \n");
		return;
	}
}


void JackParser::compileSubroutineDec()
{
	// function return type check
	getNextToken(tokenizer);
	if ( !(((tokenizer->tokenType() == TokenTypes::KEYWORD) && (tokenizer->keyWord() == KeyWords::VOID))
		    ||(tokenizer->tokenType() == TokenTypes::IDENTIFIER)) )
	{
		std::printf("Unexpected function declaration. Type expected \n");
		return;
	}

	//function name check
	getNextToken(tokenizer);
	if (tokenizer->tokenType() != TokenTypes::IDENTIFIER)
	{
		std::printf("Unexpected function declaration. Function name expected \n");
		return;
	}

	compileParameterList();

	compileSubroutineBody();
}


void JackParser::compileParameterList()
{
	//function parameter list start
	getNextToken(tokenizer);
	if (tokenizer->tokenType() != TokenTypes::SYMBOL && tokenizer->symbol() != '(')
	{
		std::printf("Unexpected function declaration. '(' expected \n");
		return;
	}

	//function parameter list check
	getNextToken(tokenizer);
	while (true)
	{
		if (tokenizer->tokenType() != TokenTypes::KEYWORD
			&& ((tokenizer->keyWord() != KeyWords::BOOLEAN) && (tokenizer->keyWord() != KeyWords::INT) && (tokenizer->keyWord() != KeyWords::CHAR))
			&& tokenizer->tokenType() != TokenTypes::IDENTIFIER)
		{
			break;
		}

		getNextToken(tokenizer);
		if (tokenizer->tokenType() != TokenTypes::IDENTIFIER)
		{
			std::printf("Unexpected parameter declaration. Identifier expected \n");
			return;
		}

		getNextToken(tokenizer);
		if (tokenizer->tokenType() != TokenTypes::SYMBOL || tokenizer->symbol() != ',')
		{
			break;
		}
	}

	//function parameter list start
	if (tokenizer->tokenType() != TokenTypes::SYMBOL || tokenizer->symbol() != ')')
	{
		std::printf("Unexpected function declaration. ')' expected \n");
		return;
	}
}


void JackParser::compileSubroutineBody()
{
	//function body start
	getNextToken(tokenizer);
	if (tokenizer->tokenType() != TokenTypes::SYMBOL && tokenizer->symbol() != '{')
	{
		std::printf("Unexpected function body open. '{' expected \n");
		return;
	}

	// TODO function body parsing

	//function body end
	getNextToken(tokenizer);
	if (tokenizer->tokenType() != TokenTypes::SYMBOL && tokenizer->symbol() != '}')
	{
		std::printf("Unexpected function body close. '}' expected \n");
		return;
	}
}

void JackParser::compileVarDec()
{
}

void JackParser::compileStatements()
{
}

void JackParser::compileLet()
{
}

void JackParser::compileIf()
{
}

void JackParser::compileWhile()
{
}

void JackParser::compileDo()
{
}

void JackParser::compileReturn()
{
}

void JackParser::compileExpressionList()
{
}

void JackParser::compileExpression()
{
}

void JackParser::compileTerm()
{
}

void JackParser::compileType()
{
	//getNextToken(tokenizer);
	//if (tokenizer->tokenType() != TokenTypes::KEYWORD
	//	&& ((tokenizer->keyWord() != KeyWords::BOOLEAN) && (tokenizer->keyWord() != KeyWords::INT) && (tokenizer->keyWord() != KeyWords::CHAR))
	//	&& tokenizer->tokenType() != TokenTypes::IDENTIFIER )
	//{

	//}
}


