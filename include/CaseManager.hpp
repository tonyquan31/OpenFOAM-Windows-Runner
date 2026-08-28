#include <optional>
#pragma once
#include <filesystem>
#include <string>

class CaseManager
{
public:
    static std::filesystem::path ResolveCaseDirectory(const std::optional<std::filesystem::path>& cliCaseArg);
    static std::string ToOpenFOAMPOSIXPath(const std::filesystem::path& winPath);
    static bool EnsureFoamFile(const std::filesystem::path& casePath);
    static bool IsValidCase(const std::filesystem::path& casePath);
};
