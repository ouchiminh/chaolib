#pragma once

#include "mp_int.hpp"
#include "convertion.hpp"

namespace chao{
inline namespace literals {
inline namespace mp_int_literals {

constexpr int128_t operator"" _i128(const char* number_str) noexcept {
    return stoi<128>(number_str);
}
constexpr int192_t operator"" _i192(const char* number_str) noexcept {
    return stoi<192>(number_str);
}
constexpr int256_t operator"" _i256(const char* number_str) noexcept {
    return stoi<256>(number_str);
}
constexpr int384_t operator"" _i384(const char* number_str) noexcept {
    return stoi<384>(number_str);
}
constexpr int384_t operator"" _i512(const char* number_str) noexcept {
    return stoi<512>(number_str);
}

constexpr uint128_t operator"" _u128(const char* number_str) noexcept {
    return stoul<128>(number_str);
}
constexpr uint192_t operator"" _u192(const char* number_str) noexcept {
    return stoul<192>(number_str);
}
constexpr uint256_t operator"" _u256(const char* number_str) noexcept {
    return stoul<256>(number_str);
}
constexpr uint384_t operator"" _u384(const char* number_str) noexcept {
    return stoul<384>(number_str);
}
constexpr uint384_t operator"" _u512(const char* number_str) noexcept {
    return stoul<512>(number_str);
}

}}}
