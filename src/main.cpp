/// Compare malloc'ing Eigen::Matrix objects contigously in a struct
/// to malloc'ing a buffer and pointing Eigen::Map objects to it
#include "lib/data.h"
#include "lib/contig_data.h"
#include <memory_resource>

void compute_dist_vecs(long na, long nb) {
  size_t N = 10;
  fmt::println("== Data ==");
  std::vector<Data> ds;
  ds.reserve(N);
  for (size_t i = 0; i < N; i++) {
    ds.emplace_back(na, nb);
    if (i == 0) {
      print_member_dist(ds[0]);
    }
  }
  check_contiguous_vec(ds);

  fmt::println("== ContDataOwned ==");
  std::vector<ContDataOwned> dsc;
  dsc.reserve(N);
  for (size_t i = 0; i < N; i++) {
    dsc.emplace_back(createContData(na, nb));
    if (i == 0) {
      print_member_dist(ds[0]);
    }
  }
  check_contiguous_vec(dsc);
  for (size_t i = 0; i < N; i++)
    dsc[i].destroy(DEFAULT_DYN_ALIGN);

  // OTHER
  fmt::println("== special_vector ==");
  special_vector vec = createContiguousVectorOfData(N, na, nb);
  print_member_dist(vec.data[0]);
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
  long na = 10;
  long nb = 10;
  Eigen::initParallel();

  compute_dist_vecs(na, nb);

  make_pmr_data(na, nb);
}
