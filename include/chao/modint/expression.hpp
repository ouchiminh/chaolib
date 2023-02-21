#pragma once
#include <cassert>
#include <type_traits>
#include <utility>

#include "modint.hpp"

namespace chao::detail {


#define DEFINE_MOD_EXPR(expr_name, op) \
template<detail::derived_mod_expr ME, detail::derived_mod_expr MF>\
class expr_name : public detail::mod_expr_base {\
public:\
    static constexpr unsigned int bit_length = std::remove_cvref_t<ME>::bit_length;\
    using mod_type = typename std::remove_cvref_t<ME>::mod_type;\
    using mod_type1 = typename std::remove_cvref_t<ME>::mod_type;\
    using mod_type2 = typename std::remove_cvref_t<MF>::mod_type;\
    using int_type = typename std::remove_cvref_t<ME>::int_type;\
    using mod_int_type = modint<mod_type>;\
\
    const mod_type  mod;\
\
    constexpr expr_name(ME&& exp1, MF&& exp2)\
        : mod(exp1.mod)\
        , exp1_(exp1)\
        , exp2_(exp2)\
    {\
        static_assert(detail::is_compatible_v<mod_type1, mod_type2> != compatibility::incompatible);\
    }\
\
    constexpr modint<mod_type> evaluate() const noexcept {\
        mod_int_type result = exp1_.evaluate();\
        return result op##= exp2_.evaluate();\
    }\
    template<class M>\
    constexpr void evaluate(modint<M>& dest) const noexcept {\
        exp1_.evaluate(dest);\
        dest op##= exp2_;\
    }\
\
private:\
    const ME& exp1_;\
    const MF& exp2_;\
}

DEFINE_MOD_EXPR(mod_add_expr, +);
DEFINE_MOD_EXPR(mod_sub_expr, -);
DEFINE_MOD_EXPR(mod_mul_expr, *);
DEFINE_MOD_EXPR(mod_div_expr, /);

#undef DEFINE_MOD_EXPR
}
