#include <cxxopts.hpp>
#include <list>
#include <fstream>
#include <iostream>
#include <map>
#include <numeric>
#include <regex>
#include <string>
#include "intcode.hpp"
#include "run_previous_days.hpp"

using namespace std;

using hullmap = map<pair<int, int>, bool>;

hullmap paint_hull(vector<long> program, bool start){
    hullmap hm;
    int x{0};
    int y{0};
    int dir{0}; // up
    IntcodeComputer c{program};
    int color{0};
    int delta_dir{0};
    if (start) hm[pair(x, y)] = true;
    while (!c.halt){
        int current_color;
        auto i = hm.find(pair(x, y));
        if (i != hm.end()){
            current_color = i->second;
        } else {
            current_color = 0;
        }
        c.input_buffer.push_back(current_color);
        c.interpret();
        color = c.output_buffer.front();
        c.output_buffer.pop_front();
        hm[pair(x, y)] = color;
        delta_dir = c.output_buffer.front();
        c.output_buffer.pop_front();
        dir = (dir + (delta_dir == 0 ? -1 : 1)) % 4;
        if (dir < 0) dir += 4;
        switch (dir)
        {
        case 0:
            ++y;
            break;
        case 1:
            ++x;
            break;
        case 2:
            --y;
            break;
        case 3:
            --x;
            break;
        }
    }
    return hm;
}

void render_hullmap(hullmap hm){
    int xmin{hm.begin()->first.first};
    int xmax{hm.rbegin()->first.first};
    auto [mn, mx] = minmax_element(hm.begin(), hm.end(),
        [](pair<pair<int, int>, bool> a, pair<pair<int, int>, bool> b){
            return a.first.second < b.first.second;});
    int ymin{mn->first.second};
    int ymax{mx->first.second};
    for (int y = ymax; y >= ymin; --y){
        for (int x = xmin; x <= xmax; ++x){
            auto i = hm.find(pair(x, y));
            if (i != hm.end() && i->second){
                cout << 'X';
            } else {
                cout << ' ';
            }
        }
        cout << endl;
    }
    return;
}

int main(int argv, char **argc){
    cxxopts::Options options("test", "A brief description");
    options.add_options()
        ("0", "Rough tests", cxxopts::value<bool>())
        ("1", "Solve part 1", cxxopts::value<bool>())
        ("2", "Solve part 2", cxxopts::value<bool>())
        ("day-02", "Run all day 2 programs", cxxopts::value<bool>())
        ("day-05", "Run all day 5 programs", cxxopts::value<bool>())
        ("h,help", "Print usage");
    auto result = options.parse(argv, argc);

   if (result.count("help"))
    {
      std::cout << options.help() << std::endl;
      exit(0);
    }
    if (result.count("0")) {
    }
    if (result.count("1")) {
        ifstream inf{result.unmatched()[0]};
        vector<long> program{my_parse(inf)};
        hullmap hm{paint_hull(program, false)};
        cout << hm.size() << endl;
    }
    if (result.count("2")) {
        ifstream inf{result.unmatched()[0]};
        vector<long> program{my_parse(inf)};
        hullmap hm{paint_hull(program, true)};
        render_hullmap(hm);
    }
    if (result.count("day-02")) run_day_2();
    if (result.count("day-05")) run_day_5();
    return 0;
}
