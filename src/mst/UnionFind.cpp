#include "mst/UnionFind.h"

namespace mst {

void UnionFind::reset(const std::vector<int>& nodeIds) {
  parent_.clear();
  rank_.clear();
  for (int id : nodeIds) {
    parent_[id] = id;
    rank_[id] = 0;
  }
}

int UnionFind::find(int id) {
  if (parent_.count(id) == 0) return id;
  if (parent_[id] != id)
    parent_[id] = find(parent_[id]);
  return parent_[id];
}

void UnionFind::unite(int a, int b) {
  int ra = find(a), rb = find(b);
  if (ra == rb) return;
  if (rank_[ra] < rank_[rb])
    parent_[ra] = rb;
  else if (rank_[rb] < rank_[ra])
    parent_[rb] = ra;
  else {
    parent_[rb] = ra;
    rank_[ra]++;
  }
}

bool UnionFind::connected(int a, int b) {
  return find(a) == find(b);
}

} // namespace mst
