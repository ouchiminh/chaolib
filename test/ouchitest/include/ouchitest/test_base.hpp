#pragma once
#include <iostream>
#include <list>
#include <string_view>
#include "ccol.hpp"
#include "log_helper.hpp"

namespace ouchi::test {

class test_base {
public:
    test_base() = delete;
    test_base(std::string_view testname)
        : name_(testname)
        , fail_count_{ 0 }
    {}
    /// <summary>
    /// run test case
    /// </summary>
    /// <returns>error count</returns>
    virtual void test() = 0;
    static unsigned test_all()
    {
        unsigned failed_assertion = 0;
        unsigned failed_case = 0;
        std::cout << "running " << test_cases().size() << " test cases.\n\n";
        for (auto* test_case : test_cases()) {
            // error message is displayed in test()
            unsigned f;
            try {
                test_case->test();
                f = test_case->fail_count_;
            } catch (std::exception& e) {
                error_s("An exception thrown in ", test_case->name_, ".",
                      "what() : ", e.what(), "\n\n");
                ++failed_case;
                continue;
            } catch (...) {
                error_s("Unknown exception thrown in ", test_case->name_, ".\n\n");
                ++failed_case;
                continue;
            }
            failed_assertion += f;
            failed_case += f ? 1 : 0;
        }
        ccol concol;
        concol.set_color(failed_case ? ccol::foreground_red : ccol::foreground_green);
        std::cout << failed_assertion << " assertions are error in " << failed_case << " test cases.\n";
        return failed_case;
    }
protected:
    template<class ...MaybePrintable>
    static void error_s(MaybePrintable&& ...args)
    {
        ccol concol;
        concol.set_color(ccol::foreground_red);
        log(std::forward<MaybePrintable>(args)...);
    }
    template<class ...MaybePrintable>
    void error(MaybePrintable&& ...args)
    {
        ++fail_count_;
        log("in ");
        error_s(name_);
        error_s(std::forward<MaybePrintable>(args)...);
    }
    template<class ...MaybePrintable>
    static void log(MaybePrintable&& ...args)
    {
        (log(std::forward<MaybePrintable>(args)), ...);
    }
    template<class Printable>
    static void log(Printable&& value)
    {
        detail::printer<Printable> print;
        print(std::forward<Printable>(value));
    }

    static std::list<test_base*>& test_cases() {
        static std::list<test_base*> test_cases_;
        return test_cases_;
    }
    std::string_view name_;
    unsigned fail_count_;
};

}
