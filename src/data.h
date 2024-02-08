#pragma once

#include <Eigen/Core>
#include <memory>

using MatrixX = Eigen::MatrixXd;
using VectorX = Eigen::VectorXd;

struct Data {
  MatrixX A, B;
  VectorX a, b, c;
  Data(long na, long nb, long nc) : A(na, nb), B(nb, nc), a(na), b(nb), c(nc) {}
};

inline auto *data_start_ptr(const Data &d) { return d.A.data(); }

inline auto *data_end_ptr(const Data &d) { return d.c.data() + d.c.size(); }
