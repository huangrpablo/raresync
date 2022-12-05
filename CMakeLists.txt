cmake_minimum_required(VERSION 3.20)
project(raresync)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

add_executable(raresync main.cpp src/core.cpp src/network.cpp src/network_peer.cpp src/network_server.cpp example/run_instance.cpp example/gen_conf.cpp)

find_library(BLS bls384_256 lib)

find_package(Boost 1.40 REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

target_link_libraries(raresync LINK_PUBLIC ${BLS})
