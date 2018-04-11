cmake_minimum_required(VERSION 3.8)
project(support)

find_package(LibXml2 REQUIRED)
set(Qt5_DIR /usr/local/Qt-5.10.0/lib/cmake/Qt5)
find_package(Qt5 CONFIG REQUIRED Core)
set(SOURCE AeslProgram.h AeslProgram.cpp Message.h Message.cpp)

add_library(support ${SOURCE})
target_link_libraries(support Qt5::Core)