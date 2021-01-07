#include "JackParser.h"

//TODO add const char for all the characters

static std::string remove_extension(std::string file_name_with_extension)
{
	size_t lastindex = file_name_with_extension.find_last_of(".");
	std::string rawname = file_name_with_extension.substr(0, lastindex);
	return rawname;
}


Token* JackParser::getNextToken()
{
	Token* ret_ptr = nullptr;
	if (!tokens_stack.empty())
	{
		ret_ptr = tokens_stack.top();
		tokens_stack.pop();
	}
	else if (tokenizer->hasMoreTokens())
	{
		ret_ptr = tokenizer->advance();
	}

	return ret_ptr;
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
	current_token = getNextToken();
	if (current_token->keyword != KeyWords::CLASS)
	{
		std::printf("Unexpected class definition. class keyword expected \n");
	}

	current_token = getNextToken();
	if (current_token->type != TokenTypes::IDENTIFIER)
	{
		std::printf("Unexpected class definition. class name expected \n");
	}

	current_token = getNextToken();
	if (current_token->type != TokenTypes::SYMBOL && current_token->str != "{")
	{
		std::printf("Unexpected class definition. { expected \n");
	}

	while (true)
	{
		current_token = getNextToken();
		if (current_token->keyword == KeyWords::STATIC || current_token->keyword == KeyWords::FIELD)
		{
			//Peek the token
			tokens_stack.push(current_token);
			compileClassVarDec();
			continue;
		}

		tokens_stack.push(current_token);
		break;
	}

	while (true)
	{
		current_token = getNextToken();
		if (current_token->keyword == KeyWords::CONSTRUCTOR || current_token->keyword == KeyWords::FUNCTION 
			|| current_token->keyword == KeyWords::METHOD)
		{
			//Peek the token
			tokens_stack.push(current_token);
			compileSubroutineDec();
			continue;
		}
		tokens_stack.push(current_token);
		break;
	}

	current_token = getNextToken();
	if (current_token->type != TokenTypes::SYMBOL || current_token->str != "}")
	{
		std::printf("Unexpected class definition. } expected \n");
		return;
	}

	std::printf("Compilation Success \n");
}


void JackParser::compileClassVarDec()
{
	current_token = getNextToken();
	if (current_token->keyword == KeyWords::CONSTRUCTOR || current_token->keyword == KeyWords::FUNCTION
		|| current_token->keyword == KeyWords::METHOD)
	{
		// we have already peeked at this condition and that's why we are here. 
		// therefore no need for this condition check.
		// but for the sake of completion and localization I have put it here.
		tokens_stack.push(current_token);
	}
	
	
	current_token = getNextToken();

	// if the token is for keyword then it has to be type one (boolean | char | int)
	// or it can be identifier which has to be already declared
	if ( !((current_token->type == TokenTypes::KEYWORD
				&& ((current_token->keyword == KeyWords::BOOLEAN)
					|| (current_token->keyword == KeyWords::INT) || (current_token->keyword == KeyWords::CHAR)))
			||(current_token->type == TokenTypes::IDENTIFIER)))
	{
		std::printf("Unexpected class var declaration. Type expected %d\n",tokenizer->lineNumber());
		return;
	}

	current_token = getNextToken();
	while (true)
	{
		if (current_token->type != TokenTypes::IDENTIFIER)
		{
			std::printf("Unexpected class var declaration. Identifier expected %d\n", tokenizer->lineNumber());
			return;
		}

		current_token = getNextToken();
		if (current_token->str != ",")
		{
			// no need to push as next expected token is ; and error handling is done below.
			//tokens_stack.push(current_token);
			break;
		}
	}

	// Match ';' at the end of var definitions
	if (current_token->type != TokenTypes::SYMBOL || current_token->str != ";")
	{
		tokens_stack.push(current_token);
		std::printf("Unexpected class var declaration. Should end with ; %d\n",tokenizer->lineNumber());
		return;
	}
}


void JackParser::compileSubroutineDec()
{
	current_token = getNextToken();
	if (current_token->keyword == KeyWords::CONSTRUCTOR || current_token->keyword == KeyWords::FUNCTION
		|| current_token->keyword == KeyWords::METHOD)
	{
		// this condition is obselete as we have checked it from out side
	}

	// function return type check
	current_token = getNextToken();
	if ( !(((current_token->type == TokenTypes::KEYWORD) && (current_token->keyword == KeyWords::VOID))
		    ||(current_token->type == TokenTypes::IDENTIFIER)) )
	{
		// we have already peeked at this condition and that's why we are here. 
		// therefore no need for this condition check.
		// but for the sake of completion and localization I have put it here.
	}

	//function name check
	current_token = getNextToken();
	if (current_token->type != TokenTypes::IDENTIFIER)
	{
		std::printf("Unexpected function declaration. Function name expected %d\n",tokenizer->lineNumber());
		tokens_stack.push(current_token);
	}

	//function parameter list start
	current_token = getNextToken();
	if (current_token->type != TokenTypes::SYMBOL && current_token->str != "(")
	{
		std::printf("Unexpected function declaration. '(' expected at %d\n", tokenizer->lineNumber());
		tokens_stack.push(current_token);
		return;
	}

	compileParameterList();

	current_token = getNextToken();
	//function parameter list start
	if (current_token->type != TokenTypes::SYMBOL || current_token->str != ")")
	{
		std::printf("Unexpected function declaration. ')' expected at %d\n", tokenizer->lineNumber());
		tokens_stack.push(current_token);
		return;
	}

	compileSubroutineBody();
}


void JackParser::compileParameterList()
{
	//function parameter list check
	while (true)
	{
		current_token = getNextToken();
		if (current_token->type != TokenTypes::KEYWORD
			&& ((current_token->keyword != KeyWords::BOOLEAN) && (current_token->keyword != KeyWords::INT)
				&& (current_token->keyword != KeyWords::CHAR))
			&& current_token->type != TokenTypes::IDENTIFIER)
		{
			tokens_stack.push(current_token);
			break;
		}

		current_token = getNextToken();
		if (current_token->type != TokenTypes::IDENTIFIER)
		{
			std::printf("Unexpected parameter declaration. Identifier expected at %d\n", tokenizer->lineNumber());
			tokens_stack.push(current_token);
			return;
		}

		current_token = getNextToken();
		if (current_token->type != TokenTypes::SYMBOL || current_token->str != ",")
		{
			tokens_stack.push(current_token);
			break;
		}
	}
}


void JackParser::compileSubroutineBody()
{
	//function body start
	current_token = getNextToken();
	if (current_token->type != TokenTypes::SYMBOL && current_token->str != "{")
	{
		std::printf("Unexpected function body open. '{' expected at %d\n",tokenizer->lineNumber());
		tokens_stack.push(current_token);
		return;
	}

	while (true)
	{
		current_token = getNextToken();

		if (current_token->type != TokenTypes::KEYWORD
			|| (current_token->keyword != KeyWords::VAR))
		{
			tokens_stack.push(current_token);
			break;
		}

		//line start with "static" or "field"
		compileVarDec();
	}

	// statements
	compileStatements();

	current_token = getNextToken();
	if (current_token->type != TokenTypes::SYMBOL && current_token->str != "}")
	{
		std::printf("Unexpected function body close. '}' expected. %d \n", tokenizer->lineNumber());
		return;
	}
}


void JackParser::compileVarDec()
{
	//TODO this code is similar to the code for the class variable parsing. Try to remove it
	current_token = getNextToken();
	if (!((current_token->type == TokenTypes::KEYWORD
		&& ((current_token->keyword == KeyWords::BOOLEAN) || (current_token->keyword == KeyWords::INT)
			|| (current_token->keyword == KeyWords::CHAR)))
		|| (current_token->type == TokenTypes::IDENTIFIER)))
	{
		std::printf("Unexpected var declaration. Type expected at %d.\n", tokenizer->lineNumber());
		tokens_stack.push(current_token);
		return;
	}

	while (true)
	{
		current_token = getNextToken();
		if (current_token->type != TokenTypes::IDENTIFIER)
		{
			std::printf("Unexpected var declaration. Identifier expected at %d. \n", tokenizer->lineNumber());
			tokens_stack.push(current_token);
			return;
		}

		current_token = getNextToken();
		if (current_token->type != TokenTypes::SYMBOL || current_token->str != ",")
		{
			tokens_stack.push(current_token);
			break;
		}
	}

	current_token = getNextToken();
	// Match ';' at the end of var definitions
	if (current_token->type != TokenTypes::SYMBOL || current_token->str != ";")
	{
		std::printf("Unexpected var declaration. Should end with ; at %d.\n", tokenizer->lineNumber());
		tokens_stack.push(current_token);
		return;
	}
}


void JackParser::compileStatements()
{
	while (true)
	{
		current_token = getNextToken();
		if (current_token->type == TokenTypes::KEYWORD)
		{
			tokens_stack.push(current_token);
			if (current_token->keyword == KeyWords::WHILE)
			{
				compileWhile();
			}
			else if (current_token->keyword == KeyWords::IF)
			{
				compileIf();
			}
			else if (current_token->keyword == KeyWords::LET)
			{
				compileLet();
			}
			else if (current_token->keyword == KeyWords::RETURN)
			{
				compileReturn();

				current_token = getNextToken();
				if (current_token->type != TokenTypes::SYMBOL || current_token->str != ";")
				{
					tokens_stack.push(current_token);
					std::printf("Unexpected ending for var declaration. ';' expected at %d.\n", tokenizer->lineNumber());
					return;
				}
			}
			else if (current_token->keyword == KeyWords::DO)
			{
				compileDo();
				
				current_token = getNextToken();				
				if (current_token->type != TokenTypes::SYMBOL || current_token->str != ";")
				{
					tokens_stack.push(current_token);
					std::printf("Unexpected ending for var declaration. ';' expected at %d.\n", tokenizer->lineNumber());
					return;
				}
			}
			else
			{
				break;
			}
		}
		else
		{
			tokens_stack.push(current_token);
			break;
		}
	}
}


void JackParser::compileLet()
{
	current_token = getNextToken();
	if (current_token->keyword != KeyWords::LET)
	{
		// this condition is obselete as we have checked it from out side
	}

	current_token = getNextToken();
	if (current_token->type != TokenTypes::IDENTIFIER)
	{
		std::printf("Unexpected 'let' syntax. var name expected \n");
		return;
	}

	current_token = getNextToken();
	if (current_token->type == TokenTypes::SYMBOL && current_token->str == "[")
	{
		compileExpression();

		current_token = getNextToken();
		if (current_token->type != TokenTypes::SYMBOL && current_token->str != "]")
		{
			std::printf("Unexpected var array declaration. ']' expected \n");
			return;
		}
	}
	else
	{
		tokens_stack.push(current_token);
	}

	current_token = getNextToken();
	if (current_token->type != TokenTypes::SYMBOL && current_token->str != "=")
	{
		std::printf("Unexpected assignment to var. '=' expected \n");
		return;
	}

	compileExpression();

	current_token = getNextToken();
	if (current_token->type != TokenTypes::SYMBOL || current_token->str != ";")
	{
		std::printf("Unexpected ending for var declaration. ';' expected \n");
		return;
	}
}


void JackParser::compileIf()
{
	current_token = getNextToken();
	if (current_token->keyword != KeyWords::IF)
	{
		// this condition is obselete as we have checked it from out side
	}

	current_token = getNextToken();
	if (current_token->type != TokenTypes::SYMBOL && current_token->str != "(")
	{
		std::printf("Unexpected opening to while. '(' expected \n");
		return;
	}

	compileExpression();

	if (current_token->type != TokenTypes::SYMBOL && current_token->str != ")")
	{
		std::printf("Unexpected end to while. ')' expected \n");
		return;
	}

	// parsing body of the if condition
	current_token = getNextToken();
	if (current_token->type != TokenTypes::SYMBOL && current_token->str != "{")
	{
		std::printf("Unexpected opening to while body. '{' expected \n");
		return;
	}

	compileStatements();

	if (current_token->type != TokenTypes::SYMBOL && current_token->str != "}")
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
	current_token = getNextToken();
	if (current_token->keyword != KeyWords::WHILE)
	{
		// this condition is obselete as we have checked it from out side
	}

	current_token = getNextToken();
	if (current_token->type != TokenTypes::SYMBOL && current_token->str != "(")
	{
		std::printf("Unexpected opening to while. '(' expected \n");
		return;
	}

	compileExpression();

	current_token = getNextToken();
	if (current_token->type != TokenTypes::SYMBOL && current_token->str != ")")
	{
		std::printf("Unexpected end to while. ')' expected \n");
		return;
	}

	// parsing body of the while loop
	current_token = getNextToken();
	if (current_token->type != TokenTypes::SYMBOL && current_token->str != "{")
	{
		std::printf("Unexpected opening to while body. '{' expected \n");
		return;
	}

	compileStatements();

	current_token = getNextToken();
	if (current_token->type != TokenTypes::SYMBOL && current_token->str != "}")
	{
		std::printf("Unexpected end to while body. '}' expected \n");
		return;
	}
}


void JackParser::compileDo()
{
	current_token = getNextToken();
	if (current_token->keyword != KeyWords::DO)
	{
		// this condition is obselete as we have checked it from out side
	}

	current_token = getNextToken();
	if (current_token->type == TokenTypes::IDENTIFIER)
	{
		current_token = getNextToken();
		if (current_token->type == TokenTypes::SYMBOL && current_token->str == "(")
		{
			compileExpressionList();

			if (current_token->type != TokenTypes::SYMBOL && current_token->str != ")")
			{
				std::printf("Unexpected end to expression list. ')' expected at %d.\n", tokenizer->lineNumber());
				return;
			}
		}
		else if (current_token->type == TokenTypes::SYMBOL && current_token->str == ".")
		{
			current_token = getNextToken();
			if (current_token->type == TokenTypes::IDENTIFIER)
			{
				current_token = getNextToken();
				if (current_token->type == TokenTypes::SYMBOL && current_token->str == "(")
				{
					compileExpressionList();

					current_token = getNextToken();
					if (current_token->type == TokenTypes::SYMBOL && current_token->str != ")")
					{
						std::printf("Unexpected end to expression list. ')' expected at %d.\n", tokenizer->lineNumber());
						return;
					}
				}
				else
				{
					std::printf("Unexpected syntax for subroutine call. '(' expected at %d.\n", tokenizer->lineNumber());
					return;
				}
			}
			else
			{
				std::printf("Unexpected syntax for subroutine call. 'subroutine name' expected at %d.\n", tokenizer->lineNumber());
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

		current_token = getNextToken();
		if (current_token->type != TokenTypes::SYMBOL || current_token->str != ",")
		{
			tokens_stack.push(current_token);
			break;
		}
	}
}


void JackParser::compileExpression()
{
	compileTerm();

	while (true)
	{
		current_token = getNextToken();
		if (current_token->type == TokenTypes::SYMBOL
			&& (current_token->str == "+"
				|| current_token->str == "-"
				|| current_token->str == "*"
				|| current_token->str == "/"
				|| current_token->str == "&"
				|| current_token->str == "|"
				|| current_token->str == ">"
				|| current_token->str == "<"
				|| current_token->str == "="))
		{
			compileTerm();
		}
		else
		{
			tokens_stack.push(current_token);
			break;
		}
	}
}


void JackParser::compileTerm()
{
	current_token = getNextToken();
	if (current_token->type == TokenTypes::INT_CONST
		|| current_token->type == TokenTypes::STRING_CONST
		|| (current_token->type == TokenTypes::KEYWORD
				&& (current_token->keyword == KeyWords::TRUE
					|| current_token->keyword == KeyWords::FALSE
					|| current_token->keyword == KeyWords::NULLX
					|| current_token->keyword == KeyWords::THIS)))
	{
		
	}
	else if (current_token->type == TokenTypes::IDENTIFIER)
	{
		current_token = getNextToken();
		if (current_token->type == TokenTypes::SYMBOL && current_token->str == "[")
		{
			compileExpression();

			current_token = getNextToken();
			if (current_token->type != TokenTypes::SYMBOL && current_token->str != "]")
			{
				std::printf("Unexpected end to term. ']' expected at %d.\n", tokenizer->lineNumber());
				return;
			}
		}
		// Handling subroutine calls
		else if (current_token->type == TokenTypes::SYMBOL && current_token->str == "(")
		{
			compileExpressionList();

			current_token = getNextToken();
			if (current_token->type != TokenTypes::SYMBOL && current_token->str != ")")
			{
				std::printf("Unexpected end to expression list. ')' expected at %d.\n", tokenizer->lineNumber());
				return;
			}
		}
		else if (current_token->type == TokenTypes::SYMBOL && current_token->str == ".")
		{
			current_token = getNextToken();
			if (current_token->type == TokenTypes::IDENTIFIER)
			{
				current_token = getNextToken();
				if (current_token->type == TokenTypes::SYMBOL && current_token->str == "(")
				{
					compileExpressionList();

					current_token = getNextToken();
					if (current_token->type == TokenTypes::SYMBOL && current_token->str != ")")
					{
						std::printf("Unexpected end to expression list. ')' expected at %d.\n", tokenizer->lineNumber());
						return;
					}
				}
				else
				{
					std::printf("Unexpected syntax for subroutine call. '(' expected at %d.\n", tokenizer->lineNumber());
					return;
				}
			}
			else
			{
				std::printf("Unexpected syntax for subroutine call. 'subroutine name' expected at %d.\n", tokenizer->lineNumber());
				return;
			}
		}
		else
		{
			tokens_stack.push(current_token);
		}
	}
	else if (current_token->type == TokenTypes::SYMBOL && current_token->str == "(")
	{
		compileExpression();
		
		current_token = getNextToken();
		if (current_token->type != TokenTypes::SYMBOL && current_token->str != ")")
		{
			std::printf("Unexpected end to expression. ')' expected at %d.\n", tokenizer->lineNumber());
			return;
		}
	}
	else if (current_token->type == TokenTypes::SYMBOL
		&& (current_token->str == "~" || current_token->str == "-"))
	{
		compileTerm();
	}
}


