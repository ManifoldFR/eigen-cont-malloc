#pragma once

#include "mem_req.h"
#include "common.h"

using std::unique_ptr;

// Extract an Eigen::Map from the memory buffer and shift its pointer.
template <class D, typename T = typename D::Scalar>
inline auto getEigenMap(T *&ptr, long rows, long cols,
                        size_t &remainingBufferSize) {
  constexpr size_t Align = DEFAULT_DYN_ALIGN;
  size_t size = size_t(rows * cols);
  T *mem = align_scalar_ptr(Align, size, ptr, remainingBufferSize);
  Eigen::Map<D, Align> mat(ptr, rows, cols);
  ptr = ptr + size;
  remainingBufferSize -= size;
  assert(mem != nullptr);
  return mat;
}

template <class D, typename T = typename D::Scalar>
inline auto getEigenMap(T *&ptr, long size, size_t &remainingBufferSize) {
  EIGEN_STATIC_ASSERT_VECTOR_ONLY(D);
  constexpr size_t Align = DEFAULT_DYN_ALIGN;
  T *mem = align_scalar_ptr(Align, size_t(size), ptr, remainingBufferSize);
  Eigen::Map<D, Align> mat(ptr, size);
  ptr = ptr + size;
  remainingBufferSize -= size;
  assert(mem != nullptr);
  return mat;
}

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

class alignas(64) special_vector {
  AlignedMemRequest req;

public:
  std::vector<ContDataOwned> data;
  double *mem;
  special_vector(const std::vector<ContDataOwned> &data, double *mem,
                 const AlignedMemRequest &req)
      : req(req), data(data), mem(mem) {}

  ~special_vector() {
    // fmt::println("Destroying special_vector @ {:p}", (char *)this);
    operator delete[](mem, std::align_val_t(req.align));
  }
};

inline special_vector createContiguousVectorOfData(size_t N, long na, long nb) {
  using AMR = AlignedMemRequest;
  constexpr size_t align = DEFAULT_DYN_ALIGN;
  AMR req = {0, 0};
  for (size_t i = 0; i < N; i++) {
    req = req & AMR::with_type<double>(na * nb, no_false_share_align) &
          AMR::with_type<double>(na, align) & AMR::with_type<double>(nb, align);
  }
  size_t bufferSize = req.alloc_req() / sizeof(double);
  auto *buf = new (std::align_val_t(align)) double[bufferSize];
  special_vector out{{}, buf, req};
  out.data.reserve(N);
  // fmt::println("creating contig. vector of Data:");
  for (size_t i = 0; i < N; i++) {
    // fmt::println("remaining buffer size: {:d}\n", bufferSize);
    out.data.emplace_back(createContData(na, nb, buf, bufferSize));
  }
  // fmt::println("remaining buffer size: {:d}\n", bufferSize);
  return out;
}
