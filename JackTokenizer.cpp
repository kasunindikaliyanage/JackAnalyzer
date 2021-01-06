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

void JackTokenizer::advance()
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
        currenct_token_type = TokenTypes::SYMBOL;
        parsed_str = c;
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

        currenct_token_type = TokenTypes::STRING_CONST;
    }
    else if (std::isdigit(c))
	{
		int v = 0;
		char chr;

		do {
			v = 10 * v + std::atoi(&c);
            parsed_value = v;
			chr = input_stream.peek();
			if (std::isdigit(chr))
			{
				c = input_stream.get();
				continue;
			}
			break;
		} while (true);

        currenct_token_type = TokenTypes::INT_CONST;
	}
	else if (std::isalpha(c))
	{
        parsed_str.erase(parsed_str.begin(), parsed_str.end());
		char chr;
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

		if (  !has_underscore && 
             ( compare_string(parsed_str, "class")
            || compare_string(parsed_str, "constructor")
            || compare_string(parsed_str, "function")
            || compare_string(parsed_str, "method")
            || compare_string(parsed_str, "field")
            || compare_string(parsed_str, "static")
            || compare_string(parsed_str, "var")
            || compare_string(parsed_str, "int")
            || compare_string(parsed_str, "char")
            || compare_string(parsed_str, "boolean")
            || compare_string(parsed_str, "void")
            || compare_string(parsed_str, "true")
            || compare_string(parsed_str, "false")
            || compare_string(parsed_str, "null")
            || compare_string(parsed_str, "this")
            || compare_string(parsed_str, "let")
            || compare_string(parsed_str, "do")
            || compare_string(parsed_str, "if")
            || compare_string(parsed_str, "else")
            || compare_string(parsed_str, "while")
            || compare_string(parsed_str, "return")))
		{
            currenct_token_type = TokenTypes::KEYWORD;
		}
        else
        {
            currenct_token_type = TokenTypes::IDENTIFIER;
        }
	}
    else if (input_stream.eof())
    {
        currenct_token_type = TokenTypes::EOFILE;
    }
}

TokenTypes JackTokenizer::tokenType()
{
    return currenct_token_type;
}

KeyWords JackTokenizer::keyWord()
{
    if (compare_string(parsed_str, "class"))
    {
        return KeyWords::CLASS;
    }
    else if (compare_string(parsed_str, "constructor"))
    {
        return KeyWords::CONSTRUCTOR;
    }
    else if (compare_string(parsed_str, "function"))
    {
        return KeyWords::FUNCTION;
    }
    else if (compare_string(parsed_str, "method"))
    {
        return KeyWords::METHOD;
    }
    else if (compare_string(parsed_str, "field"))
    {
        return KeyWords::FIELD;
    }
    else if (compare_string(parsed_str, "static"))
    {
        return KeyWords::STATIC;
    }
    else if (compare_string(parsed_str, "var"))
    {
        return KeyWords::VAR;
    }
    else if (compare_string(parsed_str, "int"))
    {
        return KeyWords::INT;
    }
    else if (compare_string(parsed_str, "char"))
    {
        return KeyWords::CHAR;
    }
    else if (compare_string(parsed_str, "boolean"))
    {
        return KeyWords::BOOLEAN;
    }
    else if (compare_string(parsed_str, "void"))
    {
        return KeyWords::VOID;
    }
    else if (compare_string(parsed_str, "true"))
    {
        return KeyWords::TRUE;
    }
    else if (compare_string(parsed_str, "false"))
    {
        return KeyWords::FALSE;
    }
    else if (compare_string(parsed_str, "null"))
    {
        return KeyWords::NULLX;
    }
    else if (compare_string(parsed_str, "this"))
    {
        return KeyWords::THIS;
    }
    else if (compare_string(parsed_str, "let"))
    {
        return KeyWords::LET;
    }
    else if (compare_string(parsed_str, "do"))
    {
        return KeyWords::DO;
    }
    else if (compare_string(parsed_str, "if"))
    {
        return KeyWords::IF;
    }
    else if (compare_string(parsed_str, "else"))
    {
        return KeyWords::ELSE;
    }
    else if (compare_string(parsed_str, "while"))
    {
        return KeyWords::WHILE;
    }
    else
    {
        return KeyWords::RETURN;
    }
}

char JackTokenizer::symbol()
{
    return parsed_str[0];
}

std::string JackTokenizer::indentifier()
{
    return parsed_str;
}

int JackTokenizer::intVal()
{
    return parsed_value;
}

std::string JackTokenizer::stringVal()
{
    return parsed_str;
}
