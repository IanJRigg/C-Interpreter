#ifndef VIRTUAL_MACHINE_H
#define VIRTUAL_MACHINE_H

#include <cstdint>
#include <vector>
#include <memory>

class Virtual_Machine
{
public:
    Virtual_Machine();

    virtual ~Virtual_Machine();

    void load(const std::vector<int32_t>& program);
    void execute();

private:
	class Impl;
	std::unique_ptr<Impl> state;
};

#endif
