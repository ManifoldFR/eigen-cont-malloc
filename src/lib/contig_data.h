#pragma once

#include "mem_req.h"
#include "common.h"

struct alignas(64) ContDataOwned {
  MatrixMap A;
  VectorMap a, b;
  double *mem;
  void destroy(size_t align) {
    ::operator delete[](mem, std::align_val_t(align));
  }
};

ContDataOwned createContData(long na, long nb, double *&buf,
                             size_t &bufferSize);
ContDataOwned createContData(long na, long nb);

constexpr auto no_false_share_align = 64;

struct alignas(64) special_vector {
  std::vector<ContDataOwned> data;
  double *mem;
  AlignedMemRequest req;

  ~special_vector() {
    // fmt::println("Destroying special_vector @ {:p}", (char *)this);
    operator delete[](mem, std::align_val_t(req.align));
  }
};

special_vector createContiguousVectorOfData(size_t N, long na, long nb);
