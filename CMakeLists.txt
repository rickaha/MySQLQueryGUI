cmake_minimum_required(VERSION 3.10)
project(MySQLQueryGUI)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find Qt package
find_package(Qt5 COMPONENTS Core Widgets Gui REQUIRED)

# Find MySQL Connector/C++
find_path(MYSQLCPPCONN_INCLUDE_DIR mysql_connection.h
    PATHS "/usr/include/cppconn" "/usr/local/include/cppconn" "/usr/include/mysql" "/usr/local/include/mysql")
find_library(MYSQLCPPCONN_LIBRARY mysqlcppconn
    PATHS "/usr/lib" "/usr/local/lib" "/usr/lib/mysql" "/usr/local/lib/mysql")

include_directories(${MYSQLCPPCONN_INCLUDE_DIR})

# Source files
add_executable(MySQLQueryGUI mysqlquerygui.cpp)

# Link libraries
target_link_libraries(MySQLQueryGUI
    Qt5::Core
    Qt5::Widgets
    Qt5::Gui
    ${MYSQLCPPCONN_LIBRARY}
)
