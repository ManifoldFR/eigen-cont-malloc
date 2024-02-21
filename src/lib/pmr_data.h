#pragma once

#include <memory_resource>
#include "common.h"

template <typename T>
T *pmr_allocate_aligned(const std::pmr::polymorphic_allocator<char> &alloc,
                        size_t size, size_t alignment) {
  auto mr = alloc.resource();
  size_t bytes = size * sizeof(T);
  auto data = mr->allocate(bytes, alignment);
  return reinterpret_cast<T *>(data);
}

/// Allocator-aware
struct PmrData {
  MatrixMap A;
  VectorMap a, b;
  using allocator_type = std::pmr::polymorphic_allocator<char>;
  PmrData(long na, long nb, const allocator_type alloc = {});
};
