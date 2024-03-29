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

void interpret(vector<int> p){
    // input p is the program and is just called p because it is used so much
    const int op_size_max = 4;
    int op_size{0};
    bool modes[op_size_max - 1];
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
        int input{0};
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
                cin >> input;
                p.at(p.at(i + 1)) = input;
                break;
            case 4:
                cout << access(p, i + 1, modes[0]) << endl;
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
    return;
}

int main(int argv, char **argc){
    cxxopts::Options options("test", "A brief description");
    options.add_options()
        ("1", "Solve part 1", cxxopts::value<bool>())
        ("2", "Solve part 2", cxxopts::value<bool>())
        ("h,help", "Print usage");
    auto result = options.parse(argv, argc);

   if (result.count("help"))
    {
      std::cout << options.help() << std::endl;
      exit(0);
    }
    if (result.count("1")) { // The distinction between parts 1 and 2 is entering
    // 1 versus 5
        ifstream inf{result.unmatched()[0]};
        vector<int> program{my_parse(inf)};
        interpret(program);
    }
    // if (result.count("2")) {
    // }
    return 0;
}