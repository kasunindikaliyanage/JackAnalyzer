#include "JackTokenizer.h"

const char CHAR_UNDERSCORE  = '_';

JackTokenizer::JackTokenizer( const char* fileName )
{
    input_stream.open(fileName);

    if (!input_stream.is_open()) {
        std::cout << "Error : failed to open " << fileName << '\n';
        return;
    }
}

bool JackTokenizer::hasMoreTokens()
{
    if (!input_stream.eof())
        return true;
    return false;
}

static bool compare_string(std::string str1, std::string str2)
{
	if (str1.compare(str2) == 0)
		return true;

	return false;
}

Token* JackTokenizer::advance()
{
    //user has to call hasMoreTokens() explicitly before this.
    char c = input_stream.get();

    while (!input_stream.eof())
    {
        if ( c == ' ' || c == '\t')
        {
            c = input_stream.get();
            continue;
        }
        else if (c == '\n')
        {
            c = input_stream.get();
            line_number++;
        }
        else if (c == '/')
        {
            if (input_stream.peek() == '/')
            {
                while (c != '\n')
                {
                    c = input_stream.get();
                }
            }
            else if(input_stream.peek() == '*')
            {
               
                while (!input_stream.eof())
                {
                    c = input_stream.get();
                    if (c == '*' && input_stream.peek() == '/')
                    {
                        c = input_stream.get();
                        break;
                    }
                }
                c = input_stream.get();
            }
            else
            {
                break;
            }
        }
        else
        {
            break;
        }
    }

    if (c == '{' || c == '}'|| c == '[' || c == ']' || c == '(' || c == ')' || c == '.' || c == ',' || c == ';' || c == '+'
        || c == '-' || c == '*' || c == '/' || c == '&' || c == '|' || c == '>' || c == '<' || c == '=' || c == '~')
    {
        //currenct_token_type = TokenTypes::SYMBOL;
        parsed_str = c;
        token = new Token(TokenTypes::SYMBOL, parsed_str);
        return token;
    }
    else if ( c == '\"')
    {
        parsed_str.erase(parsed_str.begin(), parsed_str.end());
        c = input_stream.get();
        while (c != '\"')
        {
            parsed_str.append(1,c);
            c = input_stream.get();
        }

        token = new Token(TokenTypes::STRING_CONST, parsed_str);
        return token;
    }
    else if (std::isdigit(c))
	{
		int v = 0;
		char chr;

		do {
			v = 10 * v + std::atoi(&c);
            //parsed_value = v;
			chr = input_stream.peek();
			if (std::isdigit(chr))
			{
				c = input_stream.get();
				continue;
			}
			break;
		} while (true);

        token = new Token(TokenTypes::INT_CONST, parsed_str,v);
        return token;
	}
	else if (std::isalpha(c))
	{
        parsed_str.erase(parsed_str.begin(), parsed_str.end());
		char chr;
        
        // this is to minimize string comparison. It there is a '_' in the the string, it is not a keyword
        bool has_underscore = false; 

		do {
            parsed_str.append( 1, c );
			chr = input_stream.peek();

			if ( std::isalnum(chr) || chr == CHAR_UNDERSCORE )
			{
                if (chr == CHAR_UNDERSCORE)
                    has_underscore = true;

				c = input_stream.get();
				continue;
			}
			break;
		} while ( true );

        if (!has_underscore)
        {
            if (compare_string(parsed_str, "class"))
            {
                token = new Token(TokenTypes::KEYWORD, parsed_str, KeyWords::CLASS);
            }
            else if(compare_string(parsed_str, "constructor"))
            {
                token = new Token(TokenTypes::KEYWORD, parsed_str, KeyWords::CONSTRUCTOR);
            }
            else if (compare_string(parsed_str, "function"))
            {
                token = new Token(TokenTypes::KEYWORD, parsed_str, KeyWords::FUNCTION);
            }
            else if (compare_string(parsed_str, "method"))
            {
                token = new Token(TokenTypes::KEYWORD, parsed_str, KeyWords::METHOD);
            }
            else if (compare_string(parsed_str, "field"))
            {
                token = new Token(TokenTypes::KEYWORD, parsed_str, KeyWords::FIELD);
            }
            else if (compare_string(parsed_str, "static"))
            {
                token = new Token(TokenTypes::KEYWORD, parsed_str, KeyWords::STATIC);
            }
            else if (compare_string(parsed_str, "var"))
            {
                token = new Token(TokenTypes::KEYWORD, parsed_str, KeyWords::VAR);
            }
            else if (compare_string(parsed_str, "int"))
            {
                token = new Token(TokenTypes::KEYWORD, parsed_str, KeyWords::INT);
            }
            else if (compare_string(parsed_str, "char"))
            {
                token = new Token(TokenTypes::KEYWORD, parsed_str, KeyWords::CHAR);
            }
            else if (compare_string(parsed_str, "boolean"))
            {
                token = new Token(TokenTypes::KEYWORD, parsed_str, KeyWords::BOOLEAN);
            }
            else if (compare_string(parsed_str, "void"))
            {
                token = new Token(TokenTypes::KEYWORD, parsed_str, KeyWords::VOID);
            }
            else if (compare_string(parsed_str, "true"))
            {
                token = new Token(TokenTypes::KEYWORD, parsed_str, KeyWords::TRUE);
            }
            else if (compare_string(parsed_str, "false"))
            {
                token = new Token(TokenTypes::KEYWORD, parsed_str, KeyWords::FALSE);
            }
            else if (compare_string(parsed_str, "null"))
            {
                token = new Token(TokenTypes::KEYWORD, parsed_str, KeyWords::NULLX);
            }
            else if (compare_string(parsed_str, "this"))
            {
                token = new Token(TokenTypes::KEYWORD, parsed_str, KeyWords::THIS);
            }
            else if (compare_string(parsed_str, "let"))
            {
                token = new Token(TokenTypes::KEYWORD, parsed_str, KeyWords::LET);
            }
            else if (compare_string(parsed_str, "do"))
            {
                token = new Token(TokenTypes::KEYWORD, parsed_str, KeyWords::DO);
            }
            else if (compare_string(parsed_str, "if"))
            {
                token = new Token(TokenTypes::KEYWORD, parsed_str, KeyWords::NULLX);
            }
            else if (compare_string(parsed_str, "else"))
            {
                token = new Token(TokenTypes::KEYWORD, parsed_str, KeyWords::THIS);
            }
            else if (compare_string(parsed_str, "while"))
            {
                token = new Token(TokenTypes::KEYWORD, parsed_str, KeyWords::WHILE);
            }
            else if (compare_string(parsed_str, "return"))
            {
                token = new Token(TokenTypes::KEYWORD, parsed_str, KeyWords::RETURN);
            }
            else
            {
                token = new Token(TokenTypes::IDENTIFIER, parsed_str);
            }
        }
        else
        {
            token = new Token(TokenTypes::IDENTIFIER, parsed_str);
        }

        return token;
	}
    else if (input_stream.eof())
    {
        token = new Token(TokenTypes::EOFILE, parsed_str);
    }
}

int JackTokenizer::lineNumber()
{
    return line_number;
}

//TokenTypes JackTokenizer::tokenType()
//{
//    return currenct_token_type;
//}
//
//KeyWords JackTokenizer::keyWord()
//{
//    if (compare_string(parsed_str, "class"))
//    {
//        return KeyWords::CLASS;
//    }
//    else if (compare_string(parsed_str, "constructor"))
//    {
//        return KeyWords::CONSTRUCTOR;
//    }
//    else if (compare_string(parsed_str, "function"))
//    {
//        return KeyWords::FUNCTION;
//    }
//    else if (compare_string(parsed_str, "method"))
//    {
//        return KeyWords::METHOD;
//    }
//    else if (compare_string(parsed_str, "field"))
//    {
//        return KeyWords::FIELD;
//    }
//    else if (compare_string(parsed_str, "static"))
//    {
//        return KeyWords::STATIC;
//    }
//    else if (compare_string(parsed_str, "var"))
//    {
//        return KeyWords::VAR;
//    }
//    else if (compare_string(parsed_str, "int"))
//    {
//        return KeyWords::INT;
//    }
//    else if (compare_string(parsed_str, "char"))
//    {
//        return KeyWords::CHAR;
//    }
//    else if (compare_string(parsed_str, "boolean"))
//    {
//        return KeyWords::BOOLEAN;
//    }
//    else if (compare_string(parsed_str, "void"))
//    {
//        return KeyWords::VOID;
//    }
//    else if (compare_string(parsed_str, "true"))
//    {
//        return KeyWords::TRUE;
//    }
//    else if (compare_string(parsed_str, "false"))
//    {
//        return KeyWords::FALSE;
//    }
//    else if (compare_string(parsed_str, "null"))
//    {
//        return KeyWords::NULLX;
//    }
//    else if (compare_string(parsed_str, "this"))
//    {
//        return KeyWords::THIS;
//    }
//    else if (compare_string(parsed_str, "let"))
//    {
//        return KeyWords::LET;
//    }
//    else if (compare_string(parsed_str, "do"))
//    {
//        return KeyWords::DO;
//    }
//    else if (compare_string(parsed_str, "if"))
//    {
//        return KeyWords::IF;
//    }
//    else if (compare_string(parsed_str, "else"))
//    {
//        return KeyWords::ELSE;
//    }
//    else if (compare_string(parsed_str, "while"))
//    {
//        return KeyWords::WHILE;
//    }
//    else
//    {
//        return KeyWords::RETURN;
//    }
//}
//
//char JackTokenizer::symbol()
//{
//    return parsed_str[0];
//}
//
//std::string JackTokenizer::indentifier()
//{
//    return parsed_str;
//}
//
//int JackTokenizer::intVal()
//{
//    return parsed_value;
//}
//
//std::string JackTokenizer::stringVal()
//{
//    return parsed_str;
//}
