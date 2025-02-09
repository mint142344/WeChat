cmake_minimum_required(VERSION 3.16)

project(WeChat VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOUIC_SEARCH_PATHS "${CMAKE_SOURCE_DIR}/ui")

find_package(Qt6 REQUIRED COMPONENTS Widgets Network)

file(GLOB_RECURSE SOURCES "${CMAKE_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE HEADERS "${CMAKE_SOURCE_DIR}/include/*.h")
file(GLOB_RECURSE UI_FILES "${CMAKE_SOURCE_DIR}/ui/*.ui")

add_executable(WeChat rc.qrc ${SOURCES} ${HEADERS} ${UI_SOURCES})

target_link_libraries(WeChat PRIVATE
    Qt6::Widgets
    Qt6::Network
)
target_include_directories(WeChat PRIVATE ${CMAKE_SOURCE_DIR}/include)
