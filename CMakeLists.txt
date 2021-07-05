cmake_minimum_required(VERSION 3.12)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(Boost_USE_STATIC_LIBS ON)

include("cmake/HunterGate.cmake")

HunterGate(
        URL "https://github.com/cpp-pm/hunter/archive/v0.23.278.tar.gz"
        SHA1 "35fa55bc1dfcf9f4d7264f8bb26cd1759ce1ae07"
        #        LOCAL
)

project(scanner)


hunter_add_package(Beast)
find_package(Beast CONFIG REQUIRED)
hunter_add_package(Boost COMPONENTS  thread filesystem)
find_package(Boost CONFIG REQUIRED thread filesystem)
find_package(Threads)

add_library(${PROJECT_NAME} STATIC
         include/Server.h include/Scanner.hpp)

target_link_libraries(${PROJECT_NAME}
        Beast::Beast
        Boost::filesystem
        Boost::thread
        )

add_executable(demo
        ${CMAKE_CURRENT_SOURCE_DIR}/demo/main.cpp
        sources/Scanner.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<INSTALL_INTERFACE:include>"
        )

target_link_libraries(demo ${PROJECT_NAME}
        Beast::Beast
        Boost::filesystem
        Boost::thread)


