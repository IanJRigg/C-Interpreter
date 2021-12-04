#include "catch2/catch.hpp"
#include "../src/interpreter.h"
#include "constants.h"

using namespace Interpreter;

TEST_CASE("Test allowed file types")
{
    REQUIRE(Interpret(Fixtures::BASIC_C) == Response_Code::Success);
}

TEST_CASE("Test forbidden file types")
{
	REQUIRE(Interpret(Fixtures::BASIC_CPP) == Response_Code::Invalid_File_Type);
}

TEST_CASE("Test files that don't exist!")
{
	REQUIRE(Interpret(Fixtures::DOES_NOT_EXIST) == Response_Code::File_Read_Error);
}