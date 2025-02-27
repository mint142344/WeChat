
file(GLOB_RECURSE SOURCES *.cpp)

# 状态服务器
add_executable(StatusServer ${SOURCES} ${PROTO_GEN_SOURCES})
target_link_libraries(StatusServer PRIVATE
    fmt::fmt
    Boost::boost
    nlohmann_json::nlohmann_json
    gRPC::grpc++ protobuf
)
target_include_directories(StatusServer PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${PROTO_GEN_PATH}
)