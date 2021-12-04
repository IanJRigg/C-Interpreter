#include <iostream>
#include <fstream>
#include <sstream>

/**********************************************************************************************//**
 * \brief Main entry point to the interpreter
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

	std::string file_path(argv[1]);
	if(file_path.back() != 'c')
	{
		std::cerr << "Please provide a .c file." << std::endl;
		return 0;
	}

    std::ifstream stream(file_path);
    std::stringstream buffer;
    buffer << stream.rdbuf();

    const auto program = buffer.str();
    evaluate_tokens(program);

	return 0;
}