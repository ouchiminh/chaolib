#pragma once
#include <bit>
#include <chrono>
#include <cstdio>
#include <random>

#include "ouchitest/ouchitest.hpp"

#include "chao/mp_int/detail/common.hpp"
#include "chao/mp_int/detail/opimpl.hpp"

OUCHI_TEST_CASE(test_countl_zero_int_representation_128) {
    using namespace chao::detail;
    using std::countl_zero;
    int_representation<128> a;
    const auto seed = std::random_device{}();
    std::mt19937_64 r(seed);

    for(int i = 0; i < 0xFFFF; ++i) {
        const auto cz = (r() & 0x7F);
        const auto A = (unsigned __int128)1 << (127 - cz);
        a.poly[0] = (std::uint64_t)A;
        a.poly[1] = (std::uint64_t)(A >> 64);

        OUCHI_REQUIRE_EQUAL(countl_zero(a), (int)cz);
    }
}

OUCHI_TEST_CASE(test_int128_assign_test) {
    using namespace chao::detail;
    using std::countl_zero;
    int_representation<128> a;
    a = -1;
    OUCHI_REQUIRE_EQUAL(a.poly[0], ~0ull);
    OUCHI_REQUIRE_EQUAL(a.poly[1], ~0ull);
    a = (signed char)-1;
    OUCHI_REQUIRE_EQUAL(a.poly[0], ~0ull);
    OUCHI_REQUIRE_EQUAL(a.poly[1], ~0ull);
    a = ~0ull;
    OUCHI_REQUIRE_EQUAL(a.poly[0], ~0ull);
    OUCHI_REQUIRE_EQUAL(a.poly[1], 0ull);
    a = 0;
    OUCHI_REQUIRE_EQUAL(a.poly[0], 0ull);
    OUCHI_REQUIRE_EQUAL(a.poly[1], 0ull);
    a = 5;
    OUCHI_REQUIRE_EQUAL(a.poly[0], 5ull);
    OUCHI_REQUIRE_EQUAL(a.poly[1], 0ull);
}
