#pragma once
#include <string_view>
#include <string>
#include <cstring>

#include "mp_int.hpp"
#include "expression.hpp"
#include "operators.hpp"

namespace chao {

namespace detail {

constexpr int detect_base(std::string_view str) noexcept {
    if(str.size() < 2 || str.front() != '0') return 10;
    switch(str[1]) {
        case 'x': [[fallthrough]];
        case 'X':
            return 16;
        case 'b': [[fallthrough]];
        case 'B':
            return 2;
        default:
            return 8;
    }
    return 0;
}
constexpr int digit(char d, int base) noexcept {
    int result = base;
    if('A' <= d && d <= 'Z') d += 'a' - 'A';
    if('0' <= d && d <= '9') result = d - '0';
    else if ('a' <= d && d <= 'z') result = d - 'a' + 10;
    return result < base ? result : -1;
}

}

template<unsigned int BitWidth = 128>
[[nodiscard]]
constexpr mp_int<sign::mp_unsigned, BitWidth> stoul(std::string_view str, size_t* const idx = nullptr, int base = 10) {
    if(str.size() == 0) throw std::invalid_argument("string must have length longer than 1.");
    mp_int<sign::mp_unsigned, BitWidth> result = 0;
    if(base == 0) {
        base = detail::detect_base(str);
        str = str.substr(base == 16 || base == 2 ? 2 : 0);
    }
    for(unsigned int i = 0; i < str.size(); ++i) {
        if(auto d = detail::digit(str[i], base); d >= 0) {
            result *= base;
            result += d;
        } else {
            idx && (*idx = i);
            if(i == 0) throw std::invalid_argument("can't convert into mp_int from string.");
            break;
        }
    }
    return result;
}

template<unsigned int BitWidth = 128>
[[nodiscard]]
constexpr mp_int<sign::mp_signed, BitWidth> stoi(std::string_view str, size_t* const idx = nullptr, int base = 10) {
    if(str.size() == 0) throw std::invalid_argument("string must have length longer than 1.");
    bool is_negative = str.front() == '-';
    mp_int<sign::mp_signed, BitWidth> result = 0;
    str = str.substr(is_negative);
    result = stoul<BitWidth>(str, idx, base);
    return is_negative ? -result : result;
}

template<detail::derived_expression E>
std::string to_string(E&& n) {
    using expr_t = std::remove_cvref_t<E>;
    constexpr unsigned int dec_digit_length = detail::bit_length_v<expr_t> / 3;
    auto cp = n < 0 ? -n : n;
    char buffer[dec_digit_length];
    int i = dec_digit_length - 1;

    buffer[i--] = 0;
    do {
        buffer[i--] = '0' + (int)expr_to_mp_int(cp % 10);
        cp /= 10;
    } while(cp);
    if(n < 0) buffer[i] = '-';
    else ++i;
    return std::string(buffer + i);
}

}
