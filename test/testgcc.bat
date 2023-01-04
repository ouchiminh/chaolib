@echo off
g++ main.cpp -Wall -Wextra -I .\ouchitest\include\ -I ..\include\ -std=c++20 -O2 -o testall.exe && .\testall.exe
