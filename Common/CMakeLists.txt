cmake_minimum_required(VERSION 3.15)
project(Common)

file(GLOB SOURCES
    "src/*.cpp"
)

add_library(Common STATIC ${SOURCES})

target_include_directories(Common PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)