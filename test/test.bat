@echo off
clang++ main.cpp -Wall -Wextra -I .\ouchitest\include\ -I ..\include\ -std=c++20 -O3 -o testall.exe && echo ----- && .\testall.exe