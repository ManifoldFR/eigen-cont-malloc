#include "contig_data.h"
#include <new>
#ifndef NDEBUG
#include <iostream>
#endif

ContDataOwned createContData(long na, long nb, double *&buf,
                             size_t &bufferSize) {

  auto ptr_advance = buf;
  MatrixMap A =
      get_eigen_map_from_buf<MatrixX>(ptr_advance, na, nb, bufferSize);
  VectorMap a = get_eigen_map_from_buf<VectorX>(ptr_advance, na, bufferSize);
  VectorMap b = get_eigen_map_from_buf<VectorX>(ptr_advance, nb, bufferSize);
  A.setIdentity();
  b.setOnes();
  auto out = ContDataOwned{A, a, b, buf};
  buf = ptr_advance;
  return out;
}

ContDataOwned createContData(long na, long nb) {
  constexpr size_t align = DEFAULT_DYN_ALIGN;
  using AMR = AlignedMemRequest;
  AMR req = AMR::with_type<double>(na * nb, no_false_share_align) &
            AMR::with_type<double>(na, align) &
            AMR::with_type<double>(nb, align);
  size_t bufferSize = req.alloc_req() / sizeof(double);
#ifndef NDEBUG
  std::cout << "Allocating " << bufferSize << " doubles.\n";
#endif
  auto p = new (std::align_val_t(align)) double[bufferSize];
  return createContData(na, nb, p, bufferSize);
}

special_vector createContiguousVectorOfData(size_t N, long na, long nb) {
  using AMR = AlignedMemRequest;
  constexpr size_t align = DEFAULT_DYN_ALIGN;
  AMR req = {0, 0};
  for (size_t i = 0; i < N; i++) {
    req = req & AMR::with_type<double>(na * nb, no_false_share_align) &
          AMR::with_type<double>(na, align) & AMR::with_type<double>(nb, align);
  }
  size_t bufferSize = req.alloc_req() / sizeof(double);
  auto *buf = new (std::align_val_t(align)) double[bufferSize];
  special_vector out{{}, buf, req};
  out.data.reserve(N);
  // fmt::println("creating contig. vector of Data:");
  for (size_t i = 0; i < N; i++) {
    // fmt::println("remaining buffer size: {:d}\n", bufferSize);
    out.data.emplace_back(createContData(na, nb, buf, bufferSize));
  }
  // fmt::println("remaining buffer size: {:d}\n", bufferSize);
  return out;
}
