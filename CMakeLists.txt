cmake_minimum_required(VERSION 3.10)

project(GreenFunction)

set(CMAKE_CXX_STANDARD 11)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/

    ${CMAKE_CURRENT_SOURCE_DIR}/zmq_fb_example/include
    ${CMAKE_CURRENT_SOURCE_DIR}/zmq_fb_example/third_party/flatbuffers/include
    ${CMAKE_CURRENT_SOURCE_DIR}/zmq_fb_example/third_party/libzmq/include

    ${CMAKE_CURRENT_SOURCE_DIR}/interfaces
)

link_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/zmq_fb_example/third_party/libzmq/lib
)

link_libraries(
    zmq
)

add_executable(codriver src_synchronous_green_function/codriver_offline_execute_req.cc)
add_executable(green src_synchronous_green_function/green_rep.cpp)

