# 查找库
find_package(Boost CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)
find_package(fmt REQUIRED)
find_package(GTest REQUIRED)
find_library(HIREDIS_LIB hiredis)
find_library(LIB_MYSQL_CONNECTOR_CPP NAMES mysqlcppconn PATHS ${LIB_MYSQL_CONNECTOR_DIR} REQUIRED)

file(GLOB_RECURSE SOURCES src/*.cpp)
file(GLOB_RECURSE PROTO_GEN_SOURCES gen/*.cc)

include_directories(../include ../gen)

file(GLOB PROTO_FILES ../gen/*.cc)

# 测试 grpc 连接池
add_executable(test_rpc_pool
    test_rpc_pool.cpp ${PROTO_FILES}
    ../src/pool/RpcConnPool.cpp
)
add_test(NAME test_rpc_pool COMMAND test_rpc_pool)

target_link_libraries(test_rpc_pool PRIVATE
    nlohmann_json::nlohmann_json
    fmt::fmt
    gRPC::grpc++
    protobuf
    GTest::GTest
    GTest::Main
)

# 测试 asio io_context 池
add_executable(test_io_context_pool
    test_io_context_pool.cpp
    ../src/pool/IoContextPool.cpp
)
add_test(NAME test_io_context_pool COMMAND test_io_context_pool)
target_link_libraries(test_io_context_pool PRIVATE
    Boost::boost
    GTest::GTest
    GTest::Main
    fmt::fmt
)

# 测试 redis 连接池
add_executable(test_redis_pool
    test_redis_pool.cpp
    ../src/pool/RedisConnPool.cpp
)
add_test(NAME test_redis_pool COMMAND test_redis_pool)
target_link_libraries(test_redis_pool PRIVATE
    GTest::GTest
    GTest::Main
    fmt::fmt
    ${HIREDIS_LIB}
)

# 测试 mysql 连接池
add_executable(test_mysql_pool
    test_mysql_pool.cpp
    ../src/pool/MysqlConnPool.cpp
)
add_test(NAME test_mysql_pool COMMAND test_mysql_pool)
target_link_libraries(test_mysql_pool PRIVATE
    GTest::GTest
    GTest::Main
    fmt::fmt
    ${LIB_MYSQL_CONNECTOR_CPP}
)
target_include_directories(test_mysql_pool PRIVATE ${LIB_MYSQL_CONNECTOR_INLCUDE})