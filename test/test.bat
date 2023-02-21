@echo off
clang++ main.cpp -pedantic -Wall -Wextra -I .\ouchitest\include\ -I ..\include\ -std=c++20 -O3 -o testall.exe -fconstexpr-steps=2109999999 && echo ----- && .\testall.exe
