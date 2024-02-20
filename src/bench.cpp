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

void BM_basic(benchmark::State &state) {
  // using StdVecData = std::vector<Data, cache_aligned_allocator<Data>>;
  using StdVecData = std::vector<Data>;
  BOILERPLATE(state, StdVecData)
  for (uint t = 0; t < N; t++) {
    data.emplace_back(na, nb);
  }

  for (auto _ : state) {
#pragma omp parallel for num_threads(NTHREADS)
    for (uint t = 0; t < N; t++) {
      auto v = runTask(data[t]);
      benchmark::DoNotOptimize(v);
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
      auto v = runTask(data[t]);
      benchmark::DoNotOptimize(v);
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
      auto v = runTask(vec.data[t]);
      benchmark::DoNotOptimize(v);
    }
  }
}

const std::vector<long> Ns = {16, 32, 64, 96, 128, 160, 192, 224, 256, 512};

void CustomArgs(benchmark::internal::Benchmark *bench) {
  bench->ArgNames({"N", "threads"});
  bench->Unit(benchmark::kMicrosecond)->UseRealTime();
  for (long nt : {1, 2, 4, 6}) {
    for (long e : Ns) {
      bench->Args({e, nt});
    }
  }
}

BENCHMARK(BM_basic)->Apply(CustomArgs);
BENCHMARK(BM_contigs)->Apply(CustomArgs);
BENCHMARK(BM_veccontigs)->Apply(CustomArgs);

BENCHMARK_MAIN();