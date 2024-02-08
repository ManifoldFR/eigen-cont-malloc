#pragma once

#include <memory_resource>
#include "common.h"

struct PmrData {
  MatrixMap A;
  VectorMap a, b;
  using allocator_type = std::pmr::polymorphic_allocator<char>;
  PmrData(long na, long nb, const allocator_type &alloc = {})
      : A(nullptr, na, nb), a(nullptr, na), b(nullptr, nb) {}
};
