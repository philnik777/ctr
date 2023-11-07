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
in_out_result<InIter, OutIter>
uninitialized_allocator_relocate(Alloc& alloc, InIter ifirst, InIter ilast, OutIter ofirst) {
  auto orig_ofirst = ofirst;
  exception_guard g([&, orig_ifirst = ifirst] {
    ctr::allocator_destroy(alloc, orig_ifirst, ifirst);
  });
  while (ifirst != ilast) {
    std::allocator_traits<Alloc>::construct(
        alloc, std::to_address(ifirst), std::move_if_noexcept(*ofirst));

    ++ifirst;
    ++ofirst;
  }
  g.complete();
  ctr::allocator_destroy(alloc, orig_ofirst, ofirst);
  return {ifirst, ofirst};
}

template <class Alloc, class OutIter, class InIter>
in_out_result<InIter, OutIter>
uninitialized_allocator_copy(Alloc& alloc, InIter ifirst, InIter ilast, OutIter ofirst) {
  auto orig_ofirst = ofirst;
  exception_guard g([&, orig_ifirst = ifirst] {
    ctr::allocator_destroy(alloc, orig_ifirst, ifirst);
  });
  while (ifirst != ilast) {
    std::allocator_traits<Alloc>::construct(
        alloc, std::to_address(ifirst), *ofirst);

    ++ifirst;
    ++ofirst;
  }
  g.complete();
  return {ifirst, ofirst};
}
} // namespace CTR_NAMESPACE

#endif // CTR_UNINITIALIZED_ALGORITHMS_HPP
