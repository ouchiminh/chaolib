#pragma once
#include <tuple>
#include <type_traits>

#include "../mp_int.hpp"

namespace chao {

enum class compatibility {
    incompatible, compatible, dynamic
};

namespace detail {
template<expression E>
struct basic_modulus {
    using int_type = E;
    static constexpr unsigned int bit_length = bit_length_v<E>;
    static constexpr unsigned int r_bit_len = bit_length / 2 - 1;
    static constexpr int_type R = int_type(1) << (r_bit_len);
    static constexpr int_type R_minus = R - 1;
    static constexpr int_type montgomery_reduction(const int_type& T, const int_type& mod, [[maybe_unused]] const int_type& r_inv, const int_type& m_dash) noexcept {
        int_type t = (T + ((T * m_dash) & R_minus) * mod) >> (r_bit_len);
        return t >= mod ? t - mod : t;
    }
    static constexpr int_type montgomery_representation(const int_type& t, const int_type& mod, const int_type& r_inv, const int_type& r_2, const int_type& m_dash) noexcept {
        return montgomery_reduction(t * r_2, mod, r_inv, m_dash);
    }
};

} // namespace detail

template<detail::expression E>
struct dynamic_modulus;

template<unsigned int BitWidth, mp_int<sign::mp_signed, BitWidth> M>
struct [[deprecated]] modulus {
    static constexpr unsigned int bit_length = BitWidth;
    using int_type = mp_int<sign::mp_signed, bit_length>;
    static constexpr int_type value = M;
    static constexpr int_type R = detail::basic_modulus<int_type>::R;
    static constexpr int_type R_inv = mod_abs(std::get<1>(ex_gcd(R, M)), M);
    static constexpr int_type R_2 = ((R % M) * (R % M)) % M;
    static constexpr int_type M_dash = mod_abs<int_type>(R - std::get<1>(ex_gcd(M, R)), R);

    static constexpr int_type montgomery_representation(const int_type& t) noexcept {
        return detail::basic_modulus<int_type>::montgomery_representation(t, value, R_inv, R_2, M_dash);
    }
    static constexpr int_type montgomery_reduction(const int_type& T) noexcept {
        return detail::basic_modulus<int_type>::montgomery_reduction(T, value, R_inv, M_dash); 
    }
    static constexpr int_type remainder(const int_type t) noexcept {
        return montgomery_reduction(montgomery_representation(t));
    }

    constexpr modulus() = default;

    const int_type& get_modulo() const noexcept { return value; }
    template<class OtherType>
    constexpr compatibility compatible_with([[maybe_unused]] const dynamic_modulus<OtherType>& m) const noexcept {
        if(std::is_constant_evaluated()) return compatibility::dynamic;
        return m.get_modulo() == value ? compatibility::compatible : compatibility::incompatible;
    }
    template<unsigned int BW, typename detail::basic_modulus<int_type>::int_type N>
    constexpr compatibility compatible_with(modulus<BW, N>) const noexcept {
        return N == M ? compatibility::compatible : compatibility::incompatible;
    }
    constexpr operator bool() const noexcept { return true; }

    static_assert(value < R);
};

template<mp_int<sign::mp_signed, 128> M>
using modulus128 [[deprecated]] = modulus<128, M>;
template<mp_int<sign::mp_signed, 192> M>
using modulus198 [[deprecated]] = modulus<192, M>;
template<mp_int<sign::mp_signed, 256> M>
using modulus256 [[deprecated]] = modulus<256, M>;
template<mp_int<sign::mp_signed, 384> M>
using modulus384 [[deprecated]] = modulus<384, M>;
template<mp_int<sign::mp_signed, 512> M>
using modulus512 [[deprecated]] = modulus<512, M>;

template<auto M>
struct mod {
    using int_type = std::enable_if_t<detail::expression<std::remove_cvref_t<decltype(M)>> ,std::remove_cvref_t<decltype(M)>>;
    static constexpr unsigned int bit_length = detail::bit_length_v<int_type>;
    static constexpr int_type value = M;
    static constexpr int_type R = detail::basic_modulus<int_type>::R;
    static constexpr int_type R_inv = mod_abs(std::get<1>(ex_gcd(R, M)), M);
    static constexpr int_type R_2 = (R % M) * (R % M) % M;
    static constexpr int_type M_dash = mod_abs<int_type>(R - std::get<1>(ex_gcd(M, R)), R);
    static_assert(value < R);

    static constexpr int_type montgomery_representation(const int_type& t) noexcept {
        return detail::basic_modulus<int_type>::montgomery_representation(t, value, R_inv, R_2, M_dash);
    }
    static constexpr int_type montgomery_reduction(const int_type& T) noexcept {
        return detail::basic_modulus<int_type>::montgomery_reduction(T, value, R_inv, M_dash); 
    }
    static constexpr int_type remainder(const int_type t) noexcept {
        return montgomery_reduction(montgomery_representation(t));
    }
    
    constexpr mod() = default;

    const int_type& get_modulo() const noexcept { return value; }
    template<class OtherType>
    constexpr compatibility compatible_with([[maybe_unused]] const dynamic_modulus<OtherType>& m) const noexcept {
        if(std::is_constant_evaluated()) return compatibility::dynamic;
        return m.get_modulo() == value ? compatibility::compatible : compatibility::incompatible;
    }
    template<auto N>
    constexpr compatibility compatible_with(mod<N>) const noexcept {
        return N == value ? compatibility::compatible : compatibility::incompatible;
    }
    constexpr operator bool() const noexcept { return true; }
};

template<detail::expression E>
struct dynamic_modulus {
    static constexpr unsigned int bit_length = detail::bit_length_v<std::remove_cvref_t<E>>;
    using int_type = E;
    static constexpr int_type R = detail::basic_modulus<int_type>::R;
    dynamic_modulus() = default;
    dynamic_modulus(const int_type& m) noexcept {
        set_modulo(m);
    }

    int_type montgomery_representation(const int_type& t) const noexcept {
        return detail::basic_modulus<int_type>::montgomery_representation(t, value_, R_inv_, R_2_, M_dash_);
    }
    int_type montgomery_reduction(const int_type& T) const noexcept {
        return detail::basic_modulus<int_type>::montgomery_reduction(T, value_, R_inv_, M_dash_); 
    }
    int_type remainder(const int_type& t) const noexcept{
        return montgomery_reduction(montgomery_representation(t));
    }
    void set_modulo(const int_type& m) noexcept {
        assert(m < R);
        std::tie(std::ignore, R_inv_, M_dash_) = ex_gcd(R, m);

        M_dash_ = mod_abs((int_type)(R - M_dash_), R);
        R_inv_ = mod_abs(R_inv_, m);
        R_2_ = ((R % m) * (R % m)) % m;
        value_ = m;
    }
    const int_type& get_modulo() const noexcept { return value_; }

    template<class OtherType>
    compatibility compatible_with(const dynamic_modulus<OtherType>& m) const noexcept {
        if (std::is_constant_evaluated()) return compatibility::dynamic;
        return m.get_modulo() == get_modulo() || (((bool)*this ^ (bool)m) && ((bool)*this || (bool)m)) ? compatibility::compatible : compatibility::incompatible;
    }
    template<unsigned int BW, typename detail::basic_modulus<int_type>::int_type N>
    compatibility compatible_with(modulus<BW, N> m) const noexcept {
        return m.compatible_with(*this);
    }

    operator bool() const noexcept {
        return value_ >= 2 ? true : false;
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

template<auto M>
struct is_compatible<mod<M>, mod<M>>{
    static constexpr compatibility value = compatibility::compatible;
};
template<auto M, auto N>
struct is_compatible<mod<M>, mod<N>>{
    static constexpr compatibility value = M == N ? compatibility::compatible : compatibility::incompatible;
};

template<class M, class N>
struct is_compatible<dynamic_modulus<N>, M> {
    static constexpr compatibility value = compatibility::dynamic;
};
template<class M, class N>
struct is_compatible<M, dynamic_modulus<N>> {
    static constexpr compatibility value = compatibility::dynamic;
};
template<class M, class N>
struct is_compatible<dynamic_modulus<M>, dynamic_modulus<N>> {
    static constexpr compatibility value = compatibility::dynamic;
};

template<class Mod1, class Mod2>
inline constexpr compatibility is_compatible_v = is_compatible<Mod1, Mod2>::value;

} // namespace detail

}
