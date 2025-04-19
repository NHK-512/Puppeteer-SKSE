# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required(VERSION 3.16)

# Detect vcpkg toolchain if not already defined
if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
endif()

list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/vcpkg_installed/x64-windows")


project(SkyrimAIPlugin)

# Enable Hot Reload for MSVC compilers if supported.
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable vcpkg manifest mode
if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
        CACHE STRING "")
endif()

#option(ENABLE_SKYRIM_SE "Enable support for Skyrim SE in the dynamic runtime feature." ON)
#option(ENABLE_SKYRIM_AE "Enable support for Skyrim AE in the dynamic runtime feature." ON)
#option(ENABLE_SKYRIM_VR "Enable support for Skyrim VR in the dynamic runtime feature." OFF)
#set(BUILD_TESTS OFF)

add_subdirectory(external/CommonLibSSE)
find_package(fmt CONFIG REQUIRED)


add_library(${PROJECT_NAME} SHARED
    src/main.cpp
    src/AIManager.cpp
 "src/utils/EnemyScanner.h" "src/utils/EnemyScanner.cpp" "src/utils/NearbyAggroedActors.cpp" "src/utils/NearbyAggroedActors.h" "src/PCH.h" "src/utils/Puppeteer.cpp" "src/utils/roles/Ranger.cpp" "src/utils/roles/Vanguard.cpp" "src/utils/roles/Striker.cpp" "src/utils/roles/Ranger.cpp" "src/utils/roles/Ranger.h" "src/utils/roles/Ranger.h" "src/utils/roles/Striker.h")


#find_package(CommonLibSSE CONFIG REQUIRED)
target_precompile_headers(${PROJECT_NAME} PRIVATE src/PCH.h)
target_include_directories(${PROJECT_NAME} PRIVATE external/CommonLibSSE/include)
target_link_libraries(
    ${PROJECT_NAME} 
    PRIVATE 
    CommonLibSSE::CommonLibSSE
    fmt::fmt
)
