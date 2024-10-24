cmake_minimum_required(VERSION 3.10)

project(GreenFunction)

set(CMAKE_CXX_STANDARD 14)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/

    ${CMAKE_CURRENT_SOURCE_DIR}/zmq_fb_example/include
    ${CMAKE_CURRENT_SOURCE_DIR}/zmq_fb_example/third_party/flatbuffers/include
    ${CMAKE_CURRENT_SOURCE_DIR}/zmq_fb_example/third_party/libzmq/include

    ${CMAKE_CURRENT_SOURCE_DIR}/interfaces
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/onnx/include)

link_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/zmq_fb_example/third_party/libzmq/lib
)

# Add the ONNX Runtime C++ header files path
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/onnx/onnxruntime-osx-arm64-1.19.2/include)

# Add the ONNX Runtime C++ library path
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/onnx/onnxruntime-osx-arm64-1.19.2/lib)


link_libraries(
    zmq
)

add_executable(codriver
        src_synchronous_green_function/codriver_offline_execute_req.cc
)

add_executable(green
        onnx/green_rep.cpp
        onnx/print_output_data.cpp
        onnx/print_processing_time.cpp
        onnx/print_speed_limit.cpp
)

add_executable(green_onnx
        onnx/green_rep_onnx.cpp
)

set_target_properties(green_onnx
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "../bin"
)

set_target_properties(green
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "../bin"
)

set_target_properties(codriver
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "../bin"
)

# Link the ONNX Runtime library
target_link_libraries(green PRIVATE onnxruntime)
target_link_libraries(green_onnx PRIVATE onnxruntime)

