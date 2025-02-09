cmake_minimum_required(VERSION 3.20)

project(WeChat-Server)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Boost CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(fmt REQUIRED)

file(GLOB SOURCES src/*.cpp)

add_executable(GateServer ${SOURCES})
target_link_libraries(GateServer PRIVATE
    Boost::boost
    nlohmann_json::nlohmann_json
    fmt::fmt
)
target_include_directories(GateServer PRIVATE include)