#include <benchmark/benchmark.h>

#include <cstdlib>
#include <algorithm>

static void BM_simple_sum(benchmark::State& state) {
    const size_t N = 100;
    float arr[N];
    std::fill(arr, arr + N, 0);

    for (auto _ : state) {
        for (size_t i = 1; i < N; ++i) {
            arr[i] += arr[i - 1];
            benchmark::DoNotOptimize(arr[i]);
        }
    }
}

BENCHMARK(BM_simple_sum);

struct TValue {
    char pad1_[13];
    float value = 0.;
    char pad2_[123];
};

static void BM_struct_sum(benchmark::State& state) {
    const size_t N = 100;
    TValue arr[N];

    for (auto _ : state) {
        for (size_t i = 1; i < N; ++i) {
            arr[i].value += arr[i - 1].value;
            benchmark::DoNotOptimize(arr[i]);
        }
    }
}

BENCHMARK(BM_struct_sum);

BENCHMARK_MAIN();
