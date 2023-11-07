#include <cassert>
#include <ctr/small_vector.hpp>

int main() {
  {
    ctr::small_vector<int, 16> vec;
    assert(vec.empty());
  }
}
