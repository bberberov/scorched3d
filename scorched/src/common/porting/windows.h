#ifndef _WINDOWS_H_
#define _WINDOWS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdarg.h>
#include <ctype.h>
#include <common/Defines.h>

typedef char * 		LPSTR;
typedef const char *	LPCTSTR;
typedef unsigned int 	DWORD;
typedef unsigned int *  LPDWORD;
typedef unsigned char * LPBYTE;
typedef int 		INT;
typedef long         	LONG;
typedef unsigned int	UINT;
typedef unsigned short  WORD;
typedef unsigned char   BYTE;

typedef int 	HWND;
typedef void * 	HINSTANCE;
typedef int 	WPARAM;
typedef int 	LPARAM;
typedef int 	LRESULT;
typedef void *	HDC;
typedef void *  HGLRC;

#define _strnicmp(a,b,c)	strncasecmp(a,b,c)
#define stricmp(a,b)		strcasecmp(a,b)
#define _strlwr(a)		{ for (char *b=a; *b; b++) *b=tolower(*b); }

/* registry */
typedef int     HKEY; 
#define HKEY_CURRENT_USER 0

#ifndef TRUE
#define TRUE true
#endif
#ifndef FALSE
#define FALSE false
#endif

#endif
