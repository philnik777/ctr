#include <ctr/small_vector.hpp>
#include <type_traits>

template <class T, size_t Cap>
void test_cap() {
  using vec = ctr::small_vector<T, Cap>;
  static_assert(std::is_same_v<typename vec::traits, ctr::small_vector_traits<T>>);
  static_assert(std::is_same_v<typename vec::value_type, T>);
  static_assert(std::is_same_v<typename vec::allocator_type, std::allocator<T>>);
  static_assert(std::is_same_v<typename vec::iterator, ctr::contiguous_iterator_wrapper<T*>>);
  static_assert(
      std::is_same_v<typename vec::const_iterator, ctr::contiguous_iterator_wrapper<const T*>>);
  static_assert(std::is_same_v<typename vec::reverse_iterator,
                               std::reverse_iterator<ctr::contiguous_iterator_wrapper<T*>>>);
  static_assert(std::is_same_v<typename vec::const_reverse_iterator,
                               std::reverse_iterator<ctr::contiguous_iterator_wrapper<const T*>>>);
  static_assert(std::is_same_v<typename vec::size_type, size_t>);
  static_assert(std::is_same_v<typename vec::difference_type, ptrdiff_t>);
  static_assert(std::is_same_v<typename vec::pointer, T*>);
  static_assert(std::is_same_v<typename vec::const_pointer, const T*>);
}

template <class T, size_t... Capacities>
void test(std::index_sequence<Capacities...>) {}

void test() {
  test<char>(std::index_sequence<1, 2, 4, 15, 128>{});
}
