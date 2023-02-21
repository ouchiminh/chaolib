#pragma once

#include <random>
#include <chrono>
#include <cstdio>
#include <tuple>
#include <utility>

#include "ouchitest/ouchitest.hpp"

#include "chao/mp_int/detail/common.hpp"
#include "chao/mp_int/detail/opimpl.hpp"

#if 0
OUCHI_TEST_CASE(int_representation_mul_digit_test128) {
    using namespace chao::detail;
    std::uint64_t a = ~0ull, b = 2, dest;
    std::uint64_t destar[2];
    unsigned __int128 a128, b128;
    std::mt19937_64 r(std::random_device{}());
    OUCHI_CHECK_EQUAL(naive_mul::mul(dest, a, b), 1ull);
    OUCHI_CHECK_EQUAL(dest, ~0ull << 1);
    naive_mul::mul<2,1>(destar, &a, &b);
    OUCHI_CHECK_EQUAL(destar[1], 1ull);
    OUCHI_CHECK_EQUAL(destar[0], ~0ull << 1);
    for(auto i = 0u; i < 1000; ++i){
        a128 = a = r();
        b128 = b = r();
        a128 *= b128;
        naive_mul::mul<2,1>(destar, &std::as_const(a), &std::as_const(b));
        OUCHI_REQUIRE_EQUAL(naive_mul::mul(dest, a, b), static_cast<std::uint64_t>(a128 >> 64));
        OUCHI_REQUIRE_EQUAL(dest, static_cast<std::uint64_t>(a128));

        OUCHI_REQUIRE_EQUAL(destar[1], static_cast<std::uint64_t>(a128 >> 64));
        OUCHI_REQUIRE_EQUAL(destar[0], static_cast<std::uint64_t>(a128));
    }
}

OUCHI_TEST_CASE(int_representation_mul_digit_test256) {
    using namespace chao::detail;
    unsigned __int128 a128, b128, dest[2];
    std::uint64_t a[2], b[2], destar[4];
    unsigned int seed = std::random_device{}();
    std::mt19937_64 r(std::random_device{}());

    for(auto i = 0u; i < 1000; ++i){
        r.seed(seed = r());
        a128 = b128 = 0;
        for(int j = 0; j < 2; ++j) {
            a128 |= (unsigned __int128)(a[j] = r()) << (64 * (j & 1));
            b128 |= (unsigned __int128)(b[j] = r()) << (64 * (j & 1));
        }
        r.seed(seed);

        dest[1] = naive_mul::mul<std::uint64_t>(*dest, a128, b128);
        naive_mul::mul<4,2>(destar, std::as_const(a), std::as_const(b));
        OUCHI_CHECK_EQUAL(destar[1], static_cast<std::uint64_t>(dest[0] >> 64));
        OUCHI_CHECK_EQUAL(destar[0], static_cast<std::uint64_t>(dest[0]));
        OUCHI_CHECK_EQUAL(destar[3], static_cast<std::uint64_t>(dest[1] >> 64));
        OUCHI_REQUIRE_EQUAL(destar[2], static_cast<std::uint64_t>(dest[1]));
    }
}
#endif