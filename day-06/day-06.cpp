#include "cxxopts.hpp"
#include <list>
#include <map>
#include <fstream>
#include <iostream>
#include <regex>
#include <string>
#include <utility>

using namespace std;

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

map<string, string> my_parse_parent(ifstream &inf){
    string line;
    regex re("(\\w+)\\)(\\w+)");
    map<string, string> parent_graph;
    smatch m;
    while (getline(inf, line)){
        regex_match(line, m, re);
        parent_graph[m.str(2)] = m.str(1);
    }
    return parent_graph;
}

int count_transfers(map<string, string> parent_graph){
    auto santa_node{parent_graph["SAN"]};
    auto you_node{parent_graph["YOU"]};

    int santa_depth{0};
    auto next{santa_node};
    while (next != "COM"){
        next = parent_graph[next];
        ++santa_depth;
    }

    int you_depth{0};
    next = you_node;
    while (next != "COM"){
        next = parent_graph[next];
        ++you_depth;
    }

    if (santa_depth > you_depth){
        for (int i = 0; i < santa_depth - you_depth; ++i){
            santa_node = parent_graph[santa_node];
        }
    } else if (santa_depth < you_depth){
        for (int i = 0; i < you_depth - santa_depth; ++i){
            you_node = parent_graph[you_node];
        }
    }
    int transfers{abs(santa_depth - you_depth)};
    while (you_node != santa_node){
        transfers += 2;
        santa_node = parent_graph[santa_node];
        you_node = parent_graph[you_node];
    }
    return transfers;
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
        cout << count_orbits(bg) << endl;
    }
    if (result.count("2")) {
        ifstream inf{result.unmatched()[0]};
        auto parent_graph = my_parse_parent(inf);
        cout << count_transfers(parent_graph) << endl;
    }
    return 0;
}