#include "data.h"

Data::Data(long na, long nb) : A(na, nb), a(na), b(nb) {
  A.setIdentity();
  a.setZero();
  b.setOnes();
}
