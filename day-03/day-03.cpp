#include "cxxopts.hpp"
#include "Eigen/Dense"
#include <fstream>
#include <iostream>
#include <limits>
#include <numeric>
#include <regex>
#include <string>
#include <utility>

using namespace std;

struct Step{
    char dir;
    int dist;
};

vector<Step> my_parse(ifstream &inf){
    string line;
    getline(inf, line);
    regex re("([A-Z])(\\d+)");
    vector<Step> wire_path;
    transform(sregex_iterator(begin(line), end(line), re),
        sregex_iterator(),
        back_inserter(wire_path),
        [](smatch m){return Step{m.str(1)[0], stoi(m.str(2))};});
    return wire_path;
}

tuple<int, int, int, int> measure(vector<Step> wire){
    int x{0};
    int y{0};
    int min_x{0};
    int max_x{0};
    int min_y{0};
    int max_y{0};
    for (auto step: wire){
        switch (step.dir) {
            case 'R':
                x += step.dist;
                break;
            case 'U':
                y += step.dist;
                break;
            case 'L':
                x -= step.dist;
                break;
            case 'D':
                y -= step.dist;
                break;
        }
        min_x = min(min_x, x);
        max_x = max(max_x, x);
        min_y = min(min_y, y);
        max_y = max(max_y, y);
    }
    return tuple(min_x, max_x, min_y, max_y);
}

tuple<Eigen::ArrayXXi, int, int, int, int> render(vector<Step> wire){
    // to make (x, y) coordinates printable from an Eigen matrix, I swap x and y
    // and flip y. Or, like, what I'm calling y here would be a reasonable i in a_ij
    // style notation. x and y are also offset by min_x and max_y (again, y is flipped).
    using namespace Eigen;
    auto [min_x, max_x, min_y, max_y] = measure(wire);
    ArrayXXi wire_diagram = ArrayXXi::Zero(max_y - min_y + 1, max_x - min_x + 1);
    int j{-min_x};
    int i{max_y};
    int time{1};
    for (auto step: wire){
        switch (step.dir) {
            case 'R':
                wire_diagram(i, seqN(j + 1, step.dist)) = Eigen::ArrayXi::LinSpaced(
                    step.dist, time, time + step.dist - 1);
                j += step.dist;
                break;
            case 'U':
                wire_diagram(seqN(i - step.dist, step.dist), j) =
                    Eigen::ArrayXi::LinSpaced(
                    step.dist, time, time + step.dist - 1).reverse();
                i -= step.dist;
                break;
            case 'L':
                wire_diagram(i, seqN(j - step.dist, step.dist)) = 
                Eigen::ArrayXi::LinSpaced(
                    step.dist, time, time + step.dist - 1).reverse();
                j -= step.dist;
                break;
            case 'D':
                wire_diagram(seqN(i + 1, step.dist), j) =                     Eigen::ArrayXi::LinSpaced(
                    step.dist, time, time + step.dist - 1);
                i += step.dist;
                break;
        }
        time += step.dist;
    }
    return tuple(wire_diagram, min_x, max_x, min_y, max_y);
}

int find_manhattan_distance_to_cross(vector<Step> wire_1, vector<Step> wire_2){
    Eigen::ArrayXXi wire_1_diagram;
    int min_x, max_x, min_y, max_y;
    tie(wire_1_diagram, min_x, max_x, min_y, max_y) = render(wire_1);
    int j{-min_x};
    int i{max_y};
    // Start with a very large possible Manhattan distance
    int manhattan_distance = wire_1_diagram.rows() + wire_1_diagram.cols();
    int rows{(int) wire_1_diagram.rows()};
    int cols{(int) wire_1_diagram.cols()};
    auto f = [&](int i, int j, int manhattan_distance){
        if (0 <= i && i < rows &&
            0 <= j && j < cols &&
            wire_1_diagram(i, j) > 0 &&
            abs(i - max_y) + abs(j + min_x) < manhattan_distance){
                        manhattan_distance = abs(i - max_y) + abs(j + min_x);
                    }
        return manhattan_distance;
    };
    for (auto step: wire_2){
        switch (step.dir) {
            case 'R':
                ++j;
                for (int next_j{j + step.dist - 1}; j <= next_j; ++j){
                    manhattan_distance = f(i, j, manhattan_distance);
                }
                --j;
                break;
            case 'U':
                --i;
                for (int next_i{i - step.dist + 1}; i >= next_i; --i){
                    manhattan_distance = f(i, j, manhattan_distance);
                }
                ++i;
                break;
            case 'L':
                --j;
                for (int next_j{j - step.dist + 1}; j >= next_j; --j){
                    manhattan_distance = f(i, j, manhattan_distance);
                }
                ++j;
                break;
            case 'D':
                ++i;
                for (int next_i{i + step.dist - 1}; i <= next_i; ++i){
                    manhattan_distance = f(i, j, manhattan_distance);
                }
                --i;
                break;
        }
    }
    return manhattan_distance;
}

int find_total_time_to_cross(vector<Step> wire_1, vector<Step> wire_2){
    Eigen::ArrayXXi wire_1_diagram;
    int min_x, max_x, min_y, max_y;
    tie(wire_1_diagram, min_x, max_x, min_y, max_y) = render(wire_1);
    int j{-min_x};
    int i{max_y};
    // Start with a very large possible total_time
    int total_time = numeric_limits<int>::max() ;
    int rows{(int) wire_1_diagram.rows()};
    int cols{(int) wire_1_diagram.cols()};
    auto f = [&](int i, int j, int time, int total_time){
        if (0 <= i && i < rows &&
            0 <= j && j < cols &&
            wire_1_diagram(i, j) > 0 &&
            time + wire_1_diagram(i, j) < total_time){
                        total_time = time + wire_1_diagram(i, j);
                    }
        return total_time;
    };
    int time{0};
    for (auto step: wire_2){
        switch (step.dir) {
            case 'R':
                ++j;
                for (int next_j{j + step.dist - 1}; j <= next_j; ++j){
                    ++time;
                    total_time = f(i, j, time, total_time);
                }
                --j;
                break;
            case 'U':
                --i;
                for (int next_i{i - step.dist + 1}; i >= next_i; --i){
                    ++time;
                    total_time = f(i, j, time, total_time);
                }
                ++i;
                break;
            case 'L':
                --j;
                for (int next_j{j - step.dist + 1}; j >= next_j; --j){
                    ++time;
                    total_time = f(i, j, time, total_time);
                }
                ++j;
                break;
            case 'D':
                ++i;
                for (int next_i{i + step.dist - 1}; i <= next_i; ++i){
                    ++time;
                    total_time = f(i, j, time, total_time);
                }
                --i;
                break;
        }
    }
    return total_time;
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
        vector<Step> wire_1{my_parse(inf)};
        vector<Step> wire_2{my_parse(inf)};
        cout << find_manhattan_distance_to_cross(wire_1, wire_2) << endl;
    }
    if (result.count("2")) {
        ifstream inf{result.unmatched()[0]};
        vector<Step> wire_1{my_parse(inf)};
        vector<Step> wire_2{my_parse(inf)};
        cout << find_total_time_to_cross(wire_1, wire_2) << endl;
    }
    return 0;
}