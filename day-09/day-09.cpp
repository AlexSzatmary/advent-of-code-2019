#include <cxxopts.hpp>
#include <list>
#include <fstream>
#include <iostream>
#include <numeric>
#include <regex>
#include <string>
#include "day-09.hpp"
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

long Computer::my_at(long address){
    if (address >= program.size())
        program.resize(address * 2);
    return program[address];
}

void Computer::my_assign(long address, long mode, long value){
    // the output address is always found via two ats; this chases the first
    address = my_at(address);

    if (mode == 2) address += relative_base;
    if (address >= program.size())
        program.resize(address * 2);
    program[address] = value;
    return;
}

long Computer::access(long address, long mode){
    if (mode == 1){
        return my_at(address);
    } else if (mode == 2) {
        return my_at(my_at(address) + relative_base);
    } else return my_at(my_at(address));
}

void Computer::interpret(){
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
                    cout << "This should be unreachable. In case 3." << endl;
                    return;
                    // This branch was previously used to pause execution but the
                    // pause got moved to output and that seems to work better.
                }
                my_assign(i + 1, modes[0], input);
                break;
            case 4: // output
                output = access(i + 1, modes[0]);
                output_buffer.push_back(output);

                // the following could be uncommented for an interactive mode as needed
                // for day 7
                // i += op_size; // Before returning, must advance i.
                // return; // Return so other things can run
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

void Computer::print_output_buffer(){
    auto x{output_buffer.begin()};
    if (x == output_buffer.end()) return;
    cout << *x;
    ++x;
    for (; x != output_buffer.end(); ++x){
        cout << ", " << *x;
    }
    cout << endl;
}

int main(int argv, char **argc){
    cxxopts::Options options("test", "A brief description");
    options.add_options()
        ("0", "Rough tests", cxxopts::value<bool>())
        ("1", "Solve part 1", cxxopts::value<bool>())
        ("2", "Solve part 2", cxxopts::value<bool>())
        ("day-02", "Run all day 2 programs", cxxopts::value<bool>())
        ("day-05", "Run all day 5 programs", cxxopts::value<bool>())
        ("h,help", "Print usage");
    auto result = options.parse(argv, argc);

   if (result.count("help"))
    {
      std::cout << options.help() << std::endl;
      exit(0);
    }
    if (result.count("0")) {
    }
    if (result.count("1")) {
        ifstream inf{result.unmatched()[0]};
        vector<long> program{my_parse(inf)};
        Computer c{program};
        c.input_buffer.push_back(1);
        c.interpret();
        c.print_output_buffer();
    }
    if (result.count("2")) {
        ifstream inf{result.unmatched()[0]};
        vector<long> program{my_parse(inf)};
        Computer c{program};
        c.input_buffer.push_back(2);
        c.interpret();
        c.print_output_buffer();
    }
    if (result.count("day-02")) run_day_2();
    if (result.count("day-05")) run_day_5();
    return 0;
}
