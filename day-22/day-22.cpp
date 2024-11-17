#include <algorithm>
#include <fstream>
#include <iostream>
#include <limits>
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

long long multiply_by_double(long long a, long long b, long long n) {
  // takes (a * b) % n
  long long result{0};
  while (b > 0) {
    if (b % 2)
      result = (result + a) % n;
    a = (a + a + n) % n;
    b >>= 1;
  }
  return result;
}

long long exponent_by_square(long long a, long long b, long long n) {
  // takes (a ^ b) % n
  long long result{1};
  while (b > 0) {
    if (b % 2) result = multiply_by_double(result, a, n);
    a = multiply_by_double(a, a, n);
    b >>= 1;
  }
  return result;
}

pair<long long, long long> compose_ab(long long a, long long b,
                                      long long shuffles, long long n) {
  // composes f(x) = a * x + b onto itself shuffles times
  long long a_r{1};
  long long b_r{0};
  while (shuffles > 0) {
    if (shuffles % 2) {
      // f(f_r(x)) = a * (a_r * x + b_r) + b = a * a_r * x + a * b_r + b
      a_r = multiply_by_double(a, a_r, n);
      b_r = (multiply_by_double(a, b_r, n) + b) % n;
    }
    // f(f(x)) = a * (a * x + b) + b = a * a * x + a * b + b
    b = ((multiply_by_double(a, b, n) + b) % n + n) % n;
    a = multiply_by_double(a, a, n);
    shuffles >>= 1;
  }
  return pair(a_r, b_r);
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
  if (result.count("1")) {  // invoke with ./build/day-22 -1 inme-22.txt 10007
    ifstream inf{result.unmatched()[0]};
    auto shuffle_process{my_parse(inf)};
    auto deck{make_deck(stoi(result.unmatched()[1]))};
    regex cut_re{"cut (.*)"};
    regex new_stack_re{"deal into new stack"};
    regex increment_N_re{"deal with increment (.*)"};
    for (string step : shuffle_process) {
      smatch m{};
      if (regex_match(step, m, cut_re)) {
        cut(deck, stoi(m.str(1)));
      } else if (regex_match(step, m, new_stack_re)) {
        deal_into_new_stack(deck);
      } else if (regex_match(step, m, increment_N_re)) {
        deal_with_increment_N(deck, stoi(m.str(1)));
      }
    }

    auto card2019{std::find(deck.begin(), deck.end(), 2019)};
    cout << card2019 - deck.begin() << endl;
  }
  if (result.count("2")) {
    ifstream inf{result.unmatched()[0]};
    auto shuffle_process{my_parse(inf)};
    long long const n{119315717514047};
    // long long const n{10007};
    long long const shuffles{101741582076661};
    // auto deck{make_deck(n)};
    long long a{1};
    long long b{0};
    // each shuffle applies a transform of the form f(x) = a * x + b under mod n
    // we calculate a and b
    // I basically followed this tutorial https://codeforces.com/blog/entry/72593
    regex cut_re{"cut (.*)"};
    regex new_stack_re{"deal into new stack"};
    regex increment_N_re{"deal with increment (.*)"};
    for (string step : shuffle_process) {
      smatch m{};
      if (regex_match(step, m, cut_re)) {
        b = (b + -stoll(m.str(1)) + n) % n;  // the + n handles negative cuts
      } else if (regex_match(step, m, new_stack_re)) {
        a = (-a + n) % n;
        b = (-b - 1 + n) % n;
      } else if (regex_match(step, m, increment_N_re)) {
        a = multiply_by_double(a, stoll(m.str(1)), n);
        b = multiply_by_double(b, stoll(m.str(1)), n);
      }
    }
    auto [a_r, b_r] = compose_ab(a, b, shuffles, n);
    long long ainv{exponent_by_square(a_r, n - 2, n)};
    cout << multiply_by_double((2020 - b_r), ainv, n) << endl;
  }
  return 0;
}