# CMake toolchain for cross-compiling to 64-bit Windows with MinGW-w64,
# used only for the CHERRY_UI_ONLY test build (see Dockerfile.mingw-build).
set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

set(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc)
set(CMAKE_CXX_COMPILER x86_64-w64-mingw32-g++)
set(CMAKE_RC_COMPILER x86_64-w64-mingw32-windres)

set(CMAKE_FIND_ROOT_PATH /usr/x86_64-w64-mingw32)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
# BOTH, not ONLY: nlohmann_json's CMake config (from the Debian host package,
# header-only and arch-independent) lives under the host's /usr, outside the
# mingw sysroot, and still needs to be found when cross-compiling.
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE BOTH)
