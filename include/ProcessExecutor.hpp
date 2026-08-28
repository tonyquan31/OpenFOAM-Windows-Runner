#pragma once
#include <filesystem>
#include <string>
#include <vector>

class ProcessExecutor
{
public:
    static int Execute(
        const std::filesystem::path& executable,
        const std::vector<std::string>& arguments,
        const std::filesystem::path& workingDir,
        const std::wstring& environmentBlock
    );
};
