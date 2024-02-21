#include "pmr_data.h"

PmrData::PmrData(long na, long nb, const allocator_type alloc)
    : A(nullptr, na, nb), a(nullptr, na), b(nullptr, nb) {
  auto res = alloc.resource();
  constexpr auto alignment = DEFAULT_DYN_ALIGN;

  double *ptrA = pmr_allocate_aligned<double>(alloc, na * nb, alignment);
  new (&A) MatrixMap(ptrA, na, nb);

  double *ptra = pmr_allocate_aligned<double>(alloc, na, alignment);
  new (&a) VectorMap(ptra, na);

  double *ptrb = pmr_allocate_aligned<double>(alloc, nb, alignment);
  new (&b) VectorMap(ptrb, nb);

  A.setIdentity();
  b.setOnes();
}
