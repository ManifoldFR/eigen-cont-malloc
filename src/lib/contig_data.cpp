#include "contig_data.h"
#include <new>
#ifndef NDEBUG
#include <iostream>
#endif

ContDataOwned createContData(long na, long nb, double *&buf,
                             size_t &bufferSize) {

  auto ptr_advance = buf;
  MatrixMap A = getEigenMap<MatrixX>(ptr_advance, na, nb, bufferSize);
  VectorMap a = getEigenMap<VectorX>(ptr_advance, na, bufferSize);
  VectorMap b = getEigenMap<VectorX>(ptr_advance, nb, bufferSize);
  A.setIdentity();
  b.setOnes();
  auto out = ContDataOwned{A, a, b, buf};
  buf = ptr_advance;
  return out;
}

ContDataOwned createContData(long na, long nb) {
  constexpr size_t align = DEFAULT_DYN_ALIGN;
  using AMR = AlignedMemRequest;
  AMR req = AMR::with_type<double>(na * nb, align) &
            AMR::with_type<double>(na, align) &
            AMR::with_type<double>(nb, align);
  size_t bufferSize = req.alloc_req() / sizeof(double);
#ifndef NDEBUG
  std::cout << "Allocating " << bufferSize << " doubles.\n";
#endif
  auto p = new (std::align_val_t(align)) double[bufferSize];
  return createContData(na, nb, p, bufferSize);
}
