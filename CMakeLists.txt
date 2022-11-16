cmake_minimum_required(VERSION 3.20)
project(raresync)

set(CMAKE_CXX_STANDARD 17)

add_executable(raresync main.cpp include/core.h include/define.h include/conf.h include/crypto.hpp src/core.cpp include/network.h src/network.cpp include/proto.h src/network_peer.cpp src/network_server.cpp)

include_directories(include)


find_library(BLS bls384_256 lib)

find_package(Boost 1.40 REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

target_link_libraries(raresync LINK_PUBLIC ${BLS})
