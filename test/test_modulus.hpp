#pragma once
#include "ouchitest/ouchitest.hpp"

#include "chao/mp_int.hpp"
#include "chao/modint/modulus.hpp"

OUCHI_TEST_CASE(ex_gcd_test) {
    constexpr int mod = 23;

    for(int i = 1; i < mod; ++i) {
        for(int j = 1; j < mod; ++j) {
            auto [d,x,y] = chao::ex_gcd(i * j, mod);
            OUCHI_REQUIRE_EQUAL(chao::mod_abs(x * i * j, mod), 1);
        }
    }
    for(int i = 1; i < mod; ++i) {
        auto [d,x,y] = chao::ex_gcd(i, mod);
        OUCHI_REQUIRE_EQUAL(chao::mod_abs(mod - x * i, mod), mod - 1);
    }
}

OUCHI_TEST_CASE(modulus_compile_test) {
    using namespace chao::literals;
    chao::modulus<128, 7_i128> a;
    OUCHI_REQUIRE_EQUAL(chao::mod_abs<chao::int128_t>(a.R * a.R_inv, a.value), 1);
    OUCHI_REQUIRE_EQUAL(chao::mod_abs<chao::int128_t>(a.M_dash * a.value, a.R), chao::expr_to_mp_int(a.R - 1));
    OUCHI_REQUIRE_TRUE(a.R_2 > 0);
}

OUCHI_TEST_CASE(modulo_compile_test) {
    using namespace chao::literals;
    {
        chao::mod<7_i128> a;
        OUCHI_REQUIRE_EQUAL(chao::mod_abs<chao::int128_t>(a.R * a.R_inv, a.value), 1);
        OUCHI_REQUIRE_EQUAL(chao::mod_abs<chao::int128_t>(a.M_dash * a.value, a.R), chao::expr_to_mp_int(a.R - 1));
        OUCHI_REQUIRE_TRUE(a.R_2 > 0);
    }
    {
        chao::mod<7> a;
        OUCHI_REQUIRE_EQUAL(chao::mod_abs<chao::int128_t>(a.R * a.R_inv, a.value), 1);
        OUCHI_REQUIRE_EQUAL(chao::mod_abs<chao::int128_t>(a.M_dash * a.value, a.R), chao::expr_to_mp_int(a.R - 1));
        OUCHI_REQUIRE_TRUE(a.R_2 > 0);
    }
}

OUCHI_TEST_CASE(montgomery_reduction_test) {
    using namespace chao::literals;
    chao::modulus<128, 17_i128> a;
    for(int i = 1; i < 17; ++i) {
        for(int j = 1; j < 17; ++j) {
            auto mrep = a.montgomery_representation(chao::expr_to_mp_int(i * j));
            OUCHI_REQUIRE_EQUAL(a.montgomery_reduction(mrep), (i * j) % 17);
        }
    }
}

OUCHI_TEST_CASE(mr_remainder_test) {
    constexpr int modulus = 127;
    chao::mod<modulus> m;
    for(int i = 0; i < (1 << 15); ++i) {
        OUCHI_REQUIRE_EQUAL(m.remainder(i), i % modulus);
    }
}

OUCHI_TEST_CASE(mr_remainder_test32) {
    constexpr int modulus = 30011;
    chao::mod<modulus> m;
    for(int i = 0; i < (1 << 15); ++i) {
        OUCHI_REQUIRE_EQUAL(m.remainder(i), i % modulus);
    }
}
