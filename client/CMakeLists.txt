cmake_minimum_required(VERSION 3.10)

project(client)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/build")
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
file(GLOB SOURCES "src/*.cpp")
include(FetchContent)
FetchContent_Declare(SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 3.0.2
        GIT_SHALLOW ON
        EXCLUDE_FROM_ALL
        SYSTEM)
FetchContent_MakeAvailable(SFML)
add_executable(client ${SOURCES})
target_link_libraries(client PRIVATE SFML::Graphics SFML::Window SFML::System SFML::Network)

