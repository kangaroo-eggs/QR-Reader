cmake_minimum_required(VERSION 3.10)
project(QRProject)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS} ${CMAKE_SOURCE_DIR}/lib)

file(GLOB LIB_SOURCES "${CMAKE_SOURCE_DIR}/lib/*.cpp")

add_executable(qr ${CMAKE_SOURCE_DIR}/main.cpp ${LIB_SOURCES})

target_link_libraries(qr ${OpenCV_LIBS})
