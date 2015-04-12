set(Sources
    DocumentManager.cpp
    Main.cpp
    MessageSender.cpp
)

set(Headers
    Document.h
    DocumentManager.h
    MessageSender.h
    ScopeExit.h
)

add_executable(Cursor
    WIN32
    ${Sources}
    ${Headers}
)

set_target_properties(Cursor
    PROPERTIES
    OUTPUT_NAME cursor)

add_dependencies(Cursor
    LexLib
    Lexers
    Scintilla
    ScintillaEditBase
)

target_link_libraries(Cursor
    LexLib
    Lexers
    Scintilla
    ScintillaEditBase
    Qt5::Widgets
)
