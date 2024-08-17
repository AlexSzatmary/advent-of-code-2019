#include <cxxopts.hpp>
#include <list>
#include <vector>
#include <fstream>
#include <iostream>
#include "intcode.hpp"
#include "run_previous_days.hpp"

using namespace std;

int sum_alignment_parameters(IntcodeComputer ic){
    int j{0};
    int i{0};
    int width{0};
    int sum_alignment_parameters{0};
    vector<char> v{};
    for (auto x: ic.output_buffer){
        auto c{static_cast<char>(x)};
        switch (c)
        {
        case '\n':
            if (width == 0) width = j + 1;
            j = -1;
            ++i;
            // cout << c;
            break;
        case '#':
            if (j > 1 && i > 0 &&
            v.back() == '#' &&
            v.at(v.size() - 2) == '#' &&
            v.at((i - 1) * width + j - 1) == '#'){
                sum_alignment_parameters += (j - 1) * i;
                // cout << 'O';
            } else {
                // cout << c;
            }
            break;
        default:
            // cout << c;
            break;
        }
        v.push_back(static_cast<char>(c));
        ++j;
    }
    return sum_alignment_parameters;
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
        auto ic = IntcodeComputer(program);
        ic.interpret();
        cout << sum_alignment_parameters(ic) << endl;
    }
    if (result.count("2")) {
    }
    if (result.count("day-02")) run_day_2();
    if (result.count("day-05")) run_day_5();
    return 0;
}
