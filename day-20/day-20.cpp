#include <fstream>
#include <iostream>
#include <list>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

#include "Eigen/Dense"
#include "cxxopts.hpp"

using namespace std;
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
  int i{};
  int j{};
};

struct Coord3D {
  int i{};
  int j{};
  int k{};  // level
};

using WarpMap = unordered_map<string, pair<Coord, Coord>>;

inline bool operator<(const Coord& lhs, const Coord& rhs) {
  return tie(lhs.i, lhs.j) < tie(rhs.i, rhs.j);
}

inline bool operator<(const Coord3D& lhs, const Coord3D& rhs) {
  return tie(lhs.i, lhs.j, lhs.k) < tie(rhs.i, rhs.j, rhs.k);
}

string find_portal_at_location(vector<string> maze, int i, int j) {
  char me{maze.at(i).at(j)};
  string portal{};
  if (maze.at(i).at(j + 1) == '.') {
    portal.push_back(maze.at(i).at(j - 1));
    portal.push_back(me);
  }
  if (maze.at(i).at(j - 1) == '.') {
    portal.push_back(me);
    portal.push_back(maze.at(i).at(j + 1));
  }
  if (maze.at(i + 1).at(j) == '.') {
    portal.push_back(maze.at(i - 1).at(j));
    portal.push_back(me);
  }
  if (maze.at(i - 1).at(j) == '.') {
    portal.push_back(me);
    portal.push_back(maze.at(i + 1).at(j));
  }
  return portal;
}

WarpMap make_warp_map(vector<string> maze) {
  WarpMap warp_map{};
  for (int i{1}; i < maze.size() - 1; ++i) {
    for (int j{1}; j < maze[0].size() - 1; ++j) {
      char me{maze.at(i).at(j)};
      if (isupper(me)) {
        string portal{find_portal_at_location(maze, i, j)};
        Coord coord{};
        if (portal.size()) {
          coord.i = i;
          coord.j = j;
          auto node{warp_map.find(portal)};
          if (node == warp_map.end()) {
            warp_map[portal] = pair(coord, Coord());
          } else {
            node->second.second = coord;
          }
        }
      }
    }
  }
  return warp_map;
}

Coord crawl_for_a_real_spot(vector<string> maze, Coord point) {
  for (auto [di, dj] :
       vector({pair(-1, 0), pair(1, 0), pair(0, -1), pair(0, 1)})) {
    Coord next_step{point.i + di, point.j + dj};
    char here{maze.at(next_step.i).at(next_step.j)};
    if (here == '.') return next_step;
  }
  return point;  // disable warning
}

int flood_fill_maze(vector<string> maze, WarpMap warp_map) {
  int dist{0};
  list<Coord> now_edges{crawl_for_a_real_spot(maze, warp_map["AA"].first)};
  list<Coord> next_edges{};
  set<Coord> processed{};
  while (dist < 10000 && now_edges.size() > 0) {
    // cout << endl;
    // cout << "dist " << dist << endl;
    // vector<string> maze_image{maze};
    // for (auto edge: now_edges){
    //   maze_image.at(edge.i).at(edge.j) = '*';
    // }
    // for (auto s: maze_image) cout << s << endl;

    for (Coord step : now_edges) {
      for (auto [di, dj] :
           vector({pair(-1, 0), pair(1, 0), pair(0, -1), pair(0, 1)})) {
        Coord next_step{step.i + di, step.j + dj};
        char here{maze.at(next_step.i).at(next_step.j)};
        if (here == '#') continue;
        if (processed.find(next_step) != processed.end()) continue;
        if (here != '.') {
          string portal{
              find_portal_at_location(maze, next_step.i, next_step.j)};
          if (portal == "AA") continue;
          if (portal == "ZZ") return dist;
          auto node{warp_map[portal]};
          if (node.first.i == next_step.i && node.first.j == next_step.j) {
            next_step = node.second;
          } else
            next_step = node.first;
          next_step = crawl_for_a_real_spot(maze, next_step);
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
  return dist;
}

bool is_outer(vector<string>& maze, int i, int j) {
  return i < 2 || i >= maze.size() - 2 || j < 2 || j >= maze.at(0).size() - 4;
}

int flood_fill_maze_3D(vector<string> maze, WarpMap warp_map) {
  int dist{0};
  Coord start{crawl_for_a_real_spot(maze, warp_map["AA"].first)};
  list<Coord3D> now_edges{Coord3D{start.i, start.j, 0}};
  list<Coord3D> next_edges{};
  set<Coord3D> processed{};
  while (dist < 10000 && now_edges.size() > 0) {
    // cout << endl;
    // cout << "dist " << dist << endl;
    // vector<string> maze_image{maze};
    // for (auto edge: now_edges){
    //   maze_image.at(edge.i).at(edge.j) = '0' + edge.k % 10;
    // }
    // for (auto s: maze_image) cout << s << endl;

    for (Coord3D step : now_edges) {
      for (auto [di, dj] :
           vector({pair(-1, 0), pair(1, 0), pair(0, -1), pair(0, 1)})) {
        Coord3D next_step{step.i + di, step.j + dj, step.k};
        char here{maze.at(next_step.i).at(next_step.j)};
        if (here == '#') continue;
        if (processed.find(next_step) != processed.end()) continue;
        if (here != '.') {
          processed.insert(next_step);
          string portal{
              find_portal_at_location(maze, next_step.i, next_step.j)};
          if (portal == "AA") continue;
          if (portal == "ZZ") {
            if (next_step.k == 0)
              return dist;
            else
              continue;
          }
          if (is_outer(maze, next_step.i, next_step.j)) {
            if (next_step.k == 0) continue;
            --next_step.k;
          } else
            ++next_step.k;

          Coord jumped_step{};
          auto node{warp_map[portal]};
          if (node.first.i == next_step.i && node.first.j == next_step.j) {
            jumped_step = node.second;
          } else
            jumped_step = node.first;

          next_step.i = jumped_step.i;
          next_step.j = jumped_step.j;
          processed.insert(next_step);

          auto next_step_2D = crawl_for_a_real_spot(maze, jumped_step);
          next_step.i = next_step_2D.i;
          next_step.j = next_step_2D.j;
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
    auto warp_map = make_warp_map(maze);
    cout << flood_fill_maze(maze, warp_map) << endl;
  }
  if (result.count("2")) {
    ifstream inf{result.unmatched()[0]};
    auto maze = my_parse(inf);
    auto warp_map = make_warp_map(maze);
    cout << flood_fill_maze_3D(maze, warp_map) << endl;
  }
  return 0;
}