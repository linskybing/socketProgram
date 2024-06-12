cmake_minimum_required(VERSION 3.6)

set(APP_NAME loadbalance)
project(${APP_NAME})

set(CONFIGURATION_SUFFIX "$<LOWER_CASE:$<CONFIGURATION>>")
SET(GCC_COVERAGE_COMPILE_FLAGS "-std=c++17")
add_executable(socket_test
        client.cpp
)

target_link_libraries(socket_test ws2_32)