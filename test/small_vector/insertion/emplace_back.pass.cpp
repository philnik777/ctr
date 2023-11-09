#include <algorithm>
#include <cassert>
#include <ctr/small_vector.hpp>
#include <vector>

constexpr bool test() {
  { // simple test
    ctr::small_vector<int, 16> vec;
    vec.emplace_back(1);
    assert(vec[0] == 1);
    vec.emplace_back(5);
    assert(vec[0] == 1);
    assert(vec[1] == 5);
  }

  { // emplace_back until small buffer is full
    ctr::small_vector<int, 16> vec;
    for (size_t i = 0; i != 16; ++i)
      vec.emplace_back(i);
    std::vector<int> expected = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    assert(std::ranges::equal(vec, expected));
  }

  { // emplace_back to force an allocation
    ctr::small_vector<int, 16> vec;
    for (size_t i = 0; i != 17; ++i)
      vec.emplace_back(i);
    std::vector<int> expected = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    assert(std::ranges::equal(vec, expected));
  }

  return true;
}

int main() {
  test();
  static_assert(test());
}
