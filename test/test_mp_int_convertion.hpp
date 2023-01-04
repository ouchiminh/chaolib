#pragma once
#include "ouchitest/ouchitest.hpp"

#include <string>
#include <sstream>

#include "chao/mp_int.hpp"

OUCHI_TEST_CASE(test_cvt_into_mp_int_from_str) {
    size_t idx;
    OUCHI_REQUIRE_EQUAL(chao::stoi("1"), chao::expr_to_mp_int(1));
    OUCHI_REQUIRE_EQUAL(chao::stoi("123456"), chao::expr_to_mp_int(123456));
    OUCHI_REQUIRE_EQUAL(chao::stoi("-123456"), chao::expr_to_mp_int(-123456));
    OUCHI_REQUIRE_EQUAL(chao::stoi("0x123", &idx), chao::expr_to_mp_int(0));
    OUCHI_REQUIRE_EQUAL(idx, 1u);
    OUCHI_REQUIRE_EQUAL(chao::stoi("123", nullptr, 16), chao::expr_to_mp_int(0x123));
    OUCHI_REQUIRE_EQUAL(chao::stoi("0x123", nullptr, 0), chao::expr_to_mp_int(0x123));
    OUCHI_REQUIRE_EQUAL(chao::stoi("0xabcd123", nullptr, 0), chao::expr_to_mp_int(0xabcd123));
    OUCHI_REQUIRE_EQUAL(chao::stoi("-0xabcd123", nullptr, 0), chao::expr_to_mp_int(-0xabcd123));
    OUCHI_REQUIRE_EQUAL(chao::stoi("0xABCDEF", nullptr, 0), chao::expr_to_mp_int(0xABCDEF));
    OUCHI_REQUIRE_EQUAL(chao::stoi("07711", nullptr, 0), chao::expr_to_mp_int(07711));
    OUCHI_REQUIRE_EQUAL(chao::stoi("081", &idx, 0), chao::expr_to_mp_int(0));
    OUCHI_REQUIRE_EQUAL(idx, 1u);
    OUCHI_REQUIRE_EQUAL(chao::stoi("z", nullptr, 36), chao::expr_to_mp_int(35));
}

OUCHI_TEST_CASE(test_cvt_into_str_from_mp_int) {
    using namespace std::literals;
    using namespace chao::literals;

    OUCHI_REQUIRE_EQUAL(chao::to_string(123456_i128), "123456"s);
    OUCHI_REQUIRE_EQUAL(chao::to_string(-123456_i128), "-123456"s);
    OUCHI_REQUIRE_EQUAL(chao::to_string(0_i128), "0"s);

}

OUCHI_TEST_CASE(test_io_mp_int) {
    using namespace chao::literals;
    std::string src = "123456 -1 12345 -1";
    std::stringstream ss(src);
    std::stringstream result;
    chao::int128_t i128;
    chao::uint128_t u128;
    ss >> i128;
    result << i128;
    OUCHI_REQUIRE_EQUAL(i128, 123456_i128);
    ss >> i128;
    result << i128;
    OUCHI_REQUIRE_EQUAL(i128, -1_i128);
    ss >> u128;
    result << u128;
    OUCHI_REQUIRE_EQUAL(u128, 12345);
    OUCHI_CHECK_THROW(ss >> u128, std::invalid_argument);

    OUCHI_REQUIRE_EQUAL(result.str(), "123456-112345");
}
