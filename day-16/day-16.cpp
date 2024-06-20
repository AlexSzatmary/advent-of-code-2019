#include "cxxopts.hpp"
#include "Eigen/Dense"
#include <fstream>
#include <iostream>
#include <string>

using namespace std;

Eigen::VectorXi my_parse(ifstream &inf){
    string line{};
    getline(inf, line);
    Eigen::VectorXi signal(line.size(), 1);
    for (int i{0}; i < line.size(); ++i){
        signal(i) = int(line.at(i)) - '0';
    }
    return signal;
}

Eigen::VectorXi FFT(Eigen::VectorXi signal, int phases){
    int pattern[4] = {0, 1, 0, -1};
    Eigen::MatrixXi A{Eigen::MatrixXi::Zero(signal.size(), signal.size())};
    for (int i{0}; i < signal.size(); ++i){
        int i_p{0};
        int k{0};
        int p{0};
        for (int j{0}; j < signal.size(); ++j){
            ++k;
            if (k > i) {
                i_p = (i_p + 1) % 4;
                p = pattern[i_p];
                k = 0;
            }
            A(i, j) = p;
        }
    }
    cout << A << endl;
    for (int i{0}; i < phases; ++i){
        signal = (A * signal).array().abs().unaryExpr([](int j){return j % 10;});
    }
    return signal;
}

Eigen::VectorXi FFT_part_2(Eigen::VectorXi signal, int phases){
    // uses observation that we're looking for an answer from the second half of the
    // result, which dramatically simplifies things
    for (int i{0}; i < phases; ++i){
        for (long j{signal.rows() - 2}; j >= 0; --j){
            signal(j) = (signal(j) + signal(j + 1)) % 10;
        }
    }
    return signal;
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
        auto signal = my_parse(inf);
        auto FFT_signal{FFT(signal, 100)};
        for (int i{0}; i < 8; ++i) cout << FFT_signal(i);
        cout << endl;
    }
    if (result.count("2")) {
        ifstream inf{result.unmatched()[0]};
        auto signal = my_parse(inf);
        inf.seekg(0);
        string line{};
        getline(inf, line);
        int offset{stoi(line.substr(0, 7))};
        Eigen::VectorXi big_signal{signal.replicate(10000, 1)};
        auto offset_signal{big_signal.tail(big_signal.rows() - offset)};
        auto FFT_signal{FFT_part_2(big_signal.tail(big_signal.rows() - offset), 100)};
        for (int i{0}; i < 8; ++i) cout << FFT_signal(i);
        cout << endl;
    }
    return 0;
}