
/*  A Bison parser, made from g:\\cxx\\scorched\\3dsparse\\aseFile.y
 by  GNU Bison version 1.25
  */

#define YYBISON 1  /* Identify Bison output.  */

#if !defined(__FreeBSD__) && !defined(__DARWIN__) && !defined(_WIN32)
#define alloca 
#endif //!defined(__FreeBSD__) && !defined(__DARWIN__)

#define yyparse aseparse
#define yylex aselex
#define yyerror aseerror
#define yylval aselval
#define yychar asechar
#define yydebug asedebug
#define yynerrs asenerrs
#define	MAX_DIGIT	258
#define	MAX_FLOAT	259
#define	MAX_STRING	260
#define	MAX_LBRACKET	261
#define	MAX_RBRACKET	262
#define	MAX_ASCIIEXPORT_SYM	263
#define	MAX_COMMENT	264
#define	MAX_MESH_FACE_ID	265
#define	MAX_SCENE	266
#define	MAX_SCENE_TOKEN	267
#define	MAX_MATERIAL_LIST	268
#define	MAX_MATERIAL_LIST_TOKEN	269
#define	MAX_GEOMOBJECT	270
#define	MAX_NODE_NAME	271
#define	MAX_NODE_TM	272
#define	MAX_NODE_TM_TOKEN	273
#define	MAX_MESH	274
#define	MAX_TIMEVALUE	275
#define	MAX_MESH_NUMVERTEX	276
#define	MAX_MESH_NUMFACES	277
#define	MAX_MESH_VERTEX_LIST	278
#define	MAX_MESH_VERTEX	279
#define	MAX_MESH_FACE_LIST	280
#define	MAX_MESH_FACE	281
#define	MAX_MESH_SMOOTHING	282
#define	MAX_MESH_MTLID	283
#define	MAX_MESH_NUMTVERTEX	284
#define	MAX_MESH_NORMALS	285
#define	MAX_MESH_FACENORMAL	286
#define	MAX_MESH_VERTEXNORMAL	287
#define	MAX_PROP_MOTIONBLUR	288
#define	MAX_PROP_CASTSHADOW	289
#define	MAX_PROP_RECVSHADOW	290
#define	MAX_WIREFRAME_COLOR	291
#define	MAX_TM_ROW0	292
#define	MAX_TM_ROW1	293
#define	MAX_TM_ROW2	294
#define	MAX_TM_ROW3	295

#include <common/Defines.h>
#include <3dsparse/ASEModelFactory.h>

static FixedVector tmRow[3];

Face aseFace;
int aseFaceNo;

extern FixedVector aseVector;
extern char aseString[1024];
extern float aseFloat;
extern int aseDigit;
extern int aselex();

int aseerror(const char *errmsg);
int asewarning(const char *warningmsg);
int asedebug(const char *debugmsg);

extern char *asetext;
extern int aselineno;

#ifndef YYSTYPE
#define YYSTYPE int
#endif
#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		166
#define	YYFLAG		32768
#define	YYNTBASE	41

#define YYTRANSLATE(x) ((unsigned)(x) <= 295 ? yytranslate[x] : 70)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     1,     2,     3,     4,     5,
     6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
    16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
    26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
    36,    37,    38,    39,    40
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     6,     9,    12,    30,    36,    38,    41,    55,    58,
    61,    96,    99,   102,   105,   129,   131,   134,   138,   140,
   143,   159,   162,   165,   168,   170,   173,   178,   182,   186,
   190,   194,   196
};

static const short yyrhs[] = {    42,
    43,    44,    45,    46,     0,     8,     3,     0,     9,    68,
     0,    11,     6,    12,     5,    12,     3,    12,     3,    12,
     3,    12,     3,    12,    69,    12,    69,     7,     0,    13,
     6,    14,     3,     7,     0,    47,     0,    46,    47,     0,
    15,     6,    49,    50,    54,    33,     3,    34,     3,    35,
     3,    48,     7,     0,    36,    69,     0,    16,    68,     0,
    17,     6,    16,     5,    18,     3,     3,     3,    18,     3,
     3,     3,    18,     3,     3,     3,    51,    52,    53,    40,
    69,    18,    69,    18,    69,    18,     4,    18,    69,    18,
    69,    18,     4,     7,     0,    37,    69,     0,    38,    69,
     0,    39,    69,     0,    19,     6,    20,     3,    21,     3,
    22,     3,    23,     6,    55,     7,    25,     6,    57,     7,
    29,     3,    30,     6,    62,     7,     7,     0,    56,     0,
    55,    56,     0,    24,     3,    69,     0,    58,     0,    57,
    58,     0,    26,    10,    59,    60,    61,    10,     3,    10,
     3,    10,     3,    27,     3,    28,     3,     0,    10,     3,
     0,    10,     3,     0,    10,     3,     0,    63,     0,    62,
    63,     0,    64,    65,    66,    67,     0,    31,     3,    69,
     0,    32,     3,    69,     0,    32,     3,    69,     0,    32,
     3,    69,     0,     5,     0,     4,     4,     4,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
    44,    52,    56,    60,    72,    78,    79,    83,    95,   100,
   105,   124,   129,   134,   139,   151,   152,   156,   161,   162,
   166,   178,   183,   188,   196,   197,   201,   208,   213,   218,
   223,   228,   232
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","MAX_DIGIT",
"MAX_FLOAT","MAX_STRING","MAX_LBRACKET","MAX_RBRACKET","MAX_ASCIIEXPORT_SYM",
"MAX_COMMENT","MAX_MESH_FACE_ID","MAX_SCENE","MAX_SCENE_TOKEN","MAX_MATERIAL_LIST",
"MAX_MATERIAL_LIST_TOKEN","MAX_GEOMOBJECT","MAX_NODE_NAME","MAX_NODE_TM","MAX_NODE_TM_TOKEN",
"MAX_MESH","MAX_TIMEVALUE","MAX_MESH_NUMVERTEX","MAX_MESH_NUMFACES","MAX_MESH_VERTEX_LIST",
"MAX_MESH_VERTEX","MAX_MESH_FACE_LIST","MAX_MESH_FACE","MAX_MESH_SMOOTHING",
"MAX_MESH_MTLID","MAX_MESH_NUMTVERTEX","MAX_MESH_NORMALS","MAX_MESH_FACENORMAL",
"MAX_MESH_VERTEXNORMAL","MAX_PROP_MOTIONBLUR","MAX_PROP_CASTSHADOW","MAX_PROP_RECVSHADOW",
"MAX_WIREFRAME_COLOR","MAX_TM_ROW0","MAX_TM_ROW1","MAX_TM_ROW2","MAX_TM_ROW3",
"MaxFile","MaxExport","MaxComment","MaxScene","MaxMaterialList","MaxGeomObjectList",
"MaxGeomObject","MaxColor","MaxGeomObjectInit","MaxNodeTm","MaxNodeTmRow0","MaxNodeTmRow1",
"MaxNodeTmRow2","MaxMesh","MaxVertexList","MaxVertex","MaxFaceList","MaxFace",
"MaxFaceA","MaxFaceB","MaxFaceC","MaxNormalList","MaxNormal","MaxFaceNormal",
"MaxVertexNormal0","MaxVertexNormal1","MaxVertexNormal2","MaxString","MaxVector", NULL
};
#endif

static const short yyr1[] = {     0,
    41,    42,    43,    44,    45,    46,    46,    47,    48,    49,
    50,    51,    52,    53,    54,    55,    55,    56,    57,    57,
    58,    59,    60,    61,    62,    62,    63,    64,    65,    66,
    67,    68,    69
};

static const short yyr2[] = {     0,
     5,     2,     2,    17,     5,     1,     2,    13,     2,     2,
    34,     2,     2,     2,    23,     1,     2,     3,     1,     2,
    15,     2,     2,     2,     1,     2,     4,     3,     3,     3,
     3,     1,     3
};

static const short yydefact[] = {     0,
     0,     0,     2,     0,     0,    32,     3,     0,     0,     0,
     0,     0,     0,     0,     0,     1,     6,     0,     0,     0,
     7,     0,     0,     0,     0,     0,     5,    10,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     9,     8,
    33,     0,     0,     0,     4,     0,     0,     0,    16,     0,
     0,     0,    17,     0,    18,     0,     0,     0,     0,     0,
     0,    19,     0,     0,     0,     0,    20,    12,     0,     0,
     0,     0,     0,    13,     0,     0,    22,     0,     0,     0,
    14,     0,    23,     0,     0,     0,     0,    24,     0,     0,
     0,     0,     0,     0,    25,     0,     0,     0,     0,     0,
    26,     0,     0,     0,     0,    28,    15,     0,     0,     0,
     0,     0,    29,     0,     0,    27,     0,     0,    30,     0,
     0,     0,    31,     0,     0,     0,     0,     0,    21,     0,
     0,     0,    11,     0,     0,     0
};

static const short yydefgoto[] = {   164,
     2,     5,     9,    12,    16,    17,    64,    25,    30,    94,
   100,   106,    34,    78,    79,    91,    92,   102,   109,   115,
   124,   125,   126,   133,   140,   146,     7,    60
};

static const short yypact[] = {    -4,
     3,    -1,-32768,     2,     0,-32768,-32768,     4,     1,    -3,
     6,     5,     8,     9,    10,     5,-32768,     7,    12,    11,
-32768,    14,    17,     2,    15,    16,-32768,-32768,    20,    18,
    26,    19,    25,    13,    21,    29,    23,    35,    36,    22,
    38,    27,    32,    42,    30,    44,    45,    47,    49,    24,
    41,    51,    33,    53,    56,    39,    59,    28,    61,    55,
    60,    46,    56,    63,    64,    56,    69,    67,-32768,-32768,
-32768,    70,    71,    52,-32768,    62,    75,    -2,-32768,    76,
    56,    57,-32768,    80,-32768,    78,    82,    66,    65,    79,
    -5,-32768,    56,    48,    83,    68,-32768,-32768,    56,    72,
    85,    84,    87,-32768,    56,    58,-32768,    93,    89,    73,
-32768,    56,-32768,    97,    91,    98,    88,-32768,   102,    77,
    56,    99,   104,    -6,-32768,    81,    92,   109,    56,   107,
-32768,   112,    86,    56,   106,-32768,-32768,    56,   114,    90,
   101,   117,-32768,    56,   118,-32768,   119,   100,-32768,    56,
   108,   121,-32768,    56,   103,   110,   122,    56,-32768,   111,
   126,   125,-32768,   133,   134,-32768
};

static const short yypgoto[] = {-32768,
-32768,-32768,-32768,-32768,-32768,   120,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,    74,-32768,    50,-32768,-32768,-32768,
-32768,    31,-32768,-32768,-32768,-32768,   113,   -63
};


#define	YYLAST		155


static const short yytable[] = {    69,
   130,    96,    72,     1,    82,     3,     6,     4,    13,    10,
     8,    14,    18,    11,    23,    20,    26,    85,    22,    15,
    90,    77,    19,    27,   123,    32,    24,    31,    35,    98,
    37,    29,    39,    40,    36,   104,    33,    42,    43,    44,
    45,   111,    41,    47,    48,    38,    50,    51,   117,    52,
    49,    53,    55,    56,    57,    58,    61,   127,    54,    59,
    46,    62,    67,    63,    65,   136,    66,    71,    68,    70,
   141,    73,    74,    76,   143,    77,    75,    81,    84,    80,
   149,    86,    87,    88,    89,    99,   153,   107,    95,   110,
   156,    90,   101,   108,   160,   113,   103,   112,   114,   118,
   119,    93,   116,   120,   122,   121,   129,   123,   128,   134,
   105,   135,   132,   137,   138,   142,   144,   139,   147,   148,
   150,   145,   151,   155,   159,   154,   152,   158,   161,   162,
   157,   163,   165,   166,     0,    21,    28,     0,     0,     0,
    97,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,    83,     0,     0,   131
};

static const short yycheck[] = {    63,
     7,     7,    66,     8,     7,     3,     5,     9,    12,     6,
    11,     6,     5,    13,     3,     6,     3,    81,    12,    15,
    26,    24,    14,     7,    31,     6,    16,    12,     3,    93,
     6,    17,    12,     5,    16,    99,    19,     3,     3,    18,
     3,   105,    20,    12,     3,    33,     3,     3,   112,     3,
    21,     3,    12,     3,    22,     3,    18,   121,    35,     4,
    34,     3,     3,    36,     4,   129,    12,     4,    23,     7,
   134,     3,     6,     3,   138,    24,     7,     3,     3,    18,
   144,    25,     3,     6,     3,    38,   150,     3,    10,     3,
   154,    26,    10,    10,   158,     3,    29,    40,    10,     3,
    10,    37,    30,     6,     3,    18,     3,    31,    10,    18,
    39,     3,    32,     7,     3,    10,     3,    32,    18,     3,
     3,    32,     4,     3,     3,    18,    27,    18,    18,     4,
    28,     7,     0,     0,    -1,    16,    24,    -1,    -1,    -1,
    91,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    78,    -1,    -1,   124
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */

/* Skeleton output parser for bison,
   Copyright (C) 1984, 1989, 1990 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

#ifndef alloca
#ifdef __GNUC__
#define alloca __builtin_alloca
#else /* not GNU C.  */
#if (!defined (__STDC__) && defined (sparc)) || defined (__sparc__) || defined (__sparc) || defined (__sgi)
#include <alloca.h>
#else /* not sparc */
#if defined (MSDOS) && !defined (__TURBOC__)
#include <malloc.h>
#else /* not MSDOS, or __TURBOC__ */
#if defined(_AIX)
#include <malloc.h>
 #pragma alloca
#else /* not MSDOS, __TURBOC__, or _AIX */
#ifdef __hpux
#ifdef __cplusplus
extern "C" {
void *alloca (unsigned int);
};
#else /* not __cplusplus */
void *alloca ();
#endif /* not __cplusplus */
#endif /* __hpux */
#endif /* not _AIX */
#endif /* not MSDOS, or __TURBOC__ */
#endif /* not sparc.  */
#endif /* not GNU C.  */
#endif /* alloca not defined.  */

/* This is the parser code that is written into each bison parser
  when the %semantic_parser declaration is not specified in the grammar.
  It was written by Richard Stallman by simplifying the hairy parser
  used when %semantic_parser is specified.  */

/* Note: there must be only one dollar sign in this file.
   It is replaced by the list of actions, each action
   as one case of the switch.  */

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	return(0)
#define YYABORT 	return(1)
#define YYERROR		goto yyerrlab1
/* Like YYERROR except do call yyerror.
   This remains here temporarily to ease the
   transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto yyerrlab
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(token, value) \
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    { yychar = (token), yylval = (value);			\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { yyerror ("syntax error: cannot back up"); YYERROR; }	\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

#ifndef YYPURE
#define YYLEX		yylex()
#endif

#ifdef YYPURE
#ifdef YYLSP_NEEDED
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, &yylloc, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval, &yylloc)
#endif
#else /* not YYLSP_NEEDED */
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval)
#endif
#endif /* not YYLSP_NEEDED */
#endif

/* If nonreentrant, generate the variables here */

#ifndef YYPURE

int	yychar;			/*  the lookahead symbol		*/
YYSTYPE	yylval;			/*  the semantic value of the		*/
				/*  lookahead symbol			*/

#ifdef YYLSP_NEEDED
YYLTYPE yylloc;			/*  location data for the lookahead	*/
				/*  symbol				*/
#endif

int yynerrs;			/*  number of parse errors so far       */
#endif  /* not YYPURE */

#if YYDEBUG != 0
int yydebug;			/*  nonzero means print parse trace	*/
/* Since this is uninitialized, it does not stop multiple parsers
   from coexisting.  */
#endif

/*  YYINITDEPTH indicates the initial size of the parser's stacks	*/

#ifndef	YYINITDEPTH
#define YYINITDEPTH 200
#endif

/*  YYMAXDEPTH is the maximum size the stacks can grow to
    (effective only if the built-in stack extension method is used).  */

#if YYMAXDEPTH == 0
#undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
#define YYMAXDEPTH 10000
#endif

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
int yyparse (void);
#endif
#if __GNUC__ > 1		/* GNU C and GNU C++ define this.  */
#define __yy_memcpy(TO,FROM,COUNT)	__builtin_memcpy(TO,FROM,COUNT)
#else				/* not GNU C or C++ */
#ifndef __cplusplus

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (to, from, count)
     char *to;
     char *from;
     int count;
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#else /* __cplusplus */

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (char *to, char *from, int count)
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#endif
#endif

/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
#ifdef __cplusplus
#define YYPARSE_PARAM_ARG void *YYPARSE_PARAM
#define YYPARSE_PARAM_DECL
#else /* not __cplusplus */
#define YYPARSE_PARAM_ARG YYPARSE_PARAM
#define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
#endif /* not __cplusplus */
#else /* not YYPARSE_PARAM */
#define YYPARSE_PARAM_ARG
#define YYPARSE_PARAM_DECL
#endif /* not YYPARSE_PARAM */

int
yyparse(YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  register int yystate;
  register int yyn;
  register short *yyssp;
  register YYSTYPE *yyvsp;
  int yyerrstatus;	/*  number of tokens to shift before error messages enabled */
  int yychar1 = 0;		/*  lookahead token as an internal (translated) token number */

  short	yyssa[YYINITDEPTH];	/*  the state stack			*/
  YYSTYPE yyvsa[YYINITDEPTH];	/*  the semantic value stack		*/

  short *yyss = yyssa;		/*  refer to the stacks thru separate pointers */
  YYSTYPE *yyvs = yyvsa;	/*  to allow yyoverflow to reallocate them elsewhere */

#ifdef YYLSP_NEEDED
  YYLTYPE yylsa[YYINITDEPTH];	/*  the location stack			*/
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;

#define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
#define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  int yystacksize = YYINITDEPTH;

#ifdef YYPURE
  int yychar;
  YYSTYPE yylval;
  int yynerrs;
#ifdef YYLSP_NEEDED
  YYLTYPE yylloc;
#endif
#endif

  YYSTYPE yyval;		/*  the variable used to return		*/
				/*  semantic values from the action	*/
				/*  routines				*/

  int yylen;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Starting parse\n");
#endif

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss - 1;
  yyvsp = yyvs;
#ifdef YYLSP_NEEDED
  yylsp = yyls;
#endif

/* Push a new state, which is found in  yystate  .  */
/* In all cases, when you get here, the value and location stacks
   have just been pushed. so pushing a state here evens the stacks.  */
yynewstate:

  *++yyssp = (short) yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Give user a chance to reallocate the stack */
      /* Use copies of these so that the &'s don't force the real ones into memory. */
      YYSTYPE *yyvs1 = yyvs;
      short *yyss1 = yyss;
#ifdef YYLSP_NEEDED
      YYLTYPE *yyls1 = yyls;
#endif

      /* Get the current used size of the three stacks, in elements.  */
      int size = (int)(yyssp - yyss + 1);

#ifdef yyoverflow
      /* Each stack pointer address is followed by the size of
	 the data in use in that stack, in bytes.  */
#ifdef YYLSP_NEEDED
      /* This used to be a conditional around just the two extra args,
	 but that might be undefined if yyoverflow is a macro.  */
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yyls1, size * sizeof (*yylsp),
		 &yystacksize);
#else
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yystacksize);
#endif

      yyss = yyss1; yyvs = yyvs1;
#ifdef YYLSP_NEEDED
      yyls = yyls1;
#endif
#else /* no yyoverflow */
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	{
	  yyerror("parser stack overflow");
	  return 2;
	}
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;
      yyss = (short *) alloca (yystacksize * sizeof (*yyssp));
      __yy_memcpy ((char *)yyss, (char *)yyss1, size * sizeof (*yyssp));
      yyvs = (YYSTYPE *) alloca (yystacksize * sizeof (*yyvsp));
      __yy_memcpy ((char *)yyvs, (char *)yyvs1, size * sizeof (*yyvsp));
#ifdef YYLSP_NEEDED
      yyls = (YYLTYPE *) alloca (yystacksize * sizeof (*yylsp));
      __yy_memcpy ((char *)yyls, (char *)yyls1, size * sizeof (*yylsp));
#endif
#endif /* no yyoverflow */

      yyssp = yyss + size - 1;
      yyvsp = yyvs + size - 1;
#ifdef YYLSP_NEEDED
      yylsp = yyls + size - 1;
#endif

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Stack size increased to %d\n", yystacksize);
#endif

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Entering state %d\n", yystate);
#endif

  goto yybackup;
 yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Reading a token: ");
#endif
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Now at end of input.\n");
#endif
    }
  else
    {
      yychar1 = YYTRANSLATE(yychar);

#if YYDEBUG != 0
      if (yydebug)
	{
	  fprintf (stderr, "Next token is %d (%s", yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise meaning
	     of a token, for further debugging info.  */
#ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
#endif
	  fprintf (stderr, ")\n");
	}
#endif
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    goto yydefault;

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
     Negative => reduce, -yyn is rule number.
     Positive => shift, yyn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting token %d (%s), ", yychar, yytname[yychar1]);
#endif

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* count tokens shifted since error; after three, turn off error status.  */
  if (yyerrstatus) yyerrstatus--;

  yystate = yyn;
  goto yynewstate;

/* Do the default action for the current state.  */
yydefault:

  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;

/* Do a reduction.  yyn is the number of a rule to reduce with.  */
yyreduce:
  yylen = yyr2[yyn];
  if (yylen > 0)
    yyval = yyvsp[1-yylen]; /* implement default value of the action */

#if YYDEBUG != 0
  if (yydebug)
    {
      int i;

      fprintf (stderr, "Reducing via rule %d (line %d), ",
	       yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (i = yyprhs[yyn]; yyrhs[i] > 0; i++)
	fprintf (stderr, "%s ", yytname[yyrhs[i]]);
      fprintf (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif


  switch (yyn) {

case 9:
{ 
	FixedVector4 ambientColor(fixed(true, 6000), fixed(true, 6000), fixed(true, 6000), 1);
	FixedVector4 diffuseColor(fixed(true, 8000), fixed(true, 8000), fixed(true, 8000), 1);
	ASEModelFactory::getCurrent()->getCurrentMesh()->getAmbientColor() = ambientColor;
	ASEModelFactory::getCurrent()->getCurrentMesh()->getDiffuseColor() = diffuseColor;
	ASEModelFactory::getCurrent()->getCurrentMesh()->getEmissiveColor() = FixedVector4::getNullVector();
	ASEModelFactory::getCurrent()->getCurrentMesh()->getSpecularColor() = FixedVector4::getNullVector();

	FixedVector ambientNoTexColor = aseVector * fixed(true, 6000);
	FixedVector diffuseNoTexColor = aseVector * fixed(true, 8000);
	ASEModelFactory::getCurrent()->getCurrentMesh()->getAmbientNoTexColor() = ambientNoTexColor;
	ASEModelFactory::getCurrent()->getCurrentMesh()->getDiffuseNoTexColor() = diffuseNoTexColor;
	ASEModelFactory::getCurrent()->getCurrentMesh()->getEmissiveNoTexColor() = FixedVector4::getNullVector();
	ASEModelFactory::getCurrent()->getCurrentMesh()->getSpecularNoTexColor() = FixedVector4::getNullVector();

	ASEModelFactory::getCurrent()->getCurrentMesh()->getShininessColor() = 0;

	break;
}
case 10:
{ ASEModelFactory::getCurrent()->addMesh(aseString); ;
    break;}
case 12:
{ tmRow[0] = aseVector.Normalize(); 
    break;}
case 13:
{ tmRow[1] = aseVector.Normalize(); 
    break;}
case 14:
{ tmRow[2] = aseVector.Normalize(); 
    break;}
case 18:
{ 
	Vertex vertex;
	vertex.position = aseVector;
	vertex.boneIndex = -1;
	ASEModelFactory::getCurrent()->getCurrentMesh()->insertVertex(vertex); ;
    break;}
case 22:
{ aseFace.v[0] = aseDigit; ;
    break;}
case 23:
{ aseFace.v[1] = aseDigit; ;
    break;}
case 24:
{ 
		aseFace.v[2] = aseDigit;
		ASEModelFactory::getCurrent()->getCurrentMesh()->insertFace(aseFace);	
	;
    break;}
case 28:
{ aseFaceNo = aseDigit; ;
    break;}
case 29:
{ 
	FixedVector n(
		aseVector[0] * tmRow[0][0] + aseVector[1] * tmRow[1][0] + aseVector[2] * tmRow[2][0],
		aseVector[0] * tmRow[0][1] + aseVector[1] * tmRow[1][1] + aseVector[2] * tmRow[2][1],
		aseVector[0] * tmRow[0][2] + aseVector[1] * tmRow[1][2] + aseVector[2] * tmRow[2][2]);

	ASEModelFactory::getCurrent()->getCurrentMesh()->setFaceNormal(n, aseFaceNo, 0);
    break;}
case 30:
{ 
	FixedVector n(
		aseVector[0] * tmRow[0][0] + aseVector[1] * tmRow[1][0] + aseVector[2] * tmRow[2][0],
		aseVector[0] * tmRow[0][1] + aseVector[1] * tmRow[1][1] + aseVector[2] * tmRow[2][1],
		aseVector[0] * tmRow[0][2] + aseVector[1] * tmRow[1][2] + aseVector[2] * tmRow[2][2]);

	ASEModelFactory::getCurrent()->getCurrentMesh()->setFaceNormal(n, aseFaceNo, 1);
    break;}
case 31:
{ 
	FixedVector n(
		aseVector[0] * tmRow[0][0] + aseVector[1] * tmRow[1][0] + aseVector[2] * tmRow[2][0],
		aseVector[0] * tmRow[0][1] + aseVector[1] * tmRow[1][1] + aseVector[2] * tmRow[2][1],
		aseVector[0] * tmRow[0][2] + aseVector[1] * tmRow[1][2] + aseVector[2] * tmRow[2][2]);

	ASEModelFactory::getCurrent()->getCurrentMesh()->setFaceNormal(n, aseFaceNo, 2);
    break;}
}
   /* the action file gets copied in in place of this dollarsign */

  yyvsp -= yylen;
  yyssp -= yylen;
#ifdef YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;

#ifdef YYLSP_NEEDED
  yylsp++;
  if (yylen == 0)
    {
      yylsp->first_line = yylloc.first_line;
      yylsp->first_column = yylloc.first_column;
      yylsp->last_line = (yylsp-1)->last_line;
      yylsp->last_column = (yylsp-1)->last_column;
      yylsp->text = 0;
    }
  else
    {
      yylsp->last_line = (yylsp+yylen-1)->last_line;
      yylsp->last_column = (yylsp+yylen-1)->last_column;
    }
#endif

  /* Now "shift" the result of the reduction.
     Determine what state that goes to,
     based on the state we popped back to
     and the rule number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;

yyerrlab:   /* here on detecting error */

  if (! yyerrstatus)
    /* If not already recovering from an error, report this error.  */
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  int size = 0;
	  char *msg;
	  int x, count;

	  count = 0;
	  /* Start X at -yyn if nec to avoid negative indexes in yycheck.  */
	  for (x = (yyn < 0 ? -yyn : 0);
	       x < (sizeof(yytname) / sizeof(char *)); x++)
	    if (yycheck[x + yyn] == x)
	      size += strlen(yytname[x]) + 15, count++;
	  msg = (char *) malloc(size + 15);
	  if (msg != 0)
	    {
	      strcpy(msg, "parse error");

	      if (count < 5)
		{
		  count = 0;
		  for (x = (yyn < 0 ? -yyn : 0);
		       x < (sizeof(yytname) / sizeof(char *)); x++)
		    if (yycheck[x + yyn] == x)
		      {
			strcat(msg, count == 0 ? ", expecting `" : " or `");
			strcat(msg, yytname[x]);
			strcat(msg, "'");
			count++;
		      }
		}
	      yyerror(msg);
	      free(msg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exceeded");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror("parse error");
    }

  goto yyerrlab1;
yyerrlab1:   /* here on error raised explicitly by an action */

  if (yyerrstatus == 3)
    {
      /* if just tried and failed to reuse lookahead token after an error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Discarding token %d (%s).\n", yychar, yytname[yychar1]);
#endif

      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token
     after shifting the error token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;

yyerrdefault:  /* current state does not do anything special for the error token. */

#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */
  yyn = yydefact[yystate];  /* If its default is to accept any token, ok.  Otherwise pop it.*/
  if (yyn) goto yydefault;
#endif

yyerrpop:   /* pop the current state because it cannot handle the error token */

  if (yyssp == yyss) YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#ifdef YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "Error: state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

yyerrhandle:

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yyerrdefault;

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    goto yyerrdefault;

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrpop;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;

  if (yyn == YYFINAL)
    YYACCEPT;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting error token, ");
#endif

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  yystate = yyn;
  goto yynewstate;
}

int aseerror(const char *errmsg)
{
	char buffer[1024];
	snprintf(buffer, 1024, "%i:%s\n", aselineno, errmsg);
	S3D::dialogMessage("Lexer", buffer);
    return 0;
}

int asewarning(const char *warningmsg)
{
	char buffer[1024];
	snprintf(buffer, 1024, "%i:%s\n", aselineno, warningmsg);
	S3D::dialogMessage("Lexer", buffer);
    return 0;
}

int asedebug(const char *debugmsg)
{
	char buffer[1024];
	snprintf(buffer, 1024, "%i:%s\n", aselineno, debugmsg);
	S3D::dialogMessage("Lexer", buffer);
    return 0;
}
