cmake_minimum_required(VERSION 3.6)

set(APP_NAME loadbalance)
project(${APP_NAME})

set(CONFIGURATION_SUFFIX "$<LOWER_CASE:$<CONFIGURATION>>")

add_executable(socket_test
        client.cpp
)
