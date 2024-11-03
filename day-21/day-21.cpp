#include <cxxopts.hpp>
#include <fstream>
#include <iostream>

#include "intcode.hpp"
#include "run_previous_days.hpp"

using namespace std;
using std::cout;

int main(int argv, char **argc) {
  cxxopts::Options options("test", "A brief description");
  options.add_options()("0", "Rough tests", cxxopts::value<bool>())(
      "1", "Solve part 1", cxxopts::value<bool>())("2", "Solve part 2",
                                                   cxxopts::value<bool>())(
      "day-02", "Run all day 2 programs", cxxopts::value<bool>())(
      "day-05", "Run all day 5 programs", cxxopts::value<bool>())(
      "h,help", "Print usage");
  auto result = options.parse(argv, argc);

  if (result.count("help")) {
    std::cout << options.help() << std::endl;
    exit(0);
  }
  if (result.count("0")) {
  }
  if (result.count("1")) {
    ifstream inf{result.unmatched()[0]};
    vector<long> program{my_parse(inf)};
    IntcodeComputer ic{program};
    ifstream springscript{result.unmatched()[1]};
    string line{};
    while (getline(springscript, line) && !line.empty()) {
      for (char c : line.substr(0, line.size())) ic.input_buffer.push_back(c);
      ic.input_buffer.push_back('\n');
    }

    ic.interpret();
    for (long x : ic.output_buffer){
      if (x < 256) {
        cout << static_cast<char>(x);
      } else {
        cout << endl << x << endl;
      }
    }

  }
  if (result.count("2")) {
  }
  if (result.count("day-02")) run_day_2();
  if (result.count("day-05")) run_day_5();
  return 0;
}
