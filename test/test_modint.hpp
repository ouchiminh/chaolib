#pragma once
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
