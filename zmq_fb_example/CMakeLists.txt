cmake_minimum_required(VERSION 3.10)

project(GreenFunction)

set(CMAKE_CXX_STANDARD 11)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/interfaces
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/flatbuffers/include
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/libzmq/include
)

link_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/libzmq/lib
)

link_libraries(
    zmq
)
#add_executable(codriver src/codriver.cpp)
add_executable(codriver src/codriver_offline_execute_per_req.cc)
add_executable(green src/green_req_rep.cpp)

