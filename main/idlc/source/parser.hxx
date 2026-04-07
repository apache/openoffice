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

#ifndef YY_YY_MAIN_IDLC_SOURCE_PARSER_HXX_INCLUDED
# define YY_YY_MAIN_IDLC_SOURCE_PARSER_HXX_INCLUDED
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
    IDL_IDENTIFIER = 258,          /* IDL_IDENTIFIER  */
    IDL_ATTRIBUTE = 259,           /* IDL_ATTRIBUTE  */
    IDL_BOUND = 260,               /* IDL_BOUND  */
    IDL_CASE = 261,                /* IDL_CASE  */
    IDL_CONST = 262,               /* IDL_CONST  */
    IDL_CONSTANTS = 263,           /* IDL_CONSTANTS  */
    IDL_CONSTRAINED = 264,         /* IDL_CONSTRAINED  */
    IDL_DEFAULT = 265,             /* IDL_DEFAULT  */
    IDL_ENUM = 266,                /* IDL_ENUM  */
    IDL_EXCEPTION = 267,           /* IDL_EXCEPTION  */
    IDL_INTERFACE = 268,           /* IDL_INTERFACE  */
    IDL_MAYBEAMBIGUOUS = 269,      /* IDL_MAYBEAMBIGUOUS  */
    IDL_MAYBEDEFAULT = 270,        /* IDL_MAYBEDEFAULT  */
    IDL_MAYBEVOID = 271,           /* IDL_MAYBEVOID  */
    IDL_MODULE = 272,              /* IDL_MODULE  */
    IDL_NEEDS = 273,               /* IDL_NEEDS  */
    IDL_OBSERVES = 274,            /* IDL_OBSERVES  */
    IDL_OPTIONAL = 275,            /* IDL_OPTIONAL  */
    IDL_PROPERTY = 276,            /* IDL_PROPERTY  */
    IDL_RAISES = 277,              /* IDL_RAISES  */
    IDL_READONLY = 278,            /* IDL_READONLY  */
    IDL_REMOVEABLE = 279,          /* IDL_REMOVEABLE  */
    IDL_SERVICE = 280,             /* IDL_SERVICE  */
    IDL_SEQUENCE = 281,            /* IDL_SEQUENCE  */
    IDL_SINGLETON = 282,           /* IDL_SINGLETON  */
    IDL_STRUCT = 283,              /* IDL_STRUCT  */
    IDL_SWITCH = 284,              /* IDL_SWITCH  */
    IDL_TYPEDEF = 285,             /* IDL_TYPEDEF  */
    IDL_TRANSIENT = 286,           /* IDL_TRANSIENT  */
    IDL_UNION = 287,               /* IDL_UNION  */
    IDL_ANY = 288,                 /* IDL_ANY  */
    IDL_CHAR = 289,                /* IDL_CHAR  */
    IDL_BOOLEAN = 290,             /* IDL_BOOLEAN  */
    IDL_BYTE = 291,                /* IDL_BYTE  */
    IDL_DOUBLE = 292,              /* IDL_DOUBLE  */
    IDL_FLOAT = 293,               /* IDL_FLOAT  */
    IDL_HYPER = 294,               /* IDL_HYPER  */
    IDL_LONG = 295,                /* IDL_LONG  */
    IDL_SHORT = 296,               /* IDL_SHORT  */
    IDL_VOID = 297,                /* IDL_VOID  */
    IDL_STRING = 298,              /* IDL_STRING  */
    IDL_TYPE = 299,                /* IDL_TYPE  */
    IDL_UNSIGNED = 300,            /* IDL_UNSIGNED  */
    IDL_TRUE = 301,                /* IDL_TRUE  */
    IDL_FALSE = 302,               /* IDL_FALSE  */
    IDL_IN = 303,                  /* IDL_IN  */
    IDL_OUT = 304,                 /* IDL_OUT  */
    IDL_INOUT = 305,               /* IDL_INOUT  */
    IDL_ONEWAY = 306,              /* IDL_ONEWAY  */
    IDL_GET = 307,                 /* IDL_GET  */
    IDL_SET = 308,                 /* IDL_SET  */
    IDL_PUBLISHED = 309,           /* IDL_PUBLISHED  */
    IDL_ELLIPSIS = 310,            /* IDL_ELLIPSIS  */
    IDL_LEFTSHIFT = 311,           /* IDL_LEFTSHIFT  */
    IDL_RIGHTSHIFT = 312,          /* IDL_RIGHTSHIFT  */
    IDL_SCOPESEPARATOR = 313,      /* IDL_SCOPESEPARATOR  */
    IDL_INTEGER_LITERAL = 314,     /* IDL_INTEGER_LITERAL  */
    IDL_INTEGER_ULITERAL = 315,    /* IDL_INTEGER_ULITERAL  */
    IDL_FLOATING_PT_LITERAL = 316  /* IDL_FLOATING_PT_LITERAL  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 264 "main/idlc/source/parser.y"

	ExprType				etval;     /* Expression type */
	AstDeclaration*		dclval;    /* Declaration */
    AstDeclaration const * cdclval;
    DeclList * dclsval;
	AstExpression*		exval;		/* expression value */
	ExprList*				exlval;	/* expression list value */
	FeDeclarator*			fdval;		/* declarator value */
	FeDeclList*			dlval;		/* declarator list value */
	FeInheritanceHeader*	ihval;		/* inheritance header value */
	::rtl::OString*		sval;		/* OString value */
    std::vector< rtl::OString > * svals;
	sal_Char* 			strval;	/* sal_Char* value */
	sal_Bool				bval;		/* sal_Boolean* value */
	sal_Int64				ival;		/* sal_Int64 value */
    sal_uInt64 uval; /* sal_uInt64 value */
	sal_uInt32			ulval;		/* sal_uInt32 value */
	double					dval;		/* double value */
	float					fval;		/* float value */
	StringList*			slval;		/* StringList value	*/
	LabelList*			llval;		/* LabelList value	*/
	AstUnionLabel*		lbval;		/* union label value */
	AstMember*			mval;		/* member value */
    AttributeExceptions::Part attexcpval;
    AttributeExceptions attexcval;

#line 152 "main/idlc/source/parser.hxx"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_MAIN_IDLC_SOURCE_PARSER_HXX_INCLUDED  */
