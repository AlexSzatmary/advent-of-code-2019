#include <cxxopts.hpp>
#include <fstream>
#include <iostream>

#include "intcode.hpp"
#include "run_previous_days.hpp"

using namespace std;
using std::cout;

bool is_pulled(vector<long> program, const int i, const int j) {
  auto ic = IntcodeComputer(program);
  ic.input_buffer.push_back(j);
  ic.input_buffer.push_back(i);
  ic.interpret();
  bool result{ic.output_buffer.front() == 1};
  ic.output_buffer.clear();
  return result;
}

int find_pulled(vector<long> program, int const i) {
  int const j_max{2 * i};
  int increment{1};
  while (increment < j_max) {
    increment <<= 1;
  }
  while (increment > 0) {
    for (int j{increment >> 1}; j < j_max; j += increment) {
      if (is_pulled(program, i, j)) return j;
    }
    increment >>= 1;
  }
  // should be an error, or I could implement logic that bumps up
  // j_max and tries again
  return 0;
}

int find_first_j(vector<long> program, int const i, int const square_size) {
  int const i_big{i + square_size - 1};
  int hi{find_pulled(program, i_big) + 1};
  int lo{0};
  int j{(lo + hi) / 2};
  while (lo + 1 < hi) {
    if (is_pulled(program, i_big, j)) {
      hi = j;
    } else {
      lo = j;
    }
    j = (lo + hi) / 2;
  }
  j = lo + 1;
  if (is_pulled(program, i, j + square_size - 1))
    return j;
  else
    return 0;
}

int find_first_i(vector<long> program, int const square_size) {
  int lo{0};
  int hi{1};

  // find hi that works
  while (find_first_j(program, hi, square_size) == 0) hi <<= 1;

  int i{(lo + hi) / 2};
  int j{};
  while (lo + 1 < hi) {
    j = find_first_j(program, i, square_size);
    if (j != 0) {
      hi = i;
    } else {
      lo = i;
    }
    i = (lo + hi) / 2;
  }
  i = lo + 1;
  j = find_first_j(program, i, square_size);
  return j * 10'000 + i;
}

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
        n_points_being_pulled += is_pulled(program, i, j);
        cout << (is_pulled(program, i, j) ? '#' : '.');
      }
      cout << endl;
    }
    cout << n_points_being_pulled << endl;
  }
  if (result.count("2")) {
    ifstream inf{result.unmatched()[0]};
    vector<long> program{my_parse(inf)};
    int square_size{100};
    cout << find_first_i(program, square_size) << endl;
  }
  if (result.count("day-02")) run_day_2();
  if (result.count("day-05")) run_day_5();
  return 0;
}
