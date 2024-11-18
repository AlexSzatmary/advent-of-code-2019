#include <cxxopts.hpp>
#include <fstream>
#include <iostream>

#include "intcode.hpp"
#include "run_previous_days.hpp"

using namespace std;
using std::cout;

struct Packet {
  long destination{};
  long X{};
  long Y{};
  Packet(long destination, long X, long Y)
      : destination{destination}, X{X}, Y{Y} {}
};

int move_ic_output_to_packet_queue(IntcodeComputer& ic, list<Packet>& packets) {
  while (ic.output_buffer.size()) {
    long destination{ic.output_buffer.front()};
    ic.output_buffer.pop_front();
    long X{ic.output_buffer.front()};
    ic.output_buffer.pop_front();
    long Y{ic.output_buffer.front()};
    ic.output_buffer.pop_front();
    if (destination == 255) {
      cout << Y << endl;
      return 255;
    }
    packets.emplace_back(destination, X, Y);
  }
  return 0;
}

int main(int argv, char** argc) {
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
    vector<IntcodeComputer> computers{};
    list<Packet> packets{};
    for (int i{0}; i < 50; ++i) {
      auto& ic{computers.emplace_back(program)};
      ic.input_buffer.push_back(i);
      ic.input_buffer.push_back(-1);
      ic.interpret();
      int ret{move_ic_output_to_packet_queue(ic, packets)};
      if (ret == 255) return 0;
    }
    while (packets.size()) {
      Packet p{packets.front()};
      packets.pop_front();
      IntcodeComputer& c{computers[p.destination]};
      c.input_buffer.push_back(p.X);
      c.input_buffer.push_back(p.Y);
      c.interpret();
      int ret{move_ic_output_to_packet_queue(c, packets)};
      if (ret == 255) return 0;
    }
  }
  if (result.count("2")) {
  }
  if (result.count("day-02")) run_day_2();
  if (result.count("day-05")) run_day_5();
  return 0;
}
