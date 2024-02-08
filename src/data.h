#pragma once

#include <Eigen/Core>
#include <memory>
#include <fmt/core.h>

using MatrixX = Eigen::MatrixXd;
using VectorX = Eigen::VectorXd;

struct Data {
  MatrixX A, B;
  VectorX a, b, c;
  Data(long na, long nb, long nc) : A(na, nb), B(nb, nc), a(na), b(nb), c(nc) {}
};

inline auto *data_start_ptr(const Data &d) { return d.A.data(); }

inline auto *data_end_ptr(const Data &d) { return d.c.data() + d.c.size(); }

template <class A>
void check_contiguous_vec(const std::vector<Data, A> &datas) {
  const double *ps = data_start_ptr(datas[0]);
  const double *pe = data_end_ptr(datas[0]);
  for (size_t t = 0; t < datas.size(); t++) {
    ps = data_start_ptr(datas[t]);
    ptrdiff_t d = 0;
    if (t > 0)
      d = (char *)ps - (char *)pe;
    pe = data_end_ptr(datas[t]);
    fmt::print("data[{:>2d}] start = {:p} | d = {:d}\n", t, (void *)ps, d);
    fmt::print("         end   = {:p}\n", (void *)pe);
  }
}
