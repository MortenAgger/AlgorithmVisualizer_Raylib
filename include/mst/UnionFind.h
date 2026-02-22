#pragma once

#include <vector>
#include <unordered_map>

namespace mst {

class UnionFind {
public:
  void reset(const std::vector<int>& nodeIds);
  int find(int id);
  void unite(int a, int b);
  bool connected(int a, int b);

private:
  std::unordered_map<int, int> parent_;
  std::unordered_map<int, int> rank_;
};

} // namespace mst
