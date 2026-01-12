cmake_minimum_required(VERSION 4.2.1)
project(priority-queue LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(app src/main.cpp)