#ifndef VIRTUAL_MACHINE_H
#define VIRTUAL_MACHINE_H

#include <cstdint>
#include <vector>

class Virtual_Machine
{
public:
    Virtual_Machine();

    virtual ~Virtual_Machine() = default;

    void load(const std::vector<uint8_t>& program);
    void execute();

private:
    uint8_t  read_byte_from_memory(uint32_t address) const;
    uint32_t read_word_from_memory(uint32_t address) const;

    uint8_t  write_byte_to_memory(uint32_t address, uint8_t byte);
    uint32_t write_word_to_memory(uint32_t address, uint32_t word);

    void demux_instruction(const uint8_t operation);

    void handle_IMM();
    void handle_LC();
    void handle_LI();
    void handle_SC();
    void handle_SI();
    void handle_PUSH();
    void handle_JMP();
    void handle_JZ();
    void handle_JNZ();
    void handle_CALL();
    void handle_ENT();
    void handle_ADJ();
    void handle_LEV();
    void handle_LEA();

    // Arithmetic instructions
    void handle_OR();
    void handle_XOR();
    void handle_AND();
    void handle_EQ();
    void handle_NE();
    void handle_LT();
    void handle_GT();
    void handle_LE();
    void handle_GE();
    void handle_SHL();
    void handle_SHR();
    void handle_ADD();
    void handle_SUB();
    void handle_MUL();
    void handle_DIV();
    void handle_MOD();

private:
    // All of these should be std::arrays, but that would require exposing the
    // memory sizes. 
    std::vector<uint8_t> text;
    std::vector<uint8_t> stack;
    std::vector<char> data;

    uint32_t program_counter;
    uint32_t base_pointer;
    uint32_t stack_pointer;
    uint32_t ax;
};

#endif
