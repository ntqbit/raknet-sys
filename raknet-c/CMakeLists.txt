cmake_minimum_required(VERSION 3.1)
project(raknet-c C CXX)

option (RAKNET_CAT_AUDIT "Enable cat audit" FALSE)

set(RAKNET_ENABLE_SAMPLES FALSE)
set(RAKNET_ENABLE_DLL FALSE)
set(RAKNET_ENABLE_STATIC TRUE)
set(RAKNET_GENERATE_INCLUDE_ONLY_DIR TRUE)

if (RAKNET_CAT_AUDIT)
    add_compile_definitions(CAT_AUDIT)
endif()

add_subdirectory(RakNet)

include_directories(RakNet/DependentExtensions)
include_directories(RakNet/include)

add_library(raknet-c STATIC rakpeer.cpp libcat.cpp)
