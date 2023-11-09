#include <cassert>
#include <ctr/small_vector.hpp>

constexpr bool test() {
  {
    ctr::small_vector<int, 16> vec;
    assert(vec.empty());
  }

  return true;
}

int main() {
  test();
  static_assert(test());
}
