#include <cxxopts.hpp>
#include <fstream>
#include <iostream>
#include <regex>

#include "intcode.hpp"
#include "run_previous_days.hpp"

using namespace std;
using std::cout;

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
    ifstream script{"script.txt"};
    string line{};
    list<string> script_commands{};
    while (getline(script, line) && !line.empty()) {
      script_commands.push_back(line);
    }

    vector<long> program{my_parse(inf)};
    IntcodeComputer ic{program};
    ic.interpret();
    regex north{"n|(north)"};
    regex south{"s|(south)"};
    regex east{"e|(east)"};
    regex west{"w|(west)"};
    regex take{"(t|(take)) +(.*)"};
    regex drop{"(d|(drop)) +(.*)"};
    regex inv{"i|(inv)"};
    auto script_commands_it{script_commands.begin()};
    while (!ic.output_buffer.empty()) {
      for (char c : ic.output_buffer) {
        cout << c;
      }
      ic.output_buffer.clear();
      string command{};
      if (script_commands_it != script_commands.end()) {
        command = *script_commands_it++;
        cout << command << endl;
      } else {
        getline(cin, command);
      }
      smatch m;
      if (regex_match(command, m, north)) {
        command = "north";
      } else if (regex_match(command, m, south)) {
        command = "south";
      } else if (regex_match(command, m, east)) {
        command = "east";
      } else if (regex_match(command, m, west)) {
        command = "west";
      } else if (regex_match(command, m, take)) {
        command = "take " + m.str(3);
      } else if (regex_match(command, m, drop)) {
        command = "drop " + m.str(3);
      } else if (regex_match(command, m, inv)) {
        command = "inv";
      }
      for (char c : command) {
        ic.input_buffer.push_back(c);
      }
      ic.input_buffer.push_back('\n');
      ic.interpret();
    }
  }
  if (result.count("2")) {
  }
  if (result.count("day-02")) run_day_2();
  if (result.count("day-05")) run_day_5();
  return 0;
}
