#pragma once
#include <filesystem>
#include <string>
#include <vector>
#include <optional>

class EnvironmentManager
{
public:
    static std::optional<std::filesystem::path> FindEnginePath(const std::optional<std::filesystem::path>& customPath = std::nullopt);
    static bool ValidateEngine(const std::filesystem::path& enginePath);
    static std::wstring BuildEnvironmentBlock(const std::filesystem::path& enginePath, bool isParallel = false);
    static std::filesystem::path GetExecutablePath(const std::filesystem::path& enginePath, const std::string& commandName);
    static std::vector<std::string> ListAvailableCommands(const std::filesystem::path& enginePath);
};
