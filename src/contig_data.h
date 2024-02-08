#pragma once

#include <Eigen/Core>
#include <memory>
#include <fmt/core.h>

using std::unique_ptr;
static constexpr int DEFAULT_DYN_ALIGN = EIGEN_MAX_ALIGN_BYTES;
using VectorX = Eigen::VectorXd;
using MatrixX = Eigen::MatrixXd;
using MatrixMap = Eigen::Map<MatrixX, DEFAULT_DYN_ALIGN>;
using VectorMap = Eigen::Map<VectorX, DEFAULT_DYN_ALIGN>;
static_assert(
    std::is_same_v<MatrixMap, MatrixX::AlignedMapType>,
    "MatrixMap typedef and MatrixX::AlignedMapType are not the same!");

struct ContDataOwned {
  MatrixMap A, B;
  VectorMap a, b, c;
  unique_ptr<double[]> mem;
};

template <typename T> T *alignedAlloc(size_t alignment, size_t size) {
  return static_cast<T *>(std::aligned_alloc(alignment, size * sizeof(T)));
}

inline auto createContData(long na, long nb, long nc) -> ContDataOwned {
  const long ntot = na * nb + nb * nc + na + nb + nc;
  fmt::print("Allocating {:d} doubles\n", ntot);
  auto p = alignedAlloc<double>(DEFAULT_DYN_ALIGN, ntot);
  // auto p = new double[ntot];
  size_t offset = 0;
  MatrixMap A(p + offset, na, nb);
  offset += na * nb;
  MatrixMap B(p + offset, nb, nc);
  offset += nb * nc;
  VectorMap a(p + offset, na);
  offset += na;
  VectorMap b(p + offset, nb);
  offset += nb;
  VectorMap c(p + offset, nc);
  A.setZero();
  B.setZero();
  b.setZero();
  c.setZero();
  return ContDataOwned{A, B, a, b, c, decltype(ContDataOwned::mem)(p)};
}
