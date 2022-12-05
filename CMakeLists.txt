cmake_minimum_required(VERSION 3.20)
#project(raresync)
project(run_instance)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

#add_executable(raresync main.cpp src/core.cpp src/network.cpp src/network_peer.cpp src/network_server.cpp)
#add_executable(raresync example/gen_conf.cpp)
add_executable(run_instance example/run_instance.cpp src/core.cpp src/network.cpp src/network_peer.cpp src/network_server.cpp)


find_library(BLS bls384_256 bls.hpp lib)

find_package(Boost 1.40 REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

target_link_libraries(run_instance LINK_PUBLIC ${BLS})
