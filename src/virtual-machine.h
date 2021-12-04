#ifndef VIRTUAL_MACHINE_H
#define VIRTUAL_MACHINE_H

#include <cstdint>
#include <vector>

namespace
{
class Virtual_Machine
{
public:
    Virtual_Machine();

    void load_program(const std::vector<int32_t>& program);
    void execute();

private:

private:
    std::vector<int32_t> text;
    std::vector<int32_t> stack;
    std::vector<int8_t> data;

    int program_counter;
    int base_pointer;
    int stack_pointer;
    int register_A;
};
};

#endif
