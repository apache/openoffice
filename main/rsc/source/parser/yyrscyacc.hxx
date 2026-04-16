/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_YYRSCYACC_HXX_INCLUDED
# define YY_YY_YYRSCYACC_HXX_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    NUMBER = 258,                  /* NUMBER  */
    SYMBOL = 259,                  /* SYMBOL  */
    RSCDEFINE = 260,               /* RSCDEFINE  */
    STRING = 261,                  /* STRING  */
    INCLUDE_STRING = 262,          /* INCLUDE_STRING  */
    CHARACTER = 263,               /* CHARACTER  */
    BOOLEAN = 264,                 /* BOOLEAN  */
    LINE = 265,                    /* LINE  */
    AUTO_ID = 266,                 /* AUTO_ID  */
    NOT = 267,                     /* NOT  */
    XSCALE = 268,                  /* XSCALE  */
    YSCALE = 269,                  /* YSCALE  */
    RGB = 270,                     /* RGB  */
    GEOMETRY = 271,                /* GEOMETRY  */
    POSITION = 272,                /* POSITION  */
    DIMENSION = 273,               /* DIMENSION  */
    INZOOMOUTPUTSIZE = 274,        /* INZOOMOUTPUTSIZE  */
    FLOATINGPOS = 275,             /* FLOATINGPOS  */
    DEFINE = 276,                  /* DEFINE  */
    INCLUDE = 277,                 /* INCLUDE  */
    MACROTARGET = 278,             /* MACROTARGET  */
    DEFAULT = 279,                 /* DEFAULT  */
    CLASSNAME = 280,               /* CLASSNAME  */
    VARNAME = 281,                 /* VARNAME  */
    CONSTNAME = 282,               /* CONSTNAME  */
    CLASS = 283,                   /* CLASS  */
    EXTENDABLE = 284,              /* EXTENDABLE  */
    WRITEIFSET = 285,              /* WRITEIFSET  */
    LEFTSHIFT = 286,               /* LEFTSHIFT  */
    RIGHTSHIFT = 287,              /* RIGHTSHIFT  */
    UNARYMINUS = 288,              /* UNARYMINUS  */
    UNARYPLUS = 289                /* UNARYPLUS  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 34 "yyrscyacc.y"

	Atom			varid;
	struct {
		Atom			hashid;
		sal_Int32			nValue;
	}				constname;
	RscTop *		pClass;
	RSCHEADER		header;
	struct {
		CLASS_DATA	pData;
		RscTop *	pClass;
	}				instance;
	sal_Int32			value;
	sal_uInt16			ushort;
	short			exp_short;
	char *			string;
	sal_Bool			svbool;
	REF_ENUM		copyref;
	RscDefine	  * defineele;
	CharSet 		charset;
	RscExpType		macrostruct;

#line 121 "yyrscyacc.hxx"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_YYRSCYACC_HXX_INCLUDED  */
