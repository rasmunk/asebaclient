# CMake file that builds the aseba medulla interface library
cmake_minimum_required(VERSION 3.6)
project(asebaclient VERSION 0.1.0)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
include_directories (${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
find_package(Qt4 COMPONENTS QtDBus QtCore QtTest REQUIRED)
## Contains every qt core module include headers e.g QtDBus
include_directories(${QT_INCLUDE_DIR})

set(dbus_MOC_HDRS include/dbusinterface.h)
set(SOURCE_FILES ${SOURCE_FILES} src/dbusinterface.cpp)
# https://cmake.org/cmake/help/v3.0/module/FindQt4.html
# Creates meta-object code from files containing Qt class with the Q_OBJECT declaration
qt4_wrap_cpp(dbus_MOC_SRCS ${dbus_MOC_HDRS})
add_library(asebaclient SHARED ${SOURCE_FILES} ${dbus_MOC_SRCS})

install(TARGETS asebaclient EXPORT AsebaMedullaDBusConfig
	ARCHIVE DESTINATION lib
	LIBRARY DESTINATION lib
	RUNTIME DESTINATION bin)
install(DIRECTORY include/ DESTINATION include)
