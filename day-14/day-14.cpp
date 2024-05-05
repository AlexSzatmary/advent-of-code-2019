#include "cxxopts.hpp"
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <string>
#include <vector>

using namespace std;

struct equation{
    long product_coeff{};
    vector<string> reactant{};
    vector<long> reactant_coeff{};
};

map<string, equation> my_parse(ifstream &inf){
    string line;
    map<string, equation> reaction_network{};
    regex product_re{"(.*) => ([0-9]+) ([A-z]+)"};
    regex reactant_re("([0-9]+) ([A-z]+)");
    long row{0};
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

map<string, long> count_references(map<string, equation> reaction_network){
    map<string, long> references{{"ORE", 0}};
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

long find_needed_ore(map<string, equation> reaction_network, long fuel_order){
    map<string, long> orders{{"ORE", 0}};
    // ORE is not a product and thus must be initialized.
    for (auto [product, eq]: reaction_network) orders[product] = 0;
    orders["FUEL"] = fuel_order; // FUEL is the only product that starts with an order. 
    auto references = count_references(reaction_network);  // This is re-counted 
    // upon repeated calls to this function, which is a little wasteful.
    list<string> leaves{};
    for (auto [product, refs]: references){
        if (refs == 0) leaves.push_back(product);
    }

    while (!(leaves.front() == "ORE")){
        string leaf = leaves.front();
        leaves.pop_front();
        equation eq = reaction_network.at(leaf);
        long times_to_run{(orders[leaf] - 1) / eq.product_coeff + 1};
        for (long i{0}; i < eq.reactant.size(); ++i){
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
        cout << find_needed_ore(reaction_network, 1) << endl;
    }
    if (result.count("2")) {
        long starting_ore{1'000'000'000'000};
        ifstream inf{result.unmatched()[0]};
        auto reaction_network = my_parse(inf);
        long lo_fuel{0};
        long hi_fuel{4};
        long lo_ore{find_needed_ore(reaction_network, lo_fuel)};
        long hi_ore{find_needed_ore(reaction_network, hi_fuel)};
        while (hi_ore < starting_ore){
            lo_fuel = hi_fuel;
            lo_ore = hi_ore;
            hi_fuel *= 2;
            hi_ore = find_needed_ore(reaction_network, hi_fuel);
        }
        while (lo_fuel + 1 < hi_fuel){
            long nu_fuel{(lo_fuel + hi_fuel) >> 1};
            long nu_ore{find_needed_ore(reaction_network, nu_fuel)};
            if (nu_ore > starting_ore) hi_fuel = nu_fuel; else lo_fuel = nu_fuel;
        }
        cout << lo_fuel << endl;
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