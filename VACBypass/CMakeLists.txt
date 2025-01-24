cmake_minimum_required(VERSION 3.15)
project(VACBypass)

file(GLOB SOURCES
    "src/*.cpp"
    "../../Common/src/Memory.cpp"  # Dodaj bezpośrednio plik źródłowy
)

add_executable(VACBypass ${SOURCES})

target_include_directories(VACBypass PRIVATE
    ${CMAKE_SOURCE_DIR}/Common/include
    ${CMAKE_SOURCE_DIR}/libs/MinHook/include
)

target_link_directories(VACBypass PRIVATE
    ${CMAKE_SOURCE_DIR}/libs/MinHook/bin
)

target_link_libraries(VACBypass
    MinHook.x64.lib
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(WIN32)
    set_target_properties(VACBypass PROPERTIES
        LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:WinMainCRTStartup"
    )
endif()