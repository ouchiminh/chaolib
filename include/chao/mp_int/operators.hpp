#pragma once
#include "mp_int.hpp"
#include "expression.hpp"

namespace chao{

/***** BIT OPERATORS *****/

template<detail::expression L, detail::expression R>
constexpr or_expr<L, R> operator|(const L& lhs, const R& rhs) {
    return or_expr<L, R>(lhs, rhs);
}
template<sign Sign, unsigned int BitWidth>
template<detail::expression T>
constexpr mp_int<Sign, BitWidth>& mp_int<Sign, BitWidth>::operator|=(T&& e) & noexcept {
    return *this = *this | e;
}

template<detail::expression L, detail::expression R>
constexpr and_expr<L, R> operator&(const L& lhs, const R& rhs) {
    return and_expr<L, R>(lhs, rhs);
}
template<sign Sign, unsigned int BitWidth>
template<detail::expression T>
constexpr mp_int<Sign, BitWidth>& mp_int<Sign, BitWidth>::operator&=(T&& e) & noexcept {
    return *this = *this & e;
}

template<detail::expression L, detail::expression R>
constexpr xor_expr<L, R> operator^(const L& lhs, const R& rhs) {
    return xor_expr<L, R>(lhs, rhs);
}
template<sign Sign, unsigned int BitWidth>
template<detail::expression T>
constexpr mp_int<Sign, BitWidth>& mp_int<Sign, BitWidth>::operator^=(T&& e) & noexcept {
    return *this = *this ^ e;
}

template<detail::expression L, detail::expression R>
constexpr shiftr_expr<L, R> operator>>(const L& lhs, const R& rhs) {
    return shiftr_expr<L, R>(lhs, rhs);
}
template<sign Sign, unsigned int BitWidth>
template<detail::expression T>
constexpr mp_int<Sign, BitWidth>& mp_int<Sign, BitWidth>::operator>>=(T&& e) & noexcept {
    return *this = *this >> e;
}

template<detail::expression L, detail::expression R>
constexpr shiftl_expr<L, R> operator<<(const L& lhs, const R& rhs) {
    return shiftl_expr<L, R>(lhs, rhs);
}
template<sign Sign, unsigned int BitWidth>
template<detail::expression T>
constexpr mp_int<Sign, BitWidth>& mp_int<Sign, BitWidth>::operator<<=(T&& e) & noexcept {
    return *this = *this << e;
}

/***** ARITHMETIC OPERATORS *****/

template<detail::expression L, detail::expression R>
constexpr add_expr<L, R> operator+(const L& lhs, const R& rhs) {
    return add_expr<L, R>(lhs, rhs);
}
template<sign Sign, unsigned int BitWidth>
template<detail::expression T>
constexpr mp_int<Sign, BitWidth>& mp_int<Sign, BitWidth>::operator+=(T&& e) & noexcept {
    return *this = *this + e;
}

template<detail::expression L, detail::expression R>
constexpr sub_expr<L, R> operator-(const L& lhs, const R& rhs) {
    return sub_expr<L, R>(lhs, rhs);
}
template<sign Sign, unsigned int BitWidth>
template<detail::expression T>
constexpr mp_int<Sign, BitWidth>& mp_int<Sign, BitWidth>::operator-=(T&& e) & noexcept {
    return *this = *this - e;
}

template<detail::expression L, detail::expression R>
constexpr mul_expr<L, R> operator*(const L& lhs, const R& rhs) {
    return mul_expr<L, R>(lhs, rhs);
}
template<sign Sign, unsigned int BitWidth>
template<detail::expression T>
constexpr mp_int<Sign, BitWidth>& mp_int<Sign, BitWidth>::operator*=(T&& e) & noexcept {
    mp_int<Sign, BitWidth> tmp = *this;
    mp_int<Sign, BitWidth> etmp(e);
    return *this = tmp * etmp;
}

template<detail::expression L, detail::expression R>
constexpr div_expr<L, R> operator/(const L& lhs, const R& rhs) {
    return div_expr<L, R>(lhs, rhs);
}
template<sign Sign, unsigned int BitWidth>
template<detail::expression T>
constexpr mp_int<Sign, BitWidth>& mp_int<Sign, BitWidth>::operator/=(T&& e) & noexcept {
    mp_int<Sign, BitWidth> tmp = *this;
    mp_int<Sign, BitWidth> etmp(e);
    return *this = tmp / etmp;
}

template<detail::expression L, detail::expression R>
constexpr mod_expr<L, R> operator%(const L& lhs, const R& rhs) {
    return mod_expr<L, R>(lhs, rhs);
}
template<sign Sign, unsigned int BitWidth>
template<detail::expression T>
constexpr mp_int<Sign, BitWidth>& mp_int<Sign, BitWidth>::operator%=(T&& e) & noexcept {
    mp_int<Sign, BitWidth> tmp = *this;
    mp_int<Sign, BitWidth> etmp(e);
    return *this = tmp % etmp;
}

}
