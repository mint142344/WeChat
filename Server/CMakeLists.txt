cmake_minimum_required(VERSION 3.20)

project(WeChat-Server)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找库
find_package(Boost CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)
find_package(fmt REQUIRED)
find_package(OpenSSL REQUIRED)
find_library(HIREDIS_LIB hiredis REQUIRED)

# mysql-connector-c++-8.0.33
set(LIB_MYSQL_CONNECTOR_INLCUDE /usr/local/mysql/connector-c++-8.0.33/include)
set(LIB_MYSQL_CONNECTOR_DIR /usr/local/mysql/connector-c++-8.0.33/lib64/debug)
find_library(LIB_MYSQL_CONNECTOR_CPP NAMES mysqlcppconn PATHS ${LIB_MYSQL_CONNECTOR_DIR} REQUIRED)

# 设置路径
set(GATE_SERVER_PATH ${CMAKE_SOURCE_DIR}/GateServer)
set(STATUS_SERVICE_PATH ${CMAKE_SOURCE_DIR}/StatusService)
set(CHAT_SERVER_PATH ${CMAKE_SOURCE_DIR}/ChatServer)
set(PROTO_GEN_PATH ${CMAKE_SOURCE_DIR}/gen)
set(COMMON_SRC_PATH ${CMAKE_SOURCE_DIR}/Common/src)

# proto 生成文件
file(GLOB_RECURSE PROTO_GEN_SOURCES ${PROTO_GEN_PATH}/*.cc)


include_directories(${PROTO_GEN_PATH} ${CMAKE_SOURCE_DIR}/Common/include)

# 启动测试
enable_testing()
add_subdirectory(test)

# 网关服务器
add_subdirectory(./GateServer)

# 状态服务器
add_subdirectory(./StatusService)

# 聊天服务器
add_subdirectory(./ChatServer)
