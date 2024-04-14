#include "cxxopts.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>

using namespace std;

auto const WIDTH{25};
auto const HEIGHT{6};
auto const LAYER_SIZE{WIDTH * HEIGHT};

vector<string> my_parse(ifstream &inf){
    string line;
    getline(inf, line);
    vector<string> layers{};
    for (int i{0}; i + LAYER_SIZE <= line.size(); i += LAYER_SIZE){
        layers.push_back(line.substr(i, LAYER_SIZE));
    }
    return layers;
}

string get_least_corrupted_layer(vector<string> layers){
    return *min_element(begin(layers), end(layers),
    [](string sa, string sb){
        return count(begin(sa), end(sa), '0') < count(begin(sb), end(sb), '0');
    });
}

int count_1_by_count_2(string layer){
    return count(begin(layer), end(layer), '1') * count(begin(layer), end(layer), '2');
}

string stack_layers(vector<string> layers){
    string stacked(LAYER_SIZE, ' ');
    for (const auto& layer: layers){
        for (int i{0}; i < LAYER_SIZE; ++ i){
            if (stacked[i] == ' ') {
                if (layer[i] == '0') {
                    stacked[i] = '.';
                } else if (layer[i] == '1') {
                    stacked[i] = 'X';
                }
            }
        }
    }
    return stacked;
}

void render_stack(string stack){
    for (int i{0}; i < HEIGHT; ++i){
        cout << stack.substr(i * WIDTH, WIDTH) << endl;
    }
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
        auto layers{my_parse(inf)};
        string least_corrupted = get_least_corrupted_layer(layers);
        cout << count_1_by_count_2(least_corrupted) << endl;
    }
    if (result.count("2")) {
        ifstream inf{result.unmatched()[0]};
        auto layers{my_parse(inf)};
        auto stack{stack_layers(layers)};
        render_stack(stack);
    }
    return 0;
}