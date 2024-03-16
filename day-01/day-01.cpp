#include "cxxopts.hpp"
#include <fstream>
#include <iostream>
#include <numeric>
#include <string>

using namespace std;

vector<int>my_parse(ifstream &inf){
    vector<int> masses;
    string line;
    while (getline(inf, line)){
        masses.push_back(stoi(line));
    }
    return masses;
}

int total_fuel_1(vector<int> masses){
    return transform_reduce(masses.begin(), masses.end(), 0L, plus{},
    [](int mass){return mass / 3 - 2;});
}

int rocket_problem_fuel(int module_mass){
    int needed_fuel_mass{0};
    int incremental_fuel_mass{module_mass};
    while (incremental_fuel_mass > 0) {
        incremental_fuel_mass = incremental_fuel_mass / 3 - 2;
        if (incremental_fuel_mass > 0){
            needed_fuel_mass += incremental_fuel_mass;
        }
    }
    return needed_fuel_mass;
}

int total_fuel_2(vector<int> masses){
    return transform_reduce(masses.begin(), masses.end(), 0L, plus{},
    (*rocket_problem_fuel));
}

int main(int argv, char **argc){
    cxxopts::Options options("test", "A brief description");
    options.add_options()
        ("1", "Solve part 1", cxxopts::value<bool>())
        ("2", "Solve part 2", cxxopts::value<bool>())
        // ("f,foo", "Param foo", cxxopts::value<int>()->default_value("10"))
        ("h,help", "Print usage");
    auto result = options.parse(argv, argc);

   if (result.count("help"))
    {
      std::cout << options.help() << std::endl;
      exit(0);
    }
    if (result.count("1")) {
        ifstream inf{result.unmatched()[0]};
        vector<int> masses;
        masses = my_parse(inf);
        cout << total_fuel_1(masses) << endl;
    }
    if (result.count("2")) {
        ifstream inf{result.unmatched()[0]};
        vector<int> masses;
        masses = my_parse(inf);
        cout << total_fuel_2(masses) << endl;
    }
    return 0;
}