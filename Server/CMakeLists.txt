cmake_minimum_required(VERSION 3.20)

project(WeChat-Server)

# 启动测试
enable_testing()
add_subdirectory(test)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(Boost CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)
find_package(fmt REQUIRED)
find_library(HIREDIS_LIB hiredis)

# mysql-connector-c++-8.0.33
set(LIB_MYSQL_CONNECTOR_INLCUDE /usr/local/mysql/connector-c++-8.0.33/include)
set(LIB_MYSQL_CONNECTOR_DIR /usr/local/mysql/connector-c++-8.0.33/lib64/debug)
find_library(LIB_MYSQL_CONNECTOR_CPP NAMES mysqlcppconn PATHS ${LIB_MYSQL_CONNECTOR_DIR} REQUIRED)

file(GLOB_RECURSE SOURCES src/*.cpp)
file(GLOB_RECURSE PROTO_GEN_SOURCES gen/*.cc)

add_executable(GateServer ${SOURCES} ${PROTO_GEN_SOURCES})
target_link_libraries(GateServer PRIVATE
    Boost::boost
    nlohmann_json::nlohmann_json
    fmt::fmt
    gRPC::grpc++
    protobuf
    ${HIREDIS_LIB}
    ${LIB_MYSQL_CONNECTOR_CPP}
)
target_include_directories(GateServer PRIVATE include gen ${LIB_MYSQL_CONNECTOR_INLCUDE})