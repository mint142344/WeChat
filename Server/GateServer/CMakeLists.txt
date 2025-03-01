
file(GLOB_RECURSE SOURCES src/*.cpp)
file(GLOB_RECURSE COMMON_SRCS ${CMAKE_SOURCE_DIR}/Common/src/*.cpp)

add_executable(GateServer 
    ${SOURCES} 
    ${PROTO_GEN_SOURCES}
    ${COMMON_SRCS}
)

target_link_libraries(GateServer PRIVATE
    fmt::fmt
    Boost::boost
    nlohmann_json::nlohmann_json
    gRPC::grpc++ protobuf
    OpenSSL::SSL OpenSSL::Crypto
    ${HIREDIS_LIB}
    ${LIB_MYSQL_CONNECTOR_CPP}
)
target_include_directories(GateServer PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${PROTO_GEN_PATH}
    ${LIB_MYSQL_CONNECTOR_INLCUDE}
)