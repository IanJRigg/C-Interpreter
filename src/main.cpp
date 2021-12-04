#include "interpreter.h"

#include <iostream>

namespace
{

/**********************************************************************************************//**
 * \brief Basic function to print an error statement for a given reponse from the interpreter
 * \param response Argument count
 *************************************************************************************************/
void demux_response_code(const Interpreter::Response_Code response)
{
    switch(response)
    {
        case Interpreter::Response_Code::Success:
            std::cout << "Success!" << std::endl;
            break;

        case Interpreter::Response_Code::Invalid_File_Type:
            std::cerr << "Invalid file type provided to interpreter" << std::endl;
            break;

        case Interpreter::Response_Code::File_Read_Error:
            std::cerr << "Cannot access file provided to interpreter" << std::endl;
            break;

        default:
            break;
    }
}

};

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
    demux_response_code(Interpreter::Interpret(file_path));

	return 0;
}