#include "cxxopts.hpp"
#include "Eigen/Dense"
#include <fstream>
#include <iostream>
#include <numeric>
#include <vector>

using namespace std;

Eigen::ArrayXXi my_parse(ifstream &inf){
    string line;
    vector<string> lines;
    while(getline(inf, line)){
        lines.push_back(line);
    }
    Eigen::ArrayXXi asteroid_map = Eigen::ArrayXXi::Zero(lines.size(), line.size());
    for (int y{0}; y < lines.size(); ++y){
        line = lines.at(y);
        for (int x{0}; x < line.size(); ++x){
            asteroid_map(y, x) = (line.at(x) == '#' ? 1 : 0);
        }
    }
    return asteroid_map;
}

int try_this_station_loc(Eigen::ArrayXXi asteroid_map, int x0, int y0){
    Eigen::ArrayXXi scratch_map = Eigen::ArrayXXi::Zero(asteroid_map.rows(),
        asteroid_map.cols());
    for (int y{0}; y < asteroid_map.rows(); ++y){
        for (int x{0}; x < asteroid_map.cols(); ++x){
            if (asteroid_map(y, x)){
                int dx{x - x0};
                int dy{y - y0};
                if (dx == 0) {
                    if (dy != 0){ // do not count self
                        dy /= abs(dy);
                        scratch_map(y0 + dy, x0) = 1;                        
                    }
                } else {
                    if (dy == 0) {
                        dx /= abs(dx);
                        scratch_map(y0, x0 + dx) = 1;                        
                    } else {
                        int g{gcd(dx, dy)};
                        dx /= g;
                        dy /= g;
                        scratch_map(y0 + dy, x0 + dx) = 1;
                    }
                }
            }
        }
    }
    return scratch_map.sum();
}

tuple<int, int, int> find_best_station_loc(Eigen::ArrayXXi asteroid_map){
    int best_x{0};
    int best_y{0};
    int best_asteroids{0};
    for (int y{0}; y < asteroid_map.rows(); ++y){
        for (int x{0}; x < asteroid_map.cols(); ++x){
            if (asteroid_map(y, x)){
                int asteroids{try_this_station_loc(asteroid_map, x, y)};
                if (asteroids > best_asteroids){
                    best_x = x;
                    best_y = y;
                    best_asteroids = asteroids;
                }
            }
        }
    }
    return tuple(best_x, best_y, best_asteroids);
}

int main(int argv, char **argc){
    cxxopts::Options options("test", "A brief description");
    options.add_options()
        ("1", "Solve part 1", cxxopts::value<bool>())
        ("2", "Solve part 2", cxxopts::value<bool>())
        ("h,help", "Print usage");
    auto result = options.parse(argv, argc);

    if (result.count("help")){
        cout << options.help() << endl;
        exit(0);
    }
    if (result.count("1")) {
        ifstream inf{result.unmatched()[0]};
        auto asteroid_map = my_parse(inf);
        auto [best_x, best_y, best_asteroids] = find_best_station_loc(asteroid_map);
        cout << best_x << ", " << best_y << endl;
        cout << best_asteroids << endl;
    }
    if (result.count("2")) {
    }
    return 0;
}