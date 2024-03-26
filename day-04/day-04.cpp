#include "cxxopts.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <numeric>
#include <string>
#include <regex>

using namespace std;

pair<string, string> my_parse(ifstream &inf){
    string line;
    getline(inf, line);
    regex re("(\\d+)-(\\d+)");
    smatch m;
    regex_match(line, m, re);
    return pair(m.str(1), m.str(2));
}

int main(int argv, char **argc){
    cxxopts::Options options("test", "A brief description");
    options.add_options()
        ("1", "Solve part 1", cxxopts::value<bool>())
        ("2", "Solve part 2", cxxopts::value<bool>())
        ("h,help", "Print usage");
    auto result = options.parse(argv, argc);

   if (result.count("help"))
    {
      std::cout << options.help() << std::endl;
      exit(0);
    }
    if (result.count("1")) {
        ifstream inf{result.unmatched()[0]};
        auto [lo, hi] = my_parse(inf);
        cout << lo << ", " << hi << endl;
        int hi_i{stoi(hi)};
        int possible_passwords{0};
        for (int i{stoi(lo)}; i <= hi_i; ++i){
            vector<int> v{};
            auto i_s{to_string(i)};
            transform(begin(i_s), end(i_s), back_inserter(v),
                [](char c){return c - '0';});
            int prev{-1};
            bool same{false};
            bool increasing{true};
            for (auto j: v){
                if (j < prev) {
                    increasing = false;
                    break;
                }
                if (j == prev) same = true;
                prev = j;
            }
            if (same && increasing) ++possible_passwords;
        }
        cout << possible_passwords << endl;
    }
    if (result.count("2")) {
    }
    return 0;
}