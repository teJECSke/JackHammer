cmake_minimum_required(VERSION 3.25)

set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "" FORCE)

project(JackHammer VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig)

pkg_check_modules(NFT libnftables)
include_directories(${NFT_INCLUDE_DIRS})
link_directories(${NFT_LIBRARY_DIRS})

pkg_check_modules(SDJ libsystemd)
include_directories(${SDJ_INCLUDE_DIRS})
link_directories(${SDJ_LIBRARY_DIRS})

find_package(Qt6 REQUIRED COMPONENTS Core)
qt6_standard_project_setup()

add_executable(JackHammer
    JackHammer.cpp
    journalwatcher.cpp
    nftablesinterface.cpp
    filter.cpp
)

target_link_libraries(JackHammer PRIVATE Qt6::Core
    ${NFT_LIBRARIES}
    ${SDJ_LIBRARIES}
)
