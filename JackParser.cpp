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
	symbol_tbl_mgr = new JackSymbolTabelManager();

	std::string out_name = remove_extension(fileName);
	out_name.append("_out.txt");
	out_file = new std::ofstream (out_name,std::ios::out);
}


JackParser::~JackParser()
{
	delete tokenizer;
}


void JackParser::compileClass()
{
	symbol_tbl_mgr->reset_cls_table();

	current_token = getNextToken();
	if (current_token->keyword != KeyWords::CLASS)
	{
		std::printf("Unexpected class definition. class keyword expected at %d.\n", tokenizer->lineNumber());
	}

	current_token = getNextToken();
	if (current_token->type != TokenTypes::IDENTIFIER)
	{
		std::printf("Unexpected class definition. class name expected at %d.\n", tokenizer->lineNumber());
	}

	symbol_tbl_mgr->set_current_cls(current_token->str.c_str());

	current_token = getNextToken();
	if (current_token->type != TokenTypes::SYMBOL && current_token->str != "{")
	{
		std::printf("Unexpected class definition. { expected at %d.\n", tokenizer->lineNumber());
	}

	symbol_tbl_mgr->reset_cls_var_indices();
	
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

	symbol_tbl_mgr->print_cls_table();

	while (true)
	{
		current_token = getNextToken();
		if (current_token->keyword == KeyWords::CONSTRUCTOR || current_token->keyword == KeyWords::FUNCTION 
			|| current_token->keyword == KeyWords::METHOD)
		{
			//Peek the token
			tokens_stack.push(current_token);
			compileSubroutineDec();
			
			symbol_tbl_mgr->print_subrt_table();
			continue;
		}
		tokens_stack.push(current_token);
		break;
	}

	current_token = getNextToken();
	if (current_token->type != TokenTypes::SYMBOL || current_token->str != "}")
	{
		std::printf("Unexpected class definition. } expected at %d.\n", tokenizer->lineNumber());
		return;
	}

	std::printf("Compilation Success");
}


void JackParser::compileClassVarDec()
{
	VAR_KIND kind;
	std::string type;

	current_token = getNextToken();
	if (current_token->keyword != KeyWords::STATIC && current_token->keyword != KeyWords::FIELD)
	{
		// we have already peeked at this condition and that's why we are here. 
		// therefore no need for this condition check.
		// but for the sake of completion and localization I have put it here.
		tokens_stack.push(current_token);
	}
	
	// symbol table entry index setup
	if (current_token->keyword == KeyWords::STATIC)
		kind = VAR_KIND::CLS_STATIC;
	else
		kind = VAR_KIND::CLS_FIELD;

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

	type = current_token->str;

	current_token = getNextToken();
	while (true)
	{
		if (current_token->type != TokenTypes::IDENTIFIER)
		{
			std::printf("Unexpected class var declaration. Identifier expected %d\n", tokenizer->lineNumber());
			return;
		}

		// Add an entry to class symbol table 
		symbol_tbl_mgr->add_entry_cls_table(current_token->str.c_str(), type.c_str(), kind);

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
	//subroutine_symbol_table.clear();
	//subroutine_arg_var_index = 0;
	//subroutine_local_var_index = 0;

	symbol_tbl_mgr->reset_subrt_table();
	symbol_tbl_mgr->reset_subrt_var_indices();


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
	VAR_KIND kind = VAR_KIND::SUBRT_ARG;
	std::string type;

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

		type = current_token->str;

		current_token = getNextToken();
		if (current_token->type != TokenTypes::IDENTIFIER)
		{
			std::printf("Unexpected parameter declaration. Identifier expected at %d\n", tokenizer->lineNumber());
			tokens_stack.push(current_token);
			return;
		}

		// Add an entry to subroutine symbol table
		symbol_tbl_mgr->add_entry_subrt_table(current_token->str.c_str(), type.c_str(), kind);

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

		if (current_token->keyword != KeyWords::VAR)
		{
			tokens_stack.push(current_token);
			break;
		}

		//line start with "var" or "field"
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
	VAR_KIND kind = VAR_KIND::SUBRT_LOCAL;
	std::string type;

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

	type = current_token->str;

	while (true)
	{
		current_token = getNextToken();
		if (current_token->type != TokenTypes::IDENTIFIER)
		{
			std::printf("Unexpected var declaration. Identifier expected at %d. \n", tokenizer->lineNumber());
			tokens_stack.push(current_token);
			return;
		}

		// Add an entry to subroutine symbol table
		symbol_tbl_mgr->add_entry_subrt_table(current_token->str.c_str(), type.c_str(), kind);

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
		std::printf("Unexpected 'let' syntax. var name expected at %d.\n", tokenizer->lineNumber());
		return;
	}

	current_token = getNextToken();
	if (current_token->type == TokenTypes::SYMBOL && current_token->str == "[")
	{
		compileExpression();

		current_token = getNextToken();
		if (current_token->type != TokenTypes::SYMBOL && current_token->str != "]")
		{
			std::printf("Unexpected var array declaration. ']' expected at %d.\n", tokenizer->lineNumber());
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
		std::printf("Unexpected assignment to var. '=' expected at %d.\n", tokenizer->lineNumber());
		return;
	}

	compileExpression();

	current_token = getNextToken();
	if (current_token->type != TokenTypes::SYMBOL || current_token->str != ";")
	{
		std::printf("Unexpected ending for var declaration. ';' expected at %d.\n", tokenizer->lineNumber());
		return;
	}
}


void JackParser::compileIf()
{
	int temp_lbl1 = ++label_count;
	int temp_lbl2 = ++label_count;

	current_token = getNextToken();
	if (current_token->keyword != KeyWords::IF)
	{
		// this condition is obselete as we have checked it from out side
	}

	current_token = getNextToken();
	if (current_token->type != TokenTypes::SYMBOL && current_token->str != "(")
	{
		std::printf("Unexpected opening to while. '(' expected at %d.\n", tokenizer->lineNumber());
		return;
	}

	compileExpression();

	*out_file << "not \n" << "if goto L" << temp_lbl1 << "\n";

	current_token = getNextToken();
	if (current_token->type != TokenTypes::SYMBOL && current_token->str != ")")
	{
		std::printf("Unexpected end to while. ')' expected at %d.\n", tokenizer->lineNumber());
		return;
	}

	// parsing body of the if condition
	current_token = getNextToken();
	if (current_token->type != TokenTypes::SYMBOL && current_token->str != "{")
	{
		std::printf("Unexpected opening to while body. '{' expected at %d.\n", tokenizer->lineNumber());
		return;
	}

	compileStatements();

	*out_file << "goto L" << temp_lbl2 << "\n";
	*out_file << "Label L" << temp_lbl1 << "\n";

	current_token = getNextToken();
	if (current_token->type != TokenTypes::SYMBOL && current_token->str != "}")
	{
		std::printf("Unexpected end to while body. '}' expected at %d.\n", tokenizer->lineNumber());
		return;
	}

	current_token = getNextToken();
	if (current_token->keyword == KeyWords::ELSE)
	{
		// parsing body of the while loop
		current_token = getNextToken();
		if (current_token->type != TokenTypes::SYMBOL && current_token->str != "{")
		{
			std::printf("Unexpected opening to else body. '{' expected at %d.\n", tokenizer->lineNumber());
			return;
		}

		compileStatements();
		*out_file << "Label L" << temp_lbl2 << "\n";

		current_token = getNextToken();
		if (current_token->type != TokenTypes::SYMBOL && current_token->str != "}")
		{
			std::printf("Unexpected end to else body. '}' expected at %d.\n", tokenizer->lineNumber());
			return;
		}
	}
	else
	{
		tokens_stack.push(current_token);
	}
}


void JackParser::compileWhile()
{
	int temp_lbl1 = ++label_count;
	int temp_lbl2 = ++label_count;
	current_token = getNextToken();

	*out_file << "Label L" << temp_lbl1 << "\n";

	if (current_token->keyword != KeyWords::WHILE)
	{
		// this condition is obselete as we have checked it from out side
	}

	current_token = getNextToken();
	if (current_token->type != TokenTypes::SYMBOL && current_token->str != "(")
	{
		std::printf("Unexpected opening to while. '(' expected at %d.\n", tokenizer->lineNumber());
		return;
	}

	compileExpression();

	*out_file << "not \n" << "if goto L" << temp_lbl2 << "\n";

	current_token = getNextToken();
	if (current_token->type != TokenTypes::SYMBOL && current_token->str != ")")
	{
		std::printf("Unexpected end to while. ')' expected at %d.\n", tokenizer->lineNumber());
		return;
	}

	// parsing body of the while loop
	current_token = getNextToken();
	if (current_token->type != TokenTypes::SYMBOL && current_token->str != "{")
	{
		std::printf("Unexpected opening to while body. '{' expected at %d.\n", tokenizer->lineNumber());
		return;
	}

	compileStatements();

	*out_file << "goto L" << temp_lbl1 << "\n";
	*out_file << "Label L" << temp_lbl2 << "\n";

	current_token = getNextToken();
	if (current_token->type != TokenTypes::SYMBOL && current_token->str != "}")
	{
		std::printf("Unexpected end to while body. '}' expected at %d.\n", tokenizer->lineNumber());
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
		std::printf("Unexpected syntax for Do statement. 'subroutine name' expected at %d.\n", tokenizer->lineNumber());
	}
}


void JackParser::compileReturn()
{
	current_token = getNextToken();
	if (current_token->keyword != KeyWords::RETURN)
	{
		// this condition is obselete as we have checked it from out side
	}

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
			std::string code = current_token->str;
			compileTerm();
			*out_file << code << "\n";
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
	std::string code;
	current_token = getNextToken();
	if (current_token->type == TokenTypes::INT_CONST
		|| current_token->type == TokenTypes::STRING_CONST
		|| (current_token->type == TokenTypes::KEYWORD
				&& (current_token->keyword == KeyWords::TRUE
					|| current_token->keyword == KeyWords::FALSE
					|| current_token->keyword == KeyWords::NULLX
					|| current_token->keyword == KeyWords::THIS)))
	{
		if (current_token->type == TokenTypes::INT_CONST)
			*out_file << "push " << current_token->str << "\n";
	}
	else if (current_token->type == TokenTypes::IDENTIFIER)
	{
		code = current_token->str;
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
			*out_file << "call " << code << "\n";
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
			*out_file << "push " << code << "\n";
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
		code = current_token->str;
		compileTerm();
		*out_file << code << "\n";
	}
	else
	{
		tokens_stack.push(current_token);
	}
}


