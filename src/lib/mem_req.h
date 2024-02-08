#pragma once

#include <memory>
#include <cassert>
#include <cstdlib>

/// Adapt std::align to a scalar type.
template <typename T>
T *align_scalar_ptr(size_t align, size_t size, T *&ptr, size_t &remaining) {
  size_t bytes_remaining = sizeof(T) * remaining;
  size_t size_bytes = size * sizeof(T);
  void *mem = reinterpret_cast<void *>(ptr);
  void *newp = std::align(align, size_bytes, mem, bytes_remaining);
  remaining = bytes_remaining / sizeof(T);
  assert(newp != nullptr);
  ptr = reinterpret_cast<T *>(mem);
  return reinterpret_cast<T *>(newp);
}

/// Round up to the nearest multiple of a power of two.
constexpr size_t roundUpMultiplePow2(size_t a, size_t align) noexcept {
  size_t mask = (0 - align);
  return size_t((a + ~mask) & mask);
}

// Helper struct to get the total size of an aligned block of aligned memory
// chunks.
struct AlignedMemRequest {
  size_t size;
  size_t align;
  constexpr AlignedMemRequest(size_t size_, size_t align) noexcept
      : size(size_), align(align) {
    size = roundUpMultiplePow2(size_, align);
  }

  constexpr size_t alloc_req() const noexcept { return (size + align - 1); }

  friend AlignedMemRequest operator&(const AlignedMemRequest &self,
                                     const AlignedMemRequest &other) noexcept {
    size_t newalign = std::max(self.align, other.align);
    return {self.size + other.size, newalign};
  }

  friend AlignedMemRequest operator|(const AlignedMemRequest &self,
                                     const AlignedMemRequest &other) noexcept {
    size_t newalign = std::max(self.align, other.align);
    return {std::max(self.size, other.size), newalign};
  }

  template <typename T>
  static constexpr AlignedMemRequest with_type(size_t len,
                                               size_t align) noexcept {
    return {sizeof(T) * len, align};
  }
};
