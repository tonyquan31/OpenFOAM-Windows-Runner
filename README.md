# OpenFOAM-13 Windows Runner (`of-runner.exe`)

A high-performance, native C++17 Windows CLI Manager and Engine Launcher for **OpenFOAM-v13** on Windows. Built and developed with **Visual Studio 2022 (MSVC)** and **CMake**.

---

##  Key Features

1. **Seamless OpenFOAM Experience on Windows**:
   - Run OpenFOAM solvers and meshers identically to Linux: `of-runner blockMesh`, `of-runner foamRun`, `of-runner snappyHexMesh`, etc.
   - Eliminates the need to manually set environment variables or run shell wrappers before executing solvers.

2. **Automatic Engine Discovery & Environment Injection**:
   - Detects the portable OpenFOAM-13 engine automatically from:
     - Environment variable `WM_PROJECT_DIR`
     - Sibling directory next to `of-runner.exe`
     - Canonical locations: `E:\KODAMA\OpenFOAM-13-Engines`, `C:\OpenFOAM\OpenFOAM-13`, `D:\OpenFOAM-13`, etc.
   - Automatically builds an isolated, clean Windows Process Environment Block for child processes:
     - `WM_PROJECT_DIR`, `WM_PROJECT=OpenFOAM`, `WM_PROJECT_VERSION=13`
     - `MPI_BUFFER_SIZE=20000000`
     - Isolated `PATH` routing (`bin`, `lib`, `lib/dummy` for serial, `lib/msmpi` for parallel).

3. **Smart POSIX Path Normalization**:
   - Automatically converts Windows backslashes `\` to forward slashes `/` for `-case <dir>` arguments to guarantee compatibility with OpenFOAM's Unix-rooted `fileName` path parser.

4. **Auto-Generation of `.foam` ParaView Case File**:
   - When executed inside any valid OpenFOAM case (having `system/controlDict`), `of-runner` automatically ensures `<caseName>.foam` exists so you can immediately double-click or open the case in native Windows ParaView.

5. **Win32 Job Object & Robust Process Execution**:
   - Spawns child processes attached to a dedicated Windows Job Object (`JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE`).
   - Clean Ctrl+C signal forwarding and instantaneous process cleanup (no orphaned background MPI/solver processes).
   - Real-time zero-lag stream forwarding of `stdout` and `stderr`.

---

## 🛠️ Project Structure

```
OpenFOAM-Runner/
├── CMakeLists.txt              # MSVC C++17 CMake configuration
├── README.md                   # Documentation
├── include/
│   ├── ColorConsole.hpp        # ANSI / Windows VT100 console colors
│   ├── EnvironmentManager.hpp  # Engine discovery & PATH configuration
│   ├── CaseManager.hpp         # OpenFOAM case validation & .foam creation
│   ├── ProcessExecutor.hpp     # Win32 CreateProcessW & Job Object executor
│   └── CommandDispatcher.hpp   # CLI argument parsing & dispatcher
└── src/
    ├── EnvironmentManager.cpp
    ├── CaseManager.cpp
    ├── ProcessExecutor.cpp
    ├── CommandDispatcher.cpp
    └── main.cpp                # Application entry point
```

---

## 🚀 Opening & Building with Visual Studio 2022

### Method 1: Visual Studio CMake Open Folder (Recommended)
1. Open **Visual Studio 2022**.
2. Select **File -> Open -> Folder...** and choose `E:\KODAMA\OpenFOAM-v13-CrossCompile\OpenFOAM-Runner`.
3. Visual Studio will automatically detect `CMakeLists.txt` and configure the project.
4. Select `x64-Release` or `x64-Debug` from the configuration dropdown.
5. Press **Ctrl + Shift + B** (or **Build -> Build All**) to compile.
6. The output executable will be created at:
   `out/build/x64-Release/of-runner.exe` (or `build/Release/of-runner.exe`).

### Method 2: Visual Studio Solution (.sln)
To generate a traditional `.sln` solution file for VS 2022:
```powershell
cd E:\KODAMA\OpenFOAM-v13-CrossCompile\OpenFOAM-Runner
cmake -B build -G "Visual Studio 17 2022" -A x64
```
Then open `build\OpenFOAMRunner.sln` in Visual Studio 2022.

---

## 💻 Usage & CLI Examples

### 1. General Syntax
```powershell
of-runner <command> [options...]
```

### 2. View Engine Information & Available Commands
```powershell
of-runner info
```
Outputs engine location, valid case status, and all 150 available solvers/utilities.

### 3. Run Mesh Generation & CFD Simulation
Open PowerShell or CMD inside your case folder (e.g. `E:\KODAMA\OpenFOAM-13-Engines\run\cavity`):

```powershell
# 1. Generate mesh
of-runner blockMesh

# 2. Check mesh quality
of-runner checkMesh

# 3. Solve CFD case
of-runner foamRun
```

### 4. Direct Visualisation with ParaView
Once `of-runner` runs in the case folder, `cavity.foam` is automatically present. Simply drag and drop `cavity.foam` into ParaView or run:
```powershell
paraview cavity.foam
```

---

## 📦 Distribution
To install `of-runner.exe` directly into your portable OpenFOAM distribution:
```powershell
Copy-Item "E:\KODAMA\OpenFOAM-v13-CrossCompile\OpenFOAM-Runner\build\Release\of-runner.exe" -Destination "E:\KODAMA\OpenFOAM-13-Engines\bin\of-runner.exe"
```
You can also add `E:\KODAMA\OpenFOAM-13-Engines\bin` to your Windows user `PATH` to use `of-runner` from any terminal or PowerShell window.
