# 查找库
find_package(GTest REQUIRED)

# 测试 grpc 连接池
add_executable(test_email_service
    test_email_service.cpp
    ${PROTO_GEN_SOURCES}
    ${COMMON_SRC_PATH}/RpcService.cpp
)
add_test(NAME test_email_service COMMAND test_email_service)
target_include_directories(test_email_service PRIVATE ${GATE_SERVER_PATH}/include)
target_link_libraries(test_email_service PRIVATE
    fmt::fmt
    nlohmann_json::nlohmann_json
    gRPC::grpc++ protobuf
    GTest::GTest GTest::Main
)

# 测试 asio io_context 池
add_executable(test_io_context_pool
    test_io_context_pool.cpp
    ${COMMON_SRC_PATH}/pool/IoContextPool.cpp
)
add_test(NAME test_io_context_pool COMMAND test_io_context_pool)
target_include_directories(test_io_context_pool PRIVATE ${GATE_SERVER_PATH}/include)
target_link_libraries(test_io_context_pool PRIVATE
    fmt::fmt
    Boost::boost
    GTest::GTest GTest::Main
)

# 测试 redis 连接池
add_executable(test_redis_pool
    test_redis_pool.cpp
    ${COMMON_SRC_PATH}/pool/RedisConnPool.cpp
)
add_test(NAME test_redis_pool COMMAND test_redis_pool)
target_include_directories(test_redis_pool PRIVATE ${GATE_SERVER_PATH}/include)
target_link_libraries(test_redis_pool PRIVATE
    fmt::fmt
    GTest::GTest GTest::Main
    ${HIREDIS_LIB}
)

# 测试 mysql 连接池
add_executable(test_mysql_pool
    test_mysql_pool.cpp
    ${COMMON_SRC_PATH}/pool/MysqlConnPool.cpp
)
add_test(NAME test_mysql_pool COMMAND test_mysql_pool)
target_include_directories(test_mysql_pool PRIVATE ${GATE_SERVER_PATH}/include)
target_link_libraries(test_mysql_pool PRIVATE
    fmt::fmt
    GTest::GTest GTest::Main
    ${LIB_MYSQL_CONNECTOR_CPP}
)
target_include_directories(test_mysql_pool PRIVATE ${LIB_MYSQL_CONNECTOR_INLCUDE})

# 测试 mysql user dao
add_executable(test_user_dao
    test_user_dao.cpp
    ${COMMON_SRC_PATH}/MysqlService.cpp
    ${COMMON_SRC_PATH}/pool/MysqlConnPool.cpp
    ${COMMON_SRC_PATH}/DAO/mysql/MysqlUserDao.cpp
)
add_test(NAME test_user_dao COMMAND test_user_dao)
target_include_directories(test_user_dao PRIVATE ${GATE_SERVER_PATH}/include)
target_link_libraries(test_user_dao PRIVATE
    fmt::fmt
    GTest::GTest GTest::Main
    OpenSSL::SSL OpenSSL::Crypto
    ${LIB_MYSQL_CONNECTOR_CPP}
)
target_include_directories(test_user_dao PRIVATE ${LIB_MYSQL_CONNECTOR_INLCUDE})

# 测试 StatuService
add_executable(test_status_service
    test_status_service.cpp
    ${COMMON_SRC_PATH}/RpcService.cpp
    ${PROTO_GEN_SOURCES}
)
add_test(NAME test_status_service COMMAND test_status_service)
target_include_directories(test_status_service PRIVATE ${GATE_SERVER_PATH}/include)
target_link_libraries(test_status_service PRIVATE
    fmt::fmt
    nlohmann_json::nlohmann_json
    gRPC::grpc++ protobuf
    GTest::GTest GTest::Main
)