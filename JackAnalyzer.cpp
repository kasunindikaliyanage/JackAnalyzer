#include <iostream>
#include "JackTokenizer.h"
#include "JackParser.h"

const char* FILE_PATH = "F:\\Coursera\\NandToTetris\\nand2tetris\\projects\\temp\\Main.jack";

void tokenizer_test()
{
	// input can be .jack file					out - fileName.jack
// folder which contain set of .jack file
	JackTokenizer tokenizer(FILE_PATH);

	while (tokenizer.hasMoreTokens())
	{
		tokenizer.advance();
		if (tokenizer.tokenType() == TokenTypes::KEYWORD)
		{
			std::cout << "Keyword     : " << tokenizer.indentifier() << std::endl;
		}
		else if (tokenizer.tokenType() == TokenTypes::IDENTIFIER)
		{
			std::cout << "Identifiers : " << tokenizer.indentifier() << std::endl;
		}
		else if (tokenizer.tokenType() == TokenTypes::SYMBOL)
		{
			std::cout << "Symbols     : " << tokenizer.symbol() << std::endl;
		}
	}
}

int main()
{
	JackParser parser(FILE_PATH);
	parser.compileClass();
}

