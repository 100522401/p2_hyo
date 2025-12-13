#include "algorithm.hpp"
#include <cmath>

double Algorithm::h(Node n) {
  // Haversine distance in meters
  double lat1 = graph_.coords[n.id].lat;
  double lon1 = graph_.coords[n.id].lon;
  double lat2 = graph_.coords[goal_.id].lat;
  double lon2 = graph_.coords[goal_.id].lon;

  // Convert degrees to radians
  lat1 *= M_PI / 180.0;
  lon1 *= M_PI / 180.0;
  lat2 *= M_PI / 180.0;
  lon2 *= M_PI / 180.0;

  double dlat = lat2 - lat1;
  double dlon = lon2 - lon1;

  double a = sin(dlat / 2) * sin(dlat / 2) +
             cos(lat1) * cos(lat2) * sin(dlon / 2) * sin(dlon / 2);
  double c = 2 * atan2(sqrt(a), sqrt(1 - a));

  const double R = 6371000; // Earth radius in meters
  return R * c;
}

double Algorithm::g(int n) { return closed_.get(n).g; }

double Algorithm::f(int n) { return g(n) + h(n); }

std::vector<int> Algorithm::astar() {
  // Initialize lists
  open_.push(start_);

  start_.g = 0;
  start_.parent = -1;
  closed_.insert(
}