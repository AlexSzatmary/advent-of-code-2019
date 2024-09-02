#include <cxxopts.hpp>
#include <list>
#include <vector>
#include <fstream>
#include <iostream>
#include "intcode.hpp"
#include "run_previous_days.hpp"

using namespace std;
using std::cout;

int sum_alignment_parameters(IntcodeComputer ic){
    int j{0};
    int i{0};
    int width{0};
    int sum_alignment_parameters{0};
    vector<char> v{};
    for (auto x: ic.output_buffer){
        auto c{static_cast<char>(x)};
        switch (c)
        {
        case '\n':
            if (width == 0) width = j + 1;
            j = -1;
            ++i;
            // cout << c;
            break;
        case '#':
            if (j > 1 && i > 0 &&
            v.back() == '#' &&
            v.at(v.size() - 2) == '#' &&
            v.at((i - 1) * width + j - 1) == '#'){
                sum_alignment_parameters += (j - 1) * i;
                // cout << 'O';
            } else {
                // cout << c;
            }
            break;
        default:
            // cout << c;
            break;
        }
        v.push_back(static_cast<char>(c));
        ++j;
    }
    return sum_alignment_parameters;
}

char access(vector<vector<char>> v, int i, int j){
    if (i < 0 || j < 0 || i >= v.size() - 2 || j >= v.front().size()){
        return '.';
    } else return v.at(i).at(j);
}

pair<int, int> dir_ij(int i, int j, int dir){
    switch (dir)
    {
    case 0:
        return pair(i, j + 1);
        break;
    case 1:
        return pair(i - 1, j);
        break;
    case 2:
        return pair(i, j - 1);
        break;
    case 3:
        return pair(i + 1, j);
        break;
    default:
        return pair(0, 0);
        break;
    }
}

char access_dir(vector<vector<char>> v, int i, int j, int dir){
    auto [i_dir, j_dir] = dir_ij(i, j, dir);
    return access(v, i_dir, j_dir);
}

string make_path(IntcodeComputer ic){
    string path{};
    int j{0};
    int i{0};
    int start_i{0};
    int start_j{0};
    int width{0};
    int sum_alignment_parameters{0};
    vector<vector<char>> v{};
    v.push_back(vector<char>());
    for (auto x: ic.output_buffer){
        auto c{static_cast<char>(x)};
        switch (c)
        {
        case '\n':
            if (width == 0) width = j;
            j = -1;
            ++i;
            v.push_back(vector<char>());
            break;
        case '^':
        case '<':
        case '>':
        case 'v':
            // I'm faking this case and know that it's initially pointing ^
            start_i = i;
            start_j = j;
            v.back().push_back(static_cast<char>(c));
            break;
        default:
            v.back().push_back(static_cast<char>(c));
            break;
        }
        ++j;
    }
    int height{static_cast<int>(v.size())};
    i = start_i;
    j = start_j;

    int dir{1};

    // faking it
    path += "R,";
    --dir;

    while (true){
        int fwd{0};
        while (access_dir(v, i, j, dir) == '#'){
            ++fwd;
            tie(i, j) = dir_ij(i, j, dir);
        }
        path += (to_string(fwd) + ',');
        if (access_dir(v, i, j, (dir + 1) % 4) == '#') {
            dir = (dir + 1 + 4) % 4;
            path += "L,";
        } else if (access_dir(v, i, j, (dir - 1 + 4) % 4) == '#')
        {
            dir = (dir - 1 + 4) % 4;
            path += "R,";
        } else break;
    }
    return path.substr(0, path.size() - 1);
}

int count_all(string const & str, string const & substr){
    int instances{0};
    for (size_t n{str.find(substr, 0)}; n != string::npos; n = str.find(substr, n + 1))
        ++instances;
    return instances;
}

string pick_best_function(string const & str){
    int const memory_limit{20};
    size_t start{0};
    bool flag{true};
    while (flag){
        switch (str.at(start))
        {
        case ',':
        case 'A':
        case 'B':
        case 'C':
            ++start;
            break;
        default:
            flag = false;
            break;
        }
    }
    size_t best_end{start + 1};
    size_t best_end_score{0};
    for (size_t end{best_end}; end < min(start + memory_limit, str.size()); ++end){
        switch (str.at(end - 1))
        {
        case ',':
            break; // no need to check just a comma
        case 'A':
        case 'B':
        case 'C':
            // if you found a function name, you went too far
            end = start + memory_limit;
            break;
        default:
            size_t score{(end - start) *
                count_all(str, str.substr(start, end - start))};
            if (score > best_end_score){
                best_end = end;
                best_end_score = score;
            }
            break;
        }
    }
    return str.substr(start, best_end - start);
}

string replace_all(string const & str, string const & oldstr, string const & newstr){
    string replaced{str};
    string::size_type n{replaced.find(oldstr)};
    while (n != string::npos){
        replaced.replace(n, oldstr.size(), newstr);
        n = replaced.find(oldstr);
    }
    return replaced;
}

tuple<string, string, string, string> compress(string const & path){
    string A{pick_best_function(path)};
    string pathA{replace_all(path, A, "A")};
    string B{pick_best_function(pathA)};
    string pathB{replace_all(pathA, B, "B")};
    string C{pick_best_function(pathB)};
    string pathC{replace_all(pathB, C, "C")};
    return tuple(A, B, C, pathC);
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
        cout << sum_alignment_parameters(ic) << endl;
    }
    if (result.count("2")) {
        ifstream inf{result.unmatched()[0]};
        vector<long> program{my_parse(inf)};
        auto ic = IntcodeComputer(program);
        ic.interpret();
        string path{make_path(ic)};
        cout << path << endl;
        cout << replace_all(path, "R,6,L,10,R,10", "A") << endl;
        cout << count_all(path, "R,6,L,10,R,10") << endl;
        auto [A, B, C, main] = compress(path);
        cout << A << endl;
        cout << B << endl;
        cout << C << endl;
        cout << main << endl;

        ic = IntcodeComputer(program);
        ic.program.at(0) = 2;
        ic.interpret();
        ic.print_output_buffer_ascii();
        ic.output_buffer.clear();

        // Main
        for (char c: string(main)){
            cout << c;
            ic.input_buffer.push_back(c);
        }
        cout << endl;
        ic.input_buffer.push_back('\n');
        ic.interpret();
        ic.print_output_buffer_ascii();
        ic.output_buffer.clear();

        // A
        for (char c: string(A)){
            cout << c;
            ic.input_buffer.push_back(c);
        }
        cout << endl;
        ic.input_buffer.push_back('\n');
        ic.interpret();
        ic.print_output_buffer_ascii();
        ic.output_buffer.clear();

        // B
        for (char c: string(B)){
            cout << c;
            ic.input_buffer.push_back(c);
        }
        cout << endl;
        ic.input_buffer.push_back('\n');
        ic.interpret();
        ic.print_output_buffer_ascii();
        ic.output_buffer.clear();

        // C
        for (char c: string(C)){
            cout << c;
            ic.input_buffer.push_back(c);
        }
        cout << endl;
        ic.input_buffer.push_back('\n');
        ic.interpret();
        ic.print_output_buffer_ascii();
        ic.output_buffer.clear();

// R,6,L,10,R,10,R,10,L,10,L,12,R,10, A
// R,6,L,10,R,10,R,10,L,10,L,12,R,10,
// R,6,L,10,R,10,R,10,
// R,6,L,12,L,10,
// R,6,L,10,R,10,R,10, A
// R,6,L,12,L,10, C
// L,10,L,12,R,10, B
// R,6,L,12,L,10, C

// R,6,L,10,R,10,R,10, A
// L,10,L,12,R,10, B
// R,6,L,10,R,10,R,10, A
// L,10,L,12,R,10, B
// R,6,L,10,R,10,R,10, A
// R,6,L,12,L,10, C
// R,6,L,10,R,10,R,10, A
// R,6,L,12,L,10, C
// L,10,L,12,R,10, B
// R,6,L,12,L,10, C

        // continuous video feed y/n
        for (char c: string("n\n")){
            cout << c;
            ic.input_buffer.push_back(c);
        }
        ic.interpret();
        ic.print_output_buffer_ascii();
        // ic.output_buffer.clear();

        // while (!ic.halt){
        //     ic.interpret();
        //     if (!ic.halt){
        //         ic.print_output_buffer_ascii();
        //         ic.output_buffer.clear();
        //     }
        // }
        cout << endl << ic.output_buffer.back() << endl;
    }
    if (result.count("day-02")) run_day_2();
    if (result.count("day-05")) run_day_5();
    return 0;
}
