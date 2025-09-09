cmake_minimum_required(VERSION 3.20)
project(epiclibcpp LANGUAGES CXX)

# ---- C++ standard & common flags (keep <= C++14) ----
# Use C++14 to preserve older std facilities across toolchains.
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Honor CI-provided CFLAGS/CXXFLAGS (e.g., -w, -static-libstdc++, etc.)
set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} $ENV{CFLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} $ENV{CXXFLAGS}")

if (MSVC)
  # MSVC uses /std:c++14 (no c++11 switch) and other verified flags you noted.
  add_compile_options(/std:c++14 /bigobj /EHsc /W3 /wd4018 /wd5038)
  # Linker options you previously used can remain; comment out if undesired.
  # add_link_options(/LTCG)
else()
  # GCC/Clang flags that worked for you; keep warnings low in CI (CXXFLAGS adds -w).
  add_compile_options(-O2 -Wall -Wextra -fmax-errors=1
          -Wno-misleading-indentation -Wno-reorder -Wno-sign-compare)
endif()

# ---- Dependencies ----
find_package(Python3 REQUIRED COMPONENTS Interpreter Development.Module)
find_package(pybind11 CONFIG REQUIRED)

# ---- Sources ----
file(GLOB_RECURSE EPICLIB_SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/*.cpp")
include_directories("${CMAKE_SOURCE_DIR}/src")

# ---- Pybind11 module (extension name = epiclib) ----
pybind11_add_module(epiclib MODULE ${EPICLIB_SOURCES})

# You may keep any global defines you used previously
target_compile_definitions(epiclib PRIVATE
        WIN32_LEAN_AND_MEAN NOMINMAX _CRT_SECURE_NO_WARNINGS EPICLIB_WITH_PYBIND11=1
)

# Platform libs (e.g., sockets on Windows)
if (WIN32)
  target_link_libraries(epiclib PRIVATE ws2_32)
endif()

# ---- Install into the Python package so scikit-build can wheel it ----
# This is the crucial bit so the wheel contains the compiled extension.
install(TARGETS epiclib
        LIBRARY DESTINATION epiclibcpp     # .so on macOS/Linux
        RUNTIME DESTINATION epiclibcpp     # .pyd on Windows
        ARCHIVE DESTINATION epiclibcpp)    # (rarely used by modules)
