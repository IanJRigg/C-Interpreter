#include "interpreter.h"

#include <iostream>

/**********************************************************************************************//**
 * \brief Main entry point to the interpreter
 * \param argc Argument count
 * \param argv Argument vector
 *************************************************************************************************/
int main(int argc, char** argv)
{
	if(argc != 2)
	{
        std::cerr << "Please provide a file name." << std::endl;
        return 0;
	}

	const std::string file_path(argv[1]);
    Interpreter::Interpret(file_path);

	return 0;
}