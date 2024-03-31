#include "cxxopts.hpp"
#include <fstream>
#include <iostream>
#include <numeric>
#include <regex>
#include <string>

using namespace std;

vector<int> my_parse(ifstream &inf){
    string line;
    getline(inf, line);
    regex re("-?\\d+");
    vector<int> program;
    transform(sregex_iterator(begin(line), end(line), re),
        sregex_iterator(),
        back_inserter(program),
        [](smatch m){return stoi(m.str());});
    return program;
}

int access(vector<int>& program, int address, bool mode){
    if (mode){
        return program.at(address);
    } else return program.at(program.at(address));
}

int interpret(vector<int> p, vector<int> input_buffer){
    // input p is the program and is just called p because it is used so much
    const int op_size_max = 4;
    int op_size{0};
    bool modes[op_size_max - 1];
    auto input_iter = begin(input_buffer);
    int input{0};
    int output{0};
    for (int i{0}; i < p.size() && p.at(i) != 99; i += op_size){
        int op{p.at(i) % 100};
        switch (op){
            case 1: // add
            case 2: // mult
            case 7: // <
            case 8: // =
                op_size = 4;
                break;

            case 3: // input
            case 4: // output
                op_size = 2;
                break;
            case 5: // jump if true
            case 6: // jump if false
                op_size = 3;
                break;
        }

        int parameter_modes_prefix{p.at(i) / 100};
        for (int j{0}; j < op_size - 1; ++j){
            modes[j] = parameter_modes_prefix % 10 == 1;
            parameter_modes_prefix /= 10;
        }
        switch (op){
            case 1:
                p.at(p.at(i + 3)) = access(p, i + 1, modes[0]) +
                access(p, i + 2, modes[1]);
                break;
            case 2:
                p.at(p.at(i + 3)) = access(p, i + 1, modes[0]) *
                access(p, i + 2, modes[1]);
                break;
            case 3:
                if (input_iter != end(input_buffer)) {
                    input = *input_iter;
                    ++input_iter;
                } else {
                    cin >> input;
                }
                p.at(p.at(i + 1)) = input;
                break;
            case 4:
                // cout << access(p, i + 1, modes[0]) << endl;
                output = access(p, i + 1, modes[0]);
                break;
            case 5:
                if (access(p, i + 1, modes[0]) != 0){
                    i = access(p, i + 2, modes[1]);
                    op_size = 0;
                }
                break;
            case 6:
                if (access(p, i + 1, modes[0]) == 0){
                    i = access(p, i + 2, modes[1]);
                    op_size = 0;
                }
                break;
            case 7:
                p.at(p.at(i + 3)) = (access(p, i + 1, modes[0]) <
                    access(p, i + 2, modes[1]) ? 1 : 0);
                break;
            case 8:
                p.at(p.at(i + 3)) = (access(p, i + 1, modes[0]) ==
                    access(p, i + 2, modes[1]) ? 1 : 0);
                break;
        }
    }
    return output;
}

int main(int argv, char **argc){
    cxxopts::Options options("test", "A brief description");
    options.add_options()
        ("0", "Rough tests", cxxopts::value<bool>())
        ("1", "Solve part 1", cxxopts::value<bool>())
        ("2", "Solve part 2", cxxopts::value<bool>())
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
        vector<int> program{my_parse(inf)};
        vector<int> phase_setting{0, 1, 2, 3, 4};
        vector<int> max_phase_setting{phase_setting};
        int max_signal{0};
        while (next_permutation(begin(phase_setting), end(phase_setting))){
            int signal{0}; // value of the signal between amps
            for (int amp_index{0}; amp_index < 5; ++amp_index){
                vector<int> input_buffer{phase_setting.at(amp_index), signal};
                signal = interpret(program, input_buffer);
            }
            if (signal > max_signal){
                max_signal = signal;
                max_phase_setting = phase_setting;
            }
        }
        for (auto phase: max_phase_setting){
            cout << phase;
        }
        cout << endl;
        cout << max_signal << endl;
    }
    // if (result.count("2")) {
    // }
    return 0;
}