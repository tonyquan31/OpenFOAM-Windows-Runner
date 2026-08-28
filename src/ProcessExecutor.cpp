#include "ProcessExecutor.hpp"
#include "ColorConsole.hpp"
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <iostream>
#include <vector>

namespace
{
    HANDLE g_hJob = NULL;

    BOOL WINAPI ConsoleCtrlHandler(DWORD fdwCtrlType)
    {
        switch (fdwCtrlType)
        {
        case CTRL_C_EVENT:
        case CTRL_BREAK_EVENT:
        case CTRL_CLOSE_EVENT:
            if (g_hJob != NULL)
            {
                TerminateJobObject(g_hJob, 1);
            }
            return TRUE;
        default:
            return FALSE;
        }
    }
}

int ProcessExecutor::Execute(
    const std::filesystem::path& executable,
    const std::vector<std::string>& arguments,
    const std::filesystem::path& workingDir,
    const std::wstring& /*environmentBlock*/
)
{
    g_hJob = CreateJobObjectW(NULL, NULL);
    if (g_hJob != NULL)
    {
        JOBOBJECT_EXTENDED_LIMIT_INFORMATION jeli = { 0 };
        jeli.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
        SetInformationJobObject(g_hJob, JobObjectExtendedLimitInformation, &jeli, sizeof(jeli));
    }

    SetConsoleCtrlHandler(ConsoleCtrlHandler, TRUE);

    std::wstring cmdLine = L"\"" + executable.wstring() + L"\"";
    for (const auto& arg : arguments)
    {
        std::wstring warg(arg.begin(), arg.end());
        if (warg.find(L' ') != std::wstring::npos)
        {
            cmdLine += L" \"" + warg + L"\"";
        }
        else
        {
            cmdLine += L" " + warg;
        }
    }

    STARTUPINFOW si = { sizeof(si) };
    PROCESS_INFORMATION pi = { 0 };

    std::wstring wWorkingDir = workingDir.wstring();

    std::vector<wchar_t> cmdLineBuf(cmdLine.begin(), cmdLine.end());
    cmdLineBuf.push_back(L'\0');

    std::wcout << L"[of-runner] Executing: " << cmdLineBuf.data() << std::endl;
    std::wcout << L"[of-runner] In directory: " << wWorkingDir << std::endl;

    BOOL success = CreateProcessW(
        NULL,
        cmdLineBuf.data(),
        NULL,
        NULL,
        FALSE,
        0,
        NULL,
        wWorkingDir.c_str(),
        &si,
        &pi
    );

    if (!success)
    {
        DWORD err = GetLastError();
        Console::Error("Failed to launch process! Win32 Error Code: " + std::to_string(err));
        if (g_hJob) CloseHandle(g_hJob);
        return static_cast<int>(err);
    }

    if (g_hJob != NULL)
    {
        AssignProcessToJobObject(g_hJob, pi.hProcess);
    }

    WaitForSingleObject(pi.hProcess, INFINITE);

    DWORD exitCode = 0;
    GetExitCodeProcess(pi.hProcess, &exitCode);

    std::cout << "[of-runner] Process finished with exit code: " << exitCode << std::endl;

    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    if (g_hJob)
    {
        CloseHandle(g_hJob);
        g_hJob = NULL;
    }

    return static_cast<int>(exitCode);
}
