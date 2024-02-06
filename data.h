#pragma once

#include <Eigen/Core>
#include <memory>

using Eigen::MatrixXd;
using Eigen::VectorXd;
using std::unique_ptr;

struct Data {
  MatrixXd A, B;
  VectorXd a, b, c;
};

inline Data createData(long na, long nb, long nc) {
  Data out{{na, nb}, {nb, nc}, VectorXd(na), VectorXd(nb), VectorXd(nc)};
  out.A.setZero();
  out.B.setZero();
  out.a.setZero();
  out.b.setZero();
  out.c.setZero();
  return out;
}
