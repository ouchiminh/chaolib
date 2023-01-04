#pragma once
#include "modint.hpp"
#include "expression.hpp"

namespace chao {

#define DEFINE_MOD_OPERATOR(op, expr_name) \
template<detail::derived_mod_expr E, detail::derived_mod_expr F>\
constexpr detail::expr_name<E, F> operator op(E&& e, F&& f) noexcept {\
    return detail::expr_name<E, F>(std::forward<E>(e), std::forward<F>(f));\
}

DEFINE_MOD_OPERATOR(+, mod_add_expr)
DEFINE_MOD_OPERATOR(-, mod_sub_expr)
DEFINE_MOD_OPERATOR(*, mod_mul_expr)
DEFINE_MOD_OPERATOR(/, mod_div_expr)

#undef DEFINE_MOD_OPERATOR

}
