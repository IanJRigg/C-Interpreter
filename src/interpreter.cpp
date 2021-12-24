#include "interpreter.h"
#include "virtual-machine.h"

#include <iostream>
#include <fstream>
#include <sstream>

namespace Interpreter
{

/**********************************************************************************************//**
 * \brief Function to process the provided file_contents
 * \param file_contents The text for the provided file
 *************************************************************************************************/
std::vector<uint8_t> evaluate_tokens(const std::string& file_contents)
{
    std::vector<uint8_t> result;

    for(const auto character : file_contents)
    {
        // Do nothing here for now
    }

    return result;
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

    const auto file_contents = buffer.str();
    const auto program = evaluate_tokens(file_contents);

    Virtual_Machine vm;
    vm.load(program);
    vm.execute();

    return Response_Code::Success;
}

} // Namespace Interpreter
