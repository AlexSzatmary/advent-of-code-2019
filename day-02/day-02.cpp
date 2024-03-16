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
    regex re("\\d+");
    vector<int> program;
    transform(sregex_iterator(begin(line), end(line), re),
        sregex_iterator(),
        back_inserter(program),
        [](smatch m){return stoi(m.str());});
    return program;
}

int interpret(vector<int> program){
    for (int i = 0; i < program.size() && program.at(i) != 99; i += 4){
        switch (program.at(i)){
            case 1:
                program.at(program.at(i + 3)) = program.at(program.at(i + 1)) +
                program.at(program.at(i + 2));
                break;
            case 2:
                program.at(program.at(i + 3)) = program.at(program.at(i + 1)) *
                program.at(program.at(i + 2));
                break;
        }
    }
    for (int i: program){
        cout << i << ",";
    }
    cout << endl;
    return program[0];
}

int main(int argv, char **argc){
    cxxopts::Options options("test", "A brief description");
    options.add_options()
        ("1", "Solve part 1", cxxopts::value<bool>())
        ("2", "Solve part 2", cxxopts::value<bool>())
        // ("f,foo", "Param foo", cxxopts::value<int>()->default_value("10"))
        ("h,help", "Print usage");
    auto result = options.parse(argv, argc);

   if (result.count("help"))
    {
      std::cout << options.help() << std::endl;
      exit(0);
    }
    if (result.count("1")) {
        ifstream inf{result.unmatched()[0]};
        vector<int> program{my_parse(inf)};
        program.at(1) = 12;
        program.at(2) = 2;
        cout << interpret(program) << endl;
    }
    // if (result.count("2")) {
    // }
    return 0;
}