#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "Eigen/Dense"
#include "cxxopts.hpp"

using namespace std;
using std::cout;
using Deck = vector<int>;

Deck make_deck(int n) {
  Deck deck(static_cast<size_t>(n), 0);
  for (int i{0}; i < deck.size(); ++i) deck[i] = i;
  return deck;
}

void deal_into_new_stack(Deck& deck) { reverse(deck.begin(), deck.end()); }

void cut(Deck& deck, int n) {
  if (n < 0) n += deck.size();
  rotate(deck.begin(), deck.begin() + n, deck.end());
}

void deal_with_increment_N(Deck& deck, int n) {
  Deck old{deck};
  int new_index{0};
  for (int i{0}; i < old.size(); ++i) {
    deck.at(new_index) = old.at(i);
    new_index += n;
    if (new_index > deck.size() - 1) new_index -= deck.size();
  }
}

vector<string> my_parse(ifstream& inf) {
  string line{};
  vector<string> shuffle_process{};
  while (getline(inf, line) && !line.empty()) {
    shuffle_process.push_back(line.substr(0, line.size()));
  }
  return shuffle_process;
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
    auto shuffle_process{my_parse(inf)};
    auto deck{make_deck(stoi(result.unmatched()[1]))};
    regex cut_re{"cut (.*)"};
    regex new_stack_re{"deal into new stack"};
    regex increment_N_re{"deal with increment (.*)"};
    for (string step : shuffle_process) {
      smatch m{};
      if (regex_match(step, m, cut_re)) {
        // cout << "cut " << m.str(1) << endl;
        cut(deck, stoi(m.str(1)));
      } else if (regex_match(step, m, new_stack_re)) {
        // cout << "deal into new stack" << endl;
        deal_into_new_stack(deck);
      } else if (regex_match(step, m, increment_N_re)) {
        // cout << "deal with increment " << m.str(1) << endl;
        deal_with_increment_N(deck, stoi(m.str(1)));
      }
    }

    // for (int i : deck) cout << i << ' ';
    // cout << endl;
    auto card2019{std::find(deck.begin(), deck.end(), 2019)};
    cout << card2019 - deck.begin() << endl;
  }
  if (result.count("2")) {
  }
  return 0;
}