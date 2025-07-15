dnl Checks for OpenGL

AC_CHECK_HEADERS(
	[GL/gl.h],
	[
		AC_CHECK_LIB(
			[GL],
			[glEnd],
			[],
			[AC_MSG_ERROR([*** Can't find the OpenGL library Try: https://www.opengl.org])]
		)
	],
	[AC_MSG_ERROR([*** Can't find the OpenGL library Try: https://www.opengl.org])]
)

AC_CHECK_HEADERS(
	[GL/glu.h],
	[
		AC_CHECK_LIB(
			[GLU],
			[gluNewTess],
			[],
			[AC_MSG_ERROR([*** Can't find the OpenGL library Try: https://www.opengl.org])]
		)
	],
	[AC_MSG_ERROR([*** Can't find the OpenGL library Try: https://www.opengl.org])]
)

if test `uname -s` = 'Darwin'
then
	GL_LIBS='-framework OpenGL'
else
	GL_LIBS='-lGL -lGLU'
fi

AC_CHECK_HEADERS(
	[GL/glew.h],
	[
		AC_CHECK_LIB(
			[GLEW],
			[glewInit],
			[],
			[AC_MSG_ERROR([*** Can't find the GLEW library])]
		)
	],
	[AC_MSG_ERROR([*** Can't find the GLEW library])]
)

GL_LIBS="$GL_LIBS -lGLEW"
AC_SUBST(GL_LIBS)
