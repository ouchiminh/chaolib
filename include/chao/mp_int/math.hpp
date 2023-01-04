#pragma once
#include <tuple>
#include "mp_int.hpp"

namespace chao{

template<detail::derived_expression E>
constexpr auto abs(E&& e) noexcept
-> mp_int<
    detail::sign_v<std::remove_cvref_t<E>>,
    detail::bit_length_v<std::remove_cvref_t<E>>
>
{
    return expr_to_mp_int(e < 0 ? -e : e);
}

template<detail::derived_expression E, detail::expression F>
constexpr auto power(E&& base, F exp) noexcept
-> mp_int<
    detail::sign_v<std::remove_cvref_t<E>>,
    detail::bit_length_v<std::remove_cvref_t<E>>
>
{
    decltype(auto) n = expr_to_mp_int(base);
    decltype(auto) expv = expr_to_mp_int(exp);
    mp_int<
        detail::sign_v<std::remove_cvref_t<E>>,
        detail::bit_length_v<std::remove_cvref_t<E>>
    > ret = 1;
    while(expv) {
        expv >>= 1;
        if(expr_to_mp_int(expv & 1)) ret *= n;
        n *= n;
    }
    return ret;
}

/// <summary>
/// 拡張ユークリッド互除法 
/// bに素数を指定し、有限体Z/bZ上でaの逆元を求める。
/// </summary>
/// <returns>d = ax + byとなるような{d, x, y}</returns>
template<class Int>
constexpr auto ex_gcd(Int a, Int b)
-> std::tuple<Int, Int, Int>
{
    if (b == 0) return std::make_tuple((Int)a, (Int)1, (Int)0);
    auto [d, y, x] = ex_gcd<Int>(b, a % b);
    y -= a / b * x;
    return std::make_tuple(d, x, y);
}

template<class Int>
constexpr Int mod_abs(const Int& a, const Int m) noexcept {
    return a < 0 ? (Int)(a % m + m) : (Int)(a % m);
}
}
