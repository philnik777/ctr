#ifndef CTR_SMALL_VECTOR_HPP
#define CTR_SMALL_VECTOR_HPP

#include <ctr/config.hpp>
#include <ctr/pointer_like_traits.hpp>
#include <ctr/uninitialized_alogithms.hpp>
#include <limits>
#include <memory>

namespace CTR_NAMESPACE {
template <class T>
struct contiguous_iterator_wrapper {};

template <class T, class Alloc = std::allocator<T>>
struct small_vector_traits {
public:
  using allocator_type = Alloc;

private:
  using alloc_traits = std::allocator_traits<allocator_type>;

public:
  using iterator            = contiguous_iterator_wrapper<typename alloc_traits::pointer>;
  using const_iterator      = contiguous_iterator_wrapper<typename alloc_traits::const_pointer>;
  using pointer_like_traits = ctr::pointer_like_traits<typename alloc_traits::pointer>;
};

/// small_vector is similar to `std::vector` except that is stores \p SBOCapacity elements
/// in-place instead of allocating memory for a small number of elements. It's also possible to
/// control a few more parameters through \p SVTraits.
template <class T, size_t SBOCapacity, class SVTraits = small_vector_traits<T>>
class small_vector {
public:
  using traits                 = SVTraits;
  using value_type             = T;
  using allocator_type         = traits::allocator_type;
  using iterator               = traits::iterator;
  using const_iterator         = traits::const_iterator;
  using reverse_iterator       = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

private:
  using alloc_traits     = std::allocator_traits<allocator_type>;
  using ptr_traits       = std::pointer_traits<typename alloc_traits::pointer>;
  using const_ptr_traits = std::pointer_traits<typename alloc_traits::const_pointer>;

public:
  using size_type       = alloc_traits::size_type;
  using difference_type = alloc_traits::difference_type;
  using pointer         = alloc_traits::pointer;
  using const_pointer   = alloc_traits::const_pointer;

  static constexpr auto sbo_capacity = SBOCapacity;

private:
  static constexpr bool append_record_keeping =
      sizeof(value_type) * sbo_capacity >= sizeof(pointer) * 3;

  template <class = void>
  struct buffer {
    union {
      struct {
        value_type small_buffer_[sbo_capacity];
      };

      struct {
        pointer begin_ = nullptr;
        pointer end_   = nullptr;
        pointer cap_   = nullptr;
      };
    };

    template <class = void>
    struct record_keeping {
      uint16_t size_                                  : 15 = 0;
      [[CTR_PREFERRED_TYPE(bool)]] uint16_t is_large_ : 1  = false;

      static_assert(sbo_capacity <= std::numeric_limits<uint16_t>::max() / 2,
                    "The maximum supported small buffer capacity is 32768");
    };

    template <class Dummy>
      requires(std::numeric_limits<uint8_t>::max() / 2 <= sbo_capacity)
    struct record_keeping<Dummy> {
      uint8_t size_                                  : 7 = 0;
      [[CTR_PREFERRED_TYPE(bool)]] uint8_t is_large_ : 1 = false;
    };

    record_keeping<> record_keeper_;

    bool is_small() const { return !record_keeper_.is_large_; }
    void set_small(bool small) { record_keeper_.is_large_ = !small; }

    void set_short_size(size_t size) {
      CTR_INTERNAL_ASSERT(size <= cbo_capacity);
      record_keeper_.size_ = size;
    }

    pointer get_long_begin() const { return begin_; }
    pointer get_long_end() const { return end_; }
    pointer get_long_cap() const { return cap_; }

    void set_long_begin(pointer begin) { begin_ = begin; }
    void set_long_end(pointer end) { end_ = end; }
    void set_long_cap(pointer cap) { cap_ = cap; }

    value_type* data() { is_small() ? small_buffer_ : std::to_address(begin_); }
    const value_type* data() const { is_small() ? small_buffer_ : std::to_address(begin_); }

    size_t get_size() const { return is_small() ? record_keeper_.size_ : (end_ - begin_); }
    size_t get_cap() const { return is_small() ? sbo_capacity : (cap_ - begin_); }

    void set_size(size_type size) {
      if (is_small())
        set_short_size(size);
      else {
        CTR_INTERNAL_ASSERT(size <= get_cap());
        set_long_end(begin_ + size);
      }
    }
  };

  template <class Dummy>
    requires(!append_record_keeping)
  struct buffer<Dummy> {
    static_assert(false, "Not yet implemented");
  };

  [[NO_UNIQUE_ADDRESS]] buffer<> buffer_;
  [[NO_UNIQUE_ADDRESS]] allocator_type alloc_;

  size_type recommend(size_type new_size) const {
    return std::max<size_type>(2 * capacity(), new_size);
  }

  void set_pilfered() {
    buffer_.set_small(true);
    buffer_.set_short_size(0);
  }

public:
  small_vector() { set_pilfered(); }

  template <std::input_iterator Iter, std::sentinel_for<Iter> Sent>
  small_vector(Iter first, Sent last, const allocator_type& alloc = allocator_type())
      : buffer_{}, alloc_(alloc) {
    assign(first, last);
  }

  small_vector(const small_vector& other, const allocator_type& alloc) : buffer_{}, alloc_(alloc) {}

  small_vector(const small_vector& other)
      : small_vector(other, alloc_traits::select_on_container_copy_construction(other.alloc_)) {}

  small_vector(small_vector&& other) : buffer_{}, alloc_(other.alloc_) {
    if (other.is_small()) {
      ctr::uninitialized_allocator_relocate(alloc_, other.begin(), other.end(), buffer_.data());
      buffer_.set_size(other.size());
    } else {
      buffer_.set_long_begin(other.buffer_.get_long_begin());
      buffer_.set_long_end(other.buffer_.get_long_end());
      buffer_.set_long_cap(other.buffer_.get_long_cap());
      other.set_pilfered();
    }
  }

  small_vector& operator=(const small_vector&) = delete;
  small_vector& operator=(small_vector&&)      = delete;

  ~small_vector() {
    if (!buffer_.is_small())
      alloc_traits::deallocate(alloc_, buffer_.get_long_begin(), capacity());
  }

  template <std::input_iterator Iter, std::sentinel_for<Iter> Sent>
  void assign(Iter first, Sent last) {
    clear();
    for (; first != last; ++first)
      emplace_back(*first);
  }

  value_type* data() { return buffer_.data(); }
  const value_type* data() const { return buffer_.data(); }

  value_type& operator[](size_type i) { return buffer_.data()[i]; }

  size_type size() const { return buffer_.get_size(); }
  bool empty() const { return size() == 0; }

  iterator begin() { return ptr_traits::pointer_to(*data()); }
  const_iterator begin() const { return data(); }

  iterator end() { return data() + size(); }
  const_iterator end() const { return data() + size(); }

  size_type capacity() const { return buffer_.get_cap(); }

  value_type& back() { return *(data() + size() - 1); }
  const value_type& back() const { return *(data() + size() - 1); }

  template <class... Args>
  value_type& emplace_back(Args&&... args) {
    reserve(size() + 1);
    std::construct_at(data() + size(), std::forward<Args>(args)...);
    buffer_.set_short_size(size() + 1);
    return back();
  }

  void resize(size_type n) {
    reserve(n);

    if (buffer_.is_small())
      buffer_.set_small(static_cast<uint8_t>(n));
    else
      buffer_.set_long_end(buffer_.get_long_begin() + n);
  }

  void reserve(size_type n) {
    if (n < capacity())
      return;
    [&] [[NO_INLINE]] () {
      auto new_cap    = recommend(n);
      auto new_buffer = alloc_traits::allocate(alloc_, new_cap);
      exception_guard g([&] { alloc_traits::deallocate(alloc_, new_buffer, new_cap); });
      ctr::uninitialized_allocator_relocate(
          alloc_, new_buffer, buffer_.data(), buffer_.data() + buffer_.get_size());
      g.complete();

      if (!buffer_.is_small())
        alloc_traits::deallocate(alloc_, buffer_.get_long_begin(), capacity());
      buffer_.set_small(false);
      buffer_.set_long_begin(new_buffer);
      buffer_.set_long_end(new_buffer + n);
      buffer_.set_long_cap(new_buffer + new_cap);
    }();
  }

  void clear() {
    ctr::allocator_destroy(begin(), end());
    buffer_.set_size(0);
  }
};
} // namespace CTR_NAMESPACE

#endif // CTR_SMALL_VECTOR_HPP
