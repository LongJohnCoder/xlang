cmake_minimum_required(VERSION 3.9)

project(cdl)

add_executable(cdl "")
target_sources(cdl PUBLIC main.cpp pch.cpp)
target_include_directories(cdl PUBLIC ${XLANG_LIBRARY_PATH})
target_include_directories(cdl PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

if (WIN32)
    TARGET_CONFIG_MSVC_PCH(cdl pch.cpp pch.h)
    target_link_libraries(cdl windowsapp ole32 shlwapi)
else()
    target_link_libraries(cdl c++ c++abi c++experimental)
    target_link_libraries(cdl -lpthread)
endif()
