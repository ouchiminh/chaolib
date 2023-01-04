#pragma once
#include <type_traits>

#include "../mp_int.hpp"
#include "modulus.hpp"

namespace chao{

namespace detail {

struct mod_expr_base{};

template<class E>
concept mod_expr = std::is_base_of_v<mod_expr_base, std::remove_cvref_t<E>> || std::is_integral_v<std::remove_cvref_t<E>>;
template<class E>
concept derived_mod_expr = std::is_base_of_v<mod_expr_base, std::remove_cvref_t<E>>;

} // namespace detail



template<class Mod>
class modint : public detail::mod_expr_base {
public:
    using mod_type = std::remove_cvref_t<Mod>;
    static constexpr unsigned int bit_length = mod_type::bit_length;
    using int_type = mp_int<sign::mp_signed, bit_length>;
    mod_type mod;

    constexpr modint() = default;
    constexpr explicit modint(const int_type& value, const Mod & m = Mod{}) noexcept
        : mod{m}
        , mrep_(m.montgomery_representation(value))
    {}
    template<detail::derived_mod_expr E>
    constexpr modint(E&& expr) {
        *this = expr;
    }

    constexpr explicit operator int_type() const noexcept {
        return mod.montgomery_reduction(mrep_);
    }
    constexpr int_type get() const noexcept { return (int_type)*this; }

    constexpr void evaluate(modint& dest) const noexcept {
        dest = *this;
    }
    constexpr const modint& evaluate() const noexcept { return *this; }

    template<detail::derived_mod_expr E>
    constexpr modint& operator=(E&& expr) & noexcept {
        expr.evaluate(*this);
        return *this;
    }
    constexpr modint& operator=(const int_type& value) & noexcept {
        mrep_ = mod.montgomery_representation(value);
        return *this;
    }

    template<detail::derived_mod_expr E>
    constexpr modint& operator+=(E&& expr) & noexcept {
        mrep_ += expr.evaluate().mrep_;
        shorten();
        return *this;
    }
    template<detail::derived_mod_expr E>
    constexpr modint& operator-=(E&& expr) & noexcept {
        mrep_ -= expr.evaluate().mrep_;
        shorten();
        return *this;
    }
    template<detail::derived_mod_expr E>
    constexpr modint& operator*=(E&& expr) & noexcept {
        mrep_ *= expr.evaluate().mrep_;
        mrep_ = mod.montgomery_reduction(mrep_);
        return *this;
    }
    template<detail::derived_mod_expr E>
    constexpr modint& operator/=(E&& expr) & noexcept {
        auto inv = std::get<1>(ex_gcd(expr.evaluate().get(), mod.get_modulo()));
        mrep_ *= inv;
        return *this;
    }
private:
    constexpr void shorten() noexcept {
        if(mrep_ > mod.get_modulo()) mrep_ -= mod.get_modulo();
        else if (mrep_ < 0) mrep_ += mod.get_modulo();
    }
    int_type mrep_;
};

template<class Mod>
modint(const chao::mp_int<sign::mp_signed, std::remove_cvref_t<Mod>::bit_length>&, const Mod &)
->modint<Mod>;

template<detail::derived_mod_expr E>
modint(E&&)
->modint<typename std::remove_cvref_t<E>::mod_type>;
}
