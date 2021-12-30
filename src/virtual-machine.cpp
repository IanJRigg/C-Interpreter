#include "virtual-machine.h"
#include <iostream>
#include <exception>

namespace
{
constexpr auto WORD_SIZE = 4UL;

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

    // Function foundation instructions
    PUSH,
    JMP,
    JZ,
    JNZ,

    // 
    CALL,
    ENT,
    ADJ,
    LEV,

    // Replacements for the MOV instruction
    LI,
    LC,
    SI,
    SC,

    // Arithmetic Operations
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

    // Shortcuts for system calls
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

struct Word_Bytes
{
    uint8_t a;
    uint8_t b;
    uint8_t c;
    uint8_t d;
};

/**********************************************************************************************//**
 * \brief Converts four given bytes to a 32 bit word in big endian format
 * \note This parameter structure forces the user to retrieve the four bytes via the at() function.
 *       This supposed to help prevent overruns and other bugs memory bugs from the code
 * \param a
 * \param b
 * \param c
 * \param d
 * \returns The four bytes arranged in big endian format
 *************************************************************************************************/
uint32_t 
bytes_to_word(const uint8_t a, const uint8_t b, const uint8_t c, const uint8_t d)
{
    return ((a << 24UL) & 0xFF000000UL) +
           ((b << 16UL) & 0x00FF0000UL) +
           ((c << 8UL)  & 0x0000FF00UL) + 
           ((d << 0UL)  & 0x000000FFUL);
}

/**********************************************************************************************//**
 * \brief Converts a 32 bit word into a utility container which holds 4 bytes
 * \param a
 * \param b
 * \param c
 * \param d
 * \returns A utility structure containing the four bytes interpreted as big endian
 *************************************************************************************************/
Word_Bytes word_to_bytes(const uint32_t word)
{
    return {
        static_cast<uint8_t>((word >> 24UL) & 0xFFUL),
        static_cast<uint8_t>((word >> 16UL) & 0xFFUL),
        static_cast<uint8_t>((word >>  8UL) & 0xFFUL),
        static_cast<uint8_t>((word >>  0UL) & 0xFFUL)
    };
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
    uint8_t a{0U};
    uint8_t b{0U};
    uint8_t c{0U};
    uint8_t d{0U};

    // TODO: Candidate for refactor. a/b/c/d assignment happens three times.
    //       Mild complication from the typing on text vs stack and data. That
    //       will need to be resolved at some point anyway.
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
        data.at(truncated_address) = byte;
        return data.at(truncated_address);
    }
    else if((address >= TEXT_START_ADDRESS) && (address <= TEXT_END_ADDRESS))
    {
        text.at(truncated_address) = byte;
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
    const auto [a, b, c, d] = word_to_bytes(word);

    if((address >= STACK_START_ADDRESS) && (address <= STACK_END_ADDRESS))
    {
        stack.at(truncated_address + 0UL) = a;
        stack.at(truncated_address + 1UL) = b;
        stack.at(truncated_address + 2UL) = c;
        stack.at(truncated_address + 3UL) = d;
    }
    else if((address >= DATA_START_ADDRESS) && (address <= DATA_END_ADDRESS))
    {
        data.at(truncated_address + 0UL) = a;
        data.at(truncated_address + 1UL) = b;
        data.at(truncated_address + 2UL) = c;
        data.at(truncated_address + 3UL) = d;
    }
    else if((address >= TEXT_START_ADDRESS) && (address <= TEXT_END_ADDRESS))
    {
        text.at(truncated_address + 0UL) = a;
        text.at(truncated_address + 1UL) = b;
        text.at(truncated_address + 2UL) = c;
        text.at(truncated_address + 3UL) = d;
    }
    else
    {
        // TODO: Make a custom exception for this
        throw std::runtime_error("Attempt to use invalid address.");
    }

    return word;
}

/**********************************************************************************************//**
 * \brief Loads the program into the text region of the virtual machine's memory
 * \param
 *************************************************************************************************/
void Virtual_Machine::load(const std::vector<uint8_t>& program)
{
    if(program.size() > text.size())
    {
        // Consider throwing a new exception here
        return;
    }

    // I don't like this, but I haven't found a great way to keep the destination size constant
    auto i = 0;
    for(const auto& entry : program)
    {
        text.at(i) = entry;
        ++i;
    }
}

/**********************************************************************************************//**
 * \brief Using the current state of the virtual machine, execute until unable to continue, or if
 *        instructed to stop.
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
 * \brief Maps handler functions to instructions. This should be optimized to a jump table by the
 *        compiler.
 * \param operation The incoming opcode that will be de-multiplexed
 *************************************************************************************************/
void Virtual_Machine::demux_instruction(const uint8_t operation)
{
    switch(operation)
    {
        case Instructions::IMM:  handle_IMM();  break;
        case Instructions::LC:   handle_LC();   break;
        case Instructions::LI:   handle_LI();   break;
        case Instructions::SC:   handle_SC();   break;
        case Instructions::SI:   handle_SI();   break;
        case Instructions::PUSH: handle_PUSH(); break;
        case Instructions::JMP:  handle_JMP();  break;
        case Instructions::JZ:   handle_JZ();   break;
        case Instructions::JNZ:  handle_JNZ();  break;
        case Instructions::CALL: handle_CALL(); break;
        case Instructions::ENT:  handle_ENT();  break;
        case Instructions::ADJ:  handle_ADJ();  break;
        case Instructions::LEV:  handle_LEV();  break;
        case Instructions::LEA:  handle_LEA();  break;
        case Instructions::OR:   handle_OR();   break;
        case Instructions::XOR:  handle_XOR();  break;
        case Instructions::AND:  handle_AND();  break;
        case Instructions::EQ:   handle_EQ();   break;
        case Instructions::NE:   handle_NE();   break;
        case Instructions::LT:   handle_LT();   break;
        case Instructions::GT:   handle_GT();   break;
        case Instructions::LE:   handle_LE();   break;
        case Instructions::GE:   handle_GE();   break;
        case Instructions::SHL:  handle_SHL();  break;
        case Instructions::SHR:  handle_SHR();  break;
        case Instructions::ADD:  handle_ADD();  break;
        case Instructions::SUB:  handle_SUB();  break;
        case Instructions::MUL:  handle_MUL();  break;
        case Instructions::DIV:  handle_DIV();  break;
        case Instructions::MOD:  handle_MOD();  break;

        default: break;
    }
}

/**********************************************************************************************//**
 * \brief Load the current value at the program counter into the ax register. Increment the
 *        program counter
 *************************************************************************************************/
void Virtual_Machine::handle_IMM()
{
    ax = text.at(program_counter);
    ++program_counter;
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

/**********************************************************************************************//**
 * \brief Place the ax register onto the stack, and advance the stack pointer
 * \note One is added to the word size because the byte at the stack pointer address has not been
 *       written to yet.
 *************************************************************************************************/
void Virtual_Machine::handle_PUSH()
{
    stack_pointer -= (WORD_SIZE + 1UL);
    write_word_to_memory(stack_pointer, ax);
    
}

/**********************************************************************************************//**
 * \brief Reads the next word from memory and replaces the program counter with that address
 *************************************************************************************************/
void Virtual_Machine::handle_JMP()
{
    program_counter = read_word_from_memory(program_counter);
}

/**********************************************************************************************//**
 * \brief Perform the jump operation if the ax register contains 0, otherwise advance past this
 *        instruction + argument
 *************************************************************************************************/
void Virtual_Machine::handle_JZ()
{
    if(ax == 0)
    {
        handle_JMP();
    }
    else
    {
        ++program_counter;
    }
}

/**********************************************************************************************//**
 * \brief Perform the jump operation if the ax register doesn't contain 0, otherwise advance past 
 *        this instruction + argument
 *************************************************************************************************/
void Virtual_Machine::handle_JNZ()
{
    if(ax != 0)
    {
        ++program_counter;
    }
    else
    {
        handle_JMP();
    }
}

/**********************************************************************************************//**
 * \brief Performs the Call operation. Stores the current program counter on the stack and then 
 *        performs a JMP
 *************************************************************************************************/
void Virtual_Machine::handle_CALL()
{
    stack_pointer -= (WORD_SIZE + 1UL);
    write_word_to_memory(stack_pointer, program_counter);

    handle_JMP();
}

/**********************************************************************************************//**
 * \brief Performs the Enter operation. Creates a new frame on the stack. The stack frame consists
 *        of the stack pointer and N words, where N is the number of arguments for the function.
 *************************************************************************************************/
void Virtual_Machine::handle_ENT()
{
    stack_pointer -= (WORD_SIZE + 1UL);
    write_word_to_memory(stack_pointer, base_pointer);

    base_pointer = stack_pointer;

    stack_pointer -= read_word_from_memory(program_counter);
    ++program_counter;
}

/**********************************************************************************************//**
 * \brief Performs the Adjust operation. This operation removes arguments from the stack frame.
 *************************************************************************************************/
void Virtual_Machine::handle_ADJ()
{
    stack_pointer = stack_pointer + read_word_from_memory(program_counter);
    ++program_counter;
}

/**********************************************************************************************//**
 * \brief Perform the Leave operation. This removes the contents of the top-most stack frame and
 *        places them back to where they're meant to be.
 *************************************************************************************************/
void Virtual_Machine::handle_LEV()
{
    stack_pointer = base_pointer;

    base_pointer = read_word_from_memory(stack_pointer);
    stack_pointer += WORD_SIZE;

    program_counter = read_word_from_memory(stack_pointer);
    stack_pointer += WORD_SIZE;
}

/**********************************************************************************************//**
 * \brief Operation meant to retrieve a function's arguments from within the called function.
 *************************************************************************************************/
void Virtual_Machine::handle_LEA()
{
    uint8_t offset = read_byte_from_memory(program_counter);
    ++program_counter;
    
    ax = read_word_from_memory(base_pointer + (offset * WORD_SIZE));
}



/**********************************************************************************************//**
 * \brief Perform logical OR. Logical OR's the top of the stack with the ax register
 *************************************************************************************************/
void Virtual_Machine::handle_OR()
{
    uint32_t left_side = read_word_from_memory(stack_pointer);
    stack_pointer += WORD_SIZE;

    ax = (left_side | ax);
}

/**********************************************************************************************//**
 * \brief Perform logical XOR. Logical XOR's the top of the stack with the ax register
 *************************************************************************************************/
void Virtual_Machine::handle_XOR()
{
    uint32_t left_side = read_word_from_memory(stack_pointer);
    stack_pointer += WORD_SIZE;

    ax = (left_side ^ ax);
}

/**********************************************************************************************//**
 * \brief Perform logical AND. Logical AND's the top of the stack with the ax register
 *************************************************************************************************/
void Virtual_Machine::handle_AND()
{
    uint32_t left_side = read_word_from_memory(stack_pointer);
    stack_pointer += WORD_SIZE;

    ax = (left_side & ax);
}

/**********************************************************************************************//**
 * \brief Perform an equals comparison. Compares the top of the stack with the ax register
 *************************************************************************************************/
void Virtual_Machine::handle_EQ()
{
    uint32_t left_side = read_word_from_memory(stack_pointer);
    stack_pointer += WORD_SIZE;

    ax = (left_side == ax);
}

/**********************************************************************************************//**
 * \brief Perform the not equal comparison. Compares the top of the stack with the ax register
 *************************************************************************************************/
void Virtual_Machine::handle_NE()
{
    uint32_t left_side = read_word_from_memory(stack_pointer);
    stack_pointer += WORD_SIZE;

    ax = (left_side != ax);
}

/**********************************************************************************************//**
 * \brief Performs the less than comparison. Compares the top of the stack with the ax register
 *************************************************************************************************/
void Virtual_Machine::handle_LT()
{
    uint32_t left_side = read_word_from_memory(stack_pointer);
    stack_pointer += WORD_SIZE;

    ax =  (left_side < ax);
}

/**********************************************************************************************//**
 * \brief Performs the greater than comparison. Compares the top of the stack with the ax register
 *************************************************************************************************/
void Virtual_Machine::handle_GT()
{
    uint32_t left_side = read_word_from_memory(stack_pointer);
    stack_pointer += WORD_SIZE;

    ax =  (left_side > ax);
}

/**********************************************************************************************//**
 * \brief Performs the less than or equal comparison. Compares the top of the stack with the ax
 *        register
 *************************************************************************************************/
void Virtual_Machine::handle_LE()
{
    uint32_t left_side = read_word_from_memory(stack_pointer);
    stack_pointer += WORD_SIZE;

    ax =  (left_side <= ax);
}

/**********************************************************************************************//**
 * \brief Performs the greater than or equal comparison. Compares the top of the stack with the ax 
 *        register
 *************************************************************************************************/
void Virtual_Machine::handle_GE()
{
    uint32_t left_side = read_word_from_memory(stack_pointer);
    stack_pointer += WORD_SIZE;

    ax =  (left_side >= ax);
}

/**********************************************************************************************//**
 * \brief Performs the shift left operation. Shifts the top of the stack ax bits to the left
 *************************************************************************************************/
void Virtual_Machine::handle_SHL()
{
    uint32_t left_side = read_word_from_memory(stack_pointer);
    stack_pointer += WORD_SIZE;

    ax =  (left_side << ax);
}

/**********************************************************************************************//**
 * \brief Performs the shift right operation. Shifts the top of the stack ax bits to the right
 *************************************************************************************************/
void Virtual_Machine::handle_SHR()
{
    uint32_t left_side = read_word_from_memory(stack_pointer);
    stack_pointer += WORD_SIZE;

    ax =  (left_side >> ax);
}

/**********************************************************************************************//**
 * \brief Perform the add operation. Adds the top of the stack to the ax register
 *************************************************************************************************/
void Virtual_Machine::handle_ADD()
{
    uint32_t left_side = read_word_from_memory(stack_pointer);
    stack_pointer += WORD_SIZE;

    ax =  (left_side + ax);
}

/**********************************************************************************************//**
 * \brief Perform the subtract operation. Subtracts the top of the stack from the ax register
 *************************************************************************************************/
void Virtual_Machine::handle_SUB()
{
    uint32_t left_side = read_word_from_memory(stack_pointer);
    stack_pointer += WORD_SIZE;

    ax =  (left_side - ax);
}

/**********************************************************************************************//**
 * \brief Perform the multiply operation. Multiplies the top of the stack by the ax register
 *************************************************************************************************/
void Virtual_Machine::handle_MUL()
{
    uint32_t left_side = read_word_from_memory(stack_pointer);
    stack_pointer += WORD_SIZE;

    ax =  (left_side * ax);
}

/**********************************************************************************************//**
 * \brief Perform the divide operation. Divides the top of the stack by the ax register
 *************************************************************************************************/
void Virtual_Machine::handle_DIV()
{
    uint32_t left_side = read_word_from_memory(stack_pointer);
    stack_pointer += WORD_SIZE;

    ax =  (left_side / ax);
}

/**********************************************************************************************//**
 * \brief Perform the modulo operation. Divides the top of the stack by the ax register, and stores
 *        the remainder in the ax register.
 *************************************************************************************************/
void Virtual_Machine::handle_MOD()
{
    uint32_t left_side = read_word_from_memory(stack_pointer);
    stack_pointer += WORD_SIZE;

    ax =  (left_side % ax);
}

