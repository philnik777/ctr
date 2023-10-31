#ifndef CTR_EXCEPTION_GUARD_HPP
#define CTR_EXCEPTION_GUARD_HPP

#include <ctr/config.hpp>
#include <utility>

namespace CTR_NAMESPACE {

/// exception_guard is a simple helper function for exception-safe code
template <class Rollback>
class exception_guard {
public:
  exception_guard(Rollback rollback) : rollback_(std::move(rollback)) {}
  exception_guard(const exception_guard&) = delete;
  exception_guard(exception_guard&&) = delete;
  exception_guard& operator=(const exception_guard&) = delete;
  exception_guard& operator=(exception_guard&&) = delete;
  ~exception_guard() {
    if (!complete_)
      rollback_();
  }

  void complete() { complete_ = true; }

private:
  [[NO_UNIQUE_ADDRESS]] Rollback rollback_;
  bool complete_ = false;
};
} // namespace CTR_NAMESPACE

#endif // CTR_EXCEPTION_GUARD_HPP
