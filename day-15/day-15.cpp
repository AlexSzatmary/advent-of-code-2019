#include <cxxopts.hpp>
#include <list>
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <set>
#include <numeric>
#include <regex>
#include <string>
#include "intcode.hpp"
#include "run_previous_days.hpp"

using namespace std;

using shipmap = map<pair<int, int>, int>;
using pathlog = list<tuple<int, int, int>>;

pair<int, int> dir_to_vec(int dir){
    switch (dir)
    {
    case 1: // north
        return pair(0, 1);
    case 2: // south
        return pair(0, -1);
    case 3: // west
        return pair(-1, 0);
    case 4: // east
        return pair(1, 0);
    default:
        return pair(0, 0);
    }
}

int next_dir_ccw(int dir){
    switch (dir)
    {
    case 1: // north
        return 3;
    case 2: // south
        return 4;
    case 3: // west
        return 2;
    case 4: // east
        return 1;
    default:
        return -1;
    }
}

int next_dir_cw(int dir){
    switch (dir)
    {
    case 1: // north
        return 4;
    case 2: // south
        return 3;
    case 3: // west
        return 1;
    case 4: // east
        return 2;
    default:
        return -1;
    }
}

void render_shipmap(shipmap sm){
    int xmin{sm.begin()->first.first};
    int xmax{sm.rbegin()->first.first};
    auto [mn, mx] = minmax_element(sm.begin(), sm.end(),
        [](pair<pair<int, int>, bool> a, pair<pair<int, int>, bool> b){
            return a.first.second < b.first.second;});
    int ymin{mn->first.second};
    int ymax{mx->first.second};
    for (int y{ymax}; y >= ymin; --y){
        for (int x{xmin}; x <= xmax; ++x){
            auto i = sm.find(pair(x, y));
            if (i == sm.end()){
                cout << ' ';
            } else switch (i->second)
            {
            case -1:
                cout << '0';
                break;
            case 0:
                cout << '#';
                break;
            case 1:
                cout << '.';
                break;
            case 2:
                cout << '2';
                break;
            default:
                break;
            }
        }
        cout << endl;
    }
    return;
}

pair<shipmap, pathlog> walk(IntcodeComputer ic){
    shipmap sm{};
    pathlog pl{};
    int x{0};
    int y{0};
    sm[pair(x, y)] = -1;
    int dir{1}; // up
    pl.push_back(tuple(x, y, dir));

    ic.input_buffer.push_back(dir);
    ic.interpret();
    long last_reply{ic.output_buffer.front()};
    ic.output_buffer.pop_front();
    // It turns out that there are no large chambers, every corridor has width 1,
    // and the maze is solveable by hugging the right wall.
    // Running the search twice lets us fill in the maze by continuing after finding
    // the destination.
    // This could be done more efficiently by terminating when the start is found twice.
    for (int i{0}; i < 2; ++i){
        while (last_reply != 2){
            auto [dx, dy] = dir_to_vec(dir);
            if (last_reply == 0){
                sm[pair(x + dx, y + dy)] = 0;
                dir = next_dir_ccw(dir);  // bump and turn
            } else {
                x += dx;
                y += dy;
                sm[pair(x, y)] = 1;
                pl.push_back(tuple(x, y, dir));
                dir = next_dir_cw(dir);  // turn back to hug wall
            }
            ic.input_buffer.push_back(dir);
            ic.interpret();
            last_reply = ic.output_buffer.front();
            ic.output_buffer.pop_front();
        }
        auto [dx, dy] = dir_to_vec(dir);
        x += dx;
        y += dy;
        sm[pair(x, y)] = 2;
        pl.push_back(tuple(x, y, dir));
        dir = next_dir_cw(dir);  // turn back to hug wall
        ic.input_buffer.push_back(dir);
        ic.interpret();
        last_reply = ic.output_buffer.front();
        ic.output_buffer.pop_front();
    }
    sm[pair(0, 0)] = -1;
    return pair(sm, pl);
}

struct State{
    int x;
    int y;
};

inline bool operator<(const State& lhs, const State& rhs){
    return tie(lhs.x, lhs.y) < tie(rhs.x, rhs.y);
}

int min_commands_to_oxygen(shipmap& sm){
    int commands{0};
    list<State> now_edges{State{0, 0}};
    list<State> next_edges{};
    set<State> processed{};
    while (commands < 1000){
        for (State step: now_edges){
            if (processed.find(step) != processed.end()) continue;
            switch (sm[pair(step.x, step.y)]){
                case 0:
                    break;
                case 2:
                    return commands;
                    break;
                case -1:
                case 1:
                    next_edges.push_back(State{step.x - 1, step.y});
                    next_edges.push_back(State{step.x + 1, step.y});
                    next_edges.push_back(State{step.x, step.y - 1});
                    next_edges.push_back(State{step.x, step.y + 1});
            }
            processed.insert(step);
        }
        now_edges = next_edges;
        next_edges = list<State>{};
        ++commands;
    }
    return commands;
}

int fill_oxygen(shipmap& sm){
    int commands{0};
    State oxygen_xy{0, 0};
    for (auto [xy, v]: sm){
        if (v == 2){
            oxygen_xy.x = xy.first;
            oxygen_xy.y = xy.second;
            break;
        }
    }
    unique_ptr<list<State>> now_edges{
        std::make_unique<list<State>>(initializer_list<State>{oxygen_xy})};
    unique_ptr<list<State>> next_edges{std::make_unique<list<State>>()};
    set<State> processed{};
    while (commands < 1000 && now_edges->size() > 0){
        for (State step: *now_edges){
            if (processed.find(step) != processed.end()) continue;
            switch (sm[pair(step.x, step.y)]){
                case 0:
                    break;
                case 2:
                case -1:
                case 1:
                    sm[pair(step.x, step.y)] = -1;
                    next_edges->push_back(State{step.x - 1, step.y});
                    next_edges->push_back(State{step.x + 1, step.y});
                    next_edges->push_back(State{step.x, step.y - 1});
                    next_edges->push_back(State{step.x, step.y + 1});
            }
            processed.insert(step);
        }
        now_edges = std::move(next_edges);
        next_edges = make_unique<list<State>>();
        ++commands;
    }
    return commands - 2;
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
        auto ic = IntcodeComputer(program);
        auto [sm, pl] = walk(ic);
        cout << min_commands_to_oxygen(sm) << endl;
    }
    if (result.count("2")) {
        ifstream inf{result.unmatched()[0]};
        vector<long> program{my_parse(inf)};
        auto ic = IntcodeComputer(program);
        auto [sm, pl] = walk(ic);
        cout << fill_oxygen(sm) << endl;
    }
    if (result.count("day-02")) run_day_2();
    if (result.count("day-05")) run_day_5();
    return 0;
}
