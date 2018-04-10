# CMake file that builds the aseba medulla interface library
cmake_minimum_required(VERSION 3.1.0)
project(asebaclient VERSION 0.1.1)

set(CMAKE_CXX_STANDARD 14)
# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
include(FindPackageHandleStandardArgs)
find_package(dashel REQUIRED)
find_package(LibXml2 REQUIRED)
include_directories(${LIBXML2_INCLUDE_DIR})

add_subdirectory(support)
set(Qt5_DIR /usr/local/opt/qt/lib/cmake/Qt5)
find_package(Qt5 CONFIG REQUIRED DBus Core Network)

set(SOURCE_FILES
		include/dbusinterface.h
		include/AsebaInterface.h
		src/dbusinterface.cpp
		src/AsebaInterface.cpp)

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} Qt5::DBus Qt5::Core Qt5::Network asebacommon asebacompiler support ${LIBXML2_LIBRARIES} ${dashel_LIBRARIES})
install(TARGETS ${PROJECT_NAME} EXPORT AsebaMedullaDBusConfig
	ARCHIVE DESTINATION lib
	LIBRARY DESTINATION lib
	RUNTIME DESTINATION bin)
install(DIRECTORY include/ DESTINATION include/${PROJECT_NAME})
