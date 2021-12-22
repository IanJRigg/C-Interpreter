#include "virtual-machine.h"

namespace
{
constexpr auto MEMORY_SIZE = 256UL * 1024UL; // Random numbers. Will replace eventually
};

class Virtual_Machine::Impl
{
public:
    Impl() :
        text(MEMORY_SIZE, 0L),
        stack(MEMORY_SIZE, 0L),
        data(MEMORY_SIZE, 0),
        program_counter(0L),
        base_pointer(0L),
        stack_pointer(0L),
        register_A(0L)
    {

    }

    virtual ~Impl() = default;

    Impl(const Impl& other)
    {

    }

private:
	std::vector<int32_t> text;
    std::vector<int32_t> stack;
    std::vector<int8_t> data;

    int program_counter;
    int base_pointer;
    int stack_pointer;
    int register_A;
};

/**********************************************************************************************//**
 * \brief 
 *************************************************************************************************/
Virtual_Machine::Virtual_Machine() :
    state(std::make_unique<Impl>())
{

}

// Default destructor is here to enable the PIMPL pattern in the class
Virtual_Machine::~Virtual_Machine() = default;

/**********************************************************************************************//**
 * \brief 
 *************************************************************************************************/
void Virtual_Machine::load(const std::vector<int32_t>& program)
{

}

/**********************************************************************************************//**
 * \brief 
 *************************************************************************************************/
void Virtual_Machine::execute()
{

}
