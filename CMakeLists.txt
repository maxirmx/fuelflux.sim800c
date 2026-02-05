cmake_minimum_required(VERSION 3.16)
project(ppp0_authorize_demo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(CURL REQUIRED)

add_executable(ppp0_authorize_demo src/main.cpp)
target_link_libraries(ppp0_authorize_demo PRIVATE CURL::libcurl)
