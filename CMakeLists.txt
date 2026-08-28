cmake_minimum_required(VERSION 3.20)
project(OpenFOAMRunner LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_definitions(UNICODE _UNICODE _CRT_SECURE_NO_WARNINGS)

if(MSVC)
    add_compile_options(/W4 /utf-8 /permissive- /Zc:__cplusplus)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

set(SOURCES
    src/main.cpp
    src/EnvironmentManager.cpp
    src/CaseManager.cpp
    src/ProcessExecutor.cpp
    src/CommandDispatcher.cpp
)

set(HEADERS
    include/ColorConsole.hpp
    include/EnvironmentManager.hpp
    include/CaseManager.hpp
    include/ProcessExecutor.hpp
    include/CommandDispatcher.hpp
)

add_executable(of-runner ${SOURCES} ${HEADERS})

target_link_libraries(of-runner PRIVATE
    userenv
    advapi32
    shlwapi
)

set_target_properties(of-runner PROPERTIES
    OUTPUT_NAME "of-runner"
    VS_GLOBAL_VcpkgEnabled "false"
)
