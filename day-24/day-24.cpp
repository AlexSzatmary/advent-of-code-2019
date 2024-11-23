#include <algorithm>
#include <fstream>
#include <iostream>
#include <list>
#include <string>
#include <unordered_set>
#include <vector>

#include "cxxopts.hpp"

using namespace std;
using std::cout;
using BugGrid = vector<bool>;
using BugGridSpace = list<BugGrid>;

BugGrid my_parse(ifstream& inf) {
  string line{};
  BugGrid bug_grid{};
  while (getline(inf, line) && !line.empty()) {
    for (int j{0}; j < 5; ++j) {
      bug_grid.push_back(line[j] == '#');
    }
  }
  return bug_grid;
}

int bug_grid_to_score(BugGrid bug_grid) {
  int score{0};
  for (int i{0}; i < 25; ++i) {
    score += bug_grid[i] << i;
  }
  return score;
}

BugGrid update_bug_grid(BugGrid& bug_grid) {
  BugGrid new_bug_grid{};
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

BugGrid update_bug_grid_2(BugGrid& left, BugGrid& center, BugGrid& right) {
  // k array
  // [ 0  1  2  3  4
  //   5  6  7  8  9
  //  10 11 12 13 14
  //  15 16 17 18 19
  //  20 21 22 23 24]

  BugGrid new_bug_grid{};
  for (int k{0}; k < 25; ++k) {
    int i{k / 5};
    int j{k % 5};
    int count{(i > 0 && center.at(k - 5)) + (i < 4 && center.at(k + 5)) +
              (j > 0 && center.at(k - 1)) + (j < 4 && center.at(k + 1))};

    if (i == 0) count += left.at(7);
    if (j == 0) count += left.at(11);
    if (i == 4) count += left.at(17);
    if (j == 4) count += left.at(13);

    if (k == 7)
      count +=
          right.at(0) + right.at(1) + right.at(2) + right.at(3) + right.at(4);

    if (k == 11)
      count += right.at(0) + right.at(5) + right.at(10) + right.at(15) +
               right.at(20);

    if (k == 13)
      count += right.at(4) + right.at(9) + right.at(14) + right.at(19) +
               right.at(24);

    if (k == 17)
      count += right.at(20) + right.at(21) + right.at(22) + right.at(23) +
               right.at(24);

    if (k == 12) {
      new_bug_grid.push_back(false);
    } else {
      if (center[k]) {
        new_bug_grid.push_back(count == 1);
      } else {
        new_bug_grid.push_back(count == 1 || count == 2);
      }
    }
  }
  return new_bug_grid;
}

BugGridSpace update_bug_grid_space(BugGridSpace& bug_grid_space) {
  BugGridSpace new_bug_grid_space{};
  BugGrid bug_grid_empty(25, false);
  if (any_of(bug_grid_space.front().begin(), bug_grid_space.front().end(),
             [](bool b) { return b; }))
    bug_grid_space.push_front(bug_grid_empty);
  if (any_of(bug_grid_space.back().begin(), bug_grid_space.back().end(),
             [](bool b) { return b; }))
    bug_grid_space.push_back(bug_grid_empty);
  auto left{bug_grid_space.begin()};
  auto center{bug_grid_space.begin()};
  ++center;
  auto right{bug_grid_space.begin()};
  ++right;
  ++right;
  new_bug_grid_space.push_back(
      update_bug_grid_2(bug_grid_empty, *left, *center));
  while (right != bug_grid_space.end()) {
    new_bug_grid_space.push_back(update_bug_grid_2(*left, *center, *right));
    ++left;
    ++center;
    ++right;
  }
  new_bug_grid_space.push_back(
      update_bug_grid_2(*left, *center, bug_grid_empty));
  return new_bug_grid_space;
}

void print_bug_grid(BugGrid& bug_grid) {
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
    BugGrid new_bug_grid{};
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
    ifstream inf{result.unmatched()[0]};
    auto bug_grid{my_parse(inf)};
    BugGridSpace bug_grid_space{bug_grid};
    BugGridSpace new_bug_grid_space{};
    for (int k{0}; k < 200; ++k) {
      bug_grid_space = update_bug_grid_space(bug_grid_space);
      // cout << "k = " << k << endl;
      // for (auto bug_grid : bug_grid_space) {
      //   print_bug_grid(bug_grid);
      //   cout << endl;
      // }
    }
    int n_bugs{0};
    for (auto bg : bug_grid_space)
      for (auto tile : bg) n_bugs += tile;
    cout << n_bugs << endl;
  }
  return 0;
}