#include "virtual-machine.h"
#include <iostream>
#include <exception>

namespace
{
// Random numbers called out by the instructions
constexpr auto STACK_SIZE = 256UL * 1024UL;
constexpr auto STACK_START_ADDRESS = 0UL;
constexpr auto STACK_END_ADDRESS = STACK_SIZE - 1UL;

// Random numbers called out by the instructions
constexpr auto DATA_SIZE = 256UL * 1024UL;
constexpr auto DATA_START_ADDRESS = STACK_SIZE;
constexpr auto DATA_END_ADDRESS = (STACK_SIZE + DATA_SIZE) - 1UL;

// Random numbers called out by the instructions
constexpr auto TEXT_SIZE = 256UL * 1024UL;
constexpr auto TEXT_START_ADDRESS = (STACK_SIZE + DATA_SIZE);
constexpr auto TEXT_END_ADDRESS = (STACK_SIZE + DATA_SIZE + TEXT_SIZE) - 1UL;

enum Instructions
{
    LEA = 0x00,
    IMM, // Replacement for the MOV instruction
    JMP,
    CALL,
    JZ,
    JNZ,
    ENT,
    ADJ,
    LEV,

    // Replacements for the MOV instruction
    LI,
    LC,
    SI,
    SC,

    PUSH,
    OR,
    XOR,
    AND,
    EQ,
    NE,
    LT,
    GT,
    LE,
    GE,
    SHL,
    SHR,
    ADD,
    SUB,
    MUL,
    DIV,
    MOD,
    OPEN,
    READ,
    CLOS,
    PRTF,
    MALC,
    MSET,
    MCMP,
    EXIT 
};

/**********************************************************************************************//**
 * \brief 
 * \param
 * \returns
 *************************************************************************************************/
uint32_t truncate_address(const uint32_t address)
{
    if((address >= STACK_START_ADDRESS) && (address <= STACK_END_ADDRESS))
    {
        return address - STACK_START_ADDRESS;
    }
    else if((address >= DATA_START_ADDRESS) && (address <= DATA_END_ADDRESS))
    {
        return address - DATA_START_ADDRESS;
    }
    else if((address >= TEXT_START_ADDRESS) && (address <= TEXT_END_ADDRESS))
    {
        return address - TEXT_START_ADDRESS;
    }
    else
    {
        // TODO: Make a custom exception for this
        throw std::runtime_error("Attempt to use invalid address.");
    }
}

/**********************************************************************************************//**
 * \brief Converts four given bytes to a 32 bit word in little endian format
 * \note This parameter structure forces the user to retrieve the four bytes via the at() function.
 *       This supposed to help prevent overruns and other bugs memory bugs from the code
 * \param a
 * \param b
 * \param c
 * \param d
 * \returns The four bytes arranged in little endian format
 *************************************************************************************************/
uint32_t 
bytes_to_word(const uint8_t a, const uint8_t b, const uint8_t c, const uint8_t d)
{
    return ((a << 24UL) & 0xFF000000UL) +
           ((b << 16UL) & 0x00FF0000UL) +
           ((c << 8UL)  & 0x0000FF00UL) + 
           ((d << 0UL)  & 0x000000FFUL);
}

};

/**********************************************************************************************//**
 * \brief Constructor for the virtual machine
 *        base_pointer and stack_pointer will both point to the top of the stack and descend
 *        towards the bottom of the stack area.
 *************************************************************************************************/
Virtual_Machine::Virtual_Machine() :
    text(TEXT_SIZE, 0),
    stack(STACK_SIZE, 0),
    data(DATA_SIZE, 0),
    program_counter(0),
    base_pointer(STACK_SIZE - 1),
    stack_pointer(STACK_SIZE - 1),
    ax(0)
{

}

/**********************************************************************************************//**
 * \brief 
 * \param
 * \param
 * \returns
 *************************************************************************************************/
uint8_t Virtual_Machine::read_byte_from_memory(const uint32_t address) const
{
    const auto truncated_address = truncate_address(address);
    if((address >= STACK_START_ADDRESS) && (address <= STACK_END_ADDRESS))
    {
        return stack.at(truncated_address);
    }
    else if((address >= DATA_START_ADDRESS) && (address <= DATA_END_ADDRESS))
    {
        return data.at(truncated_address);
    }
    else if((address >= TEXT_START_ADDRESS) && (address <= TEXT_END_ADDRESS))
    {
        return text.at(truncated_address);
    }
    else
    {
        // TODO: Make a custom exception for this
        throw std::runtime_error("Attempt to use invalid address.");
    }
}

/**********************************************************************************************//**
 * \brief 
 * \param
 * \param
 * \returns
 *************************************************************************************************/
uint32_t Virtual_Machine::read_word_from_memory(const uint32_t address) const
{
    const auto truncated_address = truncate_address(address);
    uint8_t a = 0U;
    uint8_t b = 0U;
    uint8_t c = 0U;
    uint8_t d = 0U;

    // TODO: Candidate for refactor. a/b/c/d assignment happens three times.
    if((address >= STACK_START_ADDRESS) && (address <= STACK_END_ADDRESS))
    {
        a = stack.at(truncated_address + 0UL);
        b = stack.at(truncated_address + 1UL);
        c = stack.at(truncated_address + 2UL);
        d = stack.at(truncated_address + 3UL);
    }
    else if((address >= DATA_START_ADDRESS) && (address <= DATA_END_ADDRESS))
    {
        a = data.at(truncated_address + 0UL);
        b = data.at(truncated_address + 1UL);
        c = data.at(truncated_address + 2UL);
        d = data.at(truncated_address + 3UL);
    }
    else if((address >= TEXT_START_ADDRESS) && (address <= TEXT_END_ADDRESS))
    {
        a = text.at(truncated_address + 0UL);
        b = text.at(truncated_address + 1UL);
        c = text.at(truncated_address + 2UL);
        d = text.at(truncated_address + 3UL);
    }
    else
    {
        // TODO: Make a custom exception for this
        throw std::runtime_error("Attempt to use invalid address.");
    }

    return bytes_to_word(a, b, c, d);
}

/**********************************************************************************************//**
 * \brief 
 * \param
 * \param
 * \returns
 *************************************************************************************************/
uint8_t Virtual_Machine::write_byte_to_memory(const uint32_t address, const uint8_t byte)
{
    const auto truncated_address = truncate_address(address);
    if((address >= STACK_START_ADDRESS) && (address <= STACK_END_ADDRESS))
    {
        stack.at(truncated_address) = byte;
        return stack.at(truncated_address);
    }
    else if((address >= DATA_START_ADDRESS) && (address <= DATA_END_ADDRESS))
    {
        stack.at(truncated_address) = byte;
        return data.at(truncated_address);
    }
    else if((address >= TEXT_START_ADDRESS) && (address <= TEXT_END_ADDRESS))
    {
        stack.at(truncated_address) = byte;
        return text.at(truncated_address);
    }
    else
    {
        // TODO: Make a custom exception for this
        throw std::runtime_error("Attempt to use invalid address.");
    }
}

/**********************************************************************************************//**
 * \brief 
 * \param
 * \param
 * \returns
 *************************************************************************************************/
uint32_t Virtual_Machine::write_word_to_memory(const uint32_t address, const uint32_t word)
{
    const auto truncated_address = truncate_address(address);
    if((address >= STACK_START_ADDRESS) && (address <= STACK_END_ADDRESS))
    {
        
    }
    else if((address >= DATA_START_ADDRESS) && (address <= DATA_END_ADDRESS))
    {
        
    }
    else if((address >= TEXT_START_ADDRESS) && (address <= TEXT_END_ADDRESS))
    {
        
    }
    else
    {
        // TODO: Make a custom exception for this
        throw std::runtime_error("Attempt to use invalid address.");
    }
}

/**********************************************************************************************//**
 * \brief 
 * \param
 *************************************************************************************************/
void Virtual_Machine::load(const std::vector<int32_t>& program)
{

}

/**********************************************************************************************//**
 * \brief 
 *************************************************************************************************/
void Virtual_Machine::execute()
{
    uint8_t op{};
    while(true)
    {
        op = text.at(program_counter);
        program_counter += 1;

        try
        {
            demux_instruction(op);
        }
        catch(...)
        {
            std::cout << "Fatal error. Shutting down" << std::endl;
            break;
        }
        
    }
}

/**********************************************************************************************//**
 * \brief 
 * \param 
 *************************************************************************************************/
void Virtual_Machine::demux_instruction(const uint8_t operation)
{
    switch(operation)
    {
        case Instructions::IMM:
            handle_IMM();
            break;

        case Instructions::LC:
            handle_LC();
            break;

        case Instructions::LI:
            handle_LI();
            break;

        case Instructions::SC:
            handle_SC();
            break;

        case Instructions::SI:
            handle_SI();
            break;

        default:
            break;

    }
}

/**********************************************************************************************//**
 * \brief Load the current value at the program counter into the ax register. Increment the
 *        program counter
 *************************************************************************************************/
void Virtual_Machine::handle_IMM()
{
    ax = text.at(program_counter);
    program_counter += 1;
}

/**********************************************************************************************//**
 * \brief Treating the ax register as a memory address, retrieve the byte at that address and
 *        place it into the ax register
 *************************************************************************************************/
void Virtual_Machine::handle_LC()
{
    ax = read_byte_from_memory(ax);
}

/**********************************************************************************************//**
 * \brief Treating the ax register as a memory address, retrieve the 32 bit integer at that address
 *        and place it into the ax register
 *************************************************************************************************/
void Virtual_Machine::handle_LI()
{
    ax = read_word_from_memory(ax);
}

/**********************************************************************************************//**
 * \brief Treating the stack pointer as a memory address, store the byte in ax at the address
 *        in the stack pointer, while maintaining the value in ax
 *************************************************************************************************/
void Virtual_Machine::handle_SC()
{
    ax = write_byte_to_memory(stack_pointer, ax);
}

/**********************************************************************************************//**
 * \brief Treating the stack pointer as a memory address, store the 32 bit integer in ax in that
 *        address
 *************************************************************************************************/
void Virtual_Machine::handle_SI()
{
    ax = write_word_to_memory(stack_pointer, ax);
}

