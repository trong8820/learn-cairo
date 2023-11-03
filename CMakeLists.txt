cmake_minimum_required(VERSION 3.22)

project(learn-cairo VERSION 1.0.0)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_C_EXTENSIONS OFF)

add_executable(${PROJECT_NAME} WIN32 src/main.c)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/include)
