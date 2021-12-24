#ifndef VIRTUAL_MACHINE_H
#define VIRTUAL_MACHINE_H

#include <cstdint>
#include <vector>

class Virtual_Machine
{
public:
    Virtual_Machine();

    virtual ~Virtual_Machine() = default;

    void load(const std::vector<int32_t>& program);
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

private:
    std::vector<uint8_t> text;
    std::vector<uint8_t> stack;
    std::vector<char> data;

    uint32_t program_counter;
    uint32_t base_pointer;
    uint32_t stack_pointer;
    uint32_t ax;
};

#endif
