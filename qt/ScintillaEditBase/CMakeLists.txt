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
    SHARED
    ${Sources}
    ${Headers}
)

add_dependencies(ScintillaEditBase
    LexLib
    Lexers
    Scintilla
)

target_include_directories(ScintillaEditBase
    PRIVATE
    ${ScintillaPublicHeadersDir}
    ${LexLibHeadersDir}
    ${ScintillaHeadersDir}
)

target_link_libraries(ScintillaEditBase
    Qt5::Widgets
    LexLib
    Lexers
    Scintilla
)

add_definitions(
    -DSCINTILLA_QT=1
    -DMAKING_LIBRARY=1
)
