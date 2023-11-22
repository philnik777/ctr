#ifndef CTR_UNINITIALIZED_ALGORITHMS_HPP
#define CTR_UNINITIALIZED_ALGORITHMS_HPP

#include <ctr/config.hpp>
#include <ctr/exception_guard.hpp>
#include <memory>

namespace CTR_NAMESPACE {
template <class InIter, class OutIter>
struct in_out_result {
  InIter in;
  OutIter out;
};

template <class Alloc, class Iter>
void allocator_destroy(Alloc& alloc, Iter first, Iter last) {
  for (; first != last; ++first)
    std::allocator_traits<Alloc>::destroy(alloc, std::to_address(first));
}

template <class Alloc, class OutIter, class InIter>
constexpr in_out_result<InIter, OutIter>
uninitialized_allocator_relocate(Alloc& alloc, InIter ifirst, InIter ilast, OutIter ofirst) {
  exception_guard g([&, orig_ifirst = ifirst] {
    ctr::allocator_destroy(alloc, orig_ifirst, ifirst);
  });
  while (ifirst != ilast) {
    std::allocator_traits<Alloc>::construct(
        alloc, std::to_address(ofirst), std::move_if_noexcept(*ifirst));

    ++ifirst;
    ++ofirst;
  }
  g.complete();
  return {ifirst, ofirst};
}

template <class Alloc, class OutIter, class InIter>
constexpr in_out_result<InIter, OutIter>
uninitialized_allocator_copy(Alloc& alloc, InIter ifirst, InIter ilast, OutIter ofirst) {
  exception_guard g([&, orig_ifirst = ifirst] {
    ctr::allocator_destroy(alloc, orig_ifirst, ifirst);
  });
  while (ifirst != ilast) {
    std::allocator_traits<Alloc>::construct(alloc, std::to_address(ofirst), *ifirst);

    ++ifirst;
    ++ofirst;
  }
  g.complete();
  return {ifirst, ofirst};
}
} // namespace CTR_NAMESPACE

#endif // CTR_UNINITIALIZED_ALGORITHMS_HPP
