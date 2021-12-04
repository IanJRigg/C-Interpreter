#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <string>
#include <cstdint>

namespace Interpreter
{
	enum class Response_Code : int32_t
	{
        Success = 0,
        Invalid_File_Type = -1,
        File_Read_Error = -2
	};

	Response_Code Interpret(const std::string& file_path);
};

#endif
