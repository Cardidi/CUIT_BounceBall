cmake_minimum_required(VERSION 3.21)
project(BounceBall)
set(CMAKE_CXX_STANDARD 20)

include_directories(include)

file(GLOB_RECURSE SRC_FILES ${CMAKE_SOURCE_DIR}/src/ "*.c" "*.cpp")
#add_executable(BounceBall ${SRC_FILES})
add_executable(BounceBall src/main.cpp src/engine/Debuger.cpp src/glad/glad.c)

target_link_libraries(BounceBall "${CMAKE_SOURCE_DIR}/lib/libglfw3.a")
target_link_libraries(BounceBall "${CMAKE_SOURCE_DIR}/lib/libbox2d.a")