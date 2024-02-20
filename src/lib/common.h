#pragma once

#include <Eigen/Core>
#include <fmt/core.h>

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
  return d.b.data() + d.b.size();
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

template <class DataType> double runTask(DataType &d) {
  double out = 0.;
  for (size_t i = 0; i < 50; i++) {
    d.a.noalias() = d.A * d.b;
    d.b.noalias() = d.A.transpose() * d.a;
    out += d.b.sum();
  }
  return out;
}
