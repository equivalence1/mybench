#pragma once

#include "queue.h"

#include <array>
#include <atomic>
#include <cstdlib>
#include <cstdint>
#include <utility>
#include <new>
#include <version>

namespace mybench::queue {

namespace v1 {

template <typename T>
class TSpScQueue final {
public:
    TSpScQueue() = default;

    bool push(T&& element) {
        const auto lst = last_.load(std::memory_order_acquire);
        const auto fst = first_.load(std::memory_order_relaxed);
        if (lst + MAX_SIZE == fst) {
            return false;
        }

        data_[fst & (MAX_SIZE - 1)] = std::forward<T>(element);
        first_.fetch_add(1, std::memory_order_release);

        return true;
     }

    bool pop(T* data) {
        const auto lst = last_.load(std::memory_order_relaxed);
        const auto fst = first_.load(std::memory_order_acquire);
        if (lst >= fst) {
            return false;
        }

        *data = std::move(data_[last_ & (MAX_SIZE - 1)]);
        last_.fetch_add(1, std::memory_order_relaxed);

        return true;
    }

private:
    static const constexpr size_t MAX_SIZE = 1 << 10;

    std::array<T, MAX_SIZE> data_;
    std::atomic_uint64_t first_ = 0;
    std::atomic_uint64_t last_ = 0;
};

namespace __Npriv {

template <typename T>
class TSpScChecker {
    static_assert(CQueue<TSpScQueue<T>, T>);
};

} // namespace __Npriv

} // namespace v1

namespace v2 {

template <typename T>
class TSpScQueue final {
public:
    TSpScQueue() = default;

    bool push(T&& element) {
        const auto lst = last_.load(std::memory_order_acquire);
        const auto fst = first_.load(std::memory_order_relaxed);
        if (lst + MAX_SIZE == fst) {
            return false;
        }

        data_[fst & (MAX_SIZE - 1)] = std::forward<T>(element);
        first_.fetch_add(1, std::memory_order_release);

        return true;
     }

    bool pop(T* data) {
        const auto lst = last_.load(std::memory_order_relaxed);
        const auto fst = first_.load(std::memory_order_acquire);
        if (lst >= fst) {
            return false;
        }

        *data = std::move(data_[last_ & (MAX_SIZE - 1)]);
        last_.fetch_add(1, std::memory_order_relaxed);

        return true;
    }

private:
    static const constexpr size_t MAX_SIZE = 1 << 10;

#if __cpp_lib_hardware_interference_size
    static constexpr int CacheLine = std::hardware_destructive_interference_size;
#else
    static constexpr int CacheLine = 64;
#endif

    alignas(CacheLine) std::array<T, MAX_SIZE> data_;
    alignas(CacheLine) std::atomic_uint64_t first_ = 0;
    alignas(CacheLine) std::atomic_uint64_t last_ = 0;
};

} // namespace v2

namespace v3 {

template <typename T>
class TSpScQueue final {
public:
    TSpScQueue() = default;

    bool push(T&& element) {
        const auto fst = first_.load(std::memory_order_relaxed);
        if (last_cached_ + MAX_SIZE == fst) {
            last_cached_ = last_.load(std::memory_order_acquire);
            if (last_cached_ + MAX_SIZE == fst) {
                return false;
            }
        }

        data_[fst & (MAX_SIZE - 1)] = std::forward<T>(element);
        first_.fetch_add(1, std::memory_order_release);

        return true;
     }

    bool pop(T* data) {
        const auto lst = last_.load(std::memory_order_relaxed);
        if (lst >= first_cached_) {
            first_cached_ = first_.load(std::memory_order_acquire);
            if (lst >= first_cached_) {
                return false;
            }
        }

        *data = std::move(data_[last_ & (MAX_SIZE - 1)]);
        last_.fetch_add(1, std::memory_order_relaxed);

        return true;
    }

private:
    static const constexpr size_t MAX_SIZE = 1 << 10;

#if __cpp_lib_hardware_interference_size
    static constexpr int CacheLine = std::hardware_destructive_interference_size;
#else
    static constexpr int CacheLine = 64;
#endif

    alignas(CacheLine) std::array<T, MAX_SIZE> data_;

    alignas(CacheLine) std::atomic_uint64_t first_ = 0;
    uint64_t last_cached_ = 0;

    alignas(CacheLine) std::atomic_uint64_t last_ = 0;
    uint64_t first_cached_ = 0;
};

} // namespace v3

}
