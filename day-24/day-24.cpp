#include <fstream>
#include <iostream>
#include <string>
#include <unordered_set>
#include <vector>

#include "cxxopts.hpp"

using namespace std;
using std::cout;

vector<bool> my_parse(ifstream& inf) {
  string line{};
  vector<bool> bug_grid{};
  while (getline(inf, line) && !line.empty()) {
    for (int j{0}; j < 5; ++j) {
      bug_grid.push_back(line[j] == '#');
    }
  }
  return bug_grid;
}

int bug_grid_to_score(vector<bool> bug_grid) {
  int score{0};
  for (int i{0}; i < 25; ++i) {
    score += bug_grid[i] << i;
  }
  return score;
}

vector<bool> update_bug_grid(vector<bool>& bug_grid) {
  vector<bool> new_bug_grid{};
  for (int k{0}; k < 25; ++k) {
    int i{k / 5};
    int j{k % 5};
    int count{(i > 0 && bug_grid.at(k - 5)) + (i < 4 && bug_grid.at(k + 5)) +
              (j > 0 && bug_grid.at(k - 1)) + (j < 4 && bug_grid.at(k + 1))};
    if (bug_grid[k]) {
      new_bug_grid.push_back(count == 1);
    } else {
      new_bug_grid.push_back(count == 1 || count == 2);
    }
  }
  return new_bug_grid;
}

void print_bug_grid(vector<bool>& bug_grid) {
  for (int k{0}; k < 25; ++k) {
    cout << (bug_grid[k] ? '#' : '.');
    if (k % 5 == 4) cout << endl;
  }
  cout << endl;
}

int main(int argv, char** argc) {
  cxxopts::Options options("test", "A brief description");
  options.add_options()("1", "Solve part 1", cxxopts::value<bool>())(
      "2", "Solve part 2", cxxopts::value<bool>())("h,help", "Print usage")(
      "d", "Make graphviz dot file", cxxopts::value<bool>());
  auto result = options.parse(argv, argc);

  if (result.count("help")) {
    cout << options.help() << endl;
    exit(0);
  }
  if (result.count("1")) {
    ifstream inf{result.unmatched()[0]};
    auto bug_grid{my_parse(inf)};
    vector<bool> new_bug_grid{};
    unordered_set<int> past_scores{};
    while (true) {
      new_bug_grid = update_bug_grid(bug_grid);
      bug_grid = new_bug_grid;
      int score{bug_grid_to_score(bug_grid)};
      if (past_scores.find(score) != past_scores.end()) {
        cout << score << endl;
        return 0;
      } else
        past_scores.insert(score);
    }
  }
  if (result.count("2")) {
  }
  return 0;
}