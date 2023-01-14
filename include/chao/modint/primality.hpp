#pragma once
#include <random>

#include "modulus.hpp"

namespace chao {

template<class T>
T mod_pow(T,T,T);
template<class T>
T mod_mul(T,T,T);

template<class Rnd, class Int>
bool is_prime_number(Int&& n, Rnd random = Rnd{}, int k = 20) noexcept {
    Int d;
    n = n < 0 ? -n : n;
    if(n == 2) return true;
    if(n == 1 || (n & 1) == 0) return false;
    d = n - 1;
    while((d & 1) == 0) d >>= 1;
    for(int i = 0; i < k; ++i){
        Int a = random() % (n-2) + 1;
        Int t = d;
        Int y = mod_pow(a, t, n);
        while((t != (n - 1)) && (y != 1) && (y != n - 1)){
            y = mod_mul(y, y, n);
            t <<= 1;
        }
        if((y != n - 1) && ((t & 1) == 0)) return false;
    }
    return true;
}

}
