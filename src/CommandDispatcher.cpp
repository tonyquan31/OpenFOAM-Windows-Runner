#include "CommandDispatcher.hpp"
#include "EnvironmentManager.hpp"
#include "CaseManager.hpp"
#include "ProcessExecutor.hpp"
#include "ColorConsole.hpp"
#include <iostream>

void CommandDispatcher::PrintHelp()
{
    Console::Banner();
    std::cout << "Usage:\n"
              << "  of-runner <command> [options...]\n"
              << "  of-runner info\n"
              << "  of-runner help\n\n"
              << "Examples:\n"
              << "  of-runner blockMesh\n"
              << "  of-runner foamRun\n"
              << "  of-runner snappyHexMesh -overwrite\n"
              << "  of-runner icoFoam -case E:/KODAMA/OpenFOAM-13-Engines/run/cavity\n"
              << "  of-runner checkMesh\n\n"
              << "Key Features:\n"
              << "  * Automatic Engine Discovery & Environment Injection\n"
              << "  * POSIX Path Normalization (prevents Windows backslash issues)\n"
              << "  * Automatic <case>.foam Generation for Instant ParaView Viewing\n"
              << "  * Job Object Subprocess Management (clean Ctrl+C termination)\n";
}

void CommandDispatcher::PrintInfo()
{
    Console::Banner();
    auto engineOpt = EnvironmentManager::FindEnginePath();
    if (!engineOpt.has_value())
    {
        Console::Error("OpenFOAM Engine not found! Please set %OPENFOAM_DIR% or place engines in E:\\KODAMA\\OpenFOAM-13-Engines");
        return;
    }

    auto enginePath = engineOpt.value();
    Console::Success("OpenFOAM Engine Location: " + enginePath.string());

    auto casePath = CaseManager::ResolveCaseDirectory(std::nullopt);
    Console::Info("Current Case Directory:   " + casePath.string());
    Console::Info("Case Valid OpenFOAM:      " + std::string(CaseManager::IsValidCase(casePath) ? "YES" : "NO / Not a case folder"));

    auto cmds = EnvironmentManager::ListAvailableCommands(enginePath);
    std::cout << "\nAvailable Solvers and Utilities (" << cmds.size() << " executables):\n";
    for (size_t i = 0; i < cmds.size(); ++i)
    {
        std::cout << "  " << cmds[i];
        if ((i + 1) % 4 == 0 || i + 1 == cmds.size()) std::cout << "\n";
    }
}

int CommandDispatcher::Run(int argc, char* argv[])
{
    Console::EnableVT100();

    if (argc < 2)
    {
        PrintHelp();
        return 0;
    }

    std::string firstArg = argv[1];
    if (firstArg == "help" || firstArg == "--help" || firstArg == "-h")
    {
        PrintHelp();
        return 0;
    }
    if (firstArg == "info" || firstArg == "--info" || firstArg == "-i")
    {
        PrintInfo();
        return 0;
    }

    // 1. Discover Engine
    auto engineOpt = EnvironmentManager::FindEnginePath();
    if (!engineOpt.has_value())
    {
        Console::Error("Could not locate OpenFOAM-13 Engine! Set %OPENFOAM_DIR% or place in E:\\KODAMA\\OpenFOAM-13-Engines");
        return 1;
    }
    auto enginePath = engineOpt.value();

    // 2. Validate Command
    auto exePath = EnvironmentManager::GetExecutablePath(enginePath, firstArg);
    std::error_code ec;
    if (!std::filesystem::exists(exePath, ec))
    {
        Console::Error("Command '" + firstArg + "' not found in " + (enginePath / "bin").string());
        std::cout << "Run 'of-runner info' to see the list of all available commands.\n";
        return 1;
    }

    // 3. Parse arguments and check for -parallel / -case
    std::vector<std::string> passArgs;
    bool hasCaseArg = false;
    bool isParallel = false;
    std::string customCasePath;

    for (int i = 2; i < argc; ++i)
    {
        std::string arg = argv[i];
        if (arg == "-case" && i + 1 < argc)
        {
            hasCaseArg = true;
            customCasePath = argv[++i];
        }
        else
        {
            if (arg == "-parallel") isParallel = true;
            passArgs.push_back(arg);
        }
    }

    std::filesystem::path casePath;
    if (hasCaseArg)
    {
        casePath = CaseManager::ResolveCaseDirectory(customCasePath);
    }
    else
    {
        casePath = CaseManager::ResolveCaseDirectory(std::nullopt);
    }

    std::string posixCase = CaseManager::ToOpenFOAMPOSIXPath(casePath);
    passArgs.push_back("-case");
    passArgs.push_back(posixCase);

    // 4. Auto-generate .foam file for ParaView
    CaseManager::EnsureFoamFile(casePath);

    // 5. Setup Environment Block
    std::wstring envBlock = EnvironmentManager::BuildEnvironmentBlock(enginePath, isParallel);

    // 6. Execute Process
    return ProcessExecutor::Execute(exePath, passArgs, casePath, envBlock);
}
