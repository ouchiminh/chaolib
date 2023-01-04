#pragma once
#include <compare>
#include <concepts>
#include <type_traits>
#include <utility>

#include "detail/common.hpp"
#include "detail/opimpl.hpp"

namespace chao{

template<sign Sign, unsigned int BitWidth>
class mp_int;

template<sign Sign, unsigned int BW>
[[nodiscard]]
constexpr auto expr_to_mp_int(const mp_int<Sign, BW>& e) noexcept
-> const mp_int<Sign, BW>&;

template<class E>
[[nodiscard]]
constexpr auto expr_to_mp_int(E&& e) noexcept
-> std::enable_if_t<
    std::is_base_of_v<detail::expression_base, std::remove_cvref_t<E>>,
    mp_int<std::remove_cvref_t<E>::sign_value, std::remove_cvref_t<E>::bit_length>
>;

template<class I>
[[nodiscard]]
constexpr auto expr_to_mp_int(I e) noexcept
-> std::enable_if_t<
    std::is_fundamental_v<std::remove_cvref_t<I>>,
    mp_int<
        std::is_unsigned_v<std::remove_cvref_t<I>> ? sign::mp_unsigned : sign::mp_signed,
        64
    >
>;

template<sign Sign, unsigned int BitWidth>
class mp_int : public detail::expression_base {
    template<sign S, unsigned int BW>
    friend class mp_int;

public:
    detail::int_representation<BitWidth> value_;
    static constexpr unsigned int bit_length = detail::int_representation<BitWidth>::bit_length;
    static constexpr unsigned int length = detail::int_representation<BitWidth>::length;
    static constexpr unsigned int size = detail::int_representation<BitWidth>::size;
    static constexpr sign sign_value = Sign;
    using coeff_type = typename detail::int_representation<BitWidth>::coeff_type;

    mp_int() = default;
    explicit mp_int(const detail::int_representation<BitWidth>& i) noexcept 
        : value_(i)
    {}
    explicit mp_int(std::initializer_list<typename detail::int_representation<BitWidth>::coeff_type>&& il) noexcept
        : value_(il)
    {}
    template<std::integral T>
    constexpr mp_int(T i) noexcept
        : value_(i)
    {}
    template<detail::derived_expression T>
    constexpr mp_int(T&& e) noexcept {
        *this = e;
    }
    template<sign OtherSign, unsigned int OtherBW>
    constexpr mp_int(const mp_int<OtherSign, OtherBW>& other) noexcept
    {
        value_.template cpy<OtherSign & Sign>(other.value_);
    }

    template<sign OtherSign, unsigned int OtherBW>
    constexpr mp_int& operator=(const mp_int<OtherSign, OtherBW>& o) & noexcept {
        value_.poly.template cpy<OtherSign, OtherBW>(o.value_);
    }

    template<detail::derived_expression T>
    constexpr mp_int& operator=(T&& e) & noexcept {
        e.evaluate(*this);
        return *this;
    }
    template<class T>
    mp_int& operator=(T&& e) & noexcept {
        value_ = e;
        return *this;
    }

    // compound assignment operators are implemented in operators.hpp
    template<detail::expression T>
    constexpr mp_int& operator+=(T&& e) & noexcept;
    template<detail::expression T>
    constexpr mp_int& operator-=(T&& e) & noexcept;
    template<detail::expression T>
    constexpr mp_int& operator*=(T&& e) & noexcept;
    template<detail::expression T>
    constexpr mp_int& operator/=(T&& e) & noexcept;
    template<detail::expression T>
    constexpr mp_int& operator%=(T&& e) & noexcept;
    template<detail::expression T>
    constexpr mp_int& operator|=(T&& e) & noexcept;
    template<detail::expression T>
    constexpr mp_int& operator&=(T&& e) & noexcept;
    template<detail::expression T>
    constexpr mp_int& operator^=(T&& e) & noexcept;
    template<detail::expression T>
    constexpr mp_int& operator>>=(T&& e) & noexcept;
    template<detail::expression T>
    constexpr mp_int& operator<<=(T&& e) & noexcept;

    template<detail::expression E>
    constexpr std::strong_ordering operator<=>(const E& e) const noexcept {
        decltype(auto) c = expr_to_mp_int(e);
        auto r = detail::impl_base::cmp<Sign & detail::sign_v<E>>(value_, c.value_);
        return r == 0 ? std::strong_ordering::equal
            : r == 1 ? std::strong_ordering::greater
            : std::strong_ordering::less;
    }
    template<detail::expression E>
    constexpr bool operator==(const E& e) const noexcept {
        decltype(auto) c = expr_to_mp_int(e);
        return (*this <=> c) == std::strong_ordering::equal;
    }

    template<sign S, unsigned int BW>
    constexpr void evaluate(mp_int<S, BW>& dest) const noexcept {
        dest.value_.template cpy<S>(value_);
    }
    [[nodiscard]]
    constexpr const mp_int& evaluate() const noexcept
    {
        return *this;
    }
    [[nodiscard]]
    constexpr operator bool() const noexcept { return (bool)value_; }
    [[nodiscard]]
    constexpr bool operator!() const noexcept { return !(bool)*this; }
    template<std::integral T>
    [[nodiscard]]
    constexpr operator T() const noexcept {
        return static_cast<std::remove_cvref_t<T>>(value_.poly[0]);
    }
    [[nodiscard]]
    constexpr const mp_int& operator+() const noexcept { return *this; }
    [[nodiscard]]
    constexpr mp_int operator-() const noexcept { return (mp_int)(mp_int(0) - *this); }
    [[nodiscard]]
    constexpr mp_int operator~() const noexcept {
        mp_int tmp = *this;
        detail::bitop::opnot(tmp.value_);
        return tmp;
    }
    // prefix
    constexpr const mp_int& operator++() & noexcept {
        return *this += 1;
    }
    constexpr const mp_int& operator--() & noexcept {
        return *this -= 1;
    }
    // postfix
    constexpr mp_int operator++(int) & noexcept {
        auto cp = *this;
        *this += 1;
        return cp;
    }
    constexpr mp_int operator--(int) & noexcept {
        auto cp = *this;
        *this -= 1;
        return cp;
    }
};

template<std::integral T>
mp_int(T i)
-> mp_int<
    std::is_unsigned_v<T> ? sign::mp_unsigned : sign::mp_signed,
    std::max(64u, (unsigned int)(sizeof(T) * CHAR_BIT))
>;

template<sign Sign, unsigned int BW>
[[nodiscard]]
constexpr auto expr_to_mp_int(const mp_int<Sign, BW>& e) noexcept
-> const mp_int<Sign, BW>&
{
    return e;
}

template<class E>
[[nodiscard]]
constexpr auto expr_to_mp_int(E&& e) noexcept
-> std::enable_if_t<
    std::is_base_of_v<detail::expression_base, std::remove_cvref_t<E>>,
    mp_int<std::remove_cvref_t<E>::sign_value, std::remove_cvref_t<E>::bit_length>
>
{
    return e.evaluate();
}

template<class I>
[[nodiscard]]
constexpr auto expr_to_mp_int(I e) noexcept
-> std::enable_if_t<
    std::is_fundamental_v<std::remove_cvref_t<I>>,
    mp_int<
        std::is_unsigned_v<std::remove_cvref_t<I>> ? sign::mp_unsigned : sign::mp_signed,
        64
    >
>
{
    return e;
}

typedef mp_int<sign::mp_signed, 128> int128_t;
typedef mp_int<sign::mp_unsigned, 128> uint128_t;
typedef mp_int<sign::mp_signed, 192> int192_t;
typedef mp_int<sign::mp_unsigned, 192> uint192_t;
typedef mp_int<sign::mp_signed, 256> int256_t;
typedef mp_int<sign::mp_unsigned, 256> uint256_t;
typedef mp_int<sign::mp_signed, 384> int384_t;
typedef mp_int<sign::mp_unsigned, 384> uint384_t;

}
