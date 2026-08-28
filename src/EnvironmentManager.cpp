#include "EnvironmentManager.hpp"
#include "ColorConsole.hpp"
#include <windows.h>
#include <algorithm>
#include <iostream>

std::optional<std::filesystem::path> EnvironmentManager::FindEnginePath(const std::optional<std::filesystem::path>& customPath)
{
    if (customPath.has_value() && ValidateEngine(customPath.value()))
    {
        return customPath.value();
    }

    wchar_t envBuf[4096];
    if (GetEnvironmentVariableW(L"OPENFOAM_DIR", envBuf, 4096) > 0)
    {
        std::filesystem::path p(envBuf);
        if (ValidateEngine(p)) return p;
    }
    if (GetEnvironmentVariableW(L"WM_PROJECT_DIR", envBuf, 4096) > 0)
    {
        std::filesystem::path p(envBuf);
        if (ValidateEngine(p)) return p;
    }

    std::vector<std::filesystem::path> candidates = {
        L"E:\\KODAMA\\OpenFOAM-13-Engines",
        L"C:\\OpenFOAM-13-Engines",
        L"D:\\OpenFOAM-13-Engines",
        std::filesystem::current_path() / L"OpenFOAM-13-Engines",
        std::filesystem::current_path() / L".." / L"OpenFOAM-13-Engines",
        std::filesystem::current_path() / L".." / L".." / L"OpenFOAM-13-Engines"
    };

    for (const auto& cand : candidates)
    {
        std::error_code ec;
        if (std::filesystem::exists(cand, ec) && ValidateEngine(cand))
        {
            return std::filesystem::canonical(cand, ec);
        }
    }

    return std::nullopt;
}

bool EnvironmentManager::ValidateEngine(const std::filesystem::path& enginePath)
{
    std::error_code ec;
    auto binDir = enginePath / "bin";
    auto libDir = enginePath / "lib";
    auto etcDir = enginePath / "etc";

    if (!std::filesystem::is_directory(binDir, ec) ||
        !std::filesystem::is_directory(libDir, ec) ||
        !std::filesystem::is_directory(etcDir, ec))
    {
        return false;
    }

    if (!std::filesystem::exists(binDir / "foamRun.exe", ec) &&
        !std::filesystem::exists(binDir / "blockMesh.exe", ec))
    {
        return false;
    }

    return true;
}

std::filesystem::path EnvironmentManager::GetExecutablePath(const std::filesystem::path& enginePath, const std::string& commandName)
{
    std::string exeName = commandName;
    if (exeName.length() < 4 || exeName.substr(exeName.length() - 4) != ".exe")
    {
        exeName += ".exe";
    }

    return enginePath / "bin" / exeName;
}

std::vector<std::string> EnvironmentManager::ListAvailableCommands(const std::filesystem::path& enginePath)
{
    std::vector<std::string> cmds;
    std::error_code ec;
    auto binDir = enginePath / "bin";

    if (!std::filesystem::exists(binDir, ec)) return cmds;

    for (const auto& entry : std::filesystem::directory_iterator(binDir, ec))
    {
        if (entry.is_regular_file() && entry.path().extension() == ".exe")
        {
            cmds.push_back(entry.path().stem().string());
        }
    }
    std::sort(cmds.begin(), cmds.end());
    return cmds;
}

std::wstring EnvironmentManager::BuildEnvironmentBlock(const std::filesystem::path& enginePath, bool isParallel)
{
    std::wstring binPath = (enginePath / "bin").wstring();
    std::wstring libPath = (enginePath / "lib").wstring();
    std::wstring pstreamPath = isParallel 
        ? (enginePath / "lib" / "msmpi").wstring()
        : (enginePath / "lib" / "dummy").wstring();

    wchar_t oldPath[32767];
    DWORD pathLen = GetEnvironmentVariableW(L"PATH", oldPath, 32767);
    std::wstring newPath = binPath + L";" + libPath + L";" + pstreamPath;
    if (pathLen > 0)
    {
        newPath += L";" + std::wstring(oldPath);
    }

    std::string posixProjectDir = enginePath.string();
    std::replace(posixProjectDir.begin(), posixProjectDir.end(), '\\', '/');
    std::wstring wProjectDir(posixProjectDir.begin(), posixProjectDir.end());

    SetEnvironmentVariableW(L"WM_PROJECT_DIR", wProjectDir.c_str());
    SetEnvironmentVariableW(L"PATH", newPath.c_str());

    std::string envPathStr(newPath.begin(), newPath.end());
    _putenv(("PATH=" + envPathStr).c_str());
    _putenv(("WM_PROJECT_DIR=" + posixProjectDir).c_str());

    return newPath;
}
