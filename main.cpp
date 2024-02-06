/// Compare malloc'ing Eigen::Matrix objects contigously in a struct
/// to malloc'ing a buffer and pointing Eigen::Map objects to it
#include <benchmark/benchmark.h>
#include <iostream>
#include "data.h"

using MatrixMap = Eigen::Map<MatrixXd>;
using VectorMap = Eigen::Map<VectorXd>;

struct ContData {
  MatrixMap A, B;
  VectorMap b;
  VectorMap c;
  unique_ptr<double[]> mem;
};

auto createContData(long na, long nb, long nc) -> ContData {
  const long ntot = na * nb + nb * nc + nb + nc;
  auto p = new double[ntot];
  std::size_t offset = 0;
  MatrixMap A(p + offset, na, nb);
  offset += na * nb;
  MatrixMap B(p + offset, nb, nc);
  offset += nb * nc;
  VectorMap b(p + offset, nb);
  offset += nb;
  VectorMap c(p + offset, nc);
  A.setZero();
  B.setZero();
  b.setZero();
  c.setZero();
  return ContData{A, B, b, c, decltype(ContData::mem)(p)};
}

template <class S> void memberDist(const S &d) {
  auto AB = std::distance(d.A.data(), d.B.data());
  std::cout << "A-->B " << AB << "\n";
  auto Bb = std::distance(d.B.data(), d.b.data());
  std::cout << "B-->b " << Bb << "\n";
  auto bc = std::distance(d.b.data(), d.c.data());
  std::cout << "b-->c " << bc << "\n";
}

static void BM_noncont(benchmark::State &state) {
  long na = (long)state.range(0);
  long nb = na * 2;
  long nc = (long)state.range(1);
  VectorXd a(na);
  Data d = createData(na, nb, nc);

  for (auto _ : state) {
    d.b.noalias() = d.B * d.c;
    a.noalias() = d.A * d.b;
    benchmark::DoNotOptimize(a);
    benchmark::ClobberMemory();
  }
}

static void BM_cont(benchmark::State &state) {
  long na = (long)state.range(0);
  long nb = na * 2;
  long nc = (long)state.range(1);
  VectorXd a(na);
  ContData d = createContData(na, nb, nc);

  for (auto _ : state) {
    d.b.noalias() = d.B * d.c;
    a.noalias() = d.A * d.b;
    benchmark::DoNotOptimize(a);
    benchmark::ClobberMemory();
  }
}

void CustomArgs(benchmark::internal::Benchmark *bench) {
  for (long e = 2; e < 5; e++) {
    for (long p : {2, 4, 5, 6, 8, 10}) {
      bench->Args({10 * e, 10 * p});
    }
  }
}

BENCHMARK(BM_cont)->Apply(CustomArgs);
BENCHMARK(BM_noncont)->Apply(CustomArgs);

void compare_distances() {
  std::cout << "COMPARE DISTANCES\n";
  auto cd = createContData(100, 100, 40);
  memberDist(cd);

  auto d = createData(100, 100, 40);
  memberDist(d);
}

int main(int argc, char **argv) {

  compare_distances();

  char arg0_default[] = "benchmark";
  char *args_default = arg0_default;
  if (!argv) {
    argc = 1;
    argv = &args_default;
  }
  ::benchmark::Initialize(&argc, argv);
  if (::benchmark::ReportUnrecognizedArguments(argc, argv))
    return 1;
  ::benchmark::RunSpecifiedBenchmarks();
  ::benchmark::Shutdown();
  return 0;
}
