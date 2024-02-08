/// Compare malloc'ing Eigen::Matrix objects contigously in a struct
/// to malloc'ing a buffer and pointing Eigen::Map objects to it
#include "lib/data.h"
#include "lib/contig_data.h"
#include <memory_resource>

template <class S> void memberDist(const S &d) {
  fmt::print("[{}] ", __func__);
  auto Aa = std::distance(eigen_end_ptr(d.A), d.a.data());
  fmt::print("A<-->a {}", Aa);
  auto ab = std::distance(eigen_end_ptr(d.a), d.b.data());
  fmt::println(" a<-->b {}", ab);
}

void compare_distances(long na, long nb) {

  fmt::println("COMPARE DISTANCES");
  fmt::println("Data (owned)");
  uint nrep = 3;
  for (uint t = 0; t < nrep; t++) {
    Data d(na, nb);
    memberDist(d);
    runTask(d);
  }

  fmt::println("ContDataOwned");
  for (uint t = 0; t < nrep; t++) {
    auto cd = createContData(na, nb);
    memberDist(cd);
    runTask(cd);
    destroyContOwned(cd, DEFAULT_DYN_ALIGN);
  }
}

void compute_dist_vecs(long na, long nb) {
  size_t N = 40;
  fmt::println("== Data ==");
  std::vector<Data> ds;
  ds.reserve(N);
  for (size_t i = 0; i < N; i++) {
    ds.emplace_back(na, nb);
  }
  check_contiguous_vec(ds);

  fmt::println("== ContDataOwned ==");
  std::vector<ContDataOwned> dsc;
  dsc.reserve(N);
  for (size_t i = 0; i < N; i++) {
    dsc.emplace_back(createContData(na, nb));
  }
  check_contiguous_vec(dsc);
  for (size_t i = 0; i < N; i++)
    destroyContOwned(dsc[i], DEFAULT_DYN_ALIGN);

  // OTHER
  fmt::println("== special_vector ==");
  special_vector vec = createContiguousVectorOfData(N, na, nb);
  check_contiguous_vec(vec.data);
  for (size_t i = 0; i < N; i++) {
    runTask(vec.data[i]);
  }
  fmt::println("Destroy");
}

auto make_pmr_data(long na, long nb) {
  auto *mr = std::pmr::new_delete_resource();
  constexpr size_t align = DEFAULT_DYN_ALIGN;
  using AMR = AlignedMemRequest;
  AMR req = AMR::with_type<double>(na * nb, align) &
            AMR::with_type<double>(na, align) &
            AMR::with_type<double>(nb, align);
  size_t numBytes = req.alloc_req();
  void *buf = mr->allocate(numBytes, align);
  mr->deallocate(buf, numBytes, align);
}

int main(int argc, char **argv) {
  long na = 100;
  long nb = 100;
  compare_distances(na, nb);

  compute_dist_vecs(na, nb);

  make_pmr_data(na, nb);
}
