cmake_minimum_required(VERSION 3.20)
project(epiclib LANGUAGES CXX)

# Use C++14 to keep deprecated pre-C++17 facilities (e.g., std::bind1st)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if (MSVC)
  # MSVC doesn't have /std:c++11; /std:c++14 preserves bind1st, etc.
  add_compile_options(/std:c++14 /bigobj /EHsc)
  # Map some GCC warnings you previously disabled:
  # -Wno-sign-compare  -> /wd4018 (signed/unsigned mismatch)
  # -Wno-reorder       -> /wd5038 (initializer order)
  add_compile_options(/W3 /wd4018 /wd5038)
  # /GL is fine if you want LTCG; keep or drop as you prefer
  add_link_options(/LTCG)
else()
  # GCC/Clang flags you used before
  add_compile_options(-O2 -Wall -Wextra -fmax-errors=1
                      -Wno-misleading-indentation -Wno-reorder -Wno-sign-compare)
endif()

find_package(Python3 REQUIRED COMPONENTS Interpreter Development.Module)
find_package(pybind11 CONFIG REQUIRED)

file(GLOB_RECURSE EPICLIB_SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/*.cpp")
pybind11_add_module(epiclib ${EPICLIB_SOURCES})
target_include_directories(epiclib PRIVATE "${CMAKE_SOURCE_DIR}/src")
target_compile_definitions(epiclib PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX _CRT_SECURE_NO_WARNINGS EPICLIB_WITH_PYBIND11=1)
if (WIN32)
  target_link_libraries(epiclib PRIVATE ws2_32)
endif()

set(PY_SHORT "${Python3_VERSION_MAJOR}${Python3_VERSION_MINOR}")
set(OUTDIR "${CMAKE_SOURCE_DIR}/compiled_modules/windows")
add_custom_command(TARGET epiclib POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E make_directory "${OUTDIR}"
  COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:epiclib>" "${OUTDIR}/epiclib_windows_${PY_SHORT}.pyd"
  VERBATIM)
