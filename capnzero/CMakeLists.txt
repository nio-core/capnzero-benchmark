cmake_minimum_required(VERSION 2.8.3)
project(capnzero)

## Use c++ 11x std
add_definitions(-std=c++11)

# Define where to find modules for ZeroMQ
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake CACHE INTERNAL "" FORCE)

find_package(Threads)
find_package(ZeroMQ REQUIRED)
find_package(CapnProto REQUIRED)
find_package(FlatBuffers REQUIRED)
find_package(Protobuf REQUIRED)

# copying the variables makes it compatible for catkin to transitive forward the CapnProto libraries and include dirs
set(CapnProto_LIBRARIES ${CAPNP_LIBRARIES})
set(CapnProto_INCLUDE_DIRS ${CAPNP_INCLUDE_DIRS})
set(FlatBuffers_INCLUDE_DIRS ${FLATBUFFERS_INCLUDE_DIR})

# generate protobuf files
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/GenerateProtobuff.cmake)
file(GLOB ProtoFiles "${CMAKE_CURRENT_SOURCE_DIR}/msg/*.proto")
my_protobuf_generate_cpp(include/capnzero-base-msgs PROTO_SRCS PROTO_HDRS ${ProtoFiles})
include_directories(${CMAKE_CURRENT_BINARY_DIR}/generated_files)

# Print path to generated files
message ("PROTO_SRCS = ${PROTO_SRCS}")
message ("PROTO_HDRS = ${PROTO_HDRS}")

add_subdirectory(flatbuffers)
add_subdirectory(sbe)

find_package(catkin REQUIRED)
catkin_package(
        INCLUDE_DIRS include
        LIBRARIES capnzero capnzero-base-msgs
        DEPENDS ZeroMQ CapnProto
        CFG_EXTRAS CapnZero-extras.cmake
)

include_directories(
        include
        ${ZeroMQ_INCLUDE_DIRS}
        ${CAPNP_INCLUDE_DIRS}
        ${PROTOBUF_INCLUDE_DIRS}
)

############ CapnZero Communication Library

add_library(${PROJECT_NAME}
        ${PROTO_SRCS}
        ${PROTO_HDRS}
        src/PublisherMsgPack.cpp
        src/SubscriberMsgPack.cpp
        src/PublisherFlatbuffers.cpp
        src/SubscriberFlatbuffers.cpp
        src/PublisherProtobuf.cpp
        src/SubscriberProtobuf.cpp
        src/PublisherSBE.cpp
        src/SubscriberSBE.cpp
        src/PublisherCapnProto.cpp
        src/SubscriberCapnProto.cpp
        src/benchmark/BenchmarkFlatbuffers.cpp
        src/benchmark/BenchmarkCapnProto.cpp
        src/benchmark/BenchmarkProtobuf.cpp
        src/benchmark/BenchmarkSBE.cpp
        )

target_compile_definitions(${PROJECT_NAME} PUBLIC ZMQ_BUILD_DRAFT_API=1)

target_link_libraries(${PROJECT_NAME}
        PUBLIC
        ${ZeroMQ_LIBRARIES}
        ${CAPNP_LIBRARIES}
        ${PROTOBUF_LIBRARIES}
        )

############# SENDER-CONSOLE-APP

add_executable(pub
        src/pub.cpp
        ${PROTO_SRCS}
        ${PROTO_HDRS}
        )

target_link_libraries(pub
        ${PROJECT_NAME}
        capnzero-base-msgs
        ${CMAKE_THREAD_LIBS_INIT}
        )

############# RECEIVER-CONSOLE-APP

add_executable(echo
        src/echo.cpp
        ${PROTO_SRCS}
        ${PROTO_HDRS}
        )

target_link_libraries(echo
        ${PROJECT_NAME}
        capnzero-base-msgs
        ${CMAKE_THREAD_LIBS_INIT}
        )

############# BENCHMARK-CONSOLE-APP

add_executable(benchmark
        src/benchmark/benchmark.cpp
        ${PROTO_SRCS}
        ${PROTO_HDRS}
        )

target_link_libraries(benchmark
        ${PROJECT_NAME}
        capnzero-base-msgs
        ${CMAKE_THREAD_LIBS_INIT}
        )

############# BASIC MESSAGES

## Autogenerate the c++ capnproto files
set(capnp_message_folder "${CMAKE_CURRENT_SOURCE_DIR}/msg")
file(GLOB_RECURSE capnp_messages ${capnp_message_folder} *.capnp)

set(CAPNPC_SRC_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/msg)
file(MAKE_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/capnzero-base-msgs")
set(CAPNPC_OUTPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include/capnzero-base-msgs")
capnp_generate_cpp(CAPNP_SRCS CAPNP_HDRS ${capnp_messages})

add_library(capnzero-base-msgs
        ${CAPNP_SRCS}
        )

## Autogenerate the c++ flatbuffers and SBE files
add_dependencies( pub flatbuffers_message)# generate_sbe_header
get_target_property( FLATBUFF_INCLUDE_DIR flatbuffers_message INCLUDE_DIR )
target_include_directories(
        pub
        PRIVATE
        ...
        ${FLATBUFF_INCLUDE_DIR}
        ...)

add_dependencies( echo flatbuffers_message)# generate_sbe_header
get_target_property( FLATBUFF_INCLUDE_DIR flatbuffers_message INCLUDE_DIR )
target_include_directories(
        echo
        PRIVATE
        ...
        ${FLATBUFF_INCLUDE_DIR}
        ...)

add_dependencies( benchmark flatbuffers_message generate_sbe_header)
get_target_property( FLATBUFF_INCLUDE_DIR flatbuffers_message INCLUDE_DIR )
target_include_directories(
        benchmark
        PRIVATE
        ...
        ${FLATBUFF_INCLUDE_DIR}
        ...)

