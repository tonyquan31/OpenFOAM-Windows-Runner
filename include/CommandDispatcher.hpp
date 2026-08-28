#pragma once
#include <vector>
#include <string>

class CommandDispatcher
{
public:
    static int Run(int argc, char* argv[]);
private:
    static void PrintHelp();
    static void PrintInfo();
};
