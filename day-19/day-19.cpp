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
    int n_points_being_pulled{0};
    int const width{50};
    int const height{50};
    for (int i{0}; i < height; ++i) {
      for (int j{0}; j < width; ++j) {
        auto ic = IntcodeComputer(program);
        ic.input_buffer.push_back(j);
        ic.input_buffer.push_back(i);
        ic.interpret();
        n_points_being_pulled += ic.output_buffer.front();
        cout << (ic.output_buffer.front() ? '#' : ' ');
      }
      cout << endl;
    }
    cout << n_points_being_pulled << endl;
  }
  if (result.count("2")) {
  }
  if (result.count("day-02")) run_day_2();
  if (result.count("day-05")) run_day_5();
  return 0;
}
