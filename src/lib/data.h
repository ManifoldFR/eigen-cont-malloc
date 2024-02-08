#pragma once

#include "common.h"

struct Data {
  MatrixX A;
  VectorX a, b;
  Data(long na, long nb);
};
