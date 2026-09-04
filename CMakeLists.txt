cmake_minimum_required(VERSION 3.10)
project(fand C)

add_subdirectory(mayak-pwm)

add_executable(fand src/main.c)
target_link_libraries(fand PRIVATE mayak::pwm-static)

