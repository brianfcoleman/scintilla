cmake_minimum_required(VERSION 2.8.11)

project(Cursor)

add_definitions(
    -DSCI_LEXER=1
)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

set(CMAKE_PREFIX_PATH "${QT_INSTALL_DIR}")
find_package(Qt5Widgets REQUIRED)

set(ScintillaPublicHeadersDir
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

set(ScintillaPublicHeaders
    ${ScintillaPublicHeadersDir}/Scintilla.h
    ${ScintillaPublicHeadersDir}/SciLexer.h
    ${ScintillaPublicHeadersDir}/Platform.h
    ${ScintillaPublicHeadersDir}/ILexer.h
)

set(LexLibHeadersDir
    ${CMAKE_CURRENT_SOURCE_DIR}/lexlib
)

set(ScintillaHeadersDir
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

add_subdirectory(lexlib)
add_subdirectory(lexers)
add_subdirectory(src)
add_subdirectory(qt)
