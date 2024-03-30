#include "cxxopts.hpp"
#include <list>
#include <map>
#include <fstream>
#include <iostream>
#include <regex>
#include <string>
#include <utility>

using namespace std;

// struct Body{
//     string name;
//     vector<Body>satellites;
// };

using BodyGraph = map<string, list<string>>;

BodyGraph my_parse(ifstream &inf){
    string line;
    regex re("(\\w+)\\)(\\w+)");
    BodyGraph bg;
    smatch m;
    while (getline(inf, line)){
        regex_match(line, m, re);
        auto i = bg.find(m.str(1));
        if (i != bg.end()) {
            i->second.push_back(m.str(2));
        } else {
            bg[m.str(1)] = list(1, m.str(2));
        }
    }
    return bg;
}

int count_orbits(BodyGraph bg){
    list<string> this_level{};
    list<string> next_level{bg["COM"]};
    int level{1};
    int orbits{0};
    // for (auto node: bg["COM"]){
    //     cout << node << endl;
    // }
    while (!next_level.empty()){
        this_level = next_level;
        next_level = list<string>();
        for (string body: this_level) {
            // cout << body << endl;
            orbits += level;
            auto i = bg.find(body);
            if (i != bg.end()) {
                copy(i->second.begin(), i->second.end(),
                back_inserter(next_level));
            }
        }
        ++level;
    }
    return orbits;
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
        auto bg{my_parse(inf)};
        // for (auto i: bg){
        //     for (auto j: i.second){
        //         cout << i.first << "->" << j << endl;
        //     }
        // }
        cout << count_orbits(bg) << endl;
    }
    if (result.count("2")) {
    }
    return 0;
}