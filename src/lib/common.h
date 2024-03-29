#pragma once

#include <Eigen/Core>
#include <fmt/core.h>
#include <fmt/ostream.h>
#include "mem_req.h"

using MatrixX = Eigen::MatrixXd;
using VectorX = Eigen::VectorXd;
static constexpr int DEFAULT_DYN_ALIGN = Eigen::AlignedMax;
static_assert(DEFAULT_DYN_ALIGN == EIGEN_DEFAULT_ALIGN_BYTES);
using MatrixMap = Eigen::Map<MatrixX, DEFAULT_DYN_ALIGN>;
using VectorMap = Eigen::Map<VectorX, DEFAULT_DYN_ALIGN>;
static_assert(
    std::is_same_v<MatrixMap, MatrixX::AlignedMapType>,
    "MatrixMap typedef and MatrixX::AlignedMapType are not the same!");

template <class D> auto *eigen_end_ptr(const D &mat) {
  return mat.data() + mat.size();
}

template <class D> inline auto *data_start_ptr(const D &d) {
  return d.A.data();
}

template <class D> inline auto *data_end_ptr(const D &d) {
  return eigen_end_ptr(d.b);
}

template <class D, class A>
void check_contiguous_vec(const std::vector<D, A> &datas) {
  const char *p0;
  const char *p1;
  for (size_t t = 0; t < datas.size(); t++) {
    p0 = (char *)&datas[t];
    fmt::println("datas[{:>2d}].addr   = {:p}", t, p0);
    p0 = (char *)data_start_ptr(datas[t]);
    fmt::println("         .A addr = {:p}", p0);
    fmt::println("         .a addr = {:p}", (char *)datas[t].a.data());
    fmt::println("         .b addr = {:p}", (char *)datas[t].b.data());
    const char *pb = (char *)data_end_ptr(datas[t]);
    if (t + 1 != datas.size()) {
      p1 = (char *)data_start_ptr(datas[t + 1]);
      ptrdiff_t dist = p1 - p0;
      fmt::print("         .A <-> data[{:>2d}].A = {:d}\n", t + 1, dist);
      dist = p1 - pb;
      fmt::print("         .b <-> data[{:>2d}].A = {:d}\n", t + 1, dist);
    }
  }
}

template <class DataType> void runTask(DataType &d) {
  size_t iter = 10;
  for (size_t i = 0; i < iter; i++) {
    d.a.noalias() = d.A * d.b;
    d.b.noalias() = d.A.transpose() * d.a;
  }
}

template <class S> void print_member_dist(const S &d) {
  fmt::print("[{}] ", __func__);
  auto Aa = std::distance(eigen_end_ptr(d.A), d.a.data());
  fmt::print("A<-->a {}", Aa);
  auto ab = std::distance(eigen_end_ptr(d.a), d.b.data());
  fmt::println(" a<-->b {}", ab);
}

// Extract an Eigen::Map from the memory buffer and shift its pointer.
template <class D>
auto get_eigen_map_from_buf(typename D::Scalar *&ptr, long rows, long cols,
                            size_t &remainingBufferSize) {
  using T = typename D::Scalar;
  constexpr size_t Align = DEFAULT_DYN_ALIGN;
  size_t size = size_t(rows * cols);
  T *mem = align_scalar_ptr(Align, size, ptr, remainingBufferSize);
  Eigen::Map<D, Align> mat(ptr, rows, cols);
  ptr = ptr + size;
  remainingBufferSize -= size;
  assert(mem != nullptr);
  return mat;
}

template <class D>
auto get_eigen_map_from_buf(typename D::Scalar *&ptr, long size,
                            size_t &remainingBufferSize) {
  EIGEN_STATIC_ASSERT_VECTOR_ONLY(D);
  using T = typename D::Scalar;
  constexpr size_t Align = DEFAULT_DYN_ALIGN;
  T *mem = align_scalar_ptr(Align, size_t(size), ptr, remainingBufferSize);
  Eigen::Map<D, Align> mat(ptr, size);
  ptr = ptr + size;
  remainingBufferSize -= size;
  assert(mem != nullptr);
  return mat;
}
