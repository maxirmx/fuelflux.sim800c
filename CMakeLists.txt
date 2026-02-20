cmake_minimum_required(VERSION 3.16)
project(ppp0_authorize_demosim800c LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(CURL REQUIRED)

add_executable(sim800c src/main.cpp)
target_link_libraries(sim800c PRIVATE CURL::libcurl)
