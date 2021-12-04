#include "interpreter.h"

#include <iostream>
#include <fstream>
#include <sstream>

namespace Interpreter
{

/**********************************************************************************************//**
 * \brief Function to process the provided program a
 * \param program The text for the provided file
 *************************************************************************************************/
void evaluate_tokens(const std::string& program)
{
    for(const auto character : program)
    {
        std::cout << character;
    }

    std::cout << std::endl;
}

/**********************************************************************************************//**
 * \brief Main entry point to the interpreter
 * \param file_path Path to the provided file
 *************************************************************************************************/
Response_Code Interpret(const std::string& file_path)
{
	if(file_path.back() != 'c')
	{
		return Response_Code::Invalid_File_Type;
	}

    std::ifstream stream(file_path);
    if(stream.fail())
    {
        return Response_Code::File_Read_Error;
    }

    std::stringstream buffer;
    buffer << stream.rdbuf();

    const auto program = buffer.str();
    evaluate_tokens(program);

    return Response_Code::Success;
}

} // Namespace Interpreter
