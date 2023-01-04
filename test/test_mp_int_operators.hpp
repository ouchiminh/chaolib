#pragma once
#include <random>
#include <chrono>
#include <cstdio>
#include <tuple>

#include "ouchitest/ouchitest.hpp"

#include "chao/mp_int.hpp"

OUCHI_TEST_CASE(test_mp_int_ctor) {
    chao::mp_int a = 3;
    OUCHI_REQUIRE_EQUAL(a.bit_length, 64u);
    OUCHI_REQUIRE_EQUAL(a.sign_value, chao::sign::mp_signed);

    chao::mp_int b = 3u;
    OUCHI_REQUIRE_EQUAL(b.bit_length, 64u);
    OUCHI_REQUIRE_EQUAL(b.sign_value, chao::sign::mp_unsigned);

    chao::mp_int c = (short)3;
    OUCHI_REQUIRE_EQUAL(c.bit_length, 64u);
    OUCHI_REQUIRE_EQUAL(c.sign_value, chao::sign::mp_signed);
}

OUCHI_TEST_CASE(test_bitop_expr_tmpl64) {
    using namespace chao;
    mp_int<sign::mp_signed, 64> i, j, r;
    std::uint64_t I, J, R;
    const auto seed = std::random_device{}();
    std::mt19937_64 rnd{seed};
    for(int k = 0; k < 100; ++k) {
        i = I = rnd();
        j = J = rnd();
        r = i | j;
        R = I | J;
        OUCHI_REQUIRE_EQUAL(r.value_.poly[0], R);
    }
    for(int k = 0; k < 100; ++k) {
        i = I = rnd();
        j = J = rnd();
        r = i & j;
        R = I & J;
        OUCHI_REQUIRE_EQUAL(r.value_.poly[0], R);
    }
    for(int k = 0; k < 100; ++k) {
        i = I = rnd();
        j = J = rnd();
        r = i ^ j;
        R = I ^ J;
        OUCHI_REQUIRE_EQUAL(r.value_.poly[0], R);
    }
}
OUCHI_TEST_CASE(test_bitop_expr_tmpl128) {
    using namespace chao;
    mp_int<sign::mp_unsigned, 128> i, j, r;
    unsigned __int128 I, J, R;
    const auto seed = std::random_device{}();
    std::mt19937_64 rnd{seed};
    auto init = [&]() mutable {
        I = rnd();
        J = rnd();
        i = (std::uint64_t)I;
        j = (std::uint64_t)J;
        i.value_.poly[1] = rnd();
        j.value_.poly[1] = rnd();
        I |= ((unsigned __int128)i.value_.poly[1]) << 64;
        J |= ((unsigned __int128)j.value_.poly[1]) << 64;
    };
    for(int k = 0; k < 100; ++k) {
        init();
        r = i | j;
        R = I | J;
        OUCHI_REQUIRE_EQUAL(r.value_.poly[0], (std::uint64_t)R);
        OUCHI_REQUIRE_EQUAL(r.value_.poly[1], (std::uint64_t)(R >> 64));
    }
    for(int k = 0; k < 100; ++k) {
        init();
        r = i & j;
        R = I & J;
        OUCHI_REQUIRE_EQUAL(r.value_.poly[0], (std::uint64_t)R);
        OUCHI_REQUIRE_EQUAL(r.value_.poly[1], (std::uint64_t)(R >> 64));
    }
    for(int k = 0; k < 100; ++k) {
        init();
        r = i ^ j;
        R = I ^ J;
        OUCHI_REQUIRE_EQUAL(r.value_.poly[0], (std::uint64_t)R);
        OUCHI_REQUIRE_EQUAL(r.value_.poly[1], (std::uint64_t)(R >> 64));
    }
}

OUCHI_TEST_CASE(test_shift_tmpl){
    using namespace chao;
    mp_int<sign::mp_unsigned, 128> i, j, r;
    unsigned __int128 I, J, R;
    const auto seed = std::random_device{}();
    std::mt19937_64 rnd{seed};
    auto init = [&]() mutable {
        I = rnd();
        J = rnd();
        i = (std::uint64_t)I;
        j = (std::uint64_t)J;
        i.value_.poly[1] = rnd();
        j.value_.poly[1] = rnd();
        I |= ((unsigned __int128)i.value_.poly[1]) << 64;
        J |= ((unsigned __int128)j.value_.poly[1]) << 64;
    };
    for(int k = 0; k < 100; ++k) {
        auto W = rnd() % 128;
        mp_int<sign::mp_unsigned, 64> w = W;
        init();
        r = i >> w;
        R = I >> W;
        OUCHI_REQUIRE_EQUAL(r.value_.poly[0], (std::uint64_t)R);
        OUCHI_REQUIRE_EQUAL(r.value_.poly[1], (std::uint64_t)(R >> 64));
        r = j << w;
        R = J << W;
        OUCHI_REQUIRE_EQUAL(r.value_.poly[0], (std::uint64_t)R);
        OUCHI_REQUIRE_EQUAL(r.value_.poly[1], (std::uint64_t)(R >> 64));
    }
}

OUCHI_TEST_CASE(test_add_expr_tmpl) {
    using namespace chao;
    mp_int<sign::mp_signed, 64> i, j;
    i = 4; j = 5;
    mp_int<sign::mp_signed, 64> r;
    mp_int<sign::mp_signed, 64> R = 9;
    r = j + i;
    OUCHI_REQUIRE_TRUE(r == R);
    r = j + i + j;
    R = 14;
    OUCHI_REQUIRE_TRUE(r == R);
    r = (j + i) + (j + i);
    R = 18;
    OUCHI_REQUIRE_TRUE(r == R);
    r = r + r;
    R = 36;
    OUCHI_REQUIRE_EQUAL(r.value_.poly[0], R.value_.poly[0]);
    r += r;
    R = 72;
    OUCHI_REQUIRE_EQUAL(r.value_.poly[0], R.value_.poly[0]);
    // r = r + r + r;
    // R = 108;
    // OUCHI_REQUIRE_EQUAL(r.value_.poly[0], R.value_.poly[0]);
    // r = (r + r) + (r + r);
    // R = 144;
    // OUCHI_REQUIRE_EQUAL(r.value_.poly[0], R.value_.poly[0]);
}
OUCHI_TEST_CASE(test_sub_expr_tmpl) {
    using namespace chao;
    mp_int<sign::mp_signed, 128> i, j;
    i = 4; j = 5;
    mp_int<sign::mp_signed, 128> r;
    mp_int<sign::mp_signed, 128> R = 1;
    r = j - i;
    OUCHI_REQUIRE_TRUE(r == R);
    r = j - i - j;
    R = -4;
    OUCHI_REQUIRE_TRUE(r == R);
    r = (j - i) - (j - i);
    R = 0;
    OUCHI_REQUIRE_TRUE(r == R);
    r = r - r;
    R = 0;
    OUCHI_REQUIRE_EQUAL(r.value_.poly[0], R.value_.poly[0]);
}
OUCHI_TEST_CASE(test_mul_expr_tmpl) {
    using namespace chao;
    mp_int<sign::mp_signed, 128> i, j;
    i = 4; j = 5;
    mp_int<sign::mp_signed, 128> r;
    mp_int<sign::mp_signed, 128> R = 20;
    r = j * i;
    OUCHI_REQUIRE_TRUE(r == R);
    r = j * i * j;
    R = 100;
    OUCHI_REQUIRE_TRUE(r == R);
    r = (j * i) * (j * i);
    R = 400;
    OUCHI_REQUIRE_TRUE(r == R);
    r *= r;
    R = 160000;
    OUCHI_REQUIRE_EQUAL(r.value_.poly[0], R.value_.poly[0]);
}
OUCHI_TEST_CASE(test_mul_expr_tmpl192) {
    using namespace chao;
    mp_int<sign::mp_signed, 192> i, j;
    i = 4; j = 5;
    mp_int<sign::mp_signed, 192> r;
    mp_int<sign::mp_signed, 192> R = 20;
    r = j * i;
    OUCHI_REQUIRE_TRUE(r == R);
    r = j * i * j;
    R = 100;
    OUCHI_REQUIRE_TRUE(r == R);
    r = (j * i) * (j * i);
    R = 400;
    OUCHI_REQUIRE_TRUE(r == R);
    r *= r;
    R = 160000;
    OUCHI_REQUIRE_EQUAL(r.value_.poly[0], R.value_.poly[0]);
}
OUCHI_TEST_CASE(test_div_expr_tmpl) {
    using namespace chao;
    mp_int<sign::mp_signed, 128> i, j;
    i = 3; j = 8;
    mp_int<sign::mp_signed, 128> r;
    mp_int<sign::mp_signed, 128> R = 2;
    r = j / i;
    OUCHI_REQUIRE_TRUE(r == R);
    i = -3; j = -8;
    r = j / i;
    OUCHI_REQUIRE_TRUE(r == R);
    i = -2; j = 8;
    r = j / i;
    R = -4;
    OUCHI_REQUIRE_TRUE(r == R);
}
OUCHI_TEST_CASE(test_mod_expr_tmpl) {
    using namespace chao;
    mp_int<sign::mp_signed, 128> i, j;
    i = 3; j = 8;
    mp_int<sign::mp_signed, 128> r;
    mp_int<sign::mp_signed, 128> R = 2;
    r = j % i;
    OUCHI_REQUIRE_TRUE(r == R);
    i = -3; j = -8;
    r = j % i;
    R = -2;
    OUCHI_REQUIRE_EQUAL(r.value_.poly[0], R.value_.poly[0]);
    OUCHI_REQUIRE_EQUAL(r.value_.poly[1], R.value_.poly[1]);
    i = -2; j = 8;
    r = j % i;
    R = 0;
    OUCHI_REQUIRE_EQUAL(r.value_.poly[0], R.value_.poly[0]);
    OUCHI_REQUIRE_EQUAL(r.value_.poly[1], R.value_.poly[1]);
}
OUCHI_TEST_CASE(test_expr_tmpl) {
    using namespace chao;
    mp_int<sign::mp_signed, 128> i, j;
    i = 4; j = 5;
    mp_int<sign::mp_signed, 128> r;
    mp_int<sign::mp_signed, 128> R;
    r = j + i - j;
    R = 4;
    OUCHI_REQUIRE_TRUE(r == R);
    r = (j + i) - (j - i);
    R = 8;
    OUCHI_REQUIRE_TRUE(r == R);
    r = r - r;
    R = 0;
    OUCHI_REQUIRE_EQUAL(r.value_.poly[0], R.value_.poly[0]);
    i = 1;

    r = i | 5;
    r = i & 5;
    r = i ^ 5;
    OUCHI_REQUIRE_EQUAL(r, mp_int(1^5));

    r = 5 | i;
    r = 5 & i;
    r = 5 ^ i;
    OUCHI_REQUIRE_EQUAL(r, mp_int(1^5));

    r = i + 5;
    r = i - 5;
    r = i * 5;
    r = i / 5;
    r = i % 5;
    OUCHI_REQUIRE_EQUAL(r, mp_int(1));

    r = 5 + i;
    r = 5 - i;
    r = 5 * i;
    r = 5 / i;
    r = 5 % i;
    OUCHI_REQUIRE_EQUAL(r, mp_int(0));

    r += 5;
    r -= 5;
    r *= 5;
    r /= 5;
    r %= 5;
    OUCHI_REQUIRE_EQUAL(r, mp_int(0));
}

#if 0
OUCHI_TEST_CASE(mp_int_tmpexpr_add_benchmark) {
    using namespace chao;
    constexpr unsigned int loop = 0x1ffff;
    const auto seed = std::random_device{}();
    std::mt19937_64 r(seed);
    std::uint64_t A = r(), B = r();
    mp_int<sign::mp_unsigned, 64> a = A, b = B, res = 0;
    std::uint64_t RES = 0;

    typedef std::chrono::high_resolution_clock clock;
    auto t = clock::now();
    for(auto i = 0u; i < loop; ++i){

        res += a + a + b + b;
    }
    std::cout << "\nBenchmark 64 bit addition\n" << "naive implementation\t" << std::chrono::duration_cast<std::chrono::microseconds>(clock::now() - t).count() << " us\n";
    r.seed(seed);
    t = clock::now();
    for(auto i = 0u; i < loop; ++i){

        RES += A + A + B + B;
    }
    std::cout << "built-in implementation\t" << std::chrono::duration_cast<std::chrono::microseconds>(clock::now() - t).count() << " us\n";
    OUCHI_REQUIRE_EQUAL(res.value_.poly[0], (std::uint64_t)RES);
}
OUCHI_TEST_CASE(mp_int_tmpexpr_add_benchmark128) {
    using namespace chao;
    constexpr unsigned int loop = 0x1ffff;
    mp_int<sign::mp_unsigned, 128> a, b, res = 0;
    unsigned __int128 A, B;
    unsigned __int128 RES = 0;
    const auto seed = std::random_device{}();
    std::mt19937_64 r(seed);

    typedef std::chrono::high_resolution_clock clock;
    a = r();
    b = r();
    auto t = clock::now();
    for(auto i = 0u; i < loop; ++i){
        res += a + a + b + b + a + a + a + a;
    }
    std::cout << "\nBenchmark 128 bit addition\n" << "naive implementation\t" << std::chrono::duration_cast<std::chrono::microseconds>(clock::now() - t).count() << " us\n";
    r.seed(seed);
    A = r();
    B = r();
    t = clock::now();
    for(auto i = 0u; i < loop; ++i){

        RES += A + A + B + B + A + A + A + A;
    }
    std::cout << "built-in implementation\t" << std::chrono::duration_cast<std::chrono::microseconds>(clock::now() - t).count() << " us\n";
    OUCHI_REQUIRE_EQUAL(res.value_.poly[0], (std::uint64_t)RES);
    OUCHI_REQUIRE_EQUAL(res.value_.poly[1], (std::uint64_t)(RES >> 64));
}
#endif