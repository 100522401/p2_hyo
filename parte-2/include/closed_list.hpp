#ifndef CLOSED_LIST_H
#define CLOSED_LIST_H

#include <vector>

struct ClosedNode {
  int id;
  double g;
  int parent;
};

class ClosedList {
private:
  std::vector<ClosedNode> closed;

public:
  void insert(ClosedNode node);
};

#endif