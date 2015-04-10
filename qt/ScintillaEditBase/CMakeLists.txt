set(Sources
    PlatQt.cpp
    ScintillaQt.cpp
    ScintillaEditBase.cpp
)

set(Headers
    PlatQt.h
    ScintillaQt.h
    ScintillaEditBase.h
)

add_library(ScintillaEditBase
    STATIC
    ${Sources}
    ${Headers}
)

target_include_directories(ScintillaEditBase
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    $<TARGET_PROPERTY:Scintilla,INTERFACE_INCLUDE_DIRECTORIES>
    $<TARGET_PROPERTY:Qt5::Widgets,INTERFACE_INCLUDE_DIRECTORIES>
)
