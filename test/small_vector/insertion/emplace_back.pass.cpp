#include <cassert>
#include <ctr/small_vector.hpp>

int main() {
  {
    ctr::small_vector<int, 16> vec;
    vec.emplace_back(1);
    assert(vec[0] == 1);
    vec.emplace_back(5);
    assert(vec[0] == 1);
    assert(vec[1] == 5);
  }
}
