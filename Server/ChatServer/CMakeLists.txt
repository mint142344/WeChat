
file(GLOB_RECURSE SOURCES src/*.cpp)

add_executable(ChatServer
    ${SOURCES}
    ${PROTO_GEN_SOURCES}
    ${COMMON_SRC_PATH}/RpcService.cpp
)
target_link_libraries(ChatServer PRIVATE
    fmt::fmt
    Boost::boost
    nlohmann_json::nlohmann_json
    gRPC::grpc++ protobuf

    # OpenSSL::SSL OpenSSL::Crypto
    # hiredis
    # mysqlcppconn
)
target_include_directories(ChatServer PRIVATE
    ${CMAKE_SOURCE_DIR}
    ./include
)