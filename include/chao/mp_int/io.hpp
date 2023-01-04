#pragma once
#include <istream>
#include <ostream>

#include "mp_int.hpp"
#include "convertion.hpp"

namespace chao {

template<detail::derived_expression E>
std::ostream& operator<<(std::ostream& os, E&& e) {
    return os << to_string(expr_to_mp_int(std::forward<E>(e)));
}

template<sign Sign, unsigned int BitWidth>
std::istream& operator>>(std::istream& is, mp_int<Sign, BitWidth>& n) {
    std::string s;
    is >> s;
    if constexpr (Sign == sign::mp_signed) {
        n = chao::stoi<BitWidth>(s);
    } else {
        n = chao::stoul<BitWidth>(s);
    }
    return is;
}

}
