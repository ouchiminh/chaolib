#pragma once
#include <cstdint>
#include <random>

#include "chao/modint.hpp"
#include "chao/mp_int/math.hpp"
#include "ouchitest/ouchitest.hpp"

OUCHI_TEST_CASE(test_modint_addition) {
    using namespace chao::literals;
    chao::modint<chao::modulus128<23_i128>> a(1), b(2), c;
    c = a + b;
    OUCHI_REQUIRE_EQUAL(c.get(), 3);
    c = a - b;
    OUCHI_REQUIRE_EQUAL(c.get(), 22);
    for(int i = 1; i < 23; ++i) {
        for(int j = 1; j < 23; ++j) {
            a = i;
            b = j;
            c = a + b;
            OUCHI_REQUIRE_EQUAL(c.get(), (i + j) % 23);
        }
    }
}

OUCHI_TEST_CASE(test_mod_multiplication) {
    using namespace chao::literals;
    chao::modint<chao::modulus128<23_i128>> a(1), b(2), c;

    for(int i = 1; i < 23; ++i) {
        for(int j = 1; j < 23; ++j) {
            a = i;
            b = j;
            c = a * b;
            OUCHI_REQUIRE_EQUAL(c.get(), i * j % 23);
        }
    }

}

OUCHI_TEST_CASE(test_mod_division) {
    using namespace chao::literals;
    chao::modint<chao::modulus128<23_i128>> a(1), b(2), c;

    for(int i = 1; i < 23; ++i) {
        for(int j = 1; j < 23; ++j) {
            a = i;
            b = j;
            c = a / b;
            OUCHI_REQUIRE_EQUAL(c.get(), chao::mod_abs(i * std::get<1>(chao::ex_gcd(j, 23)), 23));
        }
    }
}

OUCHI_TEST_CASE(test_modint_all_value_short) {
    typedef chao::modint<chao::mod<(short)127>> modint;

    OUCHI_REQUIRE_EQUAL((modint(127) * modint(127)).evaluate().get(), 0);
    for(int i = 0; i < (1 << 13); ++i) {
        for(int j = 0; j < (1 << 13); ++j) {
            modint a(i), b(j);
            OUCHI_REQUIRE_EQUAL((a * b).evaluate().get(), chao::mod_abs(i * j, 127));
        }
    }
}

OUCHI_TEST_CASE(test_modint_inv) {
    using namespace chao::literals;
    typedef chao::modint<chao::modulus128<23_i128>> modint;
    modint a(1), b(2), c;

    for(int j = 1; j < 23; ++j) {
        b = j;
        c = modint(1) / b;
        OUCHI_REQUIRE_EQUAL(c.get(), chao::mod_abs(std::get<1>(chao::ex_gcd(j, 23)), 23));
        a = c * c;
        OUCHI_REQUIRE_EQUAL((a * b * b).evaluate().get(), 1);
    }
}

OUCHI_TEST_CASE(test_modint_equality) {
    using namespace chao::literals;
    typedef chao::modint<chao::modulus128<23_i128>> modint;
    modint a(1), b(2);
    OUCHI_REQUIRE_TRUE(a != b);
    OUCHI_REQUIRE_TRUE(a != b * b);
    b = 1;
    OUCHI_REQUIRE_TRUE(a == b);
    OUCHI_REQUIRE_TRUE(a == b * b);
}


OUCHI_TEST_CASE(test_okmr) {
    using namespace chao::literals;
    constexpr auto bit_len = 384;
    constexpr auto modulo = chao::stoi<bit_len>("1140686444542216206202086934988038350474531692111");
#if 1
    typedef chao::mp_int<chao::sign::mp_signed, bit_len> mpint;
    typedef chao::modint<chao::dynamic_modulus<mpint>> modint;
    modint g(292295048183585810449704743283848056349669201184_i384, std::in_place, modulo);
    modint h = g.inv();
    OUCHI_REQUIRE_EQUAL((h * g).evaluate().get(), modint(1, std::in_place, modulo).get());
    modint i=h*h;
    OUCHI_REQUIRE_EQUAL(i.mod.get_modulo(), modulo);
    OUCHI_REQUIRE_EQUAL((i * g * g).evaluate().get(), modint(1, std::in_place, modulo).get());
#else
    using namespace chao::literals;
    typedef chao::modint<chao::mod<modulo>> modint;
    modint g(292295048183585810449704743283848056349669201184_i384);
    modint h = modint(1) / g;
    OUCHI_REQUIRE_EQUAL((h * g).evaluate().get(), modint(1).get());
    modint i=h*h;
    OUCHI_REQUIRE_EQUAL((i * g * g).evaluate().get(), modint(1).get());
#endif
}
