#include "cxxopts.hpp"
#include "Eigen/Dense"
#include <fstream>
#include <iostream>
#include <map>
#include <numeric>
#include <tuple>
#include <vector>

using namespace std;

Eigen::ArrayXXi my_parse(ifstream &inf){
    string line;
    Eigen::ArrayXXi velocity{Eigen::ArrayXXi::Zero(4, 3)};
    regex re(".*?(-?[0-9]+).*?(-?\\d+).*?(-?\\d+).*");
    int row{0};
    while (getline(inf, line)){
        smatch m;
        regex_match(line, m, re);
        velocity(row, 0) = stoi(m.str(1));
        velocity(row, 1) = stoi(m.str(2));
        velocity(row, 2) = stoi(m.str(3));
        ++row;
    }
    return velocity;
}

pair<Eigen::ArrayXXi, Eigen::ArrayXXi> iterate(Eigen::ArrayXXi x_1,
        Eigen::ArrayXXi v_1){
    Eigen::ArrayXXi x_2{x_1};
    Eigen::ArrayXXi v_2{v_1};
    for (int j{0}; j < 3; ++j){
        for (int i1{0}; i1 < 3; ++i1){
            for (int i2{i1 + 1}; i2 < 4; ++i2){
                if (x_1(i1, j) < x_1(i2, j)){
                    ++v_2(i1, j);
                    --v_2(i2, j);
                } else if (x_1(i1, j) > x_1(i2, j)){
                    --v_2(i1, j);
                    ++v_2(i2, j);
                }
            }
        }
    }
    x_2 += v_2;
    return pair(x_2, v_2);
}

int calculate_energy(Eigen::ArrayXXi x, Eigen::ArrayXXi v){
    return (abs(x).rowwise().sum() * abs(v).rowwise().sum()).sum();
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
        auto x0 = my_parse(inf);
        Eigen::ArrayXXi v0{Eigen::ArrayXXi::Zero(4, 3)};
        Eigen::ArrayXXi x{x0};
        Eigen::ArrayXXi v{v0};
        for (int i{1}; i <= 1000; ++i){
            tie(x, v) = iterate(x, v);
        }
        cout << calculate_energy(x, v) << endl;
    }
    if (result.count("2")) {
    }
    return 0;
}