cmake_minimum_required(VERSION 3.8)
project(support)

find_package(LibXml2 REQUIRED)
set(Qt5_DIR /usr/local/Cellar/qt/5.10.1/lib/cmake/Qt5)
find_package(Qt5 CONFIG REQUIRED Core)

set(SOURCE_HEADERS
        AeslProgram.h
        Message.h
        )

set(SOURCE
        AeslProgram.cpp
        Message.cpp)

add_library(support ${SOURCE})
target_link_libraries(support Qt5::Core)

# install lib
install(TARGETS ${PROJECT_NAME}
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        )

# Install headers
install(FILES ${SOURCE_HEADERS}
        DESTINATION include/asebaclient/${PROJECT_NAME}
        )