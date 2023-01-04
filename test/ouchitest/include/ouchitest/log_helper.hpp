#pragma once
#include <utility>
#include <type_traits>
#include <iostream>

namespace ouchi::test::detail {

template<class T, class = void>
struct is_printable : std::false_type {};

template<class T>
struct is_printable<T, std::void_t<decltype(std::declval<std::ostream>() << std::declval<T>())>>
    : std::true_type {};

template<class T>
constexpr bool is_printable_v = is_printable<T>::value;

static_assert(is_printable_v<int>);
static_assert(is_printable_v<int&>);
static_assert(!is_printable_v<void>);

template<class T, bool = is_printable_v<T>>
struct printer {
    void operator()(...) { std::cout << "{unprintable}"; }
};

template<class T>
struct printer<T, true> {
    void operator()(const T& value) { std::cout << value; }
};
}
