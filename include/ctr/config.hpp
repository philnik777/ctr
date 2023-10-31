
#ifndef CTR_CONFIG_HPP
#define CTR_CONFIG_HPP

// library info
#define CTR_VERSION 1

// macro utilities
#define CTR_CONCAT_IMPL(lhs, rhs) lhs##rhs
#define CTR_CONCAT(lhs, rhs) CTR_CONCAT_IMPL(lhs, rhs)

// namespace configuration
#define CTR_NAMESPACE ctr::inline CTR_CONCAT(v, CTR_VERSION)

// Attributes
#if __has_cpp_attribute(clang::preferred_type)
#  define CTR_PREFERRED_TYPE(...) clang::preferred_type(__VA_ARGS__)
#else
#  define CTR_PREFERRED_TYPE(...)
#endif

#if __has_cpp_attribute(msvc::no_unique_address)
#  define NO_UNIQUE_ADDRESS msvc::no_unique_address
#elif __has_cpp_attribute(no_unique_address)
#  define NO_UNIQUE_ADDRESS no_unique_address
#else
#  define NO_UNIQUE_ADDRESS
#endif

#if __has_cpp_attribute(gnu::noinline)
#  define NO_INLINE gnu::noinline
#else
#  deinfe NO_INLINE
#endif

// Assertions
#ifdef CTR_TESTING
#  define CTR_INTERNAL_ASSERT(...) Not yet implemented !
#else
#  define CTR_INTERNAL_ASSERT(...)
#endif

#endif // CTR_CONFIG_HPP
