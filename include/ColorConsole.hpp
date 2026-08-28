#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <iostream>
#include <string>

namespace Console
{
    inline void EnableVT100()
    {
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hOut == INVALID_HANDLE_VALUE) return;
        DWORD dwMode = 0;
        if (GetConsoleMode(hOut, &dwMode))
        {
            dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(hOut, dwMode);
        }
    }

    inline void Info(const std::string& msg)
    {
        std::cout << "\033[1;34m[INFO]\033[0m " << msg << "\n";
    }

    inline void Success(const std::string& msg)
    {
        std::cout << "\033[1;32m[OK]\033[0m " << msg << "\n";
    }

    inline void Warning(const std::string& msg)
    {
        std::cout << "\033[1;33m[WARN]\033[0m " << msg << "\n";
    }

    inline void Error(const std::string& msg)
    {
        std::cerr << "\033[1;31m[ERROR]\033[0m " << msg << "\n";
    }

    inline void Banner()
    {
        std::cout << "\033[1;36m"
                  << "============================================================\n"
                  << "   OpenFOAM-13 Windows Runner (Native MSVC Engine Manager)  \n"
                  << "============================================================\033[0m\n";
    }
}
