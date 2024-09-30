#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <set>
#include <string>
#include <unordered_set>
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
  size_t operator()(const Coord& coord) {
    size_t hash{static_cast<size_t>((i << 8) | j)};
    return hash;
  }
};

namespace std {
template <>
struct hash<Coord> {
  const size_t operator()(const Coord& coord) {
    size_t hash{((static_cast<size_t>(coord.i) << 8) | coord.j)};
    return hash;
  }
};
}

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

using keyset = bitset<26>;
inline bool operator<(const keyset& lhs, const keyset& rhs) {
  return (lhs | rhs) == rhs && lhs != rhs;
}
inline bool operator<=(const keyset& lhs, const keyset& rhs) {
  return (lhs | rhs) == rhs;
}

struct Worker {
  int i;
  int j;
  keyset ks;
  string history;
};

tuple<int, int, keyset> scan_maze(vector<string> maze) {
  int i_at{0};
  int j_at{0};

  for (int i{0}; i < maze.size(); ++i) {
    size_t j{maze.at(i).find('@', 0)};
    if (j != string::npos) {
      i_at = i;
      j_at = j;
    }
  }

  keyset needed_keys{};

  for (char c{'a'}; c <= 'z'; ++c) {
    for (int i{0}; i < maze.size(); ++i) {
      size_t j{maze.at(i).find(c, 0)};
      if (j != string::npos) {
        needed_keys.set(c - 'a');
      }
    }
  }
  return tuple(i_at, j_at, needed_keys);
}

int flood_explore_maze(vector<string> maze) {
  auto [i, j, needed_keys] = scan_maze(maze);
  int dist{1};
  list<Worker> now_edges{Worker{i, j, 0, string()}};
  list<Worker> next_edges{};
  map<pair<int, int>, unordered_set<keyset>> processed_all{};
  map<pair<int, int>, list<keyset>> processed_good{};
  while (dist < 100'000 && now_edges.size() > 0) {
    for (Worker parent : now_edges) {
      for (auto [di, dj] :
           vector({pair(-1, 0), pair(1, 0), pair(0, -1), pair(0, 1)})) {
        Worker child{parent.i + di, parent.j + dj, parent.ks, parent.history};
        char here{maze.at(child.i).at(child.j)};
        if (here == '#') continue;

        // Have we done this exact thing before?
        auto here_log = processed_all.find(pair(child.i, child.j));
        if (here_log != processed_all.end()) {
          if (here_log->second.find(child.ks) != here_log->second.end())
            continue;
          here_log->second.insert(child.ks);

          // If we have been before, has it been with a keyset as good or better
          // than what we have now?
          bool done_better{false};
          for (keyset ks : processed_good[pair(child.i, child.j)]) {
            if (child.ks <= ks) {
              done_better = true;
              break;
            }
          }
          if (done_better) continue;
          processed_good[pair(child.i, child.j)].push_back(child.ks);
        } else {
          processed_all[pair(child.i, child.j)] = unordered_set<keyset>({child.ks});
          processed_good[pair(child.i, child.j)] = list<keyset>({child.ks});
        }

        if ('a' <= here && here <= 'z') {
          if (!child.ks.test(here - 'a')) {
            child.ks.set(here - 'a');
            if (child.ks == needed_keys) return dist;
            child.history.push_back(here);
          }
        } else if ('A' <= here && here <= 'Z') {
          if (!child.ks.test(here - 'A')) continue;
        }
        next_edges.push_back(child);
      }
    }
    now_edges.clear();
    now_edges.splice(now_edges.end(), next_edges);
    next_edges.clear();
    ++dist;
    if (dist % 10 == 0) cout << dist << endl;
  }
  return dist;
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
    cout << flood_explore_maze(maze) << endl;
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