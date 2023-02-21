#pragma once
#include <random>
#include <chrono>
#include <cstdio>
#include <tuple>

#include "ouchitest/ouchitest.hpp"

#include "chao/mp_int/detail/common.hpp"
#include "chao/mp_int/detail/opimpl.hpp"

[[nodiscard]]
unsigned __int128 to_int128(const chao::detail::int_representation<128>& a) {
    unsigned __int128 r;
    r = a.poly[0];
    r |= (unsigned __int128)a.poly[1] << 64;
    return r;
}
template<class Int128 = unsigned __int128, class Rnd>
[[nodiscard]]
auto random_int128(Rnd&& r)
-> std::tuple<chao::detail::int_representation<128>,
    chao::detail::int_representation<128>,
    Int128, Int128>
{
    using namespace chao::detail;
    int_representation<128> a, b;
    Int128 c, d;
    a.poly[0] = r();
    b.poly[0] = r();
    a.poly[1] = r();
    b.poly[1] = r();
    c = to_int128(a);
    d = to_int128(b);
    return {a,b,c,d};
}

OUCHI_TEST_CASE(int_representation_plus_test128)
{
    using namespace chao::detail;
    std::mt19937_64 r(std::random_device{}());
    int_representation<128> a, b;
    unsigned __int128 c, d;
    for(auto i = 0u; i < 1000; ++i) {
        c = a.poly[0] = r();
        d = b.poly[0] = r();
        a.poly[1] = r();
        b.poly[1] = r();
        c += (__int128)a.poly[1] << 64;
        d += (__int128)b.poly[1] << 64;

        impl_base::plus(a, b);
        c += d;
        OUCHI_REQUIRE_EQUAL(a.poly[0], (std::uint64_t)c);
        OUCHI_REQUIRE_EQUAL(a.poly[1], (std::uint64_t)(c >> 64));
        impl_base::plus(a, a);
        c += c;
        OUCHI_REQUIRE_EQUAL(a.poly[0], (std::uint64_t)c);
        OUCHI_REQUIRE_EQUAL(a.poly[1], (std::uint64_t)(c >> 64));
    }

    c = a.poly[0] = ~0ULL;
    d = b.poly[0] = 1;
    a.poly[1] = ~0ULL;
    b.poly[1] = ~0ULL;
    c += (__int128)a.poly[1] << 64;
    d += (__int128)b.poly[1] << 64;

    impl_base::plus(a, b);
    c += d;
    OUCHI_REQUIRE_EQUAL(a.poly[0], (std::uint64_t)c);
    OUCHI_REQUIRE_EQUAL(a.poly[1], (std::uint64_t)(c >> 64));
    impl_base::plus(a, a);
    c += c;
    OUCHI_REQUIRE_EQUAL(a.poly[0], (std::uint64_t)c);
    OUCHI_REQUIRE_EQUAL(a.poly[1], (std::uint64_t)(c >> 64));
}
OUCHI_TEST_CASE(int_representation_plus_test256)
{
    using namespace chao::detail;
    int_representation<256> a{0xffff'ffff'ffff'ffff}, b{1};
    impl_base::plus(a,b);
    OUCHI_CHECK_EQUAL(a.poly[0], 0ull);
    OUCHI_CHECK_EQUAL(a.poly[1], 1ull);

    a = int_representation<256>{~0ull, ~0ull};
    impl_base::plus(a,b);
    OUCHI_CHECK_EQUAL(a.poly[0], 0ull);
    OUCHI_CHECK_EQUAL(a.poly[1], 0ull);
    OUCHI_CHECK_EQUAL(a.poly[2], 1ull);
    OUCHI_CHECK_EQUAL(a.poly[3], 0ull);

    a = int_representation<256>{~0ull, 0x7fff'ffff'ffff'ffff};
    b = int_representation<256>{1, 0x8000'0000'0000'0000};
    impl_base::plus(a,b);
    OUCHI_CHECK_EQUAL(a.poly[0], 0ull);
    OUCHI_CHECK_EQUAL(a.poly[1], 0ull);
    OUCHI_CHECK_EQUAL(a.poly[2], 1ull);
    OUCHI_CHECK_EQUAL(a.poly[3], 0ull);
    a = int_representation<256>{1, 0x8000'0000'0000'0000};
    b = int_representation<256>{~0ull, 0x7fff'ffff'ffff'ffff};
    impl_base::plus(a,b);
    OUCHI_CHECK_EQUAL(a.poly[0], 0ull);
    OUCHI_CHECK_EQUAL(a.poly[1], 0ull);
    OUCHI_CHECK_EQUAL(a.poly[2], 1ull);
    OUCHI_CHECK_EQUAL(a.poly[3], 0ull);

    a = int_representation<256>{~0ull, ~0ull};
    b = int_representation<256>{1ull, 1ull};
    impl_base::plus(a,b);
    OUCHI_CHECK_EQUAL(a.poly[0], 0ull);
    OUCHI_CHECK_EQUAL(a.poly[1], 1ull);
    OUCHI_CHECK_EQUAL(a.poly[2], 1ull);
    OUCHI_CHECK_EQUAL(a.poly[3], 0ull);

    a = int_representation<256>{1, ~0ull};
    impl_base::plus(a,a);
    OUCHI_CHECK_EQUAL(a.poly[0], 2ull);
    OUCHI_CHECK_EQUAL(a.poly[1], 0xffff'ffff'ffff'fffe);
    OUCHI_CHECK_EQUAL(a.poly[2], 1ull);
    OUCHI_CHECK_EQUAL(a.poly[3], 0ull);

    a = int_representation<256>{1};
    b = int_representation<256>{~0ull, ~0ull};
    impl_base::plus(a,b);
    OUCHI_CHECK_EQUAL(a.poly[0], 0ull);
    OUCHI_CHECK_EQUAL(a.poly[1], 0ull);
    OUCHI_CHECK_EQUAL(a.poly[2], 1ull);
    OUCHI_CHECK_EQUAL(a.poly[3], 0ull);

}

OUCHI_TEST_CASE(int_representation_minus_test_fundamental)
{
    using namespace chao::detail;
    std::mt19937_64 r(std::random_device{}());
    int_representation<128> a;
    std::uint64_t b;
    unsigned __int128 c, d;
    for(auto i = 0u; i < 0xFFFF; ++i) {
        a.poly[0] = r();
        a.poly[1] = r();
        b = 0x0;

        c = to_int128(a);
        d = b;

        impl_base::minus(a, b, 0);
        c -= d;
        OUCHI_REQUIRE_EQUAL(a.poly[0], (std::uint64_t)c);
        OUCHI_REQUIRE_EQUAL(a.poly[1], (std::uint64_t)(c >> 64));
        impl_base::minus(a, a);
        OUCHI_REQUIRE_EQUAL(a.poly[0], (std::uint64_t)0);
        OUCHI_REQUIRE_EQUAL(a.poly[1], (std::uint64_t)0);
    }
    for(auto i = 0u; i < 0xFFFF; ++i) {
        a.poly[0] = r();
        a.poly[1] = r();
        b = r();

        c = to_int128(a);
        d = b;

        impl_base::minus(a, b, 0);
        c -= d;
        OUCHI_REQUIRE_EQUAL(a.poly[0], (std::uint64_t)c);
        OUCHI_REQUIRE_EQUAL(a.poly[1], (std::uint64_t)(c >> 64));
        impl_base::minus(a, a);
        OUCHI_REQUIRE_EQUAL(a.poly[0], (std::uint64_t)0);
        OUCHI_REQUIRE_EQUAL(a.poly[1], (std::uint64_t)0);
    }
}

OUCHI_TEST_CASE(int_representation_minus_test128)
{
    using namespace chao::detail;
    std::mt19937_64 r(std::random_device{}());
    int_representation<128> a, b;
    unsigned __int128 c, d;
    for(auto i = 0u; i < 0xFFFF; ++i) {
        a.poly[0] = r();
        a.poly[1] = r();
        b.poly[0] = 0;
        b.poly[1] = 0;

        c = to_int128(a);
        d = to_int128(b);

        impl_base::minus(a, b);
        c -= d;
        OUCHI_REQUIRE_EQUAL(a.poly[0], (std::uint64_t)c);
        OUCHI_REQUIRE_EQUAL(a.poly[1], (std::uint64_t)(c >> 64));
        impl_base::minus(a, a);
        OUCHI_REQUIRE_EQUAL(a.poly[0], (std::uint64_t)0);
        OUCHI_REQUIRE_EQUAL(a.poly[1], (std::uint64_t)0);
    }
    for(auto i = 0u; i < 0xFFFF; ++i) {
        a.poly[0] = r();
        a.poly[1] = r();
        b.poly[0] = r();
        b.poly[1] = r();

        c = to_int128(a);
        d = to_int128(b);

        impl_base::minus(a, b);
        c -= d;
        OUCHI_REQUIRE_EQUAL(a.poly[0], (std::uint64_t)c);
        OUCHI_REQUIRE_EQUAL(a.poly[1], (std::uint64_t)(c >> 64));
        impl_base::minus(a, a);
        OUCHI_REQUIRE_EQUAL(a.poly[0], (std::uint64_t)0);
        OUCHI_REQUIRE_EQUAL(a.poly[1], (std::uint64_t)0);
    }
}
OUCHI_TEST_CASE(int_representation_minus_test_different_size)
{
    using namespace chao::detail;
    std::mt19937_64 r(std::random_device{}());
    int_representation<128> a;
    int_representation<64> b;
    unsigned __int128 c, d;
    for(auto i = 0u; i < 0xFFFF; ++i) {
        c = a.poly[0] = r();
        d = b.poly[0] = r();
        a.poly[1] = r();
        c |= (unsigned __int128)a.poly[1] << 64;

        impl_base::minus(a, b);
        c -= d;
        OUCHI_REQUIRE_EQUAL(a.poly[0], (std::uint64_t)c);
        OUCHI_REQUIRE_EQUAL(a.poly[1], (std::uint64_t)(c >> 64));
        impl_base::minus(a, a);
        c -= c;
        OUCHI_REQUIRE_EQUAL(a.poly[0], (std::uint64_t)c);
        OUCHI_REQUIRE_EQUAL(a.poly[1], (std::uint64_t)(c >> 64));
    }
}

OUCHI_TEST_CASE(int_representation_minus_test256)
{
    using namespace chao::detail;
    int_representation<256> a{0,1}, b{1};
    impl_base::minus(a,b);
    OUCHI_CHECK_EQUAL(a.poly[0], ~0ull);
    OUCHI_CHECK_EQUAL(a.poly[1], 0ull);

    a = int_representation<256>{0ull,0ull,1ull};
    b = int_representation<256>{1ull};
    impl_base::minus(a,b);
    OUCHI_CHECK_EQUAL(a.poly[0], ~0ull);
    OUCHI_CHECK_EQUAL(a.poly[1], ~0ull);
    OUCHI_CHECK_EQUAL(a.poly[2], 0ull);
    OUCHI_CHECK_EQUAL(a.poly[3], 0ull);

    std::mt19937_64 r(std::random_device{}());
    for(auto i = 0u; i < 1000; ++i) {
        a = int_representation<256>{r(), r(), r(), r()};
        impl_base::minus(a,a);
        OUCHI_REQUIRE_EQUAL(a.poly[0], 0ull);
        OUCHI_REQUIRE_EQUAL(a.poly[1], 0ull);
        OUCHI_REQUIRE_EQUAL(a.poly[2], 0ull);
        OUCHI_REQUIRE_EQUAL(a.poly[3], 0ull);
    }
    constexpr int_representation<128> c{1,1};
    impl_base::plus(a,c);
}

OUCHI_TEST_CASE(int_representation_karatsuba_digit_test128) {
    using namespace chao::detail;
    std::uint64_t a = ~0ull, b = 2, dest;
    unsigned __int128 a128, b128;
    std::mt19937_64 r(std::random_device{}());
    OUCHI_CHECK_EQUAL(naive_mul::mul(dest, a, b), 1ull);
    OUCHI_CHECK_EQUAL(dest, ~0ull << 1);
    for(auto i = 0u; i < 1000; ++i){
        a128 = a = r();
        b128 = b = r();
        a128 *= b128;
        OUCHI_REQUIRE_EQUAL(naive_mul::mul(dest, a, b), static_cast<std::uint64_t>(a128 >> 64));
        OUCHI_REQUIRE_EQUAL(dest, static_cast<std::uint64_t>(a128));
    }
}
OUCHI_TEST_CASE(int_representation_mul_test128) {
    using namespace chao::detail;
    int_representation<128> a, b, dest;
    unsigned __int128 a128, b128;
    std::mt19937_64 r(std::random_device{}());
    for(auto i = 0u; i < 1000; ++i){
        a128 = a.poly[0] = r();
        a.poly[1] = r();
        b128 = b.poly[0] = r();
        b.poly[1] = r();
        a128 += (__int128)a.poly[1] << 64;
        b128 += (__int128)b.poly[1] << 64;

        a128 *= b128;
        naive_mul::mul(dest, a, b);
        OUCHI_REQUIRE_EQUAL(dest.poly[0], (std::uint64_t)a128);
        OUCHI_REQUIRE_EQUAL(dest.poly[1], static_cast<std::uint64_t>(a128>>64));
    }
}
OUCHI_TEST_CASE(int_representation_karatsuba_test128) {
    using namespace chao::detail;
    int_representation<128> a, b, dest;
    unsigned __int128 a128, b128;
    std::mt19937_64 r(std::random_device{}());
    for(auto i = 0u; i < 1000; ++i){
        a128 = a.poly[0] = r();
        a.poly[1] = r();
        b128 = b.poly[0] = r();
        b.poly[1] = r();
        a128 += (__int128)a.poly[1] << 64;
        b128 += (__int128)b.poly[1] << 64;

        a128 *= b128;
        karatsuba::mul(dest, a, b);
        OUCHI_REQUIRE_EQUAL(dest.poly[0], (std::uint64_t)a128);
        OUCHI_REQUIRE_EQUAL(dest.poly[1], static_cast<std::uint64_t>(a128>>64));
    }
}
OUCHI_TEST_CASE(signed_int_representation_mul_test128) {
    using namespace chao::detail;
    int_representation<128> a, b, dest;
    signed __int128 a128, b128;
    std::mt19937_64 r(std::random_device{}());
    for(auto i = 0u; i < 0xFFFF; ++i){
        a128 = a.poly[0] = r();
        a.poly[1] = r();
        b128 = b.poly[0] = r();
        b.poly[1] = r();
        a128 += (__int128)a.poly[1] << 64;
        b128 += (__int128)b.poly[1] << 64;

        a128 *= b128;
        naive_mul::mul(dest, a, b);
        OUCHI_CHECK_EQUAL(dest.poly[0], (std::uint64_t)a128);
        OUCHI_REQUIRE_EQUAL(dest.poly[1], static_cast<std::uint64_t>(a128>>64));
    }
    for(auto i = 0u; i < 0xFFFF; ++i){
        a128 = a.poly[0] = r();
        a.poly[1] = r() | (1LL << 63);
        b128 = b.poly[0] = r();
        b.poly[1] = r() | (1LL << 63);
        a128 += (__int128)a.poly[1] << 64;
        b128 += (__int128)b.poly[1] << 64;

        a128 *= b128;
        naive_mul::mul(dest, a, b);
        OUCHI_CHECK_EQUAL(dest.poly[0], (std::uint64_t)a128);
        OUCHI_REQUIRE_EQUAL(dest.poly[1], static_cast<std::uint64_t>(a128>>64));
    }
    for(auto i = 0u; i < 0xFFFF; ++i){
        a128 = a.poly[0] = r();
        a.poly[1] = r();
        b128 = b.poly[0] = r();
        b.poly[1] = r() | (1LL << 63);
        a128 |= (__int128)a.poly[1] << 64;
        b128 |= (__int128)b.poly[1] << 64;

        a128 *= b128;
        naive_mul::mul(dest, a, b);
        OUCHI_CHECK_EQUAL(dest.poly[0], (std::uint64_t)a128);
        OUCHI_REQUIRE_EQUAL(dest.poly[1], static_cast<std::uint64_t>(a128>>64));
    }
}

OUCHI_TEST_CASE(signed_int_representation_karatsuba_test128) {
    using namespace chao::detail;
    int_representation<128> a, b, dest;
    signed __int128 a128, b128;
    std::mt19937_64 r(std::random_device{}());
    for(auto i = 0u; i < 0xFFFF; ++i){
        a128 = a.poly[0] = r();
        a.poly[1] = r();
        b128 = b.poly[0] = r();
        b.poly[1] = r();
        a128 += (__int128)a.poly[1] << 64;
        b128 += (__int128)b.poly[1] << 64;

        a128 *= b128;
        karatsuba::mul(dest, a, b);
        OUCHI_CHECK_EQUAL(dest.poly[0], (std::uint64_t)a128);
        OUCHI_REQUIRE_EQUAL(dest.poly[1], static_cast<std::uint64_t>(a128>>64));
    }
    for(auto i = 0u; i < 0xFFFF; ++i){
        a128 = a.poly[0] = r();
        a.poly[1] = r() | (1LL << 63);
        b128 = b.poly[0] = r();
        b.poly[1] = r() | (1LL << 63);
        a128 += (__int128)a.poly[1] << 64;
        b128 += (__int128)b.poly[1] << 64;

        a128 *= b128;
        karatsuba::mul(dest, a, b);
        OUCHI_CHECK_EQUAL(dest.poly[0], (std::uint64_t)a128);
        OUCHI_REQUIRE_EQUAL(dest.poly[1], static_cast<std::uint64_t>(a128>>64));
    }
    for(auto i = 0u; i < 0xFFFF; ++i){
        a128 = a.poly[0] = r();
        a.poly[1] = r();
        b128 = b.poly[0] = r();
        b.poly[1] = r() | (1LL << 63);
        a128 |= (__int128)a.poly[1] << 64;
        b128 |= (__int128)b.poly[1] << 64;

        a128 *= b128;
        karatsuba::mul(dest, a, b);
        OUCHI_CHECK_EQUAL(dest.poly[0], (std::uint64_t)a128);
        OUCHI_REQUIRE_EQUAL(dest.poly[1], static_cast<std::uint64_t>(a128>>64));
    }
}
#if 0
OUCHI_TEST_CASE(mpint_mul_benchmark128) {
    using namespace chao::detail;
    constexpr unsigned int loop = 1 << 24;
    int_representation<128> a, b, c, dest{0};
    const auto seed = std::random_device{}();
    std::mt19937_64 r(seed);
    unsigned __int128 a128, b128, d128 = 0;

    typedef std::chrono::high_resolution_clock clock;
    a.poly[0] = r() | 1;
    a.poly[1] = r();
    b.poly[0] = r() | 1;
    b.poly[1] = r();
    auto t = clock::now();
    for(auto i = 0u; i < loop; ++i){

        naive_mul::mul(c, a, b);
        impl_base::plus(dest, c);
    }
    std::cout << "\nBenchmark 128 bit multiplication\n" << "naive implementation\t" << std::chrono::duration_cast<std::chrono::microseconds>(clock::now() - t).count() << " us\n";
    r = std::mt19937_64(seed);
    a128 = r() | 1;
    a128 |= (__int128)r() << 64;
    b128 = r() | 1;
    b128 |= (__int128)r() << 64;
    t = clock::now();
    for(auto i = 0u; i < loop; ++i){
        d128 += a128 * b128;
    }
    std::cout << "built-in implementation\t" << std::chrono::duration_cast<std::chrono::microseconds>(clock::now() - t).count() << " us\n";
    OUCHI_REQUIRE_EQUAL(dest.poly[0], (std::uint64_t)d128);
    OUCHI_REQUIRE_EQUAL(dest.poly[1], (std::uint64_t)(d128 >> 64));
}
#endif

OUCHI_TEST_CASE(mpint_bit_shift_test128) {
    using namespace chao::detail;
    constexpr unsigned int loop = 0x1ffff;
    int_representation<128> a;
    const auto seed = std::random_device{}();
    std::mt19937_64 r(seed);
    unsigned __int128 a128;

    for(auto i = 0u; i < loop; ++i) {
        auto w = r() & 127;
        a.poly[0] = r();
        a.poly[1] = r();
        a128 = a.poly[0];
        a128 |= (__int128)a.poly[1] << 64;

        bitop::shiftl(a, w);
        a128 = a128 << w;
        OUCHI_REQUIRE_EQUAL(a.poly[0], (std::uint64_t)a128);
        OUCHI_REQUIRE_EQUAL(a.poly[1], (std::uint64_t)(a128 >> 64));
    }
    for(auto i = 0u; i < loop; ++i) {
        auto w = r() & 127;
        a.poly[0] = r();
        a.poly[1] = r();
        a128 = a.poly[0];
        a128 |= (__int128)a.poly[1] << 64;

        bitop::shiftr<chao::sign::mp_unsigned>(a, w);
        a128 = a128 >> w;
        OUCHI_REQUIRE_EQUAL(a.poly[0], (std::uint64_t)a128);
        OUCHI_REQUIRE_EQUAL(a.poly[1], (std::uint64_t)(a128 >> 64));
    }

    signed __int128 s128;
    for(auto i = 0u; i < loop; ++i) {
        auto w = r() & 127;
        a.poly[0] = r();
        a.poly[1] = r() | (1ull << 63);
        s128 = a.poly[0];
        s128 |= (__int128)a.poly[1] << 64;

        bitop::shiftr<chao::sign::mp_signed>(a, w);
        s128 = s128 >> w;
        OUCHI_REQUIRE_EQUAL(a.poly[0], (std::uint64_t)s128);
        OUCHI_REQUIRE_EQUAL(a.poly[1], (std::uint64_t)(s128 >> 64));
    }
}

OUCHI_TEST_CASE(unsigned_mpint_cmp_test_same_size) {
    using namespace chao::detail;
    constexpr unsigned int loop = 0xffff;
    int_representation<128> a, b;
    const auto seed = std::random_device{}();
    std::mt19937_64 r(seed);
    unsigned __int128 A, B;

    for(auto i = 0u; i < loop; ++i) {
        a.poly[0] = r();
        a.poly[1] = r();
        b.poly[0] = r();
        b.poly[1] = r();

        A = to_int128(a);
        B = to_int128(b);

        auto s = impl_base::cmp<chao::sign::mp_unsigned>(a,b);
        auto S = A < B ? -1
            : A > B ? 1
            : 0;
        OUCHI_REQUIRE_EQUAL(s, S);
    }
    for(auto i = 0u; i < loop; ++i) {
        a.poly[0] = r();
        a.poly[1] = r();
        b.poly[0] = a.poly[0];
        b.poly[1] = a.poly[1];

        auto s = impl_base::cmp<chao::sign::mp_unsigned>(a,b);
        OUCHI_REQUIRE_EQUAL(s, 0);
    }
}

OUCHI_TEST_CASE(unsigned_mpint_cmp_test_different_size) {
    using namespace chao::detail;
    constexpr unsigned int loop = 0x1ffff;
    int_representation<256> a{};
    int_representation<128> b{};
    const auto seed = std::random_device{}();
    std::mt19937_64 r(seed);
    unsigned __int128 A, B;

    for(auto i = 0u; i < loop; ++i) {
        a.poly[0] = r();
        a.poly[1] = r();
        a.poly[2] = 0;
        a.poly[3] = 0;
        b.poly[0] = r();
        b.poly[1] = r();

        A = to_int128(a);
        B = to_int128(b);

        auto s = impl_base::cmp<chao::sign::mp_unsigned>(a,b);
        auto S = A < B ? -1
            : A > B ? 1
            : 0;
        OUCHI_REQUIRE_EQUAL(s, S);
    }
    for(auto i = 0u; i < loop; ++i) {
        a.poly[0] = r();
        a.poly[1] = r();
        b.poly[0] = a.poly[0];
        b.poly[1] = a.poly[1];

        auto s = impl_base::cmp<chao::sign::mp_unsigned>(a,b);
        OUCHI_REQUIRE_EQUAL(s, 0);
    }
}

OUCHI_TEST_CASE(signed_mpint_cmp_test_64) {
    using namespace chao::detail;
    constexpr unsigned int loop = 0xffff;
    int_representation<64> a, b;
    const auto seed = std::random_device{}();
    std::mt19937_64 r(seed);
    std::int64_t A, B;
    for(auto i = 0u; i < loop; ++i) {
        A = a.poly[0] = r();
        B = b.poly[0] = r();

        auto s = impl_base::cmp<chao::sign::mp_signed>(a,b);
        auto S = A < B ? -1
            : A > B ? 1
            : 0;
        impl_base::minus(a,b);
        OUCHI_REQUIRE_EQUAL(a.poly[0], (std::uint64_t)(A - B));
        OUCHI_REQUIRE_EQUAL(s, S);
    }
}

OUCHI_TEST_CASE(signed_mpint_cmp_test_128) {
    using namespace chao::detail;
    constexpr unsigned int loop = 0xffff;
    int_representation<128> a, b;
    const auto seed = std::random_device{}();
    std::mt19937_64 r(seed);
    signed __int128 A, B;

    for(auto i = 0u; i < loop; ++i) {
        a.poly[0] = r();
        a.poly[1] = r();
        b.poly[0] = r();
        b.poly[1] = r();

        A = to_int128(a);
        B = to_int128(b);

        auto s = impl_base::cmp<chao::sign::mp_signed>(a,b);
        auto S = A < B ? -1
            : A > B ? 1
            : 0;

        OUCHI_REQUIRE_EQUAL(s, S);
    }
    for(auto i = 0u; i < loop; ++i) {
        a.poly[0] = r();
        a.poly[1] = r();
        b.poly[0] = a.poly[0];
        b.poly[1] = a.poly[1];

        auto s = impl_base::cmp<chao::sign::mp_signed>(a,b);
        OUCHI_REQUIRE_EQUAL(s, 0);
    }
}

OUCHI_TEST_CASE(signed_mpint_cmp_test_different_size) {
    using namespace chao::detail;
    constexpr unsigned int loop = 0x1ffff;
    int_representation<128> a{};
    int_representation<64> b{};
    const auto seed = std::random_device{}();
    std::mt19937_64 r(seed);
    signed __int128 A, B;

    for(auto i = 0u; i < loop; ++i) {
        a.poly[0] = r();
        a.poly[1] = r();
        B = b.poly[0] = r();

        A = to_int128(a);

        auto s = impl_base::cmp<chao::sign::mp_signed>(a,b);
        auto S = A < B ? -1
            : A > B ? 1
            : 0;
        OUCHI_REQUIRE_EQUAL(s, S);
    }
    for(auto i = 0u; i < loop; ++i) {
        b.poly[0] = a.poly[0] = r();
        a.poly[1] = b.msb() ? ~0ULL : 0;
        B = b.poly[0] = a.poly[0];

        auto s = impl_base::cmp<chao::sign::mp_signed>(a,b);
        OUCHI_REQUIRE_EQUAL(s, 0);
    }
}

OUCHI_TEST_CASE(signed_mp_int_div_test) {
    using namespace chao::detail;
    constexpr unsigned int loop = 0x1ffff;
    const auto seed = std::random_device{}();
    std::mt19937_64 r(seed);
    for(auto i = 0u; i < loop; ++i) {
        int_representation<128> a = r() >> 1, b = ((r() & r()) >> 1) | 1ull, q, rm;
        std::int64_t A = a.poly[0], B = b.poly[0];
        std::int64_t Q = A/B, R = A%B;
        naive_mul::div<chao::sign::mp_signed>(q,rm,a,b);

        OUCHI_REQUIRE_EQUAL(q.poly[0], (std::uint64_t)Q);
        OUCHI_REQUIRE_EQUAL(rm.poly[0], (std::uint64_t)R);
    }
    for(auto i = 0u; i < loop; ++i) {
        int_representation<64> a = r(), b = ((r() & r())) | 1ull, q, rm;
        std::int64_t A = a.poly[0], B = b.poly[0];
        std::int64_t Q = A/B, R = A%B;
        naive_mul::div<chao::sign::mp_signed>(q,rm,a,b);

        OUCHI_REQUIRE_EQUAL(q.poly[0], (std::uint64_t)Q);
        OUCHI_REQUIRE_EQUAL(rm.poly[0], (std::uint64_t)R);
    }
}

OUCHI_TEST_CASE(unsigned_mp_int_div_test) {
    using namespace chao::detail;
    constexpr unsigned int loop = 0x1ffff;
    const auto seed = std::random_device{}();
    std::mt19937_64 r(seed);
    for(auto i = 0u; i < loop; ++i) {
        int_representation<128> a = r(), b = ((r() & r())) | 1ull, q, rm;
        std::uint64_t A = a.poly[0], B = b.poly[0];
        std::uint64_t Q = A/B, R = A%B;
        naive_mul::div<chao::sign::mp_unsigned>(q,rm,a,b);

        OUCHI_REQUIRE_EQUAL(q.poly[0], (std::uint64_t)Q);
        OUCHI_REQUIRE_EQUAL(rm.poly[0], (std::uint64_t)R);
    }
    for(auto i = 0u; i < loop; ++i) {
        int_representation<64> a = r(), b = ((r() & r())) | 1ull, q, rm;
        std::uint64_t A = a.poly[0], B = b.poly[0];
        std::uint64_t Q = A/B, R = A%B;
        naive_mul::div<chao::sign::mp_unsigned>(q,rm,a,b);

        OUCHI_REQUIRE_EQUAL(q.poly[0], (std::uint64_t)Q);
        OUCHI_REQUIRE_EQUAL(rm.poly[0], (std::uint64_t)R);
    }
}
