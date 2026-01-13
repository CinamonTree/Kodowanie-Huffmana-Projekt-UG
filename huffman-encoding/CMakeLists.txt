cmake_minimum_required(VERSION 4.2.1)
project(huffman-encoding LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(app 
    src/main.cpp
    src/arg_paraser.cpp
)