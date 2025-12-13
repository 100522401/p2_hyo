#ifndef CLOSED_LIST_H
#define CLOSED_LIST_H

#include <vector>

class ClosedList {
private:
  std::vector<char> closed_;

public:
  // Constructor reserves space
  ClosedList(int size) : closed_(size) {}

  // Methods
  inline void close(int n) { closed_[n] = 1; }

  inline bool is_closed(int n) { return closed_[n]; }
};

#endif
