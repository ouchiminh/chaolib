#pragma once

#include <algorithm>
#include <bit>
#include <cassert>
#include <climits>
#include <compare>
#include <concepts>
#include <cstdint>
#include <iterator>
#include <limits>
#include <type_traits>

#include "common.hpp"

namespace chao::detail{

class impl_base {
public:
    typedef typename int_representation<64>::coeff_type _int_type;

    /// @brief 整数同士の足し算．a += bとなり，繰り上がりが返される．
    /// @tparam Digit 整数型(符号なし基本型)
    /// @param a 左辺．
    /// @param b 右辺
    /// @return 繰り上がり
    template<class Digit = std::uint64_t>
    static constexpr auto plus(Digit& a, Digit b) noexcept
    -> std::enable_if_t<std::is_fundamental_v<Digit> && std::is_unsigned_v<Digit>, bool>
    {
        a += b;
        return a < b;
    }
    template<std::random_access_iterator Itr, class Digit>
    static constexpr auto plus(Itr dest, int destlen, Digit b) noexcept
    -> std::enable_if_t<std::is_fundamental_v<Digit> && std::is_unsigned_v<Digit> && std::is_same_v<typename std::iterator_traits<Itr>::value_type, Digit>, bool>
    {
        bool cr = false;
        for(int i = 0; i < destlen; ++i) {
            b = plus(*dest, b);
            std::advance(dest, 1);
        }
        return cr;
    }
    template<unsigned int Bits, std::integral Digit = typename int_representation<Bits>::coeff_type>
    static constexpr auto plus(int_representation<Bits>& a, Digit b, unsigned int offset_in_coeff = 0) noexcept
    -> std::enable_if_t<std::is_fundamental_v<Digit> && std::is_unsigned_v<Digit>, bool>
    {
        while(b && offset_in_coeff < a.coeff_length) {
            b = plus(a.poly[offset_in_coeff++], b);
        }
        return b;
    }

    /// @brief addition between 2 int_representation. First argument must have longer or same bit width.
    /// @tparam Bits1 bit width of a. Bits1 >= Bits2
    /// @tparam Bits2 bit width of b. Bits2 <= Bits1
    /// @param a integer
    /// @param b integer
    /// @return carry
    template<unsigned int Bits1, unsigned int Bits2>
    static constexpr auto plus(int_representation<Bits1>& a, const int_representation<Bits2>& b) noexcept
    -> std::enable_if_t<(Bits1 >= Bits2), bool>
    {
        typename int_representation<Bits1>::coeff_type carry = 0;
        auto i = 0u;
        for(; i < b.coeff_length; ++i) {
            typename int_representation<Bits1>::coeff_type bcp = b.poly[i];   
            carry = plus(a.poly[i], carry);
            carry |= plus(a.poly[i], bcp);
        }
        for(; carry && i < a.coeff_length; ++i) {
            carry = plus(a.poly[i], carry);
        }
        return !!carry;
    }


    template<unsigned int Bits, std::integral Digit>
    static constexpr auto minus(int_representation<Bits>& a, Digit b, unsigned int offset_in_coeff = 0) noexcept
    -> std::enable_if_t<std::is_fundamental_v<Digit> && std::is_unsigned_v<Digit>, bool>
    {
        using digit_t = typename int_representation<Bits>::coeff_type;
        digit_t carry = 1;
        unsigned int i;
        constexpr digit_t expansion = ~0ull;
        carry = impl_base::plus(a.poly[offset_in_coeff], carry);
        carry |= impl_base::plus(a.poly[offset_in_coeff], ~b);
        for(i = offset_in_coeff + 1; i < a.length; ++i) {
            carry = impl_base::plus(a.poly[i], carry);
            carry |= impl_base::plus(a.poly[i], expansion);
        }
        return carry;
    }

    /// @brief addition between 2 int_representation. First argument must have longer or same bit width.
    /// @tparam Bits1 bit width of a. Bits1 >= Bits2
    /// @tparam Bits2 bit width of b. Bits2 <= Bits1
    /// @param a integer
    /// @param b integer
    /// @return carry
    template<unsigned int Bits1, unsigned int Bits2>
    static constexpr auto minus(int_representation<Bits1>& a, const int_representation<Bits2>& b) noexcept
    -> std::enable_if_t<(Bits1 >= Bits2), bool>
    {
        using digit_t = typename int_representation<Bits1>::coeff_type;
        digit_t carry = 1;
        constexpr digit_t expansion = ~0ull;
        auto i = 0u;
        for(; i < b.coeff_length; ++i) {
            digit_t bcp = b.poly[i];
            carry = plus(a.poly[i], carry);
            carry |= plus(a.poly[i], ~bcp);
        }
        for(; i < a.coeff_length; ++i) {
            carry = plus(a.poly[i], carry);
            carry |= plus(a.poly[i], expansion);
        }
        return !!carry;
    }

    template<sign Sign, unsigned int Len1, unsigned int Len2>
    static constexpr auto cmp(const _int_type* a, const _int_type* b) noexcept
    -> std::enable_if_t<(Len1 >= Len2), int>
    {
        auto msb = [](const _int_type *t, unsigned int len)
        {
            return t[len - 1] >> (sizeof(_int_type) * 8 - 1);
        };
        if ((Sign == sign::mp_signed) && (msb(a, Len1) != msb(b, Len2)))
        {
            return msb(a, Len1) ? -1 : 1;
        }
        constexpr auto n = std::min(Len1, Len2), m = std::max(Len1, Len2);
        const auto d = (Sign == sign::mp_signed && msb(b, Len2)) ? ~0ULL : 0;
        auto i = m;
        while(n != m && i-- > n) {
            if(a[i] != d) return (Sign == sign::mp_signed && msb(b, Len2)) ? -1 : 1;
        }
        i = n;
        while(i--) {
            if(a[i] > b[i]) return 1;
            if(a[i] < b[i]) return -1;
        }
        return 0;
    }

    template<sign Sign, unsigned int Len1, unsigned int Len2>
    static constexpr auto cmp(const _int_type* a, const _int_type* b) noexcept
    -> std::enable_if_t<(Len2 > Len1), int>
    {
        -cmp<Sign, Len2, Len1>(b, a);
    }
    template<sign Sign, unsigned int Bits1, unsigned int Bits2>
    static constexpr auto cmp(const int_representation<Bits1>& a, const int_representation<Bits2>& b) noexcept
    -> std::enable_if_t<(Bits1 >= Bits2), int>
    {
        return cmp<Sign, a.length, b.length>(a.poly.data(), b.poly.data());
    }
    template<sign Sign, unsigned int Bits1, unsigned int Bits2>
    static constexpr auto cmp(const int_representation<Bits1>& a, const int_representation<Bits2>& b) noexcept
    -> std::enable_if_t<(Bits1 < Bits2), int>
    {
        return -cmp<Sign>(b, a);
    }
};

class bitop{
    template<sign Sign>
    static constexpr auto fill_bits(bool msb) noexcept
        -> typename int_representation<128>::coeff_type
    {
        using ctype = typename int_representation<128>::coeff_type;
        return ~((ctype)(Sign == sign::mp_signed && msb) - 1);
    }
public:
    template<sign Signb, unsigned int Bits1, unsigned int Bits2>
    static constexpr auto opor(int_representation<Bits1>& a, const int_representation<Bits2>& b) noexcept
    -> void
    {
        auto i = 0u;
        while(i < std::min(a.coeff_length, b.coeff_length)) {
            a.poly[i] |= b.poly[i];
            ++i;
        }
        if constexpr (Bits2 < Bits1) {
            // typename int_representation<Bits1>::coeff_type fill_bit = (Signb == sign::mp_signed) && b.msb() ? ~0ull : 0;
            auto fill_bit = fill_bits<Signb>(b.msb());
            while(i < a.coeff_length){
                a.poly[i++] |= fill_bit;
            }
        }
    }
    template<sign Signb, unsigned int Bits1, unsigned int Bits2>
    static constexpr auto opand(int_representation<Bits1>& a, const int_representation<Bits2>& b) noexcept
    -> void
    {
        auto i = 0u;
        while(i < std::min(a.coeff_length, b.coeff_length)) {
            a.poly[i] &= b.poly[i];
            ++i;
        }
        if constexpr (Bits2 < Bits1) {
            auto fill_bit = fill_bits<Signb>(b.msb());
            while(i < a.coeff_length){
                a.poly[i++] &= fill_bit;
            }
        }
    }
    template<unsigned int Bits>
    static constexpr auto opnot(int_representation<Bits>& a) noexcept
    -> void
    {
        for(auto i = 0u; i < a.coeff_length; ++i) {
            a.poly[i] ^= ~0ull;
        }
    }

    template<sign Signb, unsigned int Bits1, unsigned int Bits2>
    static constexpr auto opxor(int_representation<Bits1>& a, const int_representation<Bits2>& b) noexcept
    -> void
    {
        auto i = 0u;
        while(i < std::min(a.coeff_length, b.coeff_length)) {
            a.poly[i] ^= b.poly[i];
            ++i;
        }
        if constexpr (Bits2 < Bits1) {
            typename int_representation<Bits1>::coeff_type fill_bit = (Signb == sign::mp_signed) && b.msb() ? ~0ull : 0;
            while(i < a.coeff_length){
                a.poly[i++] ^= fill_bit;
            }
        }
    }
    template<sign Sign, unsigned int Bits>
    static constexpr void shiftr(int_representation<Bits>& a, unsigned int width) noexcept
    {
        using ctype = typename int_representation<Bits>::coeff_type;
        constexpr auto coeff_width = sizeof(typename int_representation<Bits>::coeff_type) * CHAR_BIT;
        typename int_representation<Bits>::coeff_type buff0 = 0, buff1;
        const auto offset = width / coeff_width; // qword
        const auto remain = width & (coeff_width - 1);
        int i = a.coeff_length - offset;

        buff0 = ~((ctype)(Sign == sign::mp_signed && a.msb()) - 1) << (coeff_width - remain);
        if(offset) {
            std::copy(a.poly.data() + offset, a.poly.data() + (a.coeff_length), a.poly.data());
            std::fill_n(a.poly.data() + a.coeff_length - offset, offset, buff0 ? ~0 : 0);
        }
        while(remain && i-->0) {
            buff1 = a.poly[i] << (coeff_width - remain);
            a.poly[i] >>= remain;
            a.poly[i] |= buff0;
            buff0 = buff1;
        }
    }
    template<sign Sign = sign::mp_unsigned, unsigned int Bits>
    static constexpr void shiftl(int_representation<Bits>& a, unsigned int width) noexcept
    {
        constexpr auto coeff_width = sizeof(typename int_representation<Bits>::coeff_type) * CHAR_BIT;
        typename int_representation<Bits>::coeff_type buff0 = 0, buff1;
        const auto offset = width >> std::countr_zero(coeff_width); // qword
        const auto remain = width & (coeff_width - 1);
        static_assert(std::has_single_bit(coeff_width));

        if(offset) {
            // std::memcpy(a.poly.data() + offset, a.poly.data(), (a.coeff_length - offset) * sizeof(typename int_representation<Bits>::coeff_type));
            std::copy_backward(a.poly.data(), a.poly.data() + (a.coeff_length - offset), a.poly.end());
            // std::memset(a.poly.data(), 0, offset * sizeof(typename int_representation<Bits>::coeff_type));
            std::fill_n(a.poly.data(), offset, 0);
        }
        for(auto i = 0u; i < a.coeff_length && remain; ++i) {
            buff1 = a.poly[i] >> (coeff_width - remain);
            a.poly[i] = a.poly[i] << remain;
            a.poly[i] |= buff0;
            buff0 = buff1;
        }
    }
};

class naive_mul {
public:
    /// @brief 組み込み整数同士の掛け算
    /// @tparam Half Digitの半分の長さを持つ型
    /// @tparam Digit n bit 整数型
    /// @param dest 整数同士の掛け算の結果の下位 n bit
    /// @param a かける数
    /// @param b かける数
    /// @return 繰り上がり(整数同士の掛け算の結果の上位 n bit)
    template<class Half = std::uint32_t, class Digit = std::uint64_t>
    static constexpr auto mul(Digit& dest, Digit a, Digit b) noexcept
    -> std::enable_if_t<std::is_unsigned_v<Digit>, Digit>
    {
        constexpr unsigned int half_bit_width = sizeof(Half) * CHAR_BIT;
        const Digit ah[] = {static_cast<Half>(a), static_cast<Half>(a >> half_bit_width)};
        const Digit bh[] = {static_cast<Half>(b), static_cast<Half>(b >> half_bit_width)};

        dest = a * b;
        Digit oz = ah[0] * bh[1];
        const Digit carry = impl_base::plus(oz, ah[1] * bh[0]);
        return ah[1] * bh[1] + (oz >> half_bit_width) + ((Half)oz > (dest >> half_bit_width)) + (carry << half_bit_width);
    }

    template<int DestLen, int MulLen, std::random_access_iterator Itr, std::random_access_iterator CItr>
    static constexpr auto mul(Itr dest, CItr a, CItr b) noexcept
    -> std::enable_if_t<std::is_same_v<typename std::iterator_traits<Itr>::value_type, typename std::iterator_traits<CItr>::value_type>, void>
    {
        typedef typename std::iterator_traits<Itr>::value_type int_type;
        int_type digit;
        std::fill_n(dest, DestLen, (int_type)0);
        for(auto i = 0; i < std::min(DestLen, 2 * MulLen); ++i) {
            for(auto j = 0; j <= std::min(i, MulLen - 1); ++j) {
                auto k = i - j;
                impl_base::plus((dest + i + 1), DestLen - i - 1, mul(digit, *(a + k), *(b + j)));
                impl_base::plus((dest + i), DestLen - i, digit);
            }
        }
    }

    template<unsigned int Bits, unsigned int Bits1, unsigned int Bits2>
    static constexpr auto mul(int_representation<Bits>& dest, const int_representation<Bits1>& a, const int_representation<Bits2>& b) noexcept
    -> std::enable_if_t<(Bits >= Bits1 && Bits1 >= Bits2), void>
    {
        typename int_representation<Bits>::coeff_type digit;
        dest.flush();
        for(auto i = 0u; i < std::min(dest.coeff_length, a.coeff_length + b.coeff_length); ++i) {
            for(auto j = 0u; j <= std::min(i, b.coeff_length - 1); ++j) {
                auto k = i - j;
                impl_base::plus(dest, mul(digit, a.poly[k], b.poly[j]), i + 1);
                impl_base::plus(dest, digit, i);
            }
        }
    }

    template<unsigned int Bits, unsigned int Bits1, unsigned int Bits2>
    static constexpr auto mul(int_representation<Bits>& dest, const int_representation<Bits1>& a, const int_representation<Bits2>& b) noexcept
    -> std::enable_if_t<(Bits >= Bits1 && Bits1 < Bits2), void>
    {
        mul(dest, b, a);
    }

    /// @brief a/bの割り算．非復元法
    /// @tparam Sign この計算での符号の扱い方
    /// @tparam Bits 商のビット幅
    /// @param quotient 商を格納する変数の参照
    /// @param remainder 商を格納する変数の参照
    /// @param a 左辺
    /// @param b 右辺
    template<sign Sign, unsigned int Bits>
    static constexpr auto div(int_representation<Bits>& quotient, int_representation<Bits>& remainder, const int_representation<Bits>& dividend, const int_representation<Bits>& divisor) noexcept
    -> std::enable_if_t<Sign == sign::mp_signed>
    {
        // https://lpha-z.hatenablog.com/entry/2018/11/11/231500
        constexpr auto N = Bits;
        quotient.flush();
        remainder = dividend.msb() ? -1 : 0;

        for( int i = N-1; i >= 0; --i ) {
            //bool new_bit = dividend & 1ull<<i;
            bool new_bit = dividend.bitat(i);
            if( (remainder.msb()) != (divisor.msb()) ) {
                bitop::shiftl(quotient, 1);
                // remainder = ((remainder<<1) | new_bit) + divisor;
                bitop::shiftl(remainder, 1);
                remainder.poly[0] |= new_bit;
                impl_base::plus(remainder, divisor);
            } else {
                // quotient = (quotient<<1) | 1;
                bitop::shiftl(quotient, 1);
                quotient.poly[0] |= 1;
                // remainder = ((remainder<<1) | new_bit) - divisor;
                bitop::shiftl(remainder, 1);
                remainder.poly[0] |= new_bit;
                impl_base::minus(remainder, divisor);
            }
        }

        // quotient = (quotient << 1) | 1;
        bitop::shiftl(quotient, 1);
        quotient.poly[0] |= 1;
        if( (bool)remainder == false ) {
            /* do nothing */
        } else if( impl_base::cmp<sign::mp_signed>(remainder, divisor) == 0 ) {
            assert( divisor.msb() );
            // quotient += 1;
            impl_base::plus(quotient, 1ull, 0);
            remainder.flush();
        } else if(auto tmp = remainder; impl_base::plus(tmp, divisor), !tmp ) {
            assert(divisor.msb() == 0 && (bool)divisor );
            impl_base::minus(quotient, 1ull, 0);
            remainder.flush();
        } else if( (remainder.msb()) != (dividend.msb()) ) {
            if( (remainder.msb()) != (divisor.msb()) ) {
                // quotient -= 1;
                impl_base::minus(quotient, 1ull, 0);
                // remainder += divisor;
                impl_base::plus(remainder, divisor);
            } else {
                // quotient += 1;
                impl_base::plus(quotient, 1ull, 0);
                // remainder -= divisor;
                impl_base::minus(remainder, divisor);
            }
        }
    }

    template<sign Sign, unsigned int Bits>
    static constexpr auto div(int_representation<Bits>& quotient, int_representation<Bits>& remainder, const int_representation<Bits>& dividend, const int_representation<Bits>& divisor) noexcept
    -> std::enable_if_t<Sign == sign::mp_unsigned>
    {
        // https://lpha-z.hatenablog.com/entry/2018/11/04/231500
        constexpr auto N = Bits;
        remainder.flush();
        quotient.flush();
        for( int i = N-1; i >= 0; --i ) {
            bitop::shiftl(quotient, 1);
            bitop::shiftl(remainder, 1);
            remainder.poly[0] |= dividend.bitat(i);

            // const lbl::uintN_t<N+1> test_sub = static_cast<lbl::uintN_t<N+1>>(remainder) - static_cast<lbl::uintN_t<N+1>>(divisor);
            auto test_sub = remainder;
            // const bool success = static_cast<lbl::intN_t<N+1>>(test_sub) >= 0;
            const bool success = impl_base::minus(test_sub, divisor);

            quotient.poly[0] |= success;
            if(success) remainder = test_sub;
        }
    }
};

class karatsuba {
public:
    using int_type = std::uint64_t;
    static constexpr int karatsuba_threashold = 8;

    template<int DestLen, int SrcLen>
    static constexpr auto add(int_type* dest, const int_type* src) noexcept
    -> int_type
    {
        constexpr int mi = std::min(DestLen, SrcLen);
        int_type cr = 0;
        auto i = 0;
        for(i = 0; i < mi; ++i) {
            dest[i] += cr;
            cr = (dest[i] < cr);
            dest[i] += src[i];
            cr |= (dest[i] < src[i]);
        }
        for(; i < DestLen && cr; ++i) {
            dest[i] += cr;
            cr = !dest[i];
        }
        return cr;
    }
    template<int DestLen>
    static constexpr auto add(int_type* dest, int_type src) noexcept
    -> int_type
    {
        int_type cr = 0;
        auto i = 0;
        dest[i] += src;
        cr = (dest[i] < src);
        for(; i < DestLen && cr; ++i) {
            dest[i] += cr;
            cr = !dest[i];
        }
        return cr;
    }
    template<int DestLen, int SrcLen>
    static constexpr auto sub(int_type* dest, const int_type* src) noexcept
    -> int_type
    {
        constexpr int mi = std::min(DestLen, SrcLen);
        int_type cr = 1;
        int i;
        for(i = 0; i < mi; ++i) {
            dest[i] += cr;
            cr = (dest[i] < cr);
            dest[i] += ~src[i];
            cr |= (dest[i] < ~src[i]);
        }
        for(; i < DestLen; ++i) {
            dest[i] += cr;
            cr = (dest[i] < cr);
            dest[i] += ~(int_type)0;
            cr |= (dest[i] < ~(int_type)0);
        }
        return cr + ~(int_type)0;
    }
    template<int DestLen, int SrcLen>
    static constexpr auto diff(int_type* dest, const int_type* src) noexcept
    -> bool
    {
        if(sub<DestLen, SrcLen>(dest, src) >> 31) {
            int_type cr = 1;
            for(int i = 0; i < DestLen; ++i) {
                dest[i] = ~dest[i];
                dest[i] += cr;
                cr = dest[i] < cr;
            }
            return true;
        }
        return false;
        // auto cmp = impl_base::cmp<DestLen, SrcLen>(dest, src);
        // if(cmp == 1) {
        //     sub<DestLen, SrcLen>(dest, src);
        // } else if(cmp == 0) {
        //     std::fill_n(dest, DestLen, (int_type)0);
        // } else {
        //     constexpr int mi = std::min(DestLen, SrcLen);
        //     int_type cr = 1;
        //     int i;
        //     for(i = 0; i < mi; ++i) {
        //         dest[i] = ~dest[i];
        //         cr = (dest[i] < cr);
        //         dest[i] += src[i];
        //         cr |= (dest[i] < ~src[i]);
        //     }
        //     for(; i < DestLen; ++i) {
        //         dest[i] = ~dest[i];
        //         dest[i] += cr;
        //         cr = (dest[i] < cr);
        //     }
        //     return cr + ~(int_type)0;
    }

    template<int DestLen, int SrcLen>
    static auto kmul(int_type* dest, const int_type* a, const int_type* b) noexcept
    -> std::enable_if_t<(SrcLen > 0) && ((SrcLen >> std::countr_zero((unsigned int)SrcLen)) <= karatsuba_threashold), void>
    {
        constexpr auto halfw = SrcLen / 2;
        if constexpr (SrcLen <= karatsuba_threashold) {
            naive_mul::mul<DestLen, SrcLen>(dest, a, b);
            return;
        }
        else {
            // karatsuba algorithm
            int_type z0[SrcLen] = {};
            int_type z2[SrcLen] = {};
            int_type *z1 = dest + halfw;
            bool ofx, ofy;
            std::fill_n(dest, DestLen, 0);

            kmul<SrcLen, halfw>(z0, a, b);
            kmul<SrcLen, halfw>(z2, a + halfw, b + halfw);
            {
                int_type x0_x1[halfw] = {}, y1_y0[halfw] = {};
                std::copy(a, a + halfw, x0_x1);
                std::copy(b + halfw, b + SrcLen,  y1_y0);
                ofx = diff<halfw, halfw>(x0_x1, a + halfw);
                ofy = diff<halfw, halfw>(y1_y0, b);
                kmul<DestLen - halfw, halfw>(z1, x0_x1, y1_y0);
            }
            // std::copy(z0, z0 + std::min(SrcLen, DestLen), dest);
            // std::fill_n(dest + SrcLen, DestLen > SrcLen ? DestLen - SrcLen : 0, 0);
            add<DestLen - SrcLen, SrcLen>(dest + SrcLen, z2);
            if (ofx ^ ofy) {
                sub<DestLen - halfw, SrcLen>(dest + halfw, z1);
            } else {
                add<DestLen - halfw, SrcLen>(dest + halfw, z1);
            }
            add<DestLen - halfw, SrcLen>(dest + halfw, z2);
            add<DestLen - halfw, SrcLen>(dest + halfw, z0);
        }
    }

    template<unsigned int BitWidthD, unsigned int BitWidth1, unsigned int BitWidth2>
    static constexpr auto mul(int_representation<BitWidthD>& dest, const int_representation<BitWidth1>& a, const int_representation<BitWidth2>& b) noexcept
    -> std::enable_if_t<(BitWidth1 > 0) && (BitWidth2 > 0)>
    {
        typedef typename int_representation<BitWidthD>::coeff_type int_type;
        constexpr auto Len1 = BitWidth1 / 8 / sizeof(int_type);
        if (std::is_constant_evaluated()) {
            naive_mul::mul(dest, a, b);
        } else if constexpr (BitWidth1 == BitWidth2 && ((Len1 >> std::countr_zero(Len1)) <= karatsuba_threashold)) {
            karatsuba::kmul<BitWidthD / 8 / sizeof(int_type), Len1>(dest.poly.data(), a.poly.data(), b.poly.data());
        } else{
            naive_mul::mul(dest, a, b);
        }
    }

};

}