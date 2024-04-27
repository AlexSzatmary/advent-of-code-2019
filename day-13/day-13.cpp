#include <cxxopts.hpp>
#include "Eigen/Dense"
#include <list>
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <numeric>
#include <regex>
#include <string>
#include "intcode.hpp"
#include "run_previous_days.hpp"

using namespace std;

void raster(list<long> x, list<long> y, list<long> tile){
    const auto [min_x, max_x] = minmax_element(begin(x), end(x));
    const auto [min_y, max_y] = minmax_element(begin(y), end(y));
    Eigen::ArrayXXi window{
        Eigen::ArrayXXi::Zero((int) *max_y - *min_y + 1, (int) *max_x - *min_x + 1)};
    auto x_it = begin(x);
    auto y_it = begin(y);
    auto tile_it = begin(tile);
    for (int i{0}; x_it != end(x); ++i){
        window(*y_it, *x_it) = *tile_it;
        ++x_it;
        ++y_it;
        ++tile_it;
    }
    for (int i{0}; i < window.rows(); ++i){
        for (int j{0}; j < window.cols(); ++j){
            switch (window(i, j)){
            case 0:
                cout << ' ';
                break;
            case 1:            
                cout << '|';
                break;
            case 2:
                cout << 'X';
                break;
            case 3:
                cout << '-';
                break;
            case 4:
                cout << 'o';
                break;
            default:
                break;
            }
        }
        cout << endl;
    }
    return;
}

int read_key(){
    char key{};
    int button{};
    cout << "Push key asd" << endl;
    cin >> key;
    switch (key)
    {
    case 'a':
        button = -1;
        break;
    case 's':
        button = 0;
        break;
    case 'd':
        button = 1;
        break;
    default:
        break;
    }
    return button;
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
        ic.interpret();
        int n_blocks{0};
        for (auto p{begin(ic.output_buffer)}; p != end(ic.output_buffer); ++p){
            ++p;
            ++p;
            if (*p == 2) n_blocks += 1;
        }
        cout << n_blocks << endl;
    }
    if (result.count("2")) {
        ifstream inf{result.unmatched()[0]};
        vector<long> program{my_parse(inf)};
        program.at(0) = 2;
        auto ic = IntcodeComputer(program);
        int score{};
        while (!ic.halt){
            ic.interpret();
            list<long> x{};
            list<long> y{};
            list<long> tile{};
            int block_x{};
            int ball_x{};
            for (auto p{begin(ic.output_buffer)}; p != end(ic.output_buffer); ++p){
                if (*p == -1) {
                    ++p;
                    ++p;
                    score = *p;
                    continue;
                }
                x.push_back(*p);
                ++p;
                y.push_back(*p);
                ++p;
                tile.push_back(*p);
                switch (tile.back())
                {
                case 3: // paddle
                    block_x = x.back();
                    break;
                case 4: // ball
                    ball_x = x.back();
                    break;
                default:
                    break;
                }
            }
            raster(x, y, tile);
            cout << "Score: " << score << endl;
            // the following line enables interactive play; if you go that way,
            // comment the if after that.
            // ic.input_buffer.push_back(read_key());
            if (ball_x > block_x) {
                ic.input_buffer.push_back(1);
            } else if (ball_x < block_x) {
                ic.input_buffer.push_back(-1);
            } else {
                ic.input_buffer.push_back(0);
            }
        }
    }
    if (result.count("day-02")) run_day_2();
    if (result.count("day-05")) run_day_5();
    return 0;
}
