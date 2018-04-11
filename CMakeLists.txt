# CMake file that builds the aseba medulla interface library
cmake_minimum_required(VERSION 3.0.2)
project(asebaclient VERSION 0.1.1)
set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 14)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
# add extra include directories
include_directories(/usr/local/include)

# add extra lib directories
link_directories(/usr/local/lib)


include(FindPackageHandleStandardArgs)
find_package(dashel REQUIRED)
find_package(LibXml2 REQUIRED)
include_directories(${LIBXML2_INCLUDE_DIR})

set(Qt5_DIR /usr/local/Cellar/qt/5.10.1/lib/cmake/Qt5)
find_package(Qt5 CONFIG REQUIRED DBus Core Network)

add_subdirectory(support)

set(dbus_MOC_HDRS
		include/AsebaInterface.h
		include/dbusinterface.h)

set(SOURCE_FILES
		src/AsebaInterface.cpp
		src/dbusinterface.cpp)

# Creates meta-object code from files containing Qt class with the Q_OBJECT declaration
qt5_wrap_cpp(dbus_MOC_SRCS ${dbus_MOC_HDRS})

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES} ${dbus_MOC_SRCS})
target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Network Qt5::DBus
		${LIBXML2_LIBRARIES} ${dashel_LIBRARIES} asebacommon asebacompiler)

install(TARGETS ${PROJECT_NAME} EXPORT AsebaMedullaDBusConfig
	ARCHIVE DESTINATION lib
	LIBRARY DESTINATION lib
	RUNTIME DESTINATION bin)

install(DIRECTORY include/ DESTINATION include/${PROJECT_NAME})
