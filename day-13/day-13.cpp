#include <cxxopts.hpp>
#include <list>
#include <fstream>
#include <iostream>
#include <map>
#include <numeric>
#include <regex>
#include <string>
#include "intcode.hpp"
#include "run_previous_days.hpp"

using namespace std;

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
        int n_blocks{0};
        for (auto p{begin(ic.output_buffer)}; p != end(ic.output_buffer); ++p){
            ++p;
            ++p;
            if (*p == 2) n_blocks += 1;
        }
        cout << n_blocks << endl;
    }
    if (result.count("2")) {
    }
    if (result.count("day-02")) run_day_2();
    if (result.count("day-05")) run_day_5();
    return 0;
}
