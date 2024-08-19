#include <cxxopts.hpp>
#include <list>
#include <fstream>
#include <iostream>
#include <numeric>
#include <regex>
#include <string>
#include "intcode.hpp"
#include "run_previous_days.hpp"

using namespace std;

void run_day_2();
void run_day_5();

vector<long> my_parse(ifstream &inf){
    string line;
    getline(inf, line);
    regex re("-?\\d+");
    vector<long> program;
    transform(sregex_iterator(begin(line), end(line), re),
        sregex_iterator(),
        back_inserter(program),
        [](smatch m){return stol(m.str());});
    return program;
}

long IntcodeComputer::my_at(long address){
    if (address >= program.size())
        program.resize(address * 2);
    return program[address];
}

void IntcodeComputer::my_assign(long address, long mode, long value){
    // the output address is always found via two ats; this chases the first
    address = my_at(address);

    if (mode == 2) address += relative_base;
    if (address >= program.size())
        program.resize(address * 2);
    program[address] = value;
    return;
}

long IntcodeComputer::access(long address, long mode){
    if (mode == 1){
        return my_at(address);
    } else if (mode == 2) {
        return my_at(my_at(address) + relative_base);
    } else return my_at(my_at(address));
}

void IntcodeComputer::interpret(){
    vector<long>& p{program};
    // p is the program and is aliased as p because it is used so much
    const long op_size_max = 4;
    long op_size{0};
    long modes[op_size_max - 1];
    auto input_iter = begin(input_buffer);
    long input{0};
    long output{0};

    for (; i < p.size() && p.at(i) != 99; i += op_size){
        long op{p.at(i) % 100};
        switch (op){
            case 1: // +
            case 2: // *
            case 7: // <
            case 8: // ==
                op_size = 4;
                break;

            case 3: // input
            case 4: // output
            case 9: // adjust relative base
                op_size = 2;
                break;
            case 5: // jump if true
            case 6: // jump if false
                op_size = 3;
                break;
        }

        long parameter_modes_prefix{p.at(i) / 100};
        for (long j{0}; j < op_size - 1; ++j){
            modes[j] = parameter_modes_prefix % 10;
            parameter_modes_prefix /= 10;
        }

        switch (op){
            case 1: // +
                my_assign(i + 3, modes[2], access(i + 1, modes[0]) +
                access(i + 2, modes[1]));
                break;
            case 2: // *
                my_assign(i + 3, modes[2], access(i + 1, modes[0]) *
                access(i + 2, modes[1]));
                break;
            case 3: // input
                if (!input_buffer.empty()) {
                    input = input_buffer.front();
                    input_buffer.pop_front();
                } else {
                    return; // Suspend execution and await input
                    // If the input buffer is empty, there is no state to capture
                    // when suspending.
                }
                my_assign(i + 1, modes[0], input);
                break;
            case 4: // output
                output = access(i + 1, modes[0]);
                output_buffer.push_back(output);
                break;
            case 5: // jmp if true
                if (access(i + 1, modes[0]) != 0){
                    i = access(i + 2, modes[1]);
                    op_size = 0;
                }
                break;
            case 6: // jmp if false
                if (access(i + 1, modes[0]) == 0){
                    i = access(i + 2, modes[1]);
                    op_size = 0;
                }
                break;
            case 7: // <
                my_assign(i + 3, modes[2], (access(i + 1, modes[0]) <
                    access(i + 2, modes[1]) ? 1 : 0));
                break;
            case 8: // ==
                my_assign(i + 3, modes[2], (access(i + 1, modes[0]) ==
                    access(i + 2, modes[1]) ? 1 : 0));
                break;
            case 9: // adjust relative_base
                relative_base += access(i + 1, modes[0]);
        }
    }
    if (p.at(i) == 99) halt = true;
    return;
}

void IntcodeComputer::print_output_buffer(){
    auto x{output_buffer.begin()};
    if (x == output_buffer.end()) return;
    cout << *x;
    ++x;
    for (; x != output_buffer.end(); ++x){
        cout << ", " << *x;
    }
    cout << endl;
}

void IntcodeComputer::print_output_buffer_ascii(){
    for (char c: output_buffer) cout << static_cast<char>(c);
}
