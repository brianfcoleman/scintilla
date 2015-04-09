cmake_minimum_required(VERSION 2.8.11)

project(Cursor)

add_definitions(
    -DSCI_LEXER=1
)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

set(CMAKE_PREFIX_PATH "/Qt/Qt5.4.1/5.4/msvc2010_opengl")
find_package(Qt5Widgets)

add_subdirectory(include)
add_subdirectory(lexlib)
add_subdirectory(lexers)
add_subdirectory(src)
add_subdirectory(qt)
