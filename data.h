#pragma once

#include <Eigen/Core>
#include <memory>

using MatrixX = Eigen::MatrixXd;
using VectorX = Eigen::VectorXd;
// using MatrixX = Eigen::Matrix<double, -1, -1, 0, 120, 120>;
// using VectorX = Eigen::Matrix<double, -1, 1, 0, 200>;
using std::unique_ptr;

struct Data {
  MatrixX A, B;
  VectorX a, b, c;
  Data(long na, long nb, long nc) : A(na, nb), B(nb, nc), a(na), b(nb), c(nc) {}
};
