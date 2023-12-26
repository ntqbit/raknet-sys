cmake_minimum_required(VERSION 3.1)
project(raknet-c C CXX)

set(RAKNET_ENABLE_SAMPLES FALSE)
set(RAKNET_ENABLE_DLL FALSE)
set(RAKNET_ENABLE_STATIC TRUE)
set(RAKNET_GENERATE_INCLUDE_ONLY_DIR TRUE)

add_subdirectory(RakNet)
include_directories(RakNet/DependentExtensions)
include_directories(RakNet/include)

add_library(raknet-c STATIC rakpeer.cpp libcat.cpp)
