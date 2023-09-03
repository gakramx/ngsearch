cmake_minimum_required(VERSION 3.14)

project(ngsearch LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

add_executable(clstool
  main.cpp filehandler.h filehandler.cpp
)
target_link_libraries(clstool Qt${QT_VERSION_MAJOR}::Core)

install(TARGETS clstool
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
# CPack settings
set(CPACK_GENERATOR "DEB")
set(CPACK_PACKAGE_NAME "clstool")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_DESCRIPTION "Effortlessly search and manage files and folders based on text file queries")
set(CPACK_PACKAGE_CONTACT " Make Life Easy Soft <hello@makelifeeasysoft.com>")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "qtbase5-dev (>= 5.0)")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Make Life Easy Soft <https://www.makelifeeasysoft.com/>")

include(CPack)
