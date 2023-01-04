#pragma once
#include <tuple>
#include <type_traits>

#include "../mp_int.hpp"

namespace chao {

enum class compatibility {
    incompatible, compatible, dynamic
};

namespace detail {
template<unsigned int BitWidth>
struct basic_modulus {
    using int_type = mp_int<sign::mp_signed, BitWidth>;
    static constexpr unsigned int r_bit_len = BitWidth / 2;
    static constexpr int_type R = int_type(1) << (r_bit_len);
    static constexpr int_type R_minus = R - 1;
    static constexpr int_type montgomery_reduction(const int_type& T, const int_type& mod, [[maybe_unused]] const int_type& r_inv, const int_type& m_dash) noexcept {
        int_type t = (T + ((T*m_dash) & (R_minus)) * mod) >> (r_bit_len);
        return t >= mod ? t - mod : t;
    }
    static constexpr int_type montgomery_representation(const int_type& t, const int_type& mod, const int_type& r_inv, const int_type& r_2, const int_type& m_dash) noexcept {
        return montgomery_reduction(t * r_2, mod, r_inv, m_dash);
    }
};

} // namespace detail

template<unsigned int BitWidth>
struct dynamic_modulus;

template<unsigned int BitWidth, mp_int<sign::mp_signed, BitWidth> M>
struct modulus {
    using int_type = typename detail::basic_modulus<BitWidth>::int_type;
    static constexpr int_type value = M;
    static constexpr int_type R = detail::basic_modulus<BitWidth>::R;
    static constexpr int_type R_inv = mod_abs(std::get<1>(ex_gcd(R, M)), M);
    static constexpr int_type R_2 = (R % M) * (R % M) % M;
    static constexpr int_type M_dash = mod_abs<int_type>(R - std::get<1>(ex_gcd(M, R)), R);
    static constexpr unsigned int bit_length = BitWidth;

    static constexpr int_type montgomery_representation(const int_type& t) noexcept {
        return detail::basic_modulus<BitWidth>::montgomery_representation(t, value, R_inv, R_2, M_dash);
    }
    static constexpr int_type montgomery_reduction(const int_type& T) noexcept {
        return detail::basic_modulus<BitWidth>::montgomery_reduction(T, value, R_inv, M_dash); 
    }
    static constexpr int_type remainder(const int_type t) noexcept {
        return montgomery_reduction(montgomery_representation(t));
    }

    constexpr modulus() = default;

    const int_type& get_modulo() const noexcept { return value; }
    template<unsigned int BW>
    constexpr compatibility compatible_with([[maybe_unused]] const dynamic_modulus<BW>& m) const noexcept {
        if(std::is_constant_evaluated()) return compatibility::dynamic;
        return m.get_modulo() == value ? compatibility::compatible : compatibility::incompatible;
    }
    template<unsigned int BW, typename detail::basic_modulus<BitWidth>::int_type N>
    constexpr compatibility compatible_with(modulus<BW, N>) const noexcept {
        return N == M ? compatibility::compatible : compatibility::incompatible;
    }
    constexpr operator bool() const noexcept { return true; }

    static_assert(value < R);
};

template<mp_int<sign::mp_signed, 128> M>
using modulus128 = modulus<128, M>;
template<mp_int<sign::mp_signed, 192> M>
using modulus198 = modulus<192, M>;
template<mp_int<sign::mp_signed, 256> M>
using modulus256 = modulus<256, M>;
template<mp_int<sign::mp_signed, 384> M>
using modulus384 = modulus<384, M>;

template<unsigned int BitWidth>
struct dynamic_modulus {
    using int_type = typename detail::basic_modulus<BitWidth>::int_type;
    static constexpr int_type R = detail::basic_modulus<BitWidth>::R;
    static constexpr unsigned int bit_length = BitWidth;
    constexpr dynamic_modulus() = default;
    constexpr dynamic_modulus(const int_type& m) noexcept {
        set_modulo(m);
    }

    constexpr int_type montgomery_representation(const int_type& t) const noexcept {
        return detail::basic_modulus<BitWidth>::montgomery_representation(t, value_, R_inv_, R_2_, M_dash_);
    }
    constexpr int_type montgomery_reduction(const int_type& T) const noexcept {
        return detail::basic_modulus<BitWidth>::montgomery_reduction(T, value_, R_inv_, M_dash_); 
    }
    constexpr int_type remainder(const int_type& t) const noexcept{
        return montgomery_reduction(montgomery_representation(t));
    }
    constexpr void set_modulo(const int_type& m) noexcept {
        assert(m < R);
        std::tie(std::ignore, R_inv_, M_dash_) = ex_gcd(R, m);

        M_dash_ = mod_abs(R - M_dash_, R);
        R_inv_ = mod_abs(R_inv_, m);
        R_2_ = (R % m) * (R % m) % m;
        value_ = m;
    }
    const int_type& get_modulo() const noexcept { return value_; }

    template<unsigned int BW>
    constexpr compatibility compatible_with(const dynamic_modulus<BW>& m) const noexcept {
        if (std::is_constant_evaluated()) return compatibility::dynamic;
        return m.get_modulo() == get_modulo() ? compatibility::compatible : compatibility::incompatible;
    }
    template<unsigned int BW, typename detail::basic_modulus<BitWidth>::int_type N>
    constexpr compatibility compatible_with(modulus<BW, N> m) const noexcept {
        return m.compatible_with(*this);
    }

    constexpr operator bool() const noexcept {
        return value_ > 2 ? true : false;
    }
private:
    int_type value_ = 0;
    int_type R_inv_;
    int_type R_2_;
    int_type M_dash_;
};

namespace detail {

template<class Mod1, class Mod2>
struct is_compatible;

template<unsigned int BW, mp_int<sign::mp_signed, BW> M>
struct is_compatible<modulus<BW, M>, modulus<BW, M>>{
    static constexpr compatibility value = compatibility::compatible;
};
template<unsigned int BW, mp_int<sign::mp_signed, BW> M, mp_int<sign::mp_signed, BW> N>
struct is_compatible<modulus<BW, M>, modulus<BW, N>>{
    static constexpr compatibility value = compatibility::incompatible;
};
template<unsigned int BW, class M>
struct is_compatible<dynamic_modulus<BW>, M> {
    static constexpr compatibility value = compatibility::dynamic;
};
template<unsigned int BW, class M>
struct is_compatible<M, dynamic_modulus<BW>> {
    static constexpr compatibility value = compatibility::dynamic;
};

template<class Mod1, class Mod2>
inline constexpr compatibility is_compatible_v = is_compatible<Mod1, Mod2>::value;

} // namespace detail

}
