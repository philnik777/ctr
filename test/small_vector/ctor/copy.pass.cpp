#include <cassert>
#include <ctr/small_vector.hpp>
#include <vector>

int main() {
  { // empty vector
    ctr::small_vector<int, 16> vec;
    auto vec2 = vec;
    assert(vec2.empty());
  }
  { // small buffer is used exactly
    ctr::small_vector<int, 16> vec;
    for (size_t i = 0; i != 16; ++i)
      vec.emplace_back(i);
    auto vec2                 = vec;
    std::vector<int> expected = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    assert(std::ranges::equal(vec2, expected));
  }
  { // in large buffer mode
    ctr::small_vector<int, 16> vec;
    for (size_t i = 0; i != 17; ++i)
      vec.emplace_back(i);
    auto vec2                 = vec;
    std::vector<int> expected = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    assert(std::ranges::equal(vec2, expected));
  }
}
