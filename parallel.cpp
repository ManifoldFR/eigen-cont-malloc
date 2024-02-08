#include "data.h"
#include <benchmark/benchmark.h>
#include <tbb/cache_aligned_allocator.h>

void run_task(Data &d) {
  d.b.noalias() = d.B * d.c;
  d.a.noalias() = d.A * d.b;
}

using StdVecData = std::vector<Data, tbb::cache_aligned_allocator<Data>>;

#define BOILERPLATE(state)                                                     \
  uint N = (uint)state.range(0);                                               \
  auto NTHREADS = state.range(1);                                              \
  StdVecData data;                                                             \
  data.reserve(N);                                                             \
  long na = 30;                                                                \
  long nb = 20;                                                                \
  long nc = 20;                                                                \
  for (uint t = 0; t < N; t++) {                                               \
    data.emplace_back(na, nb, nc);                                             \
  }

std::array<uint, 2> get_work(uint N, uint threadId, uint numThreads) {
  uint start = threadId * N / numThreads;
  uint end = (threadId + 1) * N / numThreads;
  return {start, end};
}

void BM_openmp(benchmark::State &state) {
  BOILERPLATE(state)

  Eigen::setNbThreads(1);

  for (auto _ : state) {
#pragma omp parallel for num_threads(NTHREADS) schedule(static)
    for (uint t = 0; t < N; t++) {
      run_task(data[t]);
    }
  }

  Eigen::setNbThreads(0);
}

void BM_tbb(benchmark::State &state) {
  BOILERPLATE(state)
  tbb::global_control ctrl(tbb::global_control::max_allowed_parallelism,
                           NTHREADS);

  Eigen::setNbThreads(1);

  for (auto _ : state) {
    tbb::parallel_for(0U, N, [&](uint t) { run_task(data[t]); });
  }

  Eigen::setNbThreads(0);
}

std::vector<long> Ns = {20, 40, 50, 60, 80, 100, 150};

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

BENCHMARK_MAIN();
