cmake_minimum_required(VERSION 2.8.3)
project(capnzero_monitor)

## Use c++ 11x std
add_definitions(-std=c++11)

# Define where to find modules for ZeroMQ
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake CACHE INTERNAL "" FORCE)

find_package(Threads)
find_package(ZeroMQ REQUIRED)
find_package(capnzero REQUIRED)

find_package(catkin REQUIRED capnzero)
catkin_package(
        INCLUDE_DIRS ${catkin_INCLUDE_DIRS}
        LIBRARIES capnzero
        DEPENDS ZeroMQ capnzero
        CFG_EXTRAS CapnZero-extras.cmake
)

include_directories(
        ${ZeroMQ_INCLUDE_DIRS}
        ${catkin_INCLUDE_DIRS}
)

############ CapnZero Communication Library

add_library(${PROJECT_NAME}
        src/monitor.cpp
        )

target_compile_definitions(${PROJECT_NAME} PUBLIC ZMQ_BUILD_DRAFT_API=1)

target_link_libraries(${PROJECT_NAME}
        PUBLIC
        ${ZeroMQ_LIBRARIES}
        ${CAPNP_LIBRARIES}
        ${PROTOBUF_LIBRARIES}
        )

############# MONITOR-CONSOLE-APP

add_executable(monitor
        src/monitor.cpp
        )

target_link_libraries(monitor
        ${PROJECT_NAME}
        ${catkin_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT}
        )

############# MONITOR-RECEIVER-CONSOLE-APP

add_executable(monitorReceiver
        src/monitorReceiver.cpp
        )

target_link_libraries(monitorReceiver
        ${PROJECT_NAME}
        ${catkin_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT}
        )

