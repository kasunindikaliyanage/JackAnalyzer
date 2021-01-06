#include "JackParser.h"

//TODO add const char for all the characters

static void getNextToken(JackTokenizer* _tokenizer)
{
	if (_tokenizer->hasMoreTokens())
	{
		_tokenizer->advance();
		return;
	}

	std::cout << "No more tokens \n";
	EXIT_FAILURE;
}


static std::string remove_extension(std::string file_name_with_extension)
{
	size_t lastindex = file_name_with_extension.find_last_of(".");
	std::string rawname = file_name_with_extension.substr(0, lastindex);
	return rawname;
}

JackParser::JackParser(const char* fileName)
{
	tokenizer = new JackTokenizer(fileName);

	std::string out_name = remove_extension(fileName);
	out_name.append("_out.xml");
	out_file = new std::ofstream (out_name,std::ios::out);
}

JackParser::~JackParser()
{
	delete tokenizer;
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

	//*out_file << "<class>" << std::endl;

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
	}

	while (true)
	{
		if ( tokenizer->tokenType() != TokenTypes::KEYWORD
			|| ( tokenizer->keyWord() != KeyWords::CONSTRUCTOR && tokenizer->keyWord() != KeyWords::FUNCTION && tokenizer->keyWord() != KeyWords::METHOD ) )
		{
			break;
		}
		compileSubroutineDec();
		//getNextToken(tokenizer);
	}

	if (tokenizer->tokenType() != TokenTypes::SYMBOL || tokenizer->symbol() != '}')
	{
		std::printf("Unexpected class definition. } expected \n");
		return;
	}

	//*out_file << "</class>";

	std::printf("Compilation Success \n");
}


void JackParser::compileClassVarDec()
{
	getNextToken(tokenizer);
	if ( !((tokenizer->tokenType() == TokenTypes::KEYWORD 
				&& ((tokenizer->keyWord() == KeyWords::BOOLEAN) || (tokenizer->keyWord() == KeyWords::INT) || (tokenizer->keyWord() == KeyWords::CHAR)))
			||(tokenizer->tokenType() == TokenTypes::IDENTIFIER)))
	{
		std::printf("Unexpected class var declaration. Type expected \n");
		return;
	}

	getNextToken(tokenizer);
	while (true)
	{
		if (tokenizer->tokenType() != TokenTypes::IDENTIFIER)
		{
			std::printf("Unexpected class var declaration. Identifier expected \n");
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
		std::printf("Unexpected class var declaration. Should end with ; \n");
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

	//var declaration
	getNextToken(tokenizer);

	while (true)
	{
		if (tokenizer->tokenType() != TokenTypes::KEYWORD
			|| (tokenizer->keyWord() != KeyWords::VAR))
		{
			break;
		}

		//line start with "static" or "field"
		compileVarDec();
		getNextToken(tokenizer);
	}

	// statements
	compileStatements();


	//function body end
	//getNextToken(tokenizer);
	if (tokenizer->tokenType() != TokenTypes::SYMBOL && tokenizer->symbol() != '}')
	{
		std::printf("Unexpected function body close. '}' expected \n");
		return;
	}
}


void JackParser::compileVarDec()
{
	//TODO this code is similar to the code for the class variable parsing. Try to remove it
	getNextToken(tokenizer);
	if (!((tokenizer->tokenType() == TokenTypes::KEYWORD
		&& ((tokenizer->keyWord() == KeyWords::BOOLEAN) || (tokenizer->keyWord() == KeyWords::INT) || (tokenizer->keyWord() == KeyWords::CHAR)))
		|| (tokenizer->tokenType() == TokenTypes::IDENTIFIER)))
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
		getNextToken(tokenizer);
	}

	// Match ';' at the end of var definitions
	if (tokenizer->tokenType() != TokenTypes::SYMBOL || tokenizer->symbol() != ';')
	{
		std::printf("Unexpected var declaration. Should end with ; \n");
		return;
	}
}


void JackParser::compileStatements()
{
	while (true)
	{
		if (tokenizer->tokenType() == TokenTypes::KEYWORD)
		{
			if (tokenizer->keyWord() == KeyWords::WHILE)
			{
				compileWhile();
			}
			else if (tokenizer->keyWord() == KeyWords::IF)
			{
				compileIf();
			}
			else if (tokenizer->keyWord() == KeyWords::LET)
			{
				compileLet();
			}
			else if (tokenizer->keyWord() == KeyWords::RETURN)
			{
				compileReturn();

				if (tokenizer->tokenType() != TokenTypes::SYMBOL || tokenizer->symbol() != ';')
				{
					std::printf("Unexpected ending for var declaration. ';' expected \n");
					return;
				}
			}
			else if (tokenizer->keyWord() == KeyWords::DO)
			{
				compileDo();

				if (tokenizer->tokenType() != TokenTypes::SYMBOL || tokenizer->symbol() != ';')
				{
					std::printf("Unexpected ending for var declaration. ';' expected \n");
					return;
				}
			}
			else
			{
				break;
			}

			getNextToken(tokenizer);
		}
		else
		{
			break;
		}
	}


}


void JackParser::compileLet()
{
	getNextToken(tokenizer);
	if (tokenizer->tokenType() != TokenTypes::IDENTIFIER)
	{
		std::printf("Unexpected 'let' syntax. var name expected \n");
		return;
	}

	getNextToken(tokenizer);
	if (tokenizer->tokenType() == TokenTypes::SYMBOL && tokenizer->symbol() == '[')
	{
		compileExpression();

		if (tokenizer->tokenType() != TokenTypes::SYMBOL && tokenizer->symbol() != ']')
		{
			std::printf("Unexpected var array declaration. ']' expected \n");
			return;
		}
		getNextToken(tokenizer);
	}

	if (tokenizer->tokenType() != TokenTypes::SYMBOL && tokenizer->symbol() != '=')
	{
		std::printf("Unexpected assignment to var. '=' expected \n");
		return;
	}

	compileExpression();

	if (tokenizer->tokenType() != TokenTypes::SYMBOL || tokenizer->symbol() != ';')
	{
		std::printf("Unexpected ending for var declaration. ';' expected \n");
		return;
	}

	//getNextToken(tokenizer);
}


void JackParser::compileIf()
{
	getNextToken(tokenizer);
	if (tokenizer->tokenType() != TokenTypes::SYMBOL && tokenizer->symbol() != '(')
	{
		std::printf("Unexpected opening to while. '(' expected \n");
		return;
	}

	compileExpression();

	if (tokenizer->tokenType() != TokenTypes::SYMBOL && tokenizer->symbol() != ')')
	{
		std::printf("Unexpected end to while. ')' expected \n");
		return;
	}

	// parsing body of the if condition
	getNextToken(tokenizer);
	if (tokenizer->tokenType() != TokenTypes::SYMBOL && tokenizer->symbol() != '{')
	{
		std::printf("Unexpected opening to while body. '{' expected \n");
		return;
	}

	compileStatements();

	if (tokenizer->tokenType() != TokenTypes::SYMBOL && tokenizer->symbol() != '}')
	{
		std::printf("Unexpected end to while body. '}' expected \n");
		return;
	}

	//TODO else statements
	//getNextToken(tokenizer);

	//if (tokenizer->keyWord() == KeyWords::ELSE)
	//{
	//	// parsing body of the while loop
	//	getNextToken(tokenizer);
	//	if (tokenizer->tokenType() != TokenTypes::SYMBOL && tokenizer->symbol() != '{')
	//	{
	//		std::printf("Unexpected opening to while body. '{' expected \n");
	//		return;
	//	}

	//	compileStatements();

	//	if (tokenizer->tokenType() != TokenTypes::SYMBOL && tokenizer->symbol() != '}')
	//	{
	//		std::printf("Unexpected end to while body. '}' expected \n");
	//		return;
	//	}
	//}
}


void JackParser::compileWhile()
{
	getNextToken(tokenizer);
	if (tokenizer->tokenType() != TokenTypes::SYMBOL && tokenizer->symbol() != '(')
	{
		std::printf("Unexpected opening to while. '(' expected \n");
		return;
	}

	compileExpression();

	if (tokenizer->tokenType() != TokenTypes::SYMBOL && tokenizer->symbol() != ')')
	{
		std::printf("Unexpected end to while. ')' expected \n");
		return;
	}

	// parsing body of the while loop
	getNextToken(tokenizer);
	if (tokenizer->tokenType() != TokenTypes::SYMBOL && tokenizer->symbol() != '{')
	{
		std::printf("Unexpected opening to while body. '{' expected \n");
		return;
	}

	getNextToken(tokenizer);
	compileStatements();

	if (tokenizer->tokenType() != TokenTypes::SYMBOL && tokenizer->symbol() != '}')
	{
		std::printf("Unexpected end to while body. '}' expected \n");
		return;
	}
}


void JackParser::compileDo()
{
	getNextToken(tokenizer);
	if (tokenizer->tokenType() == TokenTypes::IDENTIFIER)
	{
		getNextToken(tokenizer);
		if (tokenizer->tokenType() == TokenTypes::SYMBOL && tokenizer->symbol() == '(')
		{
			compileExpressionList();

			if (tokenizer->tokenType() != TokenTypes::SYMBOL && tokenizer->symbol() != ')')
			{
				std::printf("Unexpected end to expression list. ')' expected \n");
				return;
			}
		}
		else if (tokenizer->tokenType() == TokenTypes::SYMBOL && tokenizer->symbol() == '.')
		{
			getNextToken(tokenizer);
			if (tokenizer->tokenType() == TokenTypes::IDENTIFIER)
			{
				getNextToken(tokenizer);
				if (tokenizer->tokenType() == TokenTypes::SYMBOL && tokenizer->symbol() == '(')
				{
					compileExpressionList();

					if (tokenizer->tokenType() == TokenTypes::SYMBOL && tokenizer->symbol() != ')')
					{
						std::printf("Unexpected end to expression list. ')' expected \n");
						return;
					}
					getNextToken(tokenizer);
				}
				else
				{
					std::printf("Unexpected syntax for subroutine call. '(' expected \n");
					return;
				}
			}
			else
			{
				std::printf("Unexpected syntax for subroutine call. 'subroutine name' expected \n");
				return;
			}
		}
	}
	else
	{
		std::printf("Unexpected syntax for Do statement. 'subroutine name' expected \n");
	}
}


void JackParser::compileReturn()
{
	compileExpression();
}


void JackParser::compileExpressionList()
{
	while (true)
	{
		compileExpression();

		//getNextToken(tokenizer);
		if (tokenizer->tokenType() != TokenTypes::SYMBOL || tokenizer->symbol() != ',')
		{
			break;
		}
	}
}


void JackParser::compileExpression()
{
	compileTerm();

	//getNextToken(tokenizer);

	while (true)
	{
		if (tokenizer->tokenType() == TokenTypes::SYMBOL
			&& (tokenizer->symbol()    == '+'
				|| tokenizer->symbol() == '-'
				|| tokenizer->symbol() == '*'
				|| tokenizer->symbol() == '/'
				|| tokenizer->symbol() == '&'
				|| tokenizer->symbol() == '|'
				|| tokenizer->symbol() == '>'
				|| tokenizer->symbol() == '<'
				|| tokenizer->symbol() == '='))
		{
			compileTerm();
		}
		else
		{
			break;
		}
	}

}


void JackParser::compileTerm()
{
	getNextToken(tokenizer);
	if (	tokenizer->tokenType() == TokenTypes::INT_CONST
		||  tokenizer->tokenType() == TokenTypes::STRING_CONST
		|| (tokenizer->tokenType() == TokenTypes::KEYWORD 
				&& (	tokenizer->keyWord() == KeyWords::TRUE
					|| tokenizer->keyWord() == KeyWords::FALSE
					|| tokenizer->keyWord() == KeyWords::NULLX
					|| tokenizer->keyWord() == KeyWords::THIS)))
	{
		getNextToken(tokenizer);
	}
	else if (tokenizer->tokenType() == TokenTypes::IDENTIFIER)
	{
		getNextToken(tokenizer);
		if (tokenizer->tokenType() == TokenTypes::SYMBOL && tokenizer->symbol() == '[')
		{
			compileExpression();

			if (tokenizer->tokenType() != TokenTypes::SYMBOL && tokenizer->symbol() != ']')
			{
				std::printf("Unexpected end to term. ']' expected \n");
				return;
			}
			getNextToken(tokenizer);
		}
		// Handling subroutine calls
		else if (tokenizer->tokenType() == TokenTypes::SYMBOL && tokenizer->symbol() == '(')
		{
			compileExpressionList();

			if (tokenizer->tokenType() != TokenTypes::SYMBOL && tokenizer->symbol() != ')')
			{
				std::printf("Unexpected end to expression list. ')' expected \n");
				return;
			}
		}
		else if (tokenizer->tokenType() == TokenTypes::SYMBOL && tokenizer->symbol() == '.')
		{
			getNextToken(tokenizer);
			if (tokenizer->tokenType() == TokenTypes::IDENTIFIER)
			{
				getNextToken(tokenizer);
				if (tokenizer->tokenType() == TokenTypes::SYMBOL && tokenizer->symbol() == '(')
				{
					compileExpressionList();

					if (tokenizer->tokenType() == TokenTypes::SYMBOL && tokenizer->symbol() != ')')
					{
						std::printf("Unexpected end to expression list. ')' expected \n");
						return;
					}
					getNextToken(tokenizer);
				}
				else
				{
					std::printf("Unexpected syntax for subroutine call. '(' expected \n");
					return;
				}
			}
			else
			{
				std::printf("Unexpected syntax for subroutine call. 'subroutine name' expected \n");
				return;
			}
		}
	}
	else if (tokenizer->tokenType() == TokenTypes::SYMBOL && tokenizer->symbol() == '(')
	{
		compileExpression();

		if (tokenizer->tokenType() != TokenTypes::SYMBOL && tokenizer->symbol() != ')')
		{
			std::printf("Unexpected end to expression. ')' expected \n");
			return;
		}
	}
	else if (tokenizer->tokenType() == TokenTypes::SYMBOL 
		&& ( tokenizer->symbol() == '~' || tokenizer->symbol() == '-'))
	{
		compileTerm();
	}
}


