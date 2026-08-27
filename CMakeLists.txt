cmake_minimum_required(VERSION 3.10)
project(QRProject)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV REQUIRED)

file(GLOB QR_LIB_SOURCES "${CMAKE_SOURCE_DIR}/src/qr/*.cpp")

add_library(qr_lib STATIC ${QR_LIB_SOURCES})
target_include_directories(qr_lib PUBLIC ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(qr_lib PUBLIC ${OpenCV_LIBS})

add_executable(qr src/main.cpp)
target_link_libraries(qr PRIVATE qr_lib)
