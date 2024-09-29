#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "Eigen/Dense"
#include "cxxopts.hpp"

using namespace std;
// distgraph says for each node which nodes are connected and by what distance
using distgraph = map<char, map<char, int>>;
using std::cout;

vector<string> my_parse(ifstream& inf) {
  string line{};
  vector<string> maze{};
  while (getline(inf, line) && !line.empty()) {
    maze.push_back(line.substr(0, line.size()));
  }
  return maze;
}

struct Coord {
  int i;
  int j;
};

inline bool operator<(const Coord& lhs, const Coord& rhs) {
  return tie(lhs.i, lhs.j) < tie(rhs.i, rhs.j);
}

void flood_fill_maze(vector<string> maze, int i, int j, distgraph& dg) {
  int dist{1};
  char home{maze.at(i).at(j)};
  list<Coord> now_edges{Coord{i, j}};
  list<Coord> next_edges{};
  set<Coord> processed{};
  dg[home] = map<char, int>{};
  map<char, int>& home_edges{dg[home]};
  while (dist < 1000 && now_edges.size() > 0) {
    for (Coord step : now_edges) {
      for (auto [di, dj] :
           vector({pair(-1, 0), pair(1, 0), pair(0, -1), pair(0, 1)})) {
        Coord next_step{step.i + di, step.j + dj};
        char here{maze.at(next_step.i).at(next_step.j)};
        if (here == '#') continue;
        if (processed.find(next_step) != processed.end()) continue;
        if (here != '.') {
          auto node{home_edges.find(here)};
          if (node == home_edges.end()) {
            home_edges[here] = dist;
          }
          continue;
        }
        next_edges.push_back(next_step);
      }
      processed.insert(step);
    }
    now_edges.clear();
    now_edges.splice(now_edges.end(), next_edges);
    next_edges.clear();
    ++dist;
  }
}

distgraph make_distgraph(vector<string> maze) {
  distgraph dg{};
  for (char c{'@'}; c <= 'z'; ++c) {
    for (int i{0}; i < maze.size(); ++i) {
      size_t j{maze.at(i).find(c, 0)};
      if (j != string::npos) {
        flood_fill_maze(maze, i, j, dg);
        continue;
      }
    }
  }
  return dg;
}

int main(int argv, char** argc) {
  cxxopts::Options options("test", "A brief description");
  options.add_options()("1", "Solve part 1", cxxopts::value<bool>())(
      "2", "Solve part 2", cxxopts::value<bool>())("h,help", "Print usage")(
      "d", "Make graphviz dot file", cxxopts::value<bool>());
  auto result = options.parse(argv, argc);

  if (result.count("help")) {
    cout << options.help() << endl;
    exit(0);
  }
  if (result.count("1")) {
    ifstream inf{result.unmatched()[0]};
    auto maze = my_parse(inf);
    distgraph dg{make_distgraph(maze)};
  }
  if (result.count("2")) {
  }
  if (result.count("d")) {
    ifstream inf{result.unmatched()[0]};
    auto maze = my_parse(inf);
    distgraph dg{make_distgraph(maze)};

    cout << "graph{" << endl;
    for (auto node : dg) {
      for (auto edge : node.second) {
        if (node.first < edge.first) {
          cout << '"' << node.first << "\"--\"" << edge.first << "\" [ label=\""
               << edge.second << "\" ];" << endl;
        }
      }
    }
    cout << "}" << endl;
  }
  return 0;
}