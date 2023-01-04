#pragma once

#ifdef __linux__
#include <cstdio>
namespace ouchi::test {

class ccol {
public:
    enum color_code : unsigned short {
        foreground_blue = 34,
        foreground_green = 32,
        foreground_red = 31,
        background_blue = 44,
        background_green = 42,
        background_red = 41,
    };
    friend color_code operator|(color_code c1, color_code c2) noexcept { return (color_code)((unsigned short)c1 | (unsigned short)c2); }
    ccol() = default;
    ccol(const ccol&) = delete;
    ~ccol()
    {
        restore();
    }
    void set_color(color_code c) const noexcept
    {
        std::printf("\x1b[%dm", c);
    }
    void restore() const noexcept
    {
        std::printf("\x1b[0m");
    }
};
}
#else
// impl for windows
#include <Windows.h>

namespace ouchi::test {

/// <summary>
/// console color for cmd.exe
/// </summary>
class ccol {
    HANDLE stdout_;
    CONSOLE_SCREEN_BUFFER_INFO csbi_;
public:
    enum color_code : WORD {
        foreground_blue = FOREGROUND_BLUE,
        foreground_green = FOREGROUND_GREEN,
        foreground_red = FOREGROUND_RED,
        background_blue = BACKGROUND_BLUE,
        background_green = BACKGROUND_GREEN,
        background_red = BACKGROUND_RED,
    };
    friend color_code operator|(color_code c1, color_code c2) noexcept { return (color_code)((WORD)c1 | (WORD)c2); }
    ccol()
        : stdout_{GetStdHandle(STD_OUTPUT_HANDLE)}
    {
        GetConsoleScreenBufferInfo(stdout_, &csbi_);
    }
    ccol(const ccol&) = delete;
    ~ccol()
    {
        restore();
    }
    void set_color(color_code c) const noexcept
    {
        SetConsoleTextAttribute(stdout_, (WORD)c);
    }
    void restore() const noexcept
    {
        SetConsoleTextAttribute(stdout_, csbi_.wAttributes);
    }
};
}
#endif
