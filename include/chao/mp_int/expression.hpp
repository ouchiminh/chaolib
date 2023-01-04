#pragma once
#include <algorithm>

#include "detail/common.hpp"
#include "detail/opimpl.hpp"
#include "mp_int.hpp"

namespace chao{

/****** BIT OPERATION EXPRESSIONS ******/
template<detail::expression E1, detail::expression E2>
class or_expr : public detail::expression_base {
    const E1& e1_;
    const E2& e2_;
public:
    static constexpr unsigned int bit_length = std::max(detail::bit_length_v<E1>, detail::bit_length_v<E2>);
    static constexpr unsigned int length = std::max(detail::length_v<E1>, detail::length_v<E2>);
    static constexpr unsigned int size = std::max(detail::size_v<E1>, detail::size_v<E2>);
    static constexpr sign sign_value = detail::sign_v<E1> & detail::sign_v<E2>;
    using coeff_type = typename detail::int_representation<bit_length>::coeff_type;
    
    constexpr or_expr(const E1& e1, const E2& e2)
        : e1_(e1)
        , e2_(e2)
    {}
    template<sign Sign, unsigned int BW>
    constexpr void evaluate(mp_int<Sign, BW>& dest) const noexcept {
        dest = e1_;
        detail::bitop::opor<Sign & sign_value>(dest.value_, expr_to_mp_int(e2_).value_);
    }
    constexpr mp_int<sign_value, bit_length> evaluate() const noexcept {
        mp_int<sign_value, bit_length> r(e1_);
        detail::bitop::opor<sign_value>(r.value_, expr_to_mp_int(e2_).value_);
        return r;
    }
};
template<detail::expression E1, detail::expression E2>
class and_expr : public detail::expression_base {
    const E1& e1_;
    const E2& e2_;
public:
    static constexpr unsigned int bit_length = std::max(detail::bit_length_v<E1>, detail::bit_length_v<E2>);
    static constexpr unsigned int length = std::max(detail::length_v<E1>, detail::length_v<E2>);
    static constexpr unsigned int size = std::max(detail::size_v<E1>, detail::size_v<E2>);
    static constexpr sign sign_value = detail::sign_v<E1> & detail::sign_v<E2>;
    using coeff_type = typename detail::int_representation<bit_length>::coeff_type;
    
    constexpr and_expr(const E1& e1, const E2& e2)
        : e1_(e1)
        , e2_(e2)
    {}
    template<sign Sign, unsigned int BW>
    constexpr void evaluate(mp_int<Sign, BW>& dest) const noexcept {
        dest = e1_;
        detail::bitop::opand<Sign & sign_value>(dest.value_, expr_to_mp_int(e2_).value_);
    }
    constexpr mp_int<sign_value, bit_length> evaluate() const noexcept {
        mp_int<sign_value, bit_length> r(e1_);
        detail::bitop::opand<sign_value>(r.value_, expr_to_mp_int(e2_).value_);
        return r;
    }
};
template<detail::expression E1, detail::expression E2>
class xor_expr : public detail::expression_base {
    const E1& e1_;
    const E2& e2_;
public:
    static constexpr unsigned int bit_length = std::max(detail::bit_length_v<E1>, detail::bit_length_v<E2>);
    static constexpr unsigned int length = std::max(detail::length_v<E1>, detail::length_v<E2>);
    static constexpr unsigned int size = std::max(detail::size_v<E1>, detail::size_v<E2>);
    static constexpr sign sign_value = detail::sign_v<E1> & detail::sign_v<E2>;
    using coeff_type = typename detail::int_representation<bit_length>::coeff_type;
    
    constexpr xor_expr(const E1& e1, const E2& e2)
        : e1_(e1)
        , e2_(e2)
    {}
    template<sign Sign, unsigned int BW>
    constexpr void evaluate(mp_int<Sign, BW>& dest) const noexcept {
        dest = e1_;
        detail::bitop::opxor<Sign & sign_value>(dest.value_, expr_to_mp_int(e2_).value_);
    }
    constexpr mp_int<sign_value, bit_length> evaluate() const noexcept {
        mp_int<sign_value, bit_length> r(e1_);
        detail::bitop::opxor<sign_value>(r.value_, expr_to_mp_int(e2_).value_);
        return r;
    }
};
template<detail::expression E1, detail::expression E2>
class shiftr_expr : public detail::expression_base {
    const E1& e1_;
    const E2& e2_;
public:
    static constexpr unsigned int bit_length = std::max(detail::bit_length_v<E1>, detail::bit_length_v<E2>);
    static constexpr unsigned int length = std::max(detail::length_v<E1>, detail::length_v<E2>);
    static constexpr unsigned int size = std::max(detail::size_v<E1>, detail::size_v<E2>);
    static constexpr sign sign_value = detail::sign_v<E1> & detail::sign_v<E2>;
    using coeff_type = typename detail::int_representation<bit_length>::coeff_type;
    
    constexpr shiftr_expr(const E1& e1, const E2& e2)
        : e1_(e1)
        , e2_(e2)
    {}
    template<sign Sign, unsigned int BW>
    constexpr void evaluate(mp_int<Sign, BW>& dest) const noexcept {
        assert(expr_to_mp_int(e2_).value_.msb() == false);
        dest = e1_;
        detail::bitop::shiftr<Sign & sign_value>(dest.value_, (unsigned int)expr_to_mp_int(e2_).value_.poly[0]);
    }
    constexpr mp_int<sign_value, bit_length> evaluate() const noexcept {
        mp_int<sign_value, bit_length> r;
        assert(expr_to_mp_int(e2_).value_.msb() == false);
        r = e1_;
        detail::bitop::shiftr<sign_value>(r.value_, (unsigned int)expr_to_mp_int(e2_).value_.poly[0]);
        return r;
    }
};
template<detail::expression E1, detail::expression E2>
class shiftl_expr : public detail::expression_base {
    const E1& e1_;
    const E2& e2_;
public:
    static constexpr unsigned int bit_length = std::max(detail::bit_length_v<E1>, detail::bit_length_v<E2>);
    static constexpr unsigned int length = std::max(detail::length_v<E1>, detail::length_v<E2>);
    static constexpr unsigned int size = std::max(detail::size_v<E1>, detail::size_v<E2>);
    static constexpr sign sign_value = detail::sign_v<E1> & detail::sign_v<E2>;
    using coeff_type = typename detail::int_representation<bit_length>::coeff_type;
    
    constexpr shiftl_expr(const E1& e1, const E2& e2)
        : e1_(e1)
        , e2_(e2)
    {}
    template<sign Sign, unsigned int BW>
    constexpr void evaluate(mp_int<Sign, BW>& dest) const noexcept {
        assert(expr_to_mp_int(e2_).value_.msb() == false);
        dest = e1_;
        detail::bitop::shiftl<Sign & sign_value>(dest.value_, (unsigned int)expr_to_mp_int(e2_).value_.poly[0]);
    }
    constexpr mp_int<sign_value, bit_length> evaluate() const noexcept {
        mp_int<sign_value, bit_length> r(e1_);
        assert(expr_to_mp_int(e2_).value_.msb() == false);
        detail::bitop::shiftl<sign_value>(r.value_, (unsigned int)expr_to_mp_int(e2_).value_.poly[0]);
        return r;
    }
};


/****** ARITHMETIC OPERATION EXPRESSIONS ******/
template<detail::expression E1, detail::expression E2>
class add_expr : public detail::expression_base {
    const E1& e1_;
    const E2& e2_;
public:
    static constexpr unsigned int bit_length = std::max(detail::bit_length_v<E1>, detail::bit_length_v<E2>);
    static constexpr unsigned int length = std::max(detail::length_v<E1>, detail::length_v<E2>);
    static constexpr unsigned int size = std::max(detail::size_v<E1>, detail::size_v<E2>);
    static constexpr sign sign_value = detail::sign_v<E1> & detail::sign_v<E2>;
    using coeff_type = typename detail::int_representation<bit_length>::coeff_type;
    
    constexpr add_expr(const E1& e1, const E2& e2)
        : e1_(e1)
        , e2_(e2)
    {}
    template<sign Sign, unsigned int BW>
    constexpr void evaluate(mp_int<Sign, BW>& dest) const noexcept {
        dest = e1_;
        detail::impl_base::plus(dest.value_, expr_to_mp_int(e2_).value_);
    }
    constexpr mp_int<sign_value, bit_length> evaluate() const noexcept {
        mp_int<sign_value, bit_length> r(e1_);
        detail::impl_base::plus(r.value_, expr_to_mp_int(e2_).value_);
        return r;
    }
};

template<detail::expression E1, detail::expression E2>
class sub_expr : public detail::expression_base {
    const E1& e1_;
    const E2& e2_;
public:
    static constexpr unsigned int bit_length = std::max(detail::bit_length_v<E1>, detail::bit_length_v<E2>);
    static constexpr unsigned int length = std::max(detail::length_v<E1>, detail::length_v<E2>);
    static constexpr unsigned int size = std::max(detail::size_v<E1>, detail::size_v<E2>);
    static constexpr sign sign_value = detail::sign_v<E1> & detail::sign_v<E2>;
    using coeff_type = typename detail::int_representation<bit_length>::coeff_type;

    constexpr sub_expr(const E1& e1, const E2& e2)
        : e1_(e1)
        , e2_(e2)
    {}
    template<sign Sign, unsigned int BW>
    constexpr void evaluate(mp_int<Sign, BW>& dest) const noexcept {
        dest = e1_;
        detail::impl_base::minus(dest.value_, expr_to_mp_int(e2_).value_);
    }
    constexpr mp_int<sign_value, bit_length> evaluate() const noexcept {
        mp_int<sign_value, bit_length> r(e1_);
        detail::impl_base::minus(r.value_, expr_to_mp_int(e2_).value_);
        return r;
    }
};
template<detail::expression E1, detail::expression E2>
class mul_expr : public detail::expression_base {
    const E1& e1_;
    const E2& e2_;
public:
    static constexpr unsigned int bit_length = std::max(detail::bit_length_v<E1>, detail::bit_length_v<E2>);
    static constexpr unsigned int length = std::max(detail::length_v<E1>, detail::length_v<E2>);
    static constexpr unsigned int size = std::max(detail::size_v<E1>, detail::size_v<E2>);
    static constexpr sign sign_value = detail::sign_v<E1> & detail::sign_v<E2>;
    using coeff_type = typename detail::int_representation<bit_length>::coeff_type;

    constexpr mul_expr(const E1& e1, const E2& e2)
        : e1_(e1)
        , e2_(e2)
    {}
    template<sign Sign, unsigned int BW>
    constexpr void evaluate(mp_int<Sign, BW>& dest) const noexcept {
        detail::naive_mul::mul(dest.value_, expr_to_mp_int(e1_).value_, expr_to_mp_int(e2_).value_);
    }
    constexpr mp_int<sign_value, bit_length> evaluate() const noexcept {
        mp_int<sign_value, bit_length> r;
        detail::naive_mul::mul(r.value_, expr_to_mp_int(e1_).value_, expr_to_mp_int(e2_).value_);
        return r;
    }
};
template<detail::expression E1, detail::expression E2>
class div_expr : public detail::expression_base {
    const E1& e1_;
    const E2& e2_;
public:
    static constexpr unsigned int bit_length = std::max(detail::bit_length_v<E1>, detail::bit_length_v<E2>);
    static constexpr unsigned int length = std::max(detail::length_v<E1>, detail::length_v<E2>);
    static constexpr unsigned int size = std::max(detail::size_v<E1>, detail::size_v<E2>);
    static constexpr sign sign_value = detail::sign_v<E1> & detail::sign_v<E2>;
    using coeff_type = typename detail::int_representation<bit_length>::coeff_type;

    constexpr div_expr(const E1& e1, const E2& e2)
        : e1_(e1)
        , e2_(e2)
    {}
    template<sign Sign, unsigned int BW>
    constexpr void evaluate(mp_int<Sign, BW>& dest) const noexcept {
        mp_int<Sign, BW> rem;
        detail::naive_mul::div<sign_value & Sign, std::max(BW, bit_length)>(dest.value_, rem.value_, expr_to_mp_int(e1_).value_, expr_to_mp_int(e2_).value_);
    }
    constexpr mp_int<sign_value, bit_length> evaluate() const noexcept {
        mp_int<sign_value, bit_length> quo;
        mp_int<sign_value, bit_length> rem;
        detail::naive_mul::div<sign_value, bit_length>(quo.value_, rem.value_, expr_to_mp_int(e1_).value_, expr_to_mp_int(e2_).value_);
        return quo;
    }
};
template<detail::expression E1, detail::expression E2>
class mod_expr : public detail::expression_base {
    const E1& e1_;
    const E2& e2_;
public:
    static constexpr unsigned int bit_length = std::max(detail::bit_length_v<E1>, detail::bit_length_v<E2>);
    static constexpr unsigned int length = std::max(detail::length_v<E1>, detail::length_v<E2>);
    static constexpr unsigned int size = std::max(detail::size_v<E1>, detail::size_v<E2>);
    static constexpr sign sign_value = detail::sign_v<E1> & detail::sign_v<E2>;
    using coeff_type = typename detail::int_representation<bit_length>::coeff_type;

    constexpr mod_expr(const E1& e1, const E2& e2)
        : e1_(e1)
        , e2_(e2)
    {}
    template<sign Sign, unsigned int BW>
    constexpr void evaluate(mp_int<Sign, BW>& dest) const noexcept {
        mp_int<Sign, BW> quo;
        detail::naive_mul::div<sign_value & Sign, std::max(BW, bit_length)>(quo.value_, dest.value_, expr_to_mp_int(e1_).value_, expr_to_mp_int(e2_).value_);
    }
    constexpr mp_int<sign_value, bit_length> evaluate() const noexcept {
        mp_int<sign_value, bit_length> rem;
        mp_int<sign_value, bit_length> quo;
        detail::naive_mul::div<sign_value, bit_length>(quo.value_, rem.value_, expr_to_mp_int(e1_).value_, expr_to_mp_int(e2_).value_);
        return rem;
    }
};
}
