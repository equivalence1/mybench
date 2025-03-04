#include <queue/queue.h>
#include <queue/spsc.h>

#include <chrono>
#include <cstdint>
#include <iostream>
#include <string>
#include <thread>

constexpr const int N = 100000000;
constexpr const int PREHEAT_N = 100000;

struct TBenchResult {
    uint64_t producer_duration;
    uint64_t consumer_duration;
    uint64_t bench_duration;
};

void print_res(const std::string& name, TBenchResult result) {
    std::cout << "Results for " << name << ":" << std::endl;
    std::cout << "    producer duration average: "
              << (result.producer_duration * 1.0) / N << "ns" << std::endl;
    std::cout << "    consumer duration average: "
              << (result.consumer_duration * 1.0) / N << "ns" << std::endl;
    std::cout << "    bench duration: " << result.bench_duration << "ms"
              << std::endl;
}

using namespace mybench::queue;

template <typename Q>
    requires CQueue<Q, int64_t>
TBenchResult bench_queue() {
    Q q;

    uint64_t producer_duration;
    uint64_t consumer_duration;

    auto main_start = std::chrono::high_resolution_clock::now();

    std::thread producer([&q, &producer_duration]() {
        int produced = 0;
        while (produced < PREHEAT_N) {
            while (!q.push(1))
                ;
            ++produced;
        }
        produced = 0;

        auto start = std::chrono::high_resolution_clock::now();
        while (produced < N) {
            while (!q.push(1))
                ;
            ++produced;
        }
        auto end = std::chrono::high_resolution_clock::now();

        producer_duration =
            std::chrono::duration_cast<std::chrono::nanoseconds>(end - start)
                .count();
    });

    std::thread consumer([&q, &consumer_duration]() {
        int consumed = 0;
        while (consumed < PREHEAT_N) {
            int64_t value;
            while (!q.pop(&value))
                ;
            ++consumed;
        }
        consumed = 0;

        auto start = std::chrono::high_resolution_clock::now();
        while (consumed < N) {
            int64_t value;
            while (!q.pop(&value))
                ;
            ++consumed;
        }
        auto end = std::chrono::high_resolution_clock::now();

        consumer_duration =
            std::chrono::duration_cast<std::chrono::nanoseconds>(end - start)
                .count();
    });

    producer.join();
    consumer.join();

    auto main_end = std::chrono::high_resolution_clock::now();

    auto main_duration = std::chrono::duration_cast<std::chrono::milliseconds>(
                             main_end - main_start)
                             .count();

    return {.producer_duration = producer_duration,
            .consumer_duration = consumer_duration,
            .bench_duration = static_cast<uint64_t>(main_duration)};
}

template <typename Q>
void run(const std::string& version) {
    auto res = bench_queue<Q>();
    print_res(std::string("lock-free ") + version, res);
}

void run_v1() {
    run<v1::TSpScQueue<int64_t>>("v1");
}

void run_v2() {
    run<v2::TSpScQueue<int64_t>>("v2");
}

void run_v3() {
    run<v3::TSpScQueue<int64_t>>("v3");
}

int main() {
    // 8ns
    run_v1();

    // 5ns
    run_v2();

    // 2ns
    run_v3();
}
