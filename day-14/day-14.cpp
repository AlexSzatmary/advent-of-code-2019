#include "cxxopts.hpp"
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <string>
#include <vector>

using namespace std;

struct equation{
    int product_coeff{};
    vector<string> reactant{};
    vector<int> reactant_coeff{};
};

map<string, equation> my_parse(ifstream &inf){
    string line;
    map<string, equation> reaction_network{};
    regex product_re{"(.*) => ([0-9]+) ([A-z]+)"};
    regex reactant_re("([0-9]+) ([A-z]+)");
    int row{0};
    while (getline(inf, line) && !line.empty()){
        smatch m_product;
        regex_match(line, m_product, product_re);
        reaction_network[m_product.str(3)] = equation{stoi(m_product.str(2))};
        string reactant_str{m_product[1]};
        // auto m_reactant_iter = sregex_iterator(
        //         begin(m_product.str(1)), end(m_product.str(1)), reactant_re);
        auto m_reactant_iter{sregex_iterator(
                begin(reactant_str), end(reactant_str), reactant_re)};
        auto m_reactant_iter_end{sregex_iterator()};
        // cout << m_product.str(1) << '-' << m_product.str(2) << '-' << m_product.str(3) << endl;
        // cout << distance(m_reactant_iter, m_reactant_iter_end) << endl;
        for (sregex_iterator i = m_reactant_iter; i != m_reactant_iter_end; ++i){
            reaction_network[m_product.str(3)].reactant_coeff.push_back(
                stoi(i->str(1)));
            reaction_network[m_product.str(3)].reactant.push_back(
                i->str(2));
            // cout << i->str(1) << " " << i->str(2) << endl;
        }
    }
    return reaction_network;
}

map<string, int> count_references(map<string, equation> reaction_network){
    map<string, int> references{{"ORE", 0}};
    for (const auto& [product, eq]: reaction_network){
        references[product] = 0;
    }
    for (const auto& [product, eq]: reaction_network){
        for (const auto& r: eq.reactant){
            ++references.at(r);
        }
    }
    return references;
}

int find_needed_ore(map<string, equation> reaction_network){
    map<string, int> orders{{"ORE", 0}};
    // ORE is not a product and thus must be initialized.
    for (auto [product, eq]: reaction_network) orders[product] = 0;
    orders["FUEL"] = 1; // FUEL is the only product that starts with an order. 
    auto references = count_references(reaction_network);
    list<string> leaves{};
    for (auto [product, refs]: references){
        if (refs == 0) leaves.push_back(product);
    }

    while (!(leaves.front() == "ORE")){
        string leaf = leaves.front();
        leaves.pop_front();
        equation eq = reaction_network.at(leaf);
        int times_to_run{(orders[leaf] - 1) / eq.product_coeff + 1};
        for (int i{0}; i < eq.reactant.size(); ++i){
            orders.at(eq.reactant.at(i)) += times_to_run * eq.reactant_coeff.at(i);
            --references.at(eq.reactant.at(i));
            if (references.at(eq.reactant.at(i)) == 0){
                leaves.push_back(eq.reactant.at(i));
            }
        }
    }
    return orders.at("ORE");
}

int main(int argv, char **argc){
    cxxopts::Options options("test", "A brief description");
    options.add_options()
        ("1", "Solve part 1", cxxopts::value<bool>())
        ("2", "Solve part 2", cxxopts::value<bool>())
        ("d", "Make graphviz dot file", cxxopts::value<bool>())
        ("h,help", "Print usage");
    auto result = options.parse(argv, argc);

    if (result.count("help")){
        cout << options.help() << endl;
        exit(0);
    }
    if (result.count("1")) {
        ifstream inf{result.unmatched()[0]};
        auto reaction_network = my_parse(inf);
        cout << find_needed_ore(reaction_network) << endl;
    }
    if (result.count("2")) {
    }
    if (result.count("d")){
        ifstream inf{result.unmatched()[0]};
        auto reaction_network = my_parse(inf);
        cout << "digraph{" << endl;
        for (auto& [product, eq]: reaction_network){
            cout << '{';
            for (auto& r: eq.reactant){
                cout << r << ' ';
            }
            cout << "} -> " << product << endl;
        }
        cout << "}" << endl;
    }
    return 0;
}