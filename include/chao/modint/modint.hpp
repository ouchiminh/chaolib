#pragma once
#include <cassert>
#include <type_traits>
#include <utility>

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
    template<class M>
    friend class modint;
    using mod_type = std::remove_cvref_t<Mod>;
    static constexpr unsigned int bit_length = mod_type::bit_length;
    using int_type = typename mod_type::int_type;
    mod_type mod;

    constexpr modint() = default;
    constexpr explicit modint(const int_type& value, const Mod & m = Mod{}) noexcept
        : mod{m}
        , mrep_(m.montgomery_representation(value))
    {}
    template<class ...Args>
    constexpr explicit modint(const int_type& value, std::in_place_t, Args&& ...args) noexcept
        : mod{std::forward<Args>(args)...}
        , mrep_(mod.montgomery_representation(value))
    {}
    template<detail::derived_mod_expr E>
    constexpr modint(E&& expr) {
        *this = expr;
    }

    constexpr explicit operator int_type() const noexcept {
        return mod.montgomery_reduction(mrep_);
    }
    constexpr int_type get() const noexcept {
        assert(mod);
        return (int_type)*this;
    }

    template<class M>
    constexpr void evaluate(modint<M>& dest) const noexcept {
        assert(mod && mod.compatible_with(dest.mod) == compatibility::compatible);
        dest.mrep_ = mrep_;
        dest.mod = mod;
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
        assert(expr.mod && mod.compatible_with(expr.mod) == compatibility::compatible);
        mrep_ += expr.evaluate().mrep_;
        shorten();
        return *this;
    }
    template<detail::derived_mod_expr E>
    constexpr modint& operator-=(E&& expr) & noexcept {
        assert(expr.mod && mod.compatible_with(expr.mod) == compatibility::compatible);
        mrep_ -= expr.evaluate().mrep_;
        shorten();
        return *this;
    }
    template<detail::derived_mod_expr E>
    constexpr modint& operator*=(E&& expr) & noexcept {
        assert(expr.mod && mod.compatible_with(expr.mod) == compatibility::compatible);
        mrep_ *= expr.evaluate().mrep_;
        mrep_ = mod.montgomery_reduction(mrep_);
        return *this;
    }
    template<detail::derived_mod_expr E>
    constexpr modint& operator/=(E&& expr) & noexcept {
        assert(expr.mod && mod.compatible_with(expr.mod) == compatibility::compatible);
        *this *= expr.evaluate().inv();
        return *this;
    }
    constexpr modint inv() const noexcept {
        modint r = *this;
        r = std::get<1>(ex_gcd(get(), mod.get_modulo()));
        return r;
    }
    template<detail::derived_mod_expr E>
    constexpr auto operator==(const E& expr) const noexcept
    -> std::enable_if_t<detail::is_compatible_v<Mod, typename E::mod_type> != compatibility::incompatible, bool>
    {
        assert(mod && expr.mod && mod.compatible_with(expr.mod) == compatibility::compatible);
        return mrep_ == expr.evaluate().mrep_;
    }
    template<detail::derived_mod_expr E>
    constexpr auto operator!=(const E& expr) const noexcept
    -> std::enable_if_t<detail::is_compatible_v<Mod, typename E::mod_type> != compatibility::incompatible, bool>
    {
        return !(*this == expr);
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
->modint<std::remove_cvref_t<Mod>>;

template<detail::derived_mod_expr E>
modint(E&&)
->modint<typename std::remove_cvref_t<E>::mod_type>;
}
