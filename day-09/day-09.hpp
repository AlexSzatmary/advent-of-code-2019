#ifndef DAY_09_HPP
#define DAY_09_HPP
#include <algorithm>
#include <fstream>
#include <iostream>
#include <list>
#include <vector>

std::vector<long> my_parse(std::ifstream &inf);

class Computer{
    public:
    std::vector<long> program;
    long i{0}; // memory index
    long relative_base{0};
    std::list<long> input_buffer;
    std::list<long> output_buffer;
    bool halt;
    Computer(std::vector<long> program)
        : program{program}
        {
            i = 0;
            halt = false;
        }
    long my_at(long address);
    void my_assign(long address, long mode, long value);
    long access(long address, long mode);
    void interpret();
    void print_output_buffer();
};

#endif