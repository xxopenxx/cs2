cmake_minimum_required(VERSION 3.15)
project(CSExternal)

# Ścieżki do zależności
set(MINHOOK_DIR "${CMAKE_SOURCE_DIR}/libs/MinHook")
set(IMGUI_DIR "${CMAKE_SOURCE_DIR}/libs/imgui")

file(GLOB SOURCES
    "src/*.cpp"
    "${IMGUI_DIR}/*.cpp"
    "${IMGUI_DIR}/backends/imgui_impl_dx11.cpp"
    "${IMGUI_DIR}/backends/imgui_impl_win32.cpp"
)

add_executable(CSExternal ${SOURCES})

target_include_directories(CSExternal PRIVATE
    ${CMAKE_SOURCE_DIR}/Common/include
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    ${MINHOOK_DIR}/include  # Dodaj tę linię
    ${CMAKE_SOURCE_DIR}/CSExternal/include
)

target_link_directories(CSExternal PRIVATE
    ${MINHOOK_DIR}/bin  # Dodaj tę linię
)

target_link_libraries(CSExternal
    Common
    d3d11.lib
    dxgi.lib
    MinHook.x64.lib
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)