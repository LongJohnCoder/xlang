cmake_minimum_required(VERSION 3.9)

project(idl3)

add_executable(idl3 "")
target_sources(idl3 PUBLIC main.cpp pch.cpp)
target_include_directories(idl3 PUBLIC ${XLANG_LIBRARY_PATH})
target_include_directories(idl3 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

if (WIN32)
    TARGET_CONFIG_MSVC_PCH(idl3 pch.cpp pch.h)
    target_link_libraries(idl3 windowsapp ole32 shlwapi)
else()
    target_link_libraries(idl3 c++ c++abi c++experimental)
    target_link_libraries(idl3 -lpthread)
endif()
