#pragma once

#include <cstdlib>
#include <concepts>

namespace mybench::queue {

template <typename Q, typename T>
concept CQueue = requires(Q q, T&& t, const Q cq) {
    { q.push(std::move(t)) } -> std::same_as<bool>;
    { q.pop(&t) } -> std::same_as<bool>;
//    { cq.size() } -> std::same_as<size_t>;
};

}
