#include "CaseManager.hpp"
#include <fstream>
#include <algorithm>

std::filesystem::path CaseManager::ResolveCaseDirectory(const std::optional<std::filesystem::path>& cliCaseArg)
{
    std::error_code ec;
    std::filesystem::path target;

    if (cliCaseArg.has_value())
    {
        target = cliCaseArg.value();
    }
    else
    {
        target = std::filesystem::current_path();
    }

    if (std::filesystem::exists(target, ec))
    {
        return std::filesystem::canonical(target, ec);
    }
    return std::filesystem::absolute(target);
}

std::string CaseManager::ToOpenFOAMPOSIXPath(const std::filesystem::path& winPath)
{
    std::string p = winPath.string();
    std::replace(p.begin(), p.end(), '\\', '/');
    return p;
}

bool CaseManager::EnsureFoamFile(const std::filesystem::path& casePath)
{
    std::error_code ec;
    if (!std::filesystem::exists(casePath, ec) || !std::filesystem::is_directory(casePath, ec))
    {
        return false;
    }

    std::string caseName = casePath.filename().string();
    if (caseName.empty() || caseName == "." || caseName == "..")
    {
        caseName = "case";
    }

    auto foamFilePath = casePath / (caseName + ".foam");
    if (!std::filesystem::exists(foamFilePath, ec))
    {
        std::ofstream ofs(foamFilePath);
        return ofs.good();
    }
    return true;
}

bool CaseManager::IsValidCase(const std::filesystem::path& casePath)
{
    std::error_code ec;
    if (!std::filesystem::is_directory(casePath, ec)) return false;

    bool hasSystem = std::filesystem::is_directory(casePath / "system", ec);
    bool hasConstant = std::filesystem::is_directory(casePath / "constant", ec);
    bool hasControlDict = std::filesystem::exists(casePath / "system" / "controlDict", ec);

    return (hasSystem && hasControlDict) || hasConstant;
}
