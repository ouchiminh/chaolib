#pragma once
#include <climits>
#include <type_traits>
#include <utility>

#include "mp_int.hpp"

namespace chao {

template<class Rnd, unsigned int BitWidth>
class random_adaptor {
public:
    Rnd rnd;
    template<class ...Args>
    random_adaptor(Args&& ...args) noexcept
        : rnd(std::forward<Args>(args)...)
    {}
    random_adaptor() = default;

    auto operator()() noexcept {
        constexpr auto org_rnd_bit_size = sizeof(std::invoke_result_t<Rnd>) * CHAR_BIT;
        mp_int<sign::mp_unsigned, BitWidth> res(0);
        for(auto i = 0u; i < BitWidth / org_rnd_bit_size; ++i) {
            res <<= org_rnd_bit_size;
            res |= rnd();
        }
        return res;
    }
};

}
