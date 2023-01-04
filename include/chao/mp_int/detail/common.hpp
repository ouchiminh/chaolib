#pragma once

#include <array>
#include <bit>
#include <climits>
#include <concepts>
#include <cstdint>
#include <cstring>
#include <initializer_list>
#include <limits>
#include <type_traits>
#include <stdexcept>
#include <string>

#define ERR_PLACE (std::string(" in ") + __func__ + " at " + __FILE__ + ":" + std::to_string(__LINE__))

namespace chao{
enum class sign {
    mp_unsigned, mp_signed
};
constexpr sign operator& (sign r, sign l) noexcept {
    using sign_t = std::underlying_type_t<sign>;
    return (sign)((sign_t)r & (sign_t)l);
}
}

namespace chao::detail{

struct expression_base{};

template<class T>
concept expression = std::is_base_of_v<detail::expression_base, std::remove_cvref_t<T>> || std::is_integral_v<std::remove_cvref_t<T>>;

template<class T>
concept derived_expression = std::is_base_of_v<detail::expression_base, std::remove_cvref_t<T>>;


template<class T, class U = void>
constexpr unsigned int bit_length_v = 0;
template<class T>
constexpr unsigned int bit_length_v<
    T, std::enable_if_t<std::is_integral_v<std::remove_cvref_t<T>>>
> = sizeof(T) * CHAR_BIT;
template<class T>
constexpr unsigned int bit_length_v<
    T, std::enable_if_t<std::is_base_of_v<expression_base, std::remove_cvref_t<T>>>
> = std::remove_cvref_t<T>::bit_length;

template<class T, class U = void>
constexpr unsigned int length_v = 0;
template<class T>
constexpr unsigned int length_v<
    T, std::enable_if_t<std::is_integral_v<std::remove_cvref_t<T>>>
> = 1;
template<class T>
constexpr unsigned int length_v<
    T, std::enable_if_t<std::is_base_of_v<expression_base, std::remove_cvref_t<T>>>
> = std::remove_cvref_t<T>::length;

template<class T, class U = void>
constexpr unsigned int size_v = 0;
template<class T>
constexpr unsigned int size_v<
    T, std::enable_if_t<std::is_integral_v<std::remove_cvref_t<T>>>
> = sizeof(T);
template<class T>
constexpr unsigned int size_v<
    T, std::enable_if_t<std::is_base_of_v<expression_base, std::remove_cvref_t<T>>>
> = std::remove_cvref_t<T>::size;

template<class T, class U = void>
constexpr sign sign_v = sign::mp_signed;
template<class T>
constexpr sign sign_v<
    T, std::enable_if_t<std::is_integral_v<std::remove_cvref_t<T>>>
> = std::is_unsigned_v<std::remove_cvref_t<T>> ? sign::mp_unsigned : sign::mp_signed;
template<class T>
constexpr sign sign_v<
    T, std::enable_if_t<std::is_base_of_v<expression_base, std::remove_cvref_t<T>>>
> = std::remove_cvref_t<T>::sign_value;

template<unsigned int Bits, std::enable_if_t<(Bits > 0 && Bits % 64 == 0 /* && std::has_single_bit(Bits)*/), int> = 0>
class int_representation {
public:
    using coeff_type =  std::uint64_t;
    static constexpr unsigned int bit_length = Bits;
    static constexpr unsigned int byte_length = Bits / CHAR_BIT;
    static constexpr unsigned int coeff_length = byte_length / sizeof(coeff_type);
    static constexpr unsigned int poly_length = coeff_length;
    static constexpr unsigned int poly_byte_length = poly_length * sizeof(coeff_type);
    static constexpr unsigned int length = coeff_length;
    static constexpr unsigned int size = byte_length;

public:
    constexpr int_representation() = default;
    constexpr int_representation(const int_representation&) = default;
    template<unsigned int Other>
    int_representation& operator=(const int_representation<Other>& other) & noexcept {
        return cpy<sign::mp_unsigned, Other>(other);
    }

    template<std::integral T, std::enable_if_t<!std::is_same_v<T, bool>, int> = 0>
    constexpr int_representation& operator=(T i) & noexcept {
        if (std::is_signed_v<T> && i < 0) {
            // std::memset(poly.data(), ~0, poly_byte_length);
            for(auto& i : poly) i = ~0ull;
            poly[0] &= (std::make_signed_t<coeff_type>)i;
        } else {
            // std::memset(poly.data(), 0, poly_byte_length);
            for(auto& i : poly) i = 0ull;
            poly[0] |= (std::make_unsigned_t<coeff_type>)(std::make_unsigned_t<T>)i;
        }
        return *this;
    }
    int_representation& operator=(bool i) & noexcept {
        // std::memset(poly.data(), 0, poly_byte_length);
        for(auto& i : poly) i = 0ull;
        poly[0] = i ? 1 : 0;
        return *this;
    }

    template<sign Sign, unsigned int Other>
    constexpr int_representation& cpy(const int_representation<Other>& src) & noexcept {
        const coeff_type fill = (Sign == sign::mp_signed && src.msb()) ? ~0ull : 0;
        unsigned int i;
        for(i = 0u; i < std::min(length, src.length); ++i) {
            poly[i] = src.poly[i];
        }
        for(; i < length; ++i) {
            poly[i] = fill;
        }

        // if (Sign == sign::mp_signed && src.msb()) {
        //     std::memset(poly.data(), ~0, poly_byte_length);
        // } else {
        //     std::memset(poly.data(), 0, poly_byte_length);
        // }
        // std::memcpy(poly.data(), src.poly.data(), std::min(src.poly_byte_length, poly_byte_length));
        return *this;
    }

    template<sign Sign = sign::mp_unsigned, unsigned int Other, std::enable_if_t<Other != Bits, int> = 0>
    constexpr int_representation(const int_representation<Other>& other) noexcept
    {
        cpy<Sign, Other>(other);
    }

    template<std::integral T, std::enable_if_t<!std::is_same_v<T, bool>, int> = 0>
    constexpr int_representation(T i) noexcept {
        *this = i;
    } 
    constexpr int_representation(bool i) noexcept
        : int_representation(i ? 1 : 0)
    {} 

    // poly should be written by the form bellow
    // il[0] * 1 + il[1] * 2^64^1 + ... il[n] * 2^64^n
    constexpr int_representation(std::initializer_list<coeff_type> il)
        : poly{0}
    {
        if (il.size() > coeff_length)
            throw std::length_error("initializer_list has invalid length" + ERR_PLACE);
        
        auto i = 0ull;
        for(auto k : il) poly[i++] = k;
    }

    [[nodiscard]]
    static constexpr int_representation zero() noexcept {
        return int_representation{0};
    }
    [[nodiscard]]
    constexpr int_representation clone() const noexcept {
        return *this;
    }

    /// @brief fill by 0
    constexpr void flush() noexcept {
        for(auto& i : poly) i = 0;
    }

    [[nodiscard]]
    constexpr bool bitat(int i) const noexcept {
        constexpr unsigned int dig_bit_width = (CHAR_BIT * sizeof(coeff_type));
        static_assert(std::has_single_bit(dig_bit_width));
        return poly[i / dig_bit_width] & (1ULL << (i & (dig_bit_width - 1))); 
    }
    void bitset(int i) noexcept {
        constexpr unsigned int dig_bit_width = (CHAR_BIT * sizeof(coeff_type));
        static_assert(std::has_single_bit(dig_bit_width));
        poly[i / dig_bit_width] |= (1ULL << (i & (dig_bit_width - 1))); 
    }
    void bitreset(int i) noexcept {
        constexpr unsigned int dig_bit_width = (CHAR_BIT * sizeof(coeff_type));
        static_assert(std::has_single_bit(dig_bit_width));
        poly[i / dig_bit_width] &= ((~0ULL - 1) << (i & (dig_bit_width - 1))); 
    }

    /// @brief most significant bit
    /// @return if most significant bit is 1 then true, 0 then false
    [[nodiscard]]
    constexpr bool msb() const noexcept {
        return !!(poly[length - 1] >> (sizeof(coeff_type) * CHAR_BIT - 1));
    }
    [[nodiscard]]
    constexpr operator bool() const noexcept {
        bool a = false;
        for(auto i = 0u; i < coeff_length && !a; ++i) a = a || !!poly[i];
        return a;
    }

    std::array<coeff_type, poly_length> poly;
};

template<unsigned int Bits>
[[nodiscard]]
constexpr int countl_zero(int_representation<Bits> const & a) noexcept {
    using std::countl_zero;
    auto i = a.length;
    int c = 0;
    while (i--)
    {
        c += countl_zero(a.poly[i]);
        if (a.poly[i])
            return c;
    }
    return c;
}
}
