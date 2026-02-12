cmake_minimum_required(VERSION 3.10)

project(server)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build")
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
file(GLOB SOURCES "src/*.cpp")
add_executable(server ${SOURCES})

include(FetchContent)
FetchContent_Declare(SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 3.0.2
        GIT_SHALLOW ON
        EXCLUDE_FROM_ALL
        SYSTEM)
FetchContent_MakeAvailable(SFML)
target_link_libraries(server PRIVATE SFML::Network SFML::System)