#include "lib/data.h"
#include "lib/contig_data.h"
#include <benchmark/benchmark.h>
#include <tbb/cache_aligned_allocator.h>
#include <omp.h>

using tbb::cache_aligned_allocator;
long na = 4;
long nb = 4;

#define BOILERPLATE(state, vectype)                                            \
  auto N = (uint)state.range(0);                                               \
  auto NTHREADS = state.range(1);                                              \
  vectype data;                                                                \
  data.reserve(N);

std::array<uint, 2> get_work(uint N, uint threadId, uint numThreads) {
  uint start = threadId * N / numThreads;
  uint end = (threadId + 1) * N / numThreads;
  return {start, end};
}

void BM_openmp(benchmark::State &state) {
  // using StdVecData = std::vector<Data, cache_aligned_allocator<Data>>;
  using StdVecData = std::vector<Data>;
  BOILERPLATE(state, StdVecData)
  for (uint t = 0; t < N; t++) {
    data.emplace_back(na, nb);
  }

  for (auto _ : state) {
#pragma omp parallel for num_threads(NTHREADS)
    for (uint t = 0; t < N; t++) {
      runTask(data[t]);
    }
  }
}

void BM_contigs(benchmark::State &state) {
  using V = std::vector<ContDataOwned, cache_aligned_allocator<ContDataOwned>>;
  BOILERPLATE(state, V)
  for (uint t = 0; t < N; t++) {
    data.emplace_back(createContData(na, nb));
  }

  for (auto _ : state) {
#pragma omp parallel for num_threads(NTHREADS)
    for (uint t = 0; t < N; t++) {
      runTask(data[t]);
    }
  }

  for (size_t i = 0; i < N; i++) {
    data[i].destroy(DEFAULT_DYN_ALIGN);
  }
}

void BM_veccontigs(benchmark::State &state) {
  auto N = (uint)state.range(0);
  auto NTHREADS = state.range(1);
  auto vec = createContiguousVectorOfData(N, na, nb);

  for (auto _ : state) {
#pragma omp parallel for num_threads(NTHREADS)
    for (uint t = 0; t < N; t++) {
      runTask(vec.data[t]);
    }
  }
}

const std::vector<long> Ns = {20, 40, 50, 100, 150, 200};

void CustomArgs(benchmark::internal::Benchmark *bench) {
  bench->ArgNames({"N", "threads"});
  bench->Unit(benchmark::kMicrosecond)->UseRealTime();
  for (long e : Ns) {
    for (long nt : {1, 2, 4, 6}) {
      bench->Args({e, nt});
    }
  }
}

BENCHMARK(BM_openmp)->Apply(CustomArgs);
BENCHMARK(BM_contigs)->Apply(CustomArgs);
BENCHMARK(BM_veccontigs)->Apply(CustomArgs);

BENCHMARK_MAIN();
