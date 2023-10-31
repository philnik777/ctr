#ifndef CTR_POINTER_LIKE_TRAITS_HPP
#define CTR_POINTER_LIKE_TRAITS_HPP

#include <bit>
#include <cstddef>
#include <cstdint>
#include <ctr/config.hpp>
#include <type_traits>

namespace CTR_NAMESPACE {

/// *exposition only*
///
/// pointer_like_traits makes it possible to store a few bits of information inside the lower bits
/// of an aligned pointer to pack as much information into memory as possible.
template <class T>
struct pointer_like_traits;

#ifdef CTR_DOXYGEN
template <class T>
struct pointer_like_traits {
  /// The number of low-bits available in the pointer
  static constexpr size_t bits_available;

  static std::uintptr_t to_uintptr(PointerLikeType);
  static PointerLikeType to_pointer(std::uintptr_t);
};
#endif

template <class T>
  requires(!std::is_void_v<T>)
struct pointer_like_traits<T*> {
  static constexpr size_t bits_available = std::countr_zero(alignof(T));

  static std::uintptr_t to_uintptr(T* ptr) { return reinterpret_cast<std::uintptr_t>(ptr); }
  static T* to_pointer(std::uintptr_t ptr) { return reinterpret_cast<T*>(ptr); }
};

template <class T>
  requires std::is_void_v<T>
struct pointer_like_traits<T*> {
  static constexpr size_t bits_available = 0;

  static std::uintptr_t to_uintptr(T* ptr) { return reinterpret_cast<std::uintptr_t>(ptr); }
  static T* to_pointer(std::uintptr_t ptr) { return reinterpret_cast<T*>(ptr); }
};

} // namespace CTR_NAMESPACE

#endif // CTR_POINTER_LIKE_TRAITS_HPP
