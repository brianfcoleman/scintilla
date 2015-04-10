set(Sources
    main.cpp
)

add_executable(Cursor
    WIN32
    ${Sources}
)

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
