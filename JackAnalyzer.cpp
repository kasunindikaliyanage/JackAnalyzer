#include <iostream>
#include "JackTokenizer.h"


int main()
{
	// input can be .jack file					out - fileName.jack
	// folder which contain set of .jack file
	JackTokenizer tokenizer("F:\\Coursera\\NandToTetris\\nand2tetris\\projects\\11\\Square\\Main.jack");

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

