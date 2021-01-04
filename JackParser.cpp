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
			&& (tokenizer->keyWord() != KeyWords::STATIC && tokenizer->keyWord() != KeyWords::FIELD))
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
			&& ( tokenizer->keyWord() != KeyWords::CONSTRUCTOR && tokenizer->keyWord() != KeyWords::FUNCTION && tokenizer->keyWord() != KeyWords::METHOD ) )
		{
			break;
		}
		compileSubroutineDec();
		getNextToken(tokenizer);
	}


	getNextToken(tokenizer);
	if (tokenizer->tokenType() != TokenTypes::SYMBOL && tokenizer->symbol() != '}')
	{
		std::printf("Unexpected class definition. } expected \n");
	}

}

void JackParser::compileClassVarDec()
{
	getNextToken(tokenizer);
	if (tokenizer->tokenType() != TokenTypes::KEYWORD
		&& ((tokenizer->keyWord() != KeyWords::BOOLEAN) && (tokenizer->keyWord() != KeyWords::INT) && (tokenizer->keyWord() != KeyWords::CHAR))
		&& tokenizer->tokenType() != TokenTypes::IDENTIFIER)
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
		if (tokenizer->tokenType() != TokenTypes::SYMBOL && tokenizer->symbol() != ',')
		{
			break;
		}
	}

	// Match ';' at the end of var definitions
	if (tokenizer->tokenType() != TokenTypes::SYMBOL && tokenizer->symbol() != ';')
	{
		std::printf("Unexpected var declaration. Should end with ; \n");
		return;
	}
}


void JackParser::compileSubroutineDec()
{
}


void JackParser::compileParameterList()
{
}

void JackParser::compileSubroutineBody()
{
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


