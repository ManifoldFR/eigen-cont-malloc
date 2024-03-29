#include <omp.h>
#include <benchmark/benchmark.h>
// #include <tbb/parallel_for.h>
// #include <tbb/global_control.h>
#include <Eigen/Core>

using Eigen::Matrix4d;
using Eigen::Vector4d;

const int N = 40;
using Mat = Eigen::Matrix<double, -1, -1>;
using Vec = Eigen::Matrix<double, -1, 1>;

double workload() {
  Vec x(N);
  x.setOnes();
  Vec y = x;
  Mat M(N, N);
  M.setOnes();
  double s = 0.;
  const size_t nr = 3000;
  for (size_t i = 0; i < nr; i++) {
    y.noalias() = M * x;
    y.swap(x);
    s += x.sum();
  }
  return s;
}

const size_t nwork = 120;

void serial(benchmark::State &s) {

  for (auto _ : s) {
    for (size_t i = 0; i < nwork; i++) {
      auto v = workload();
      benchmark::DoNotOptimize(v);
    }
  }
}

void parallel(benchmark::State &s) {
  size_t num_threads = (size_t)s.range(0);
  omp_set_num_threads(num_threads);

  for (auto _ : s) {
    size_t i;
#pragma omp parallel for default(none) private(i) schedule(static)
    for (i = 0; i < nwork; i++) {
      auto v = workload();
      benchmark::DoNotOptimize(v);
    }
  }
}

// void paralleltbb(benchmark::State &s) {
//   size_t num_threads = (size_t)s.range(0);
//   tbb::global_control aq(tbb::global_control::max_allowed_parallelism,
//                          num_threads);

//   for (auto _ : s) {
//     tbb::parallel_for(0u, nwork, [](uint) {
//       auto v = workload();
//       benchmark::DoNotOptimize(v);
//     });
//   }
// }

BENCHMARK(serial)->Unit(benchmark::kMicrosecond)->UseRealTime();
BENCHMARK(parallel)
    ->Unit(benchmark::kMicrosecond)
    ->UseRealTime()
    ->Arg(2)
    ->Arg(4)
    ->Arg(6);
// BENCHMARK(paralleltbb)
//     ->Unit(benchmark::kMicrosecond)
//     ->UseRealTime()
//     ->Arg(2)
//     ->Arg(4)
//     ->Arg(6);

int main(int argc, char **argv) {
  // Eigen::initParallel();
  char arg0_default[] = "benchmark";
  char *args_default = arg0_default;
  if (!argv) {
    argc = 1;
    argv = &args_default;
  }
  benchmark::Initialize(&argc, argv);
  if (benchmark::ReportUnrecognizedArguments(argc, argv))
    return 1;
  benchmark::RunSpecifiedBenchmarks();
  benchmark::Shutdown();
  return 0;
}
