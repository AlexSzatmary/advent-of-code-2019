#include <algorithm>
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
      "1", "Run off script", cxxopts::value<bool>())("2", "Run automatically",
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
    ifstream inf{result.unmatched()[0]};
    string line{};
    list<string> script_commands{};
    vector<string> item_names{"jam",           "loom",         "mug",
                              "spool of cat6", "prime number", "food ration",
                              "fuel cell",     "manifold"};
    vector<string> const direction_names{"east", "north", "west", "south"};
    int times_at_security_checkpoint{0};
    sort(item_names.begin(), item_names.end());
    vector<long> program{my_parse(inf)};
    IntcodeComputer ic{program};
    ic.interpret();
    regex location_re{"== (.*) =="};
    string location{};
    regex item_re{"- (.*)"};
    string item{};
    int direction{0};
    vector<bool> doors{false, false, false, false};
    int loop_limit{0};
    while (!ic.output_buffer.empty() && times_at_security_checkpoint < 2) {
      string command{};
      stringstream output{
          string(ic.output_buffer.begin(), ic.output_buffer.end())};
      while (getline(output, line)) {
        cout << line << endl;
        smatch m;
        if (regex_match(line, m, location_re)) {
          location = m.str(1);
          if (location == "Security Checkpoint") {
            ++times_at_security_checkpoint;
            while (getline(output, line)) {
              cout << line << endl;
            }
            if (times_at_security_checkpoint > 1) {
              command = "inv";
            } else {
              command = "east";
              direction = 0;
            }
          }
        }
        if (line == "Doors here lead:") {
          doors = {false, false, false, false};
          while (getline(output, line) && !line.empty()) {
            cout << line << endl;
            if (regex_match(line, m, item_re)) {
              if (m.str(1) == "east") {
                doors[0] = true;
              } else if (m.str(1) == "north") {
                doors[1] = true;
              } else if (m.str(1) == "west") {
                doors[2] = true;
              } else if (m.str(1) == "south") {
                doors[3] = true;
              }
            }
          }
          direction = (direction + 1) % 4;  // first turn left
          while (!doors[direction]) {
            direction = (direction + 3) % 4;  // try turning right as needed
          }
        }

        if (line == "Items here:") {
          getline(output, line);
          cout << line << endl;
          if (regex_match(line, m, item_re) &&
              binary_search(item_names.begin(), item_names.end(), m.str(1))) {
            command = "take " + m.str(1);
          }
        }
      }
      if (!command.size()) {  // if we didn't make a command
        // to take an item pick a new location. If we had already issued a
        // command to take an item, the direction had still been updated when we
        // entered this location.
        command = direction_names[direction];
      }
      ic.output_buffer.clear();
      cout << command << endl;
      for (char c : command) ic.input_buffer.push_back(c);
      ic.input_buffer.push_back('\n');
      ic.interpret();
    }
    // stringstream output{
    //     string(ic.output_buffer.begin(), ic.output_buffer.end())};
    // while (getline(output, line)) {
    //   cout << line << endl;
    // }

    // take and drop different combinations of items, then go north.
    int inventory{0};
    int k{0};
    int take{0};
    int drop{255};
    while (!ic.output_buffer.empty() && inventory < 256) {
      string command{};
      stringstream output{
          string(ic.output_buffer.begin(), ic.output_buffer.end())};
      while (getline(output, line)) {
        cout << line << endl;
      }
      while (k < 8) {
        if ((take >> k) & 1) {
          command = "take " + item_names[k];
          ++k;
          break;
        } else if ((drop >> k) & 1) {
          command = "drop " + item_names[k];
          ++k;
          break;
        }
        ++k;
      }
      if (!command.size()) {
        command = "north";
        ++inventory;
        take = inventory & (inventory ^ (inventory - 1));
        drop = (inventory - 1) & (inventory ^ (inventory - 1));
        k = 0;
      }

      ic.output_buffer.clear();
      cout << command << endl;
      for (char c : command) ic.input_buffer.push_back(c);
      ic.input_buffer.push_back('\n');
      ic.interpret();
    }
  }
  if (result.count("day-02")) run_day_2();
  if (result.count("day-05")) run_day_5();
  return 0;
}
