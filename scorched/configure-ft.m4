dnl Check for FreeType2
m4_ifdef(
    [AC_CHECK_FT2],
    [AC_CHECK_FT2(
        [7.0.1],
        [],
        [AC_MSG_ERROR([
            FreeType2 must be installed on your system
            but freetype-config couldn't be found.
            Please check that freetype-config is in the path, the directory
            where the freetype libraries are installed (returned by
            'freetype-config --libs' command) is in LD_LIBRARY_PATH or
            equivalent variable and freetype version is 7.0.1 or above
            Try https://www.freetype.org to download freetype.
        ])]
    )],
    [PKG_CHECK_MODULES(
        [FT2],
        [freetype2 >= 7.0.1],
        [],
        [AC_MSG_ERROR([FreeType2 must be installed on your system])]
    )]
)
