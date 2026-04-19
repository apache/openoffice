/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1


/* Substitute the variable and function names.  */
#define yyparse         SQLyyparse
#define yylex           SQLyylex
#define yyerror         SQLyyerror
#define yydebug         SQLyydebug
#define yynerrs         SQLyynerrs
#define yylval          SQLyylval
#define yychar          SQLyychar

/* First part of user prologue.  */

/**************************************************************
 * 
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 * 
 *************************************************************/

#include <vector>
#include <string.h>

#ifndef _CONNECTIVITY_SQLNODE_HXX
#include <connectivity/sqlnode.hxx>
#endif
#ifndef _CONNECTIVITY_SQLPARSE_HXX
#include <connectivity/sqlparse.hxx>
#endif
#ifndef _CONNECTIVITY_SQLINTERNALNODE_HXX
#include <internalnode.hxx>
#endif
#ifndef _COM_SUN_STAR_LANG_LOCALE_HPP_
#include <com/sun/star/lang/Locale.hpp>
#endif
#ifndef _COM_SUN_STAR_SDBC_DATATYPE_HPP_
#include <com/sun/star/sdbc/DataType.hpp>
#endif
#ifndef _COM_SUN_STAR_UTIL_DATE_HPP_
#include <com/sun/star/util/Date.hpp>
#endif
#ifndef _COM_SUN_STAR_UTIL_DATETIME_HPP_
#include <com/sun/star/util/DateTime.hpp>
#endif
#ifndef _COM_SUN_STAR_UTIL_TIME_HPP_
#include <com/sun/star/util/Time.hpp>
#endif
#ifndef _COM_SUN_STAR_UTIL_XNUMBERFORMATTER_HPP_
#include <com/sun/star/util/XNumberFormatter.hpp>
#endif
#ifndef _COM_SUN_STAR_UTIL_XNUMBERFORMATSSUPPLIER_HPP_
#include <com/sun/star/util/XNumberFormatsSupplier.hpp>
#endif
#ifndef _COM_SUN_STAR_UTIL_XNUMBERFORMATS_HPP_
#include <com/sun/star/util/XNumberFormats.hpp>
#endif
#ifndef _COM_SUN_STAR_UTIL_NUMBERFORMAT_HPP_
#include <com/sun/star/util/NumberFormat.hpp>
#endif
#ifndef _COM_SUN_STAR_UTIL_XNUMBERFORMATTYPES_HPP_
#include <com/sun/star/util/XNumberFormatTypes.hpp>
#endif
#ifndef _COM_SUN_STAR_BEANS_XPROPERTYSET_HPP_
#include <com/sun/star/beans/XPropertySet.hpp>
#endif
#ifndef _COM_SUN_STAR_I18N_KPARSETYPE_HPP_
#include <com/sun/star/i18n/KParseType.hpp>
#endif
#ifndef _COM_SUN_STAR_I18N_KPARSETOKENS_HPP_
#include <com/sun/star/i18n/KParseTokens.hpp>
#endif
#ifndef _CONNECTIVITY_SQLSCAN_HXX
#include "sqlscan.hxx"
#endif
#ifndef _OSL_DIAGNOSE_H_
#include <osl/diagnose.h>
#endif
#ifndef _DBHELPER_DBCONVERSION_HXX_
#include "connectivity/dbconversion.hxx"
#endif
#include <rtl/ustrbuf.hxx>

#if defined __GNUC__
    #pragma GCC system_header
#elif defined __SUNPRO_CC
#pragma disable_warn
#elif defined _MSC_VER
#pragma warning(push, 1)
#pragma warning(disable:4273 4701 4706)
#endif

static ::rtl::OUString aEmptyString;

static connectivity::OSQLInternalNode* newNode(const sal_Char* pNewValue,
							     const connectivity::SQLNodeType eNodeType,
								 const sal_uInt32 nNodeID = 0)
{
	return new connectivity::OSQLInternalNode(pNewValue, eNodeType, nNodeID);
}

static connectivity::OSQLInternalNode* newNode(const ::rtl::OString& _NewValue,
							    const connectivity::SQLNodeType eNodeType,
								const sal_uInt32 nNodeID = 0)
{
	return new connectivity::OSQLInternalNode(_NewValue, eNodeType, nNodeID);
}

static connectivity::OSQLInternalNode* newNode(const ::rtl::OUString& _NewValue,
							    const connectivity::SQLNodeType eNodeType,
								const sal_uInt32 nNodeID = 0)
{
	return new connectivity::OSQLInternalNode(_NewValue, eNodeType, nNodeID);
}


// yyi ist die interne Nr. der Regel, die gerade reduziert wird.
// Ueber die Mapping-Tabelle yyrmap wird daraus eine externe Regel-Nr.
#define SQL_NEW_RULE 			newNode(aEmptyString, SQL_NODE_RULE, yyr1[yyn])
#define SQL_NEW_LISTRULE 		newNode(aEmptyString, SQL_NODE_LISTRULE, yyr1[yyn])
#define SQL_NEW_COMMALISTRULE   newNode(aEmptyString, SQL_NODE_COMMALISTRULE, yyr1[yyn])


connectivity::OSQLParser* xxx_pGLOBAL_SQLPARSER;

#define YYDEBUG 1
#define YYERROR_VERBOSE

#define SQLyyerror(s)						\
{											\
	xxx_pGLOBAL_SQLPARSER->error(s);		\
}

using namespace connectivity;
#define SQLyylex xxx_pGLOBAL_SQLPARSER->SQLlex


# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "sqlbison.hxx"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_3_ = 3,                         /* '('  */
  YYSYMBOL_4_ = 4,                         /* ')'  */
  YYSYMBOL_5_ = 5,                         /* ','  */
  YYSYMBOL_6_ = 6,                         /* ':'  */
  YYSYMBOL_7_ = 7,                         /* ';'  */
  YYSYMBOL_8_ = 8,                         /* '?'  */
  YYSYMBOL_9_ = 9,                         /* '['  */
  YYSYMBOL_10_ = 10,                       /* ']'  */
  YYSYMBOL_11_ = 11,                       /* '{'  */
  YYSYMBOL_12_ = 12,                       /* '}'  */
  YYSYMBOL_13_ = 13,                       /* '.'  */
  YYSYMBOL_14_K_ = 14,                     /* 'K'  */
  YYSYMBOL_15_M_ = 15,                     /* 'M'  */
  YYSYMBOL_16_G_ = 16,                     /* 'G'  */
  YYSYMBOL_17_T_ = 17,                     /* 'T'  */
  YYSYMBOL_18_P_ = 18,                     /* 'P'  */
  YYSYMBOL_SQL_TOKEN_STRING = 19,          /* SQL_TOKEN_STRING  */
  YYSYMBOL_SQL_TOKEN_ACCESS_DATE = 20,     /* SQL_TOKEN_ACCESS_DATE  */
  YYSYMBOL_SQL_TOKEN_INT = 21,             /* SQL_TOKEN_INT  */
  YYSYMBOL_SQL_TOKEN_REAL_NUM = 22,        /* SQL_TOKEN_REAL_NUM  */
  YYSYMBOL_SQL_TOKEN_INTNUM = 23,          /* SQL_TOKEN_INTNUM  */
  YYSYMBOL_SQL_TOKEN_APPROXNUM = 24,       /* SQL_TOKEN_APPROXNUM  */
  YYSYMBOL_SQL_TOKEN_NOT = 25,             /* SQL_TOKEN_NOT  */
  YYSYMBOL_SQL_TOKEN_NAME = 26,            /* SQL_TOKEN_NAME  */
  YYSYMBOL_SQL_TOKEN_UMINUS = 27,          /* SQL_TOKEN_UMINUS  */
  YYSYMBOL_SQL_TOKEN_ALL = 28,             /* SQL_TOKEN_ALL  */
  YYSYMBOL_SQL_TOKEN_ALTER = 29,           /* SQL_TOKEN_ALTER  */
  YYSYMBOL_SQL_TOKEN_AMMSC = 30,           /* SQL_TOKEN_AMMSC  */
  YYSYMBOL_SQL_TOKEN_ANY = 31,             /* SQL_TOKEN_ANY  */
  YYSYMBOL_SQL_TOKEN_AS = 32,              /* SQL_TOKEN_AS  */
  YYSYMBOL_SQL_TOKEN_ASC = 33,             /* SQL_TOKEN_ASC  */
  YYSYMBOL_SQL_TOKEN_AT = 34,              /* SQL_TOKEN_AT  */
  YYSYMBOL_SQL_TOKEN_AUTHORIZATION = 35,   /* SQL_TOKEN_AUTHORIZATION  */
  YYSYMBOL_SQL_TOKEN_AVG = 36,             /* SQL_TOKEN_AVG  */
  YYSYMBOL_SQL_TOKEN_BETWEEN = 37,         /* SQL_TOKEN_BETWEEN  */
  YYSYMBOL_SQL_TOKEN_BIT = 38,             /* SQL_TOKEN_BIT  */
  YYSYMBOL_SQL_TOKEN_BOTH = 39,            /* SQL_TOKEN_BOTH  */
  YYSYMBOL_SQL_TOKEN_BY = 40,              /* SQL_TOKEN_BY  */
  YYSYMBOL_SQL_TOKEN_CAST = 41,            /* SQL_TOKEN_CAST  */
  YYSYMBOL_SQL_TOKEN_CHARACTER = 42,       /* SQL_TOKEN_CHARACTER  */
  YYSYMBOL_SQL_TOKEN_CHECK = 43,           /* SQL_TOKEN_CHECK  */
  YYSYMBOL_SQL_TOKEN_COLLATE = 44,         /* SQL_TOKEN_COLLATE  */
  YYSYMBOL_SQL_TOKEN_COMMIT = 45,          /* SQL_TOKEN_COMMIT  */
  YYSYMBOL_SQL_TOKEN_CONTINUE = 46,        /* SQL_TOKEN_CONTINUE  */
  YYSYMBOL_SQL_TOKEN_CONVERT = 47,         /* SQL_TOKEN_CONVERT  */
  YYSYMBOL_SQL_TOKEN_COUNT = 48,           /* SQL_TOKEN_COUNT  */
  YYSYMBOL_SQL_TOKEN_CREATE = 49,          /* SQL_TOKEN_CREATE  */
  YYSYMBOL_SQL_TOKEN_CROSS = 50,           /* SQL_TOKEN_CROSS  */
  YYSYMBOL_SQL_TOKEN_CURRENT = 51,         /* SQL_TOKEN_CURRENT  */
  YYSYMBOL_SQL_TOKEN_CURSOR = 52,          /* SQL_TOKEN_CURSOR  */
  YYSYMBOL_SQL_TOKEN_DATE = 53,            /* SQL_TOKEN_DATE  */
  YYSYMBOL_SQL_TOKEN_DATEVALUE = 54,       /* SQL_TOKEN_DATEVALUE  */
  YYSYMBOL_SQL_TOKEN_DAY = 55,             /* SQL_TOKEN_DAY  */
  YYSYMBOL_SQL_TOKEN_DEC = 56,             /* SQL_TOKEN_DEC  */
  YYSYMBOL_SQL_TOKEN_DECIMAL = 57,         /* SQL_TOKEN_DECIMAL  */
  YYSYMBOL_SQL_TOKEN_DECLARE = 58,         /* SQL_TOKEN_DECLARE  */
  YYSYMBOL_SQL_TOKEN_DEFAULT = 59,         /* SQL_TOKEN_DEFAULT  */
  YYSYMBOL_SQL_TOKEN_DELETE = 60,          /* SQL_TOKEN_DELETE  */
  YYSYMBOL_SQL_TOKEN_DESC = 61,            /* SQL_TOKEN_DESC  */
  YYSYMBOL_SQL_TOKEN_DISTINCT = 62,        /* SQL_TOKEN_DISTINCT  */
  YYSYMBOL_SQL_TOKEN_DOUBLE = 63,          /* SQL_TOKEN_DOUBLE  */
  YYSYMBOL_SQL_TOKEN_DROP = 64,            /* SQL_TOKEN_DROP  */
  YYSYMBOL_SQL_TOKEN_ESCAPE = 65,          /* SQL_TOKEN_ESCAPE  */
  YYSYMBOL_SQL_TOKEN_EXCEPT = 66,          /* SQL_TOKEN_EXCEPT  */
  YYSYMBOL_SQL_TOKEN_EXISTS = 67,          /* SQL_TOKEN_EXISTS  */
  YYSYMBOL_SQL_TOKEN_FALSE = 68,           /* SQL_TOKEN_FALSE  */
  YYSYMBOL_SQL_TOKEN_FETCH = 69,           /* SQL_TOKEN_FETCH  */
  YYSYMBOL_SQL_TOKEN_FLOAT = 70,           /* SQL_TOKEN_FLOAT  */
  YYSYMBOL_SQL_TOKEN_FOR = 71,             /* SQL_TOKEN_FOR  */
  YYSYMBOL_SQL_TOKEN_FOREIGN = 72,         /* SQL_TOKEN_FOREIGN  */
  YYSYMBOL_SQL_TOKEN_FOUND = 73,           /* SQL_TOKEN_FOUND  */
  YYSYMBOL_SQL_TOKEN_FROM = 74,            /* SQL_TOKEN_FROM  */
  YYSYMBOL_SQL_TOKEN_FULL = 75,            /* SQL_TOKEN_FULL  */
  YYSYMBOL_SQL_TOKEN_GRANT = 76,           /* SQL_TOKEN_GRANT  */
  YYSYMBOL_SQL_TOKEN_GROUP = 77,           /* SQL_TOKEN_GROUP  */
  YYSYMBOL_SQL_TOKEN_HAVING = 78,          /* SQL_TOKEN_HAVING  */
  YYSYMBOL_SQL_TOKEN_IN = 79,              /* SQL_TOKEN_IN  */
  YYSYMBOL_SQL_TOKEN_INDICATOR = 80,       /* SQL_TOKEN_INDICATOR  */
  YYSYMBOL_SQL_TOKEN_INNER = 81,           /* SQL_TOKEN_INNER  */
  YYSYMBOL_SQL_TOKEN_INTEGER = 82,         /* SQL_TOKEN_INTEGER  */
  YYSYMBOL_SQL_TOKEN_INTO = 83,            /* SQL_TOKEN_INTO  */
  YYSYMBOL_SQL_TOKEN_IS = 84,              /* SQL_TOKEN_IS  */
  YYSYMBOL_SQL_TOKEN_INTERSECT = 85,       /* SQL_TOKEN_INTERSECT  */
  YYSYMBOL_SQL_TOKEN_JOIN = 86,            /* SQL_TOKEN_JOIN  */
  YYSYMBOL_SQL_TOKEN_KEY = 87,             /* SQL_TOKEN_KEY  */
  YYSYMBOL_SQL_TOKEN_LEADING = 88,         /* SQL_TOKEN_LEADING  */
  YYSYMBOL_SQL_TOKEN_LIKE = 89,            /* SQL_TOKEN_LIKE  */
  YYSYMBOL_SQL_TOKEN_LOCAL = 90,           /* SQL_TOKEN_LOCAL  */
  YYSYMBOL_SQL_TOKEN_LOWER = 91,           /* SQL_TOKEN_LOWER  */
  YYSYMBOL_SQL_TOKEN_MAX = 92,             /* SQL_TOKEN_MAX  */
  YYSYMBOL_SQL_TOKEN_MIN = 93,             /* SQL_TOKEN_MIN  */
  YYSYMBOL_SQL_TOKEN_NATURAL = 94,         /* SQL_TOKEN_NATURAL  */
  YYSYMBOL_SQL_TOKEN_NCHAR = 95,           /* SQL_TOKEN_NCHAR  */
  YYSYMBOL_SQL_TOKEN_NULL = 96,            /* SQL_TOKEN_NULL  */
  YYSYMBOL_SQL_TOKEN_NUMERIC = 97,         /* SQL_TOKEN_NUMERIC  */
  YYSYMBOL_SQL_TOKEN_OCTET_LENGTH = 98,    /* SQL_TOKEN_OCTET_LENGTH  */
  YYSYMBOL_SQL_TOKEN_OF = 99,              /* SQL_TOKEN_OF  */
  YYSYMBOL_SQL_TOKEN_ON = 100,             /* SQL_TOKEN_ON  */
  YYSYMBOL_SQL_TOKEN_OPTION = 101,         /* SQL_TOKEN_OPTION  */
  YYSYMBOL_SQL_TOKEN_ORDER = 102,          /* SQL_TOKEN_ORDER  */
  YYSYMBOL_SQL_TOKEN_OUTER = 103,          /* SQL_TOKEN_OUTER  */
  YYSYMBOL_SQL_TOKEN_PRECISION = 104,      /* SQL_TOKEN_PRECISION  */
  YYSYMBOL_SQL_TOKEN_PRIMARY = 105,        /* SQL_TOKEN_PRIMARY  */
  YYSYMBOL_SQL_TOKEN_PRIVILEGES = 106,     /* SQL_TOKEN_PRIVILEGES  */
  YYSYMBOL_SQL_TOKEN_PROCEDURE = 107,      /* SQL_TOKEN_PROCEDURE  */
  YYSYMBOL_SQL_TOKEN_PUBLIC = 108,         /* SQL_TOKEN_PUBLIC  */
  YYSYMBOL_SQL_TOKEN_REAL = 109,           /* SQL_TOKEN_REAL  */
  YYSYMBOL_SQL_TOKEN_REFERENCES = 110,     /* SQL_TOKEN_REFERENCES  */
  YYSYMBOL_SQL_TOKEN_ROLLBACK = 111,       /* SQL_TOKEN_ROLLBACK  */
  YYSYMBOL_SQL_TOKEN_SCHEMA = 112,         /* SQL_TOKEN_SCHEMA  */
  YYSYMBOL_SQL_TOKEN_SELECT = 113,         /* SQL_TOKEN_SELECT  */
  YYSYMBOL_SQL_TOKEN_SET = 114,            /* SQL_TOKEN_SET  */
  YYSYMBOL_SQL_TOKEN_SIZE = 115,           /* SQL_TOKEN_SIZE  */
  YYSYMBOL_SQL_TOKEN_SMALLINT = 116,       /* SQL_TOKEN_SMALLINT  */
  YYSYMBOL_SQL_TOKEN_SOME = 117,           /* SQL_TOKEN_SOME  */
  YYSYMBOL_SQL_TOKEN_SQLCODE = 118,        /* SQL_TOKEN_SQLCODE  */
  YYSYMBOL_SQL_TOKEN_SQLERROR = 119,       /* SQL_TOKEN_SQLERROR  */
  YYSYMBOL_SQL_TOKEN_SUM = 120,            /* SQL_TOKEN_SUM  */
  YYSYMBOL_SQL_TOKEN_TABLE = 121,          /* SQL_TOKEN_TABLE  */
  YYSYMBOL_SQL_TOKEN_TIME = 122,           /* SQL_TOKEN_TIME  */
  YYSYMBOL_SQL_TOKEN_TIMESTAMP = 123,      /* SQL_TOKEN_TIMESTAMP  */
  YYSYMBOL_SQL_TOKEN_TIMEZONE_HOUR = 124,  /* SQL_TOKEN_TIMEZONE_HOUR  */
  YYSYMBOL_SQL_TOKEN_TIMEZONE_MINUTE = 125, /* SQL_TOKEN_TIMEZONE_MINUTE  */
  YYSYMBOL_SQL_TOKEN_TO = 126,             /* SQL_TOKEN_TO  */
  YYSYMBOL_SQL_TOKEN_TRAILING = 127,       /* SQL_TOKEN_TRAILING  */
  YYSYMBOL_SQL_TOKEN_TRANSLATE = 128,      /* SQL_TOKEN_TRANSLATE  */
  YYSYMBOL_SQL_TOKEN_TRIM = 129,           /* SQL_TOKEN_TRIM  */
  YYSYMBOL_SQL_TOKEN_TRUE = 130,           /* SQL_TOKEN_TRUE  */
  YYSYMBOL_SQL_TOKEN_UNION = 131,          /* SQL_TOKEN_UNION  */
  YYSYMBOL_SQL_TOKEN_UNIQUE = 132,         /* SQL_TOKEN_UNIQUE  */
  YYSYMBOL_SQL_TOKEN_UNKNOWN = 133,        /* SQL_TOKEN_UNKNOWN  */
  YYSYMBOL_SQL_TOKEN_UPDATE = 134,         /* SQL_TOKEN_UPDATE  */
  YYSYMBOL_SQL_TOKEN_UPPER = 135,          /* SQL_TOKEN_UPPER  */
  YYSYMBOL_SQL_TOKEN_USAGE = 136,          /* SQL_TOKEN_USAGE  */
  YYSYMBOL_SQL_TOKEN_USER = 137,           /* SQL_TOKEN_USER  */
  YYSYMBOL_SQL_TOKEN_USING = 138,          /* SQL_TOKEN_USING  */
  YYSYMBOL_SQL_TOKEN_VALUES = 139,         /* SQL_TOKEN_VALUES  */
  YYSYMBOL_SQL_TOKEN_VIEW = 140,           /* SQL_TOKEN_VIEW  */
  YYSYMBOL_SQL_TOKEN_WHERE = 141,          /* SQL_TOKEN_WHERE  */
  YYSYMBOL_SQL_TOKEN_WITH = 142,           /* SQL_TOKEN_WITH  */
  YYSYMBOL_SQL_TOKEN_WORK = 143,           /* SQL_TOKEN_WORK  */
  YYSYMBOL_SQL_TOKEN_ZONE = 144,           /* SQL_TOKEN_ZONE  */
  YYSYMBOL_SQL_TOKEN_CALL = 145,           /* SQL_TOKEN_CALL  */
  YYSYMBOL_SQL_TOKEN_D = 146,              /* SQL_TOKEN_D  */
  YYSYMBOL_SQL_TOKEN_FN = 147,             /* SQL_TOKEN_FN  */
  YYSYMBOL_SQL_TOKEN_T = 148,              /* SQL_TOKEN_T  */
  YYSYMBOL_SQL_TOKEN_TS = 149,             /* SQL_TOKEN_TS  */
  YYSYMBOL_SQL_TOKEN_OJ = 150,             /* SQL_TOKEN_OJ  */
  YYSYMBOL_SQL_TOKEN_ASCII = 151,          /* SQL_TOKEN_ASCII  */
  YYSYMBOL_SQL_TOKEN_BIT_LENGTH = 152,     /* SQL_TOKEN_BIT_LENGTH  */
  YYSYMBOL_SQL_TOKEN_CHAR = 153,           /* SQL_TOKEN_CHAR  */
  YYSYMBOL_SQL_TOKEN_CHAR_LENGTH = 154,    /* SQL_TOKEN_CHAR_LENGTH  */
  YYSYMBOL_SQL_TOKEN_SQL_TOKEN_INTNUM = 155, /* SQL_TOKEN_SQL_TOKEN_INTNUM  */
  YYSYMBOL_SQL_TOKEN_CONCAT = 156,         /* SQL_TOKEN_CONCAT  */
  YYSYMBOL_SQL_TOKEN_DIFFERENCE = 157,     /* SQL_TOKEN_DIFFERENCE  */
  YYSYMBOL_SQL_TOKEN_INSERT = 158,         /* SQL_TOKEN_INSERT  */
  YYSYMBOL_SQL_TOKEN_LCASE = 159,          /* SQL_TOKEN_LCASE  */
  YYSYMBOL_SQL_TOKEN_LEFT = 160,           /* SQL_TOKEN_LEFT  */
  YYSYMBOL_SQL_TOKEN_LENGTH = 161,         /* SQL_TOKEN_LENGTH  */
  YYSYMBOL_SQL_TOKEN_LOCATE = 162,         /* SQL_TOKEN_LOCATE  */
  YYSYMBOL_SQL_TOKEN_LOCATE_2 = 163,       /* SQL_TOKEN_LOCATE_2  */
  YYSYMBOL_SQL_TOKEN_LTRIM = 164,          /* SQL_TOKEN_LTRIM  */
  YYSYMBOL_SQL_TOKEN_POSITION = 165,       /* SQL_TOKEN_POSITION  */
  YYSYMBOL_SQL_TOKEN_REPEAT = 166,         /* SQL_TOKEN_REPEAT  */
  YYSYMBOL_SQL_TOKEN_REPLACE = 167,        /* SQL_TOKEN_REPLACE  */
  YYSYMBOL_SQL_TOKEN_RIGHT = 168,          /* SQL_TOKEN_RIGHT  */
  YYSYMBOL_SQL_TOKEN_RTRIM = 169,          /* SQL_TOKEN_RTRIM  */
  YYSYMBOL_SQL_TOKEN_SOUNDEX = 170,        /* SQL_TOKEN_SOUNDEX  */
  YYSYMBOL_SQL_TOKEN_SPACE = 171,          /* SQL_TOKEN_SPACE  */
  YYSYMBOL_SQL_TOKEN_SUBSTRING = 172,      /* SQL_TOKEN_SUBSTRING  */
  YYSYMBOL_SQL_TOKEN_UCASE = 173,          /* SQL_TOKEN_UCASE  */
  YYSYMBOL_SQL_TOKEN_CURRENT_DATE = 174,   /* SQL_TOKEN_CURRENT_DATE  */
  YYSYMBOL_SQL_TOKEN_CURRENT_TIME = 175,   /* SQL_TOKEN_CURRENT_TIME  */
  YYSYMBOL_SQL_TOKEN_CURRENT_TIMESTAMP = 176, /* SQL_TOKEN_CURRENT_TIMESTAMP  */
  YYSYMBOL_SQL_TOKEN_CURDATE = 177,        /* SQL_TOKEN_CURDATE  */
  YYSYMBOL_SQL_TOKEN_CURTIME = 178,        /* SQL_TOKEN_CURTIME  */
  YYSYMBOL_SQL_TOKEN_DAYNAME = 179,        /* SQL_TOKEN_DAYNAME  */
  YYSYMBOL_SQL_TOKEN_DAYOFMONTH = 180,     /* SQL_TOKEN_DAYOFMONTH  */
  YYSYMBOL_SQL_TOKEN_DAYOFWEEK = 181,      /* SQL_TOKEN_DAYOFWEEK  */
  YYSYMBOL_SQL_TOKEN_DAYOFYEAR = 182,      /* SQL_TOKEN_DAYOFYEAR  */
  YYSYMBOL_SQL_TOKEN_EXTRACT = 183,        /* SQL_TOKEN_EXTRACT  */
  YYSYMBOL_SQL_TOKEN_HOUR = 184,           /* SQL_TOKEN_HOUR  */
  YYSYMBOL_SQL_TOKEN_MINUTE = 185,         /* SQL_TOKEN_MINUTE  */
  YYSYMBOL_SQL_TOKEN_MONTH = 186,          /* SQL_TOKEN_MONTH  */
  YYSYMBOL_SQL_TOKEN_MONTHNAME = 187,      /* SQL_TOKEN_MONTHNAME  */
  YYSYMBOL_SQL_TOKEN_NOW = 188,            /* SQL_TOKEN_NOW  */
  YYSYMBOL_SQL_TOKEN_QUARTER = 189,        /* SQL_TOKEN_QUARTER  */
  YYSYMBOL_SQL_TOKEN_DATEDIFF = 190,       /* SQL_TOKEN_DATEDIFF  */
  YYSYMBOL_SQL_TOKEN_SECOND = 191,         /* SQL_TOKEN_SECOND  */
  YYSYMBOL_SQL_TOKEN_TIMESTAMPADD = 192,   /* SQL_TOKEN_TIMESTAMPADD  */
  YYSYMBOL_SQL_TOKEN_TIMESTAMPDIFF = 193,  /* SQL_TOKEN_TIMESTAMPDIFF  */
  YYSYMBOL_SQL_TOKEN_TIMEVALUE = 194,      /* SQL_TOKEN_TIMEVALUE  */
  YYSYMBOL_SQL_TOKEN_WEEK = 195,           /* SQL_TOKEN_WEEK  */
  YYSYMBOL_SQL_TOKEN_YEAR = 196,           /* SQL_TOKEN_YEAR  */
  YYSYMBOL_SQL_TOKEN_ABS = 197,            /* SQL_TOKEN_ABS  */
  YYSYMBOL_SQL_TOKEN_ACOS = 198,           /* SQL_TOKEN_ACOS  */
  YYSYMBOL_SQL_TOKEN_ASIN = 199,           /* SQL_TOKEN_ASIN  */
  YYSYMBOL_SQL_TOKEN_ATAN = 200,           /* SQL_TOKEN_ATAN  */
  YYSYMBOL_SQL_TOKEN_ATAN2 = 201,          /* SQL_TOKEN_ATAN2  */
  YYSYMBOL_SQL_TOKEN_CEILING = 202,        /* SQL_TOKEN_CEILING  */
  YYSYMBOL_SQL_TOKEN_COS = 203,            /* SQL_TOKEN_COS  */
  YYSYMBOL_SQL_TOKEN_COT = 204,            /* SQL_TOKEN_COT  */
  YYSYMBOL_SQL_TOKEN_DEGREES = 205,        /* SQL_TOKEN_DEGREES  */
  YYSYMBOL_SQL_TOKEN_EXP = 206,            /* SQL_TOKEN_EXP  */
  YYSYMBOL_SQL_TOKEN_FLOOR = 207,          /* SQL_TOKEN_FLOOR  */
  YYSYMBOL_SQL_TOKEN_LOGF = 208,           /* SQL_TOKEN_LOGF  */
  YYSYMBOL_SQL_TOKEN_LOG = 209,            /* SQL_TOKEN_LOG  */
  YYSYMBOL_SQL_TOKEN_LN = 210,             /* SQL_TOKEN_LN  */
  YYSYMBOL_SQL_TOKEN_LOG10 = 211,          /* SQL_TOKEN_LOG10  */
  YYSYMBOL_SQL_TOKEN_MOD = 212,            /* SQL_TOKEN_MOD  */
  YYSYMBOL_SQL_TOKEN_PI = 213,             /* SQL_TOKEN_PI  */
  YYSYMBOL_SQL_TOKEN_POWER = 214,          /* SQL_TOKEN_POWER  */
  YYSYMBOL_SQL_TOKEN_RADIANS = 215,        /* SQL_TOKEN_RADIANS  */
  YYSYMBOL_SQL_TOKEN_RAND = 216,           /* SQL_TOKEN_RAND  */
  YYSYMBOL_SQL_TOKEN_ROUNDMAGIC = 217,     /* SQL_TOKEN_ROUNDMAGIC  */
  YYSYMBOL_SQL_TOKEN_ROUND = 218,          /* SQL_TOKEN_ROUND  */
  YYSYMBOL_SQL_TOKEN_SIGN = 219,           /* SQL_TOKEN_SIGN  */
  YYSYMBOL_SQL_TOKEN_SIN = 220,            /* SQL_TOKEN_SIN  */
  YYSYMBOL_SQL_TOKEN_SQRT = 221,           /* SQL_TOKEN_SQRT  */
  YYSYMBOL_SQL_TOKEN_TAN = 222,            /* SQL_TOKEN_TAN  */
  YYSYMBOL_SQL_TOKEN_TRUNCATE = 223,       /* SQL_TOKEN_TRUNCATE  */
  YYSYMBOL_SQL_TOKEN_EVERY = 224,          /* SQL_TOKEN_EVERY  */
  YYSYMBOL_SQL_TOKEN_INTERSECTION = 225,   /* SQL_TOKEN_INTERSECTION  */
  YYSYMBOL_SQL_TOKEN_FUSION = 226,         /* SQL_TOKEN_FUSION  */
  YYSYMBOL_SQL_TOKEN_COLLECT = 227,        /* SQL_TOKEN_COLLECT  */
  YYSYMBOL_SQL_TOKEN_VAR_POP = 228,        /* SQL_TOKEN_VAR_POP  */
  YYSYMBOL_SQL_TOKEN_VAR_SAMP = 229,       /* SQL_TOKEN_VAR_SAMP  */
  YYSYMBOL_SQL_TOKEN_STDDEV_SAMP = 230,    /* SQL_TOKEN_STDDEV_SAMP  */
  YYSYMBOL_SQL_TOKEN_STDDEV_POP = 231,     /* SQL_TOKEN_STDDEV_POP  */
  YYSYMBOL_SQL_TOKEN_RANK = 232,           /* SQL_TOKEN_RANK  */
  YYSYMBOL_SQL_TOKEN_DENSE_RANK = 233,     /* SQL_TOKEN_DENSE_RANK  */
  YYSYMBOL_SQL_TOKEN_PERCENT_RANK = 234,   /* SQL_TOKEN_PERCENT_RANK  */
  YYSYMBOL_SQL_TOKEN_CUME_DIST = 235,      /* SQL_TOKEN_CUME_DIST  */
  YYSYMBOL_SQL_TOKEN_PERCENTILE_CONT = 236, /* SQL_TOKEN_PERCENTILE_CONT  */
  YYSYMBOL_SQL_TOKEN_PERCENTILE_DISC = 237, /* SQL_TOKEN_PERCENTILE_DISC  */
  YYSYMBOL_SQL_TOKEN_WITHIN = 238,         /* SQL_TOKEN_WITHIN  */
  YYSYMBOL_SQL_TOKEN_ARRAY_AGG = 239,      /* SQL_TOKEN_ARRAY_AGG  */
  YYSYMBOL_SQL_TOKEN_CASE = 240,           /* SQL_TOKEN_CASE  */
  YYSYMBOL_SQL_TOKEN_THEN = 241,           /* SQL_TOKEN_THEN  */
  YYSYMBOL_SQL_TOKEN_END = 242,            /* SQL_TOKEN_END  */
  YYSYMBOL_SQL_TOKEN_NULLIF = 243,         /* SQL_TOKEN_NULLIF  */
  YYSYMBOL_SQL_TOKEN_COALESCE = 244,       /* SQL_TOKEN_COALESCE  */
  YYSYMBOL_SQL_TOKEN_WHEN = 245,           /* SQL_TOKEN_WHEN  */
  YYSYMBOL_SQL_TOKEN_ELSE = 246,           /* SQL_TOKEN_ELSE  */
  YYSYMBOL_SQL_TOKEN_BEFORE = 247,         /* SQL_TOKEN_BEFORE  */
  YYSYMBOL_SQL_TOKEN_AFTER = 248,          /* SQL_TOKEN_AFTER  */
  YYSYMBOL_SQL_TOKEN_INSTEAD = 249,        /* SQL_TOKEN_INSTEAD  */
  YYSYMBOL_SQL_TOKEN_EACH = 250,           /* SQL_TOKEN_EACH  */
  YYSYMBOL_SQL_TOKEN_REFERENCING = 251,    /* SQL_TOKEN_REFERENCING  */
  YYSYMBOL_SQL_TOKEN_BEGIN = 252,          /* SQL_TOKEN_BEGIN  */
  YYSYMBOL_SQL_TOKEN_ATOMIC = 253,         /* SQL_TOKEN_ATOMIC  */
  YYSYMBOL_SQL_TOKEN_TRIGGER = 254,        /* SQL_TOKEN_TRIGGER  */
  YYSYMBOL_SQL_TOKEN_ROW = 255,            /* SQL_TOKEN_ROW  */
  YYSYMBOL_SQL_TOKEN_STATEMENT = 256,      /* SQL_TOKEN_STATEMENT  */
  YYSYMBOL_SQL_TOKEN_NEW = 257,            /* SQL_TOKEN_NEW  */
  YYSYMBOL_SQL_TOKEN_OLD = 258,            /* SQL_TOKEN_OLD  */
  YYSYMBOL_SQL_TOKEN_VALUE = 259,          /* SQL_TOKEN_VALUE  */
  YYSYMBOL_SQL_TOKEN_CURRENT_CATALOG = 260, /* SQL_TOKEN_CURRENT_CATALOG  */
  YYSYMBOL_SQL_TOKEN_CURRENT_DEFAULT_TRANSFORM_GROUP = 261, /* SQL_TOKEN_CURRENT_DEFAULT_TRANSFORM_GROUP  */
  YYSYMBOL_SQL_TOKEN_CURRENT_PATH = 262,   /* SQL_TOKEN_CURRENT_PATH  */
  YYSYMBOL_SQL_TOKEN_CURRENT_ROLE = 263,   /* SQL_TOKEN_CURRENT_ROLE  */
  YYSYMBOL_SQL_TOKEN_CURRENT_SCHEMA = 264, /* SQL_TOKEN_CURRENT_SCHEMA  */
  YYSYMBOL_SQL_TOKEN_CURRENT_USER = 265,   /* SQL_TOKEN_CURRENT_USER  */
  YYSYMBOL_SQL_TOKEN_SESSION_USER = 266,   /* SQL_TOKEN_SESSION_USER  */
  YYSYMBOL_SQL_TOKEN_SYSTEM_USER = 267,    /* SQL_TOKEN_SYSTEM_USER  */
  YYSYMBOL_SQL_TOKEN_VARCHAR = 268,        /* SQL_TOKEN_VARCHAR  */
  YYSYMBOL_SQL_TOKEN_VARBINARY = 269,      /* SQL_TOKEN_VARBINARY  */
  YYSYMBOL_SQL_TOKEN_VARYING = 270,        /* SQL_TOKEN_VARYING  */
  YYSYMBOL_SQL_TOKEN_OBJECT = 271,         /* SQL_TOKEN_OBJECT  */
  YYSYMBOL_SQL_TOKEN_NCLOB = 272,          /* SQL_TOKEN_NCLOB  */
  YYSYMBOL_SQL_TOKEN_NATIONAL = 273,       /* SQL_TOKEN_NATIONAL  */
  YYSYMBOL_SQL_TOKEN_LARGE = 274,          /* SQL_TOKEN_LARGE  */
  YYSYMBOL_SQL_TOKEN_CLOB = 275,           /* SQL_TOKEN_CLOB  */
  YYSYMBOL_SQL_TOKEN_BLOB = 276,           /* SQL_TOKEN_BLOB  */
  YYSYMBOL_SQL_TOKEN_BIGINT = 277,         /* SQL_TOKEN_BIGINT  */
  YYSYMBOL_SQL_TOKEN_BINARY = 278,         /* SQL_TOKEN_BINARY  */
  YYSYMBOL_SQL_TOKEN_WITHOUT = 279,        /* SQL_TOKEN_WITHOUT  */
  YYSYMBOL_SQL_TOKEN_BOOLEAN = 280,        /* SQL_TOKEN_BOOLEAN  */
  YYSYMBOL_SQL_TOKEN_INTERVAL = 281,       /* SQL_TOKEN_INTERVAL  */
  YYSYMBOL_SQL_TOKEN_OVER = 282,           /* SQL_TOKEN_OVER  */
  YYSYMBOL_SQL_TOKEN_ROW_NUMBER = 283,     /* SQL_TOKEN_ROW_NUMBER  */
  YYSYMBOL_SQL_TOKEN_NTILE = 284,          /* SQL_TOKEN_NTILE  */
  YYSYMBOL_SQL_TOKEN_LEAD = 285,           /* SQL_TOKEN_LEAD  */
  YYSYMBOL_SQL_TOKEN_LAG = 286,            /* SQL_TOKEN_LAG  */
  YYSYMBOL_SQL_TOKEN_RESPECT = 287,        /* SQL_TOKEN_RESPECT  */
  YYSYMBOL_SQL_TOKEN_IGNORE = 288,         /* SQL_TOKEN_IGNORE  */
  YYSYMBOL_SQL_TOKEN_NULLS = 289,          /* SQL_TOKEN_NULLS  */
  YYSYMBOL_SQL_TOKEN_FIRST_VALUE = 290,    /* SQL_TOKEN_FIRST_VALUE  */
  YYSYMBOL_SQL_TOKEN_LAST_VALUE = 291,     /* SQL_TOKEN_LAST_VALUE  */
  YYSYMBOL_SQL_TOKEN_NTH_VALUE = 292,      /* SQL_TOKEN_NTH_VALUE  */
  YYSYMBOL_SQL_TOKEN_FIRST = 293,          /* SQL_TOKEN_FIRST  */
  YYSYMBOL_SQL_TOKEN_LAST = 294,           /* SQL_TOKEN_LAST  */
  YYSYMBOL_SQL_TOKEN_EXCLUDE = 295,        /* SQL_TOKEN_EXCLUDE  */
  YYSYMBOL_SQL_TOKEN_OTHERS = 296,         /* SQL_TOKEN_OTHERS  */
  YYSYMBOL_SQL_TOKEN_TIES = 297,           /* SQL_TOKEN_TIES  */
  YYSYMBOL_SQL_TOKEN_FOLLOWING = 298,      /* SQL_TOKEN_FOLLOWING  */
  YYSYMBOL_SQL_TOKEN_UNBOUNDED = 299,      /* SQL_TOKEN_UNBOUNDED  */
  YYSYMBOL_SQL_TOKEN_PRECEDING = 300,      /* SQL_TOKEN_PRECEDING  */
  YYSYMBOL_SQL_TOKEN_RANGE = 301,          /* SQL_TOKEN_RANGE  */
  YYSYMBOL_SQL_TOKEN_ROWS = 302,           /* SQL_TOKEN_ROWS  */
  YYSYMBOL_SQL_TOKEN_PARTITION = 303,      /* SQL_TOKEN_PARTITION  */
  YYSYMBOL_SQL_TOKEN_WINDOW = 304,         /* SQL_TOKEN_WINDOW  */
  YYSYMBOL_SQL_TOKEN_NO = 305,             /* SQL_TOKEN_NO  */
  YYSYMBOL_SQL_TOKEN_LIMIT = 306,          /* SQL_TOKEN_LIMIT  */
  YYSYMBOL_SQL_TOKEN_OFFSET = 307,         /* SQL_TOKEN_OFFSET  */
  YYSYMBOL_SQL_TOKEN_NEXT = 308,           /* SQL_TOKEN_NEXT  */
  YYSYMBOL_SQL_TOKEN_ONLY = 309,           /* SQL_TOKEN_ONLY  */
  YYSYMBOL_SQL_TOKEN_OR = 310,             /* SQL_TOKEN_OR  */
  YYSYMBOL_SQL_TOKEN_AND = 311,            /* SQL_TOKEN_AND  */
  YYSYMBOL_SQL_LESSEQ = 312,               /* SQL_LESSEQ  */
  YYSYMBOL_SQL_GREATEQ = 313,              /* SQL_GREATEQ  */
  YYSYMBOL_SQL_NOTEQUAL = 314,             /* SQL_NOTEQUAL  */
  YYSYMBOL_SQL_LESS = 315,                 /* SQL_LESS  */
  YYSYMBOL_SQL_GREAT = 316,                /* SQL_GREAT  */
  YYSYMBOL_SQL_EQUAL = 317,                /* SQL_EQUAL  */
  YYSYMBOL_318_ = 318,                     /* '+'  */
  YYSYMBOL_319_ = 319,                     /* '-'  */
  YYSYMBOL_SQL_CONCAT = 320,               /* SQL_CONCAT  */
  YYSYMBOL_321_ = 321,                     /* '*'  */
  YYSYMBOL_322_ = 322,                     /* '/'  */
  YYSYMBOL_323_ = 323,                     /* '='  */
  YYSYMBOL_SQL_TOKEN_INVALIDSYMBOL = 324,  /* SQL_TOKEN_INVALIDSYMBOL  */
  YYSYMBOL_YYACCEPT = 325,                 /* $accept  */
  YYSYMBOL_sql_single_statement = 326,     /* sql_single_statement  */
  YYSYMBOL_sql = 327,                      /* sql  */
  YYSYMBOL_schema_element = 328,           /* schema_element  */
  YYSYMBOL_base_table_def = 329,           /* base_table_def  */
  YYSYMBOL_base_table_element_commalist = 330, /* base_table_element_commalist  */
  YYSYMBOL_base_table_element = 331,       /* base_table_element  */
  YYSYMBOL_column_def = 332,               /* column_def  */
  YYSYMBOL_column_def_opt_list = 333,      /* column_def_opt_list  */
  YYSYMBOL_nil_fkt = 334,                  /* nil_fkt  */
  YYSYMBOL_unique_spec = 335,              /* unique_spec  */
  YYSYMBOL_column_def_opt = 336,           /* column_def_opt  */
  YYSYMBOL_table_constraint_def = 337,     /* table_constraint_def  */
  YYSYMBOL_op_column_commalist = 338,      /* op_column_commalist  */
  YYSYMBOL_column_commalist = 339,         /* column_commalist  */
  YYSYMBOL_view_def = 340,                 /* view_def  */
  YYSYMBOL_opt_with_check_option = 341,    /* opt_with_check_option  */
  YYSYMBOL_opt_column_commalist = 342,     /* opt_column_commalist  */
  YYSYMBOL_privilege_def = 343,            /* privilege_def  */
  YYSYMBOL_opt_with_grant_option = 344,    /* opt_with_grant_option  */
  YYSYMBOL_privileges = 345,               /* privileges  */
  YYSYMBOL_operation_commalist = 346,      /* operation_commalist  */
  YYSYMBOL_operation = 347,                /* operation  */
  YYSYMBOL_grantee_commalist = 348,        /* grantee_commalist  */
  YYSYMBOL_grantee = 349,                  /* grantee  */
  YYSYMBOL_opt_order_by_clause = 350,      /* opt_order_by_clause  */
  YYSYMBOL_ordering_spec_commalist = 351,  /* ordering_spec_commalist  */
  YYSYMBOL_ordering_spec = 352,            /* ordering_spec  */
  YYSYMBOL_opt_asc_desc = 353,             /* opt_asc_desc  */
  YYSYMBOL_sql_not = 354,                  /* sql_not  */
  YYSYMBOL_manipulative_statement = 355,   /* manipulative_statement  */
  YYSYMBOL_union_statement = 356,          /* union_statement  */
  YYSYMBOL_commit_statement = 357,         /* commit_statement  */
  YYSYMBOL_delete_statement_searched = 358, /* delete_statement_searched  */
  YYSYMBOL_fetch_statement = 359,          /* fetch_statement  */
  YYSYMBOL_insert_statement = 360,         /* insert_statement  */
  YYSYMBOL_values_or_query_spec = 361,     /* values_or_query_spec  */
  YYSYMBOL_table_value_const_list = 362,   /* table_value_const_list  */
  YYSYMBOL_row_value_constructor = 363,    /* row_value_constructor  */
  YYSYMBOL_row_value_constructor_elem = 364, /* row_value_constructor_elem  */
  YYSYMBOL_rollback_statement = 365,       /* rollback_statement  */
  YYSYMBOL_select_statement_into = 366,    /* select_statement_into  */
  YYSYMBOL_opt_all_distinct = 367,         /* opt_all_distinct  */
  YYSYMBOL_assignment_commalist = 368,     /* assignment_commalist  */
  YYSYMBOL_assignment = 369,               /* assignment  */
  YYSYMBOL_update_source = 370,            /* update_source  */
  YYSYMBOL_update_statement_searched = 371, /* update_statement_searched  */
  YYSYMBOL_target_commalist = 372,         /* target_commalist  */
  YYSYMBOL_target = 373,                   /* target  */
  YYSYMBOL_opt_where_clause = 374,         /* opt_where_clause  */
  YYSYMBOL_query_term = 375,               /* query_term  */
  YYSYMBOL_select_statement = 376,         /* select_statement  */
  YYSYMBOL_selection = 377,                /* selection  */
  YYSYMBOL_opt_result_offset_clause = 378, /* opt_result_offset_clause  */
  YYSYMBOL_result_offset_clause = 379,     /* result_offset_clause  */
  YYSYMBOL_opt_fetch_first_row_count = 380, /* opt_fetch_first_row_count  */
  YYSYMBOL_first_or_next = 381,            /* first_or_next  */
  YYSYMBOL_row_or_rows = 382,              /* row_or_rows  */
  YYSYMBOL_opt_fetch_first_clause = 383,   /* opt_fetch_first_clause  */
  YYSYMBOL_fetch_first_clause = 384,       /* fetch_first_clause  */
  YYSYMBOL_offset_row_count = 385,         /* offset_row_count  */
  YYSYMBOL_fetch_first_row_count = 386,    /* fetch_first_row_count  */
  YYSYMBOL_opt_limit_offset_clause = 387,  /* opt_limit_offset_clause  */
  YYSYMBOL_opt_offset = 388,               /* opt_offset  */
  YYSYMBOL_limit_offset_clause = 389,      /* limit_offset_clause  */
  YYSYMBOL_table_exp = 390,                /* table_exp  */
  YYSYMBOL_from_clause = 391,              /* from_clause  */
  YYSYMBOL_table_ref_commalist = 392,      /* table_ref_commalist  */
  YYSYMBOL_opt_as = 393,                   /* opt_as  */
  YYSYMBOL_opt_row = 394,                  /* opt_row  */
  YYSYMBOL_table_primary_as_range_column = 395, /* table_primary_as_range_column  */
  YYSYMBOL_table_ref = 396,                /* table_ref  */
  YYSYMBOL_where_clause = 397,             /* where_clause  */
  YYSYMBOL_opt_group_by_clause = 398,      /* opt_group_by_clause  */
  YYSYMBOL_column_ref_commalist = 399,     /* column_ref_commalist  */
  YYSYMBOL_opt_having_clause = 400,        /* opt_having_clause  */
  YYSYMBOL_truth_value = 401,              /* truth_value  */
  YYSYMBOL_boolean_primary = 402,          /* boolean_primary  */
  YYSYMBOL_parenthesized_boolean_value_expression = 403, /* parenthesized_boolean_value_expression  */
  YYSYMBOL_boolean_test = 404,             /* boolean_test  */
  YYSYMBOL_boolean_factor = 405,           /* boolean_factor  */
  YYSYMBOL_boolean_term = 406,             /* boolean_term  */
  YYSYMBOL_search_condition = 407,         /* search_condition  */
  YYSYMBOL_predicate = 408,                /* predicate  */
  YYSYMBOL_comparison_predicate_part_2 = 409, /* comparison_predicate_part_2  */
  YYSYMBOL_comparison_predicate = 410,     /* comparison_predicate  */
  YYSYMBOL_comparison = 411,               /* comparison  */
  YYSYMBOL_between_predicate_part_2 = 412, /* between_predicate_part_2  */
  YYSYMBOL_between_predicate = 413,        /* between_predicate  */
  YYSYMBOL_character_like_predicate_part_2 = 414, /* character_like_predicate_part_2  */
  YYSYMBOL_other_like_predicate_part_2 = 415, /* other_like_predicate_part_2  */
  YYSYMBOL_like_predicate = 416,           /* like_predicate  */
  YYSYMBOL_opt_escape = 417,               /* opt_escape  */
  YYSYMBOL_null_predicate_part_2 = 418,    /* null_predicate_part_2  */
  YYSYMBOL_test_for_null = 419,            /* test_for_null  */
  YYSYMBOL_in_predicate_value = 420,       /* in_predicate_value  */
  YYSYMBOL_in_predicate_part_2 = 421,      /* in_predicate_part_2  */
  YYSYMBOL_in_predicate = 422,             /* in_predicate  */
  YYSYMBOL_quantified_comparison_predicate_part_2 = 423, /* quantified_comparison_predicate_part_2  */
  YYSYMBOL_all_or_any_predicate = 424,     /* all_or_any_predicate  */
  YYSYMBOL_any_all_some = 425,             /* any_all_some  */
  YYSYMBOL_existence_test = 426,           /* existence_test  */
  YYSYMBOL_unique_test = 427,              /* unique_test  */
  YYSYMBOL_subquery = 428,                 /* subquery  */
  YYSYMBOL_scalar_exp_commalist = 429,     /* scalar_exp_commalist  */
  YYSYMBOL_select_sublist = 430,           /* select_sublist  */
  YYSYMBOL_parameter_ref = 431,            /* parameter_ref  */
  YYSYMBOL_literal = 432,                  /* literal  */
  YYSYMBOL_as_clause = 433,                /* as_clause  */
  YYSYMBOL_position_exp = 434,             /* position_exp  */
  YYSYMBOL_num_value_fct = 435,            /* num_value_fct  */
  YYSYMBOL_char_length_exp = 436,          /* char_length_exp  */
  YYSYMBOL_octet_length_exp = 437,         /* octet_length_exp  */
  YYSYMBOL_bit_length_exp = 438,           /* bit_length_exp  */
  YYSYMBOL_length_exp = 439,               /* length_exp  */
  YYSYMBOL_datetime_field = 440,           /* datetime_field  */
  YYSYMBOL_extract_field = 441,            /* extract_field  */
  YYSYMBOL_time_zone_field = 442,          /* time_zone_field  */
  YYSYMBOL_extract_exp = 443,              /* extract_exp  */
  YYSYMBOL_unsigned_value_spec = 444,      /* unsigned_value_spec  */
  YYSYMBOL_general_value_spec = 445,       /* general_value_spec  */
  YYSYMBOL_set_fct_spec = 446,             /* set_fct_spec  */
  YYSYMBOL_function_name0 = 447,           /* function_name0  */
  YYSYMBOL_function_name1 = 448,           /* function_name1  */
  YYSYMBOL_function_name2 = 449,           /* function_name2  */
  YYSYMBOL_function_name12 = 450,          /* function_name12  */
  YYSYMBOL_function_name23 = 451,          /* function_name23  */
  YYSYMBOL_function_name3 = 452,           /* function_name3  */
  YYSYMBOL_function_name = 453,            /* function_name  */
  YYSYMBOL_string_function_1Argument = 454, /* string_function_1Argument  */
  YYSYMBOL_string_function_2Argument = 455, /* string_function_2Argument  */
  YYSYMBOL_string_function_3Argument = 456, /* string_function_3Argument  */
  YYSYMBOL_string_function_4Argument = 457, /* string_function_4Argument  */
  YYSYMBOL_string_function = 458,          /* string_function  */
  YYSYMBOL_date_function_0Argument = 459,  /* date_function_0Argument  */
  YYSYMBOL_date_function_1Argument = 460,  /* date_function_1Argument  */
  YYSYMBOL_date_function = 461,            /* date_function  */
  YYSYMBOL_numeric_function_0Argument = 462, /* numeric_function_0Argument  */
  YYSYMBOL_numeric_function_1Argument = 463, /* numeric_function_1Argument  */
  YYSYMBOL_numeric_function_2Argument = 464, /* numeric_function_2Argument  */
  YYSYMBOL_numeric_function = 465,         /* numeric_function  */
  YYSYMBOL_window_function = 466,          /* window_function  */
  YYSYMBOL_window_function_type = 467,     /* window_function_type  */
  YYSYMBOL_ntile_function = 468,           /* ntile_function  */
  YYSYMBOL_dynamic_parameter_specification = 469, /* dynamic_parameter_specification  */
  YYSYMBOL_simple_value_specification = 470, /* simple_value_specification  */
  YYSYMBOL_number_of_tiles = 471,          /* number_of_tiles  */
  YYSYMBOL_opt_lead_or_lag_function = 472, /* opt_lead_or_lag_function  */
  YYSYMBOL_opt_null_treatment = 473,       /* opt_null_treatment  */
  YYSYMBOL_lead_or_lag_function = 474,     /* lead_or_lag_function  */
  YYSYMBOL_lead_or_lag = 475,              /* lead_or_lag  */
  YYSYMBOL_lead_or_lag_extent = 476,       /* lead_or_lag_extent  */
  YYSYMBOL_offset = 477,                   /* offset  */
  YYSYMBOL_default_expression = 478,       /* default_expression  */
  YYSYMBOL_null_treatment = 479,           /* null_treatment  */
  YYSYMBOL_first_or_last_value_function = 480, /* first_or_last_value_function  */
  YYSYMBOL_first_or_last_value = 481,      /* first_or_last_value  */
  YYSYMBOL_opt_from_first_or_last = 482,   /* opt_from_first_or_last  */
  YYSYMBOL_nth_value_function = 483,       /* nth_value_function  */
  YYSYMBOL_nth_row = 484,                  /* nth_row  */
  YYSYMBOL_from_first_or_last = 485,       /* from_first_or_last  */
  YYSYMBOL_window_name = 486,              /* window_name  */
  YYSYMBOL_window_name_or_specification = 487, /* window_name_or_specification  */
  YYSYMBOL_in_line_window_specification = 488, /* in_line_window_specification  */
  YYSYMBOL_opt_window_clause = 489,        /* opt_window_clause  */
  YYSYMBOL_window_clause = 490,            /* window_clause  */
  YYSYMBOL_window_definition_list = 491,   /* window_definition_list  */
  YYSYMBOL_window_definition = 492,        /* window_definition  */
  YYSYMBOL_new_window_name = 493,          /* new_window_name  */
  YYSYMBOL_window_specification = 494,     /* window_specification  */
  YYSYMBOL_opt_existing_window_name = 495, /* opt_existing_window_name  */
  YYSYMBOL_opt_window_partition_clause = 496, /* opt_window_partition_clause  */
  YYSYMBOL_opt_window_frame_clause = 497,  /* opt_window_frame_clause  */
  YYSYMBOL_window_specification_details = 498, /* window_specification_details  */
  YYSYMBOL_existing_window_name = 499,     /* existing_window_name  */
  YYSYMBOL_window_partition_clause = 500,  /* window_partition_clause  */
  YYSYMBOL_window_partition_column_reference_list = 501, /* window_partition_column_reference_list  */
  YYSYMBOL_window_partition_column_reference = 502, /* window_partition_column_reference  */
  YYSYMBOL_opt_window_frame_exclusion = 503, /* opt_window_frame_exclusion  */
  YYSYMBOL_window_frame_clause = 504,      /* window_frame_clause  */
  YYSYMBOL_window_frame_units = 505,       /* window_frame_units  */
  YYSYMBOL_window_frame_extent = 506,      /* window_frame_extent  */
  YYSYMBOL_window_frame_start = 507,       /* window_frame_start  */
  YYSYMBOL_window_frame_preceding = 508,   /* window_frame_preceding  */
  YYSYMBOL_window_frame_between = 509,     /* window_frame_between  */
  YYSYMBOL_window_frame_bound_1 = 510,     /* window_frame_bound_1  */
  YYSYMBOL_window_frame_bound_2 = 511,     /* window_frame_bound_2  */
  YYSYMBOL_window_frame_bound = 512,       /* window_frame_bound  */
  YYSYMBOL_window_frame_following = 513,   /* window_frame_following  */
  YYSYMBOL_window_frame_exclusion = 514,   /* window_frame_exclusion  */
  YYSYMBOL_op_parameter = 515,             /* op_parameter  */
  YYSYMBOL_odbc_call_spec = 516,           /* odbc_call_spec  */
  YYSYMBOL_op_odbc_call_parameter = 517,   /* op_odbc_call_parameter  */
  YYSYMBOL_odbc_parameter_commalist = 518, /* odbc_parameter_commalist  */
  YYSYMBOL_odbc_parameter = 519,           /* odbc_parameter  */
  YYSYMBOL_odbc_fct_spec = 520,            /* odbc_fct_spec  */
  YYSYMBOL_odbc_fct_type = 521,            /* odbc_fct_type  */
  YYSYMBOL_general_set_fct = 522,          /* general_set_fct  */
  YYSYMBOL_set_fct_type = 523,             /* set_fct_type  */
  YYSYMBOL_ordered_set_function = 524,     /* ordered_set_function  */
  YYSYMBOL_hypothetical_set_function = 525, /* hypothetical_set_function  */
  YYSYMBOL_within_group_specification = 526, /* within_group_specification  */
  YYSYMBOL_hypothetical_set_function_value_expression_list = 527, /* hypothetical_set_function_value_expression_list  */
  YYSYMBOL_inverse_distribution_function = 528, /* inverse_distribution_function  */
  YYSYMBOL_inverse_distribution_function_argument = 529, /* inverse_distribution_function_argument  */
  YYSYMBOL_inverse_distribution_function_type = 530, /* inverse_distribution_function_type  */
  YYSYMBOL_array_aggregate_function = 531, /* array_aggregate_function  */
  YYSYMBOL_rank_function_type = 532,       /* rank_function_type  */
  YYSYMBOL_outer_join_type = 533,          /* outer_join_type  */
  YYSYMBOL_join_condition = 534,           /* join_condition  */
  YYSYMBOL_join_spec = 535,                /* join_spec  */
  YYSYMBOL_join_type = 536,                /* join_type  */
  YYSYMBOL_cross_union = 537,              /* cross_union  */
  YYSYMBOL_qualified_join = 538,           /* qualified_join  */
  YYSYMBOL_joined_table = 539,             /* joined_table  */
  YYSYMBOL_named_columns_join = 540,       /* named_columns_join  */
  YYSYMBOL_simple_table = 541,             /* simple_table  */
  YYSYMBOL_non_join_query_primary = 542,   /* non_join_query_primary  */
  YYSYMBOL_non_join_query_term = 543,      /* non_join_query_term  */
  YYSYMBOL_query_primary = 544,            /* query_primary  */
  YYSYMBOL_non_join_query_exp = 545,       /* non_join_query_exp  */
  YYSYMBOL_all = 546,                      /* all  */
  YYSYMBOL_query_exp = 547,                /* query_exp  */
  YYSYMBOL_scalar_subquery = 548,          /* scalar_subquery  */
  YYSYMBOL_cast_operand = 549,             /* cast_operand  */
  YYSYMBOL_cast_target = 550,              /* cast_target  */
  YYSYMBOL_cast_spec = 551,                /* cast_spec  */
  YYSYMBOL_value_exp_primary = 552,        /* value_exp_primary  */
  YYSYMBOL_num_primary = 553,              /* num_primary  */
  YYSYMBOL_factor = 554,                   /* factor  */
  YYSYMBOL_term = 555,                     /* term  */
  YYSYMBOL_num_value_exp = 556,            /* num_value_exp  */
  YYSYMBOL_datetime_primary = 557,         /* datetime_primary  */
  YYSYMBOL_datetime_value_fct = 558,       /* datetime_value_fct  */
  YYSYMBOL_time_zone = 559,                /* time_zone  */
  YYSYMBOL_time_zone_specifier = 560,      /* time_zone_specifier  */
  YYSYMBOL_datetime_factor = 561,          /* datetime_factor  */
  YYSYMBOL_datetime_term = 562,            /* datetime_term  */
  YYSYMBOL_datetime_value_exp = 563,       /* datetime_value_exp  */
  YYSYMBOL_non_second_datetime_field = 564, /* non_second_datetime_field  */
  YYSYMBOL_start_field = 565,              /* start_field  */
  YYSYMBOL_end_field = 566,                /* end_field  */
  YYSYMBOL_single_datetime_field = 567,    /* single_datetime_field  */
  YYSYMBOL_interval_qualifier = 568,       /* interval_qualifier  */
  YYSYMBOL_function_arg_commalist2 = 569,  /* function_arg_commalist2  */
  YYSYMBOL_function_arg_commalist3 = 570,  /* function_arg_commalist3  */
  YYSYMBOL_function_arg_commalist4 = 571,  /* function_arg_commalist4  */
  YYSYMBOL_value_exp_commalist = 572,      /* value_exp_commalist  */
  YYSYMBOL_function_arg = 573,             /* function_arg  */
  YYSYMBOL_function_args_commalist = 574,  /* function_args_commalist  */
  YYSYMBOL_value_exp = 575,                /* value_exp  */
  YYSYMBOL_string_value_exp = 576,         /* string_value_exp  */
  YYSYMBOL_char_value_exp = 577,           /* char_value_exp  */
  YYSYMBOL_concatenation = 578,            /* concatenation  */
  YYSYMBOL_char_primary = 579,             /* char_primary  */
  YYSYMBOL_collate_clause = 580,           /* collate_clause  */
  YYSYMBOL_char_factor = 581,              /* char_factor  */
  YYSYMBOL_string_value_fct = 582,         /* string_value_fct  */
  YYSYMBOL_bit_value_fct = 583,            /* bit_value_fct  */
  YYSYMBOL_bit_substring_fct = 584,        /* bit_substring_fct  */
  YYSYMBOL_bit_value_exp = 585,            /* bit_value_exp  */
  YYSYMBOL_bit_factor = 586,               /* bit_factor  */
  YYSYMBOL_bit_primary = 587,              /* bit_primary  */
  YYSYMBOL_char_value_fct = 588,           /* char_value_fct  */
  YYSYMBOL_for_length = 589,               /* for_length  */
  YYSYMBOL_char_substring_fct = 590,       /* char_substring_fct  */
  YYSYMBOL_upper_lower = 591,              /* upper_lower  */
  YYSYMBOL_fold = 592,                     /* fold  */
  YYSYMBOL_form_conversion = 593,          /* form_conversion  */
  YYSYMBOL_char_translation = 594,         /* char_translation  */
  YYSYMBOL_trim_fct = 595,                 /* trim_fct  */
  YYSYMBOL_trim_operands = 596,            /* trim_operands  */
  YYSYMBOL_trim_spec = 597,                /* trim_spec  */
  YYSYMBOL_derived_column = 598,           /* derived_column  */
  YYSYMBOL_table_node = 599,               /* table_node  */
  YYSYMBOL_catalog_name = 600,             /* catalog_name  */
  YYSYMBOL_schema_name = 601,              /* schema_name  */
  YYSYMBOL_table_name = 602,               /* table_name  */
  YYSYMBOL_column_ref = 603,               /* column_ref  */
  YYSYMBOL_column_val = 604,               /* column_val  */
  YYSYMBOL_data_type = 605,                /* data_type  */
  YYSYMBOL_opt_char_set_spec = 606,        /* opt_char_set_spec  */
  YYSYMBOL_opt_collate_clause = 607,       /* opt_collate_clause  */
  YYSYMBOL_predefined_type = 608,          /* predefined_type  */
  YYSYMBOL_character_string_type = 609,    /* character_string_type  */
  YYSYMBOL_opt_paren_precision = 610,      /* opt_paren_precision  */
  YYSYMBOL_paren_char_length = 611,        /* paren_char_length  */
  YYSYMBOL_opt_paren_char_large_length = 612, /* opt_paren_char_large_length  */
  YYSYMBOL_paren_character_large_object_length = 613, /* paren_character_large_object_length  */
  YYSYMBOL_large_object_length = 614,      /* large_object_length  */
  YYSYMBOL_opt_multiplier = 615,           /* opt_multiplier  */
  YYSYMBOL_character_large_object_type = 616, /* character_large_object_type  */
  YYSYMBOL_national_character_string_type = 617, /* national_character_string_type  */
  YYSYMBOL_national_character_large_object_type = 618, /* national_character_large_object_type  */
  YYSYMBOL_binary_string_type = 619,       /* binary_string_type  */
  YYSYMBOL_binary_large_object_string_type = 620, /* binary_large_object_string_type  */
  YYSYMBOL_numeric_type = 621,             /* numeric_type  */
  YYSYMBOL_opt_paren_precision_scale = 622, /* opt_paren_precision_scale  */
  YYSYMBOL_exact_numeric_type = 623,       /* exact_numeric_type  */
  YYSYMBOL_approximate_numeric_type = 624, /* approximate_numeric_type  */
  YYSYMBOL_boolean_type = 625,             /* boolean_type  */
  YYSYMBOL_datetime_type = 626,            /* datetime_type  */
  YYSYMBOL_opt_with_or_without_time_zone = 627, /* opt_with_or_without_time_zone  */
  YYSYMBOL_interval_type = 628,            /* interval_type  */
  YYSYMBOL_column = 629,                   /* column  */
  YYSYMBOL_case_expression = 630,          /* case_expression  */
  YYSYMBOL_case_abbreviation = 631,        /* case_abbreviation  */
  YYSYMBOL_case_specification = 632,       /* case_specification  */
  YYSYMBOL_simple_case = 633,              /* simple_case  */
  YYSYMBOL_searched_case = 634,            /* searched_case  */
  YYSYMBOL_simple_when_clause_list = 635,  /* simple_when_clause_list  */
  YYSYMBOL_simple_when_clause = 636,       /* simple_when_clause  */
  YYSYMBOL_when_operand_list = 637,        /* when_operand_list  */
  YYSYMBOL_when_operand = 638,             /* when_operand  */
  YYSYMBOL_searched_when_clause_list = 639, /* searched_when_clause_list  */
  YYSYMBOL_searched_when_clause = 640,     /* searched_when_clause  */
  YYSYMBOL_else_clause = 641,              /* else_clause  */
  YYSYMBOL_result = 642,                   /* result  */
  YYSYMBOL_result_expression = 643,        /* result_expression  */
  YYSYMBOL_case_operand = 644,             /* case_operand  */
  YYSYMBOL_cursor = 645,                   /* cursor  */
  YYSYMBOL_parameter = 646,                /* parameter  */
  YYSYMBOL_range_variable = 647,           /* range_variable  */
  YYSYMBOL_user = 648,                     /* user  */
  YYSYMBOL_trigger_definition = 649,       /* trigger_definition  */
  YYSYMBOL_op_referencing = 650,           /* op_referencing  */
  YYSYMBOL_trigger_action_time = 651,      /* trigger_action_time  */
  YYSYMBOL_trigger_event = 652,            /* trigger_event  */
  YYSYMBOL_op_trigger_columnlist = 653,    /* op_trigger_columnlist  */
  YYSYMBOL_trigger_column_list = 654,      /* trigger_column_list  */
  YYSYMBOL_triggered_action = 655,         /* triggered_action  */
  YYSYMBOL_op_triggered_action_for = 656,  /* op_triggered_action_for  */
  YYSYMBOL_trigger_for = 657,              /* trigger_for  */
  YYSYMBOL_triggered_when_clause = 658,    /* triggered_when_clause  */
  YYSYMBOL_triggered_SQL_statement = 659,  /* triggered_SQL_statement  */
  YYSYMBOL_SQL_procedure_statement_list = 660, /* SQL_procedure_statement_list  */
  YYSYMBOL_SQL_procedure_statement = 661,  /* SQL_procedure_statement  */
  YYSYMBOL_transition_table_or_variable_list = 662, /* transition_table_or_variable_list  */
  YYSYMBOL_transition_table_or_variable = 663, /* transition_table_or_variable  */
  YYSYMBOL_old_transition_table_name = 664, /* old_transition_table_name  */
  YYSYMBOL_new_transition_table_name = 665, /* new_transition_table_name  */
  YYSYMBOL_transition_table_name = 666,    /* transition_table_name  */
  YYSYMBOL_old_transition_variable_name = 667, /* old_transition_variable_name  */
  YYSYMBOL_new_transition_variable_name = 668, /* new_transition_variable_name  */
  YYSYMBOL_trigger_name = 669              /* trigger_name  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int16 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  372
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   8790

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  325
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  345
/* YYNRULES -- Number of rules.  */
#define YYNRULES  789
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  1306

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   558


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int16 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       3,     4,   321,   318,     5,   319,    13,   322,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     6,     7,
       2,   323,     2,     8,     2,     2,     2,     2,     2,     2,
       2,    16,     2,     2,     2,    14,     2,    15,     2,     2,
      18,     2,     2,     2,    17,     2,     2,     2,     2,     2,
       2,     9,     2,    10,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    11,     2,    12,     2,     2,     2,     2,
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
       2,     2,     2,     2,     2,     2,     1,     2,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      91,    92,    93,    94,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,   105,   106,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,   122,   123,   124,   125,   126,   127,   128,   129,   130,
     131,   132,   133,   134,   135,   136,   137,   138,   139,   140,
     141,   142,   143,   144,   145,   146,   147,   148,   149,   150,
     151,   152,   153,   154,   155,   156,   157,   158,   159,   160,
     161,   162,   163,   164,   165,   166,   167,   168,   169,   170,
     171,   172,   173,   174,   175,   176,   177,   178,   179,   180,
     181,   182,   183,   184,   185,   186,   187,   188,   189,   190,
     191,   192,   193,   194,   195,   196,   197,   198,   199,   200,
     201,   202,   203,   204,   205,   206,   207,   208,   209,   210,
     211,   212,   213,   214,   215,   216,   217,   218,   219,   220,
     221,   222,   223,   224,   225,   226,   227,   228,   229,   230,
     231,   232,   233,   234,   235,   236,   237,   238,   239,   240,
     241,   242,   243,   244,   245,   246,   247,   248,   249,   250,
     251,   252,   253,   254,   255,   256,   257,   258,   259,   260,
     261,   262,   263,   264,   265,   266,   267,   268,   269,   270,
     271,   272,   273,   274,   275,   276,   277,   278,   279,   280,
     281,   282,   283,   284,   285,   286,   287,   288,   289,   290,
     291,   292,   293,   294,   295,   296,   297,   298,   299,   300,
     301,   302,   303,   304,   305,   306,   307,   308,   309,   310,
     311,   312,   313,   314,   315,   316,   317,   320,   324
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   309,   309,   311,   319,   320,   378,   379,   380,   381,
     385,   396,   399,   405,   406,   410,   419,   420,   426,   429,
     430,   438,   442,   443,   447,   451,   455,   461,   462,   468,
     472,   482,   488,   497,   509,   517,   518,   526,   529,   535,
     547,   548,   556,   557,   565,   578,   579,   587,   591,   595,
     598,   604,   605,   609,   610,   614,   618,   623,   626,   632,
     633,   639,   640,   648,   651,   662,   667,   674,   675,   676,
     692,   693,   699,   701,   702,   703,   704,   705,   707,   708,
     709,   719,   720,   730,   749,   758,   767,   776,   786,   789,
     802,   813,   814,   819,   828,   839,   840,   841,   860,   863,
     869,   876,   877,   880,   890,   893,   899,   903,   904,   910,
     918,   929,   934,   937,   938,   941,   950,   951,   954,   955,
     958,   959,   962,   963,   966,   977,   980,   984,   985,   988,
     989,   997,  1006,  1022,  1030,  1033,  1039,  1040,  1043,  1044,
    1047,  1048,  1056,  1062,  1069,  1070,  1078,  1087,  1094,  1095,
    1103,  1106,  1109,  1112,  1118,  1119,  1127,  1128,  1129,  1130,
    1133,  1134,  1141,  1167,  1176,  1177,  1187,  1188,  1196,  1197,
    1206,  1207,  1216,  1217,  1218,  1219,  1220,  1221,  1222,  1223,
    1226,  1233,  1240,  1265,  1266,  1267,  1268,  1269,  1270,  1273,
    1311,  1317,  1320,  1330,  1340,  1346,  1352,  1374,  1399,  1400,
    1404,  1415,  1424,  1430,  1446,  1450,  1458,  1467,  1473,  1489,
    1498,  1504,  1521,  1522,  1523,  1527,  1533,  1539,  1548,  1553,
    1568,  1573,  1605,  1606,  1607,  1608,  1609,  1611,  1623,  1635,
    1647,  1663,  1664,  1670,  1673,  1683,  1693,  1694,  1695,  1698,
    1706,  1717,  1727,  1737,  1742,  1747,  1754,  1759,  1766,  1767,
    1768,  1771,  1776,  1795,  1807,  1808,  1811,  1812,  1814,  1815,
    1816,  1817,  1818,  1819,  1820,  1821,  1822,  1823,  1824,  1827,
    1828,  1835,  1842,  1849,  1857,  1865,  1873,  1881,  1889,  1902,
    1917,  1918,  1921,  1922,  1923,  1926,  1927,  1930,  1931,  1932,
    1933,  1936,  1937,  1940,  1943,  1944,  1945,  1946,  1949,  1950,
    1951,  1952,  1953,  1954,  1955,  1959,  1960,  1961,  1964,  1967,
    1971,  1972,  1973,  1974,  1975,  1978,  1979,  1980,  1983,  1984,
    1985,  1986,  1987,  1988,  1989,  1990,  1991,  1992,  1993,  1994,
    1995,  1996,  2000,  2001,  2004,  2007,  2008,  2009,  2010,  2011,
    2012,  2013,  2014,  2015,  2016,  2017,  2018,  2019,  2020,  2021,
    2022,  2023,  2024,  2027,  2028,  2029,  2032,  2033,  2037,  2046,
    2053,  2060,  2061,  2062,  2063,  2064,  2067,  2077,  2080,  2083,
    2084,  2087,  2088,  2094,  2104,  2105,  2109,  2121,  2122,  2125,
    2128,  2131,  2134,  2135,  2138,  2149,  2150,  2153,  2154,  2157,
    2171,  2172,  2175,  2181,  2189,  2192,  2193,  2196,  2199,  2200,
    2203,  2211,  2214,  2219,  2228,  2231,  2240,  2241,  2244,  2245,
    2248,  2249,  2252,  2258,  2261,  2270,  2273,  2278,  2286,  2287,
    2290,  2299,  2300,  2303,  2304,  2307,  2313,  2314,  2322,  2330,
    2340,  2343,  2346,  2347,  2353,  2356,  2364,  2371,  2377,  2383,
    2392,  2393,  2401,  2412,  2413,  2423,  2426,  2433,  2434,  2435,
    2439,  2445,  2454,  2455,  2456,  2460,  2469,  2477,  2486,  2487,
    2490,  2491,  2492,  2493,  2494,  2495,  2496,  2497,  2498,  2499,
    2500,  2501,  2502,  2503,  2507,  2508,  2511,  2520,  2534,  2537,
    2548,  2552,  2562,  2565,  2566,  2570,  2582,  2583,  2584,  2585,
    2588,  2593,  2598,  2605,  2613,  2614,  2617,  2618,  2623,  2624,
    2632,  2644,  2653,  2662,  2665,  2668,  2678,  2679,  2683,  2684,
    2693,  2694,  2704,  2707,  2708,  2716,  2726,  2727,  2730,  2733,
    2736,  2739,  2740,  2743,  2755,  2756,  2757,  2758,  2759,  2760,
    2761,  2768,  2772,  2773,  2776,  2777,  2783,  2792,  2793,  2800,
    2810,  2811,  2818,  2832,  2839,  2844,  2849,  2856,  2864,  2878,
    2883,  2891,  2917,  2978,  2979,  2980,  2981,  2982,  2985,  2993,
    2994,  3003,  3009,  3018,  3025,  3029,  3035,  3044,  3054,  3057,
    3061,  3073,  3074,  3081,  3089,  3098,  3101,  3105,  3118,  3119,
    3120,  3127,  3135,  3136,  3139,  3146,  3156,  3157,  3160,  3168,
    3169,  3177,  3178,  3181,  3188,  3201,  3225,  3232,  3245,  3246,
    3247,  3252,  3257,  3264,  3265,  3273,  3284,  3294,  3295,  3298,
    3308,  3318,  3330,  3342,  3352,  3360,  3367,  3374,  3380,  3384,
    3385,  3386,  3390,  3399,  3400,  3401,  3404,  3411,  3420,  3430,
    3436,  3445,  3451,  3458,  3468,  3492,  3495,  3502,  3505,  3506,
    3515,  3516,  3519,  3526,  3532,  3533,  3534,  3535,  3536,  3539,
    3545,  3551,  3558,  3565,  3571,  3574,  3575,  3578,  3587,  3588,
    3591,  3601,  3609,  3610,  3615,  3620,  3625,  3630,  3637,  3645,
    3653,  3661,  3668,  3675,  3681,  3689,  3697,  3704,  3707,  3716,
    3724,  3732,  3738,  3745,  3751,  3754,  3762,  3770,  3771,  3774,
    3775,  3782,  3793,  3799,  3805,  3811,  3812,  3813,  3814,  3817,
    3825,  3826,  3827,  3828,  3836,  3839,  3840,  3847,  3856,  3857,
    3864,  3873,  3883,  3884,  3890,  3896,  3904,  3905,  3908,  3916,
    3924,  3934,  3935,  3938,  3949,  3959,  3964,  3971,  3981,  3984,
    3989,  3990,  3991,  3992,  3993,  3994,  3997,  4002,  4009,  4019,
    4020,  4028,  4029,  4032,  4035,  4038,  4051,  4055,  4058,  4073,
    4074,  4081,  4086,  4100,  4103,  4118,  4121,  4129,  4130,  4131,
    4139,  4140,  4141,  4149,  4152,  4160,  4163,  4172,  4175,  4184,
    4185,  4188,  4191,  4199,  4200,  4211,  4216,  4223,  4227,  4232,
    4240,  4248,  4256,  4264,  4274,  4277,  4280,  4283,  4286,  4289
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "'('", "')'", "','",
  "':'", "';'", "'?'", "'['", "']'", "'{'", "'}'", "'.'", "'K'", "'M'",
  "'G'", "'T'", "'P'", "SQL_TOKEN_STRING", "SQL_TOKEN_ACCESS_DATE",
  "SQL_TOKEN_INT", "SQL_TOKEN_REAL_NUM", "SQL_TOKEN_INTNUM",
  "SQL_TOKEN_APPROXNUM", "SQL_TOKEN_NOT", "SQL_TOKEN_NAME",
  "SQL_TOKEN_UMINUS", "SQL_TOKEN_ALL", "SQL_TOKEN_ALTER",
  "SQL_TOKEN_AMMSC", "SQL_TOKEN_ANY", "SQL_TOKEN_AS", "SQL_TOKEN_ASC",
  "SQL_TOKEN_AT", "SQL_TOKEN_AUTHORIZATION", "SQL_TOKEN_AVG",
  "SQL_TOKEN_BETWEEN", "SQL_TOKEN_BIT", "SQL_TOKEN_BOTH", "SQL_TOKEN_BY",
  "SQL_TOKEN_CAST", "SQL_TOKEN_CHARACTER", "SQL_TOKEN_CHECK",
  "SQL_TOKEN_COLLATE", "SQL_TOKEN_COMMIT", "SQL_TOKEN_CONTINUE",
  "SQL_TOKEN_CONVERT", "SQL_TOKEN_COUNT", "SQL_TOKEN_CREATE",
  "SQL_TOKEN_CROSS", "SQL_TOKEN_CURRENT", "SQL_TOKEN_CURSOR",
  "SQL_TOKEN_DATE", "SQL_TOKEN_DATEVALUE", "SQL_TOKEN_DAY",
  "SQL_TOKEN_DEC", "SQL_TOKEN_DECIMAL", "SQL_TOKEN_DECLARE",
  "SQL_TOKEN_DEFAULT", "SQL_TOKEN_DELETE", "SQL_TOKEN_DESC",
  "SQL_TOKEN_DISTINCT", "SQL_TOKEN_DOUBLE", "SQL_TOKEN_DROP",
  "SQL_TOKEN_ESCAPE", "SQL_TOKEN_EXCEPT", "SQL_TOKEN_EXISTS",
  "SQL_TOKEN_FALSE", "SQL_TOKEN_FETCH", "SQL_TOKEN_FLOAT", "SQL_TOKEN_FOR",
  "SQL_TOKEN_FOREIGN", "SQL_TOKEN_FOUND", "SQL_TOKEN_FROM",
  "SQL_TOKEN_FULL", "SQL_TOKEN_GRANT", "SQL_TOKEN_GROUP",
  "SQL_TOKEN_HAVING", "SQL_TOKEN_IN", "SQL_TOKEN_INDICATOR",
  "SQL_TOKEN_INNER", "SQL_TOKEN_INTEGER", "SQL_TOKEN_INTO", "SQL_TOKEN_IS",
  "SQL_TOKEN_INTERSECT", "SQL_TOKEN_JOIN", "SQL_TOKEN_KEY",
  "SQL_TOKEN_LEADING", "SQL_TOKEN_LIKE", "SQL_TOKEN_LOCAL",
  "SQL_TOKEN_LOWER", "SQL_TOKEN_MAX", "SQL_TOKEN_MIN", "SQL_TOKEN_NATURAL",
  "SQL_TOKEN_NCHAR", "SQL_TOKEN_NULL", "SQL_TOKEN_NUMERIC",
  "SQL_TOKEN_OCTET_LENGTH", "SQL_TOKEN_OF", "SQL_TOKEN_ON",
  "SQL_TOKEN_OPTION", "SQL_TOKEN_ORDER", "SQL_TOKEN_OUTER",
  "SQL_TOKEN_PRECISION", "SQL_TOKEN_PRIMARY", "SQL_TOKEN_PRIVILEGES",
  "SQL_TOKEN_PROCEDURE", "SQL_TOKEN_PUBLIC", "SQL_TOKEN_REAL",
  "SQL_TOKEN_REFERENCES", "SQL_TOKEN_ROLLBACK", "SQL_TOKEN_SCHEMA",
  "SQL_TOKEN_SELECT", "SQL_TOKEN_SET", "SQL_TOKEN_SIZE",
  "SQL_TOKEN_SMALLINT", "SQL_TOKEN_SOME", "SQL_TOKEN_SQLCODE",
  "SQL_TOKEN_SQLERROR", "SQL_TOKEN_SUM", "SQL_TOKEN_TABLE",
  "SQL_TOKEN_TIME", "SQL_TOKEN_TIMESTAMP", "SQL_TOKEN_TIMEZONE_HOUR",
  "SQL_TOKEN_TIMEZONE_MINUTE", "SQL_TOKEN_TO", "SQL_TOKEN_TRAILING",
  "SQL_TOKEN_TRANSLATE", "SQL_TOKEN_TRIM", "SQL_TOKEN_TRUE",
  "SQL_TOKEN_UNION", "SQL_TOKEN_UNIQUE", "SQL_TOKEN_UNKNOWN",
  "SQL_TOKEN_UPDATE", "SQL_TOKEN_UPPER", "SQL_TOKEN_USAGE",
  "SQL_TOKEN_USER", "SQL_TOKEN_USING", "SQL_TOKEN_VALUES",
  "SQL_TOKEN_VIEW", "SQL_TOKEN_WHERE", "SQL_TOKEN_WITH", "SQL_TOKEN_WORK",
  "SQL_TOKEN_ZONE", "SQL_TOKEN_CALL", "SQL_TOKEN_D", "SQL_TOKEN_FN",
  "SQL_TOKEN_T", "SQL_TOKEN_TS", "SQL_TOKEN_OJ", "SQL_TOKEN_ASCII",
  "SQL_TOKEN_BIT_LENGTH", "SQL_TOKEN_CHAR", "SQL_TOKEN_CHAR_LENGTH",
  "SQL_TOKEN_SQL_TOKEN_INTNUM", "SQL_TOKEN_CONCAT", "SQL_TOKEN_DIFFERENCE",
  "SQL_TOKEN_INSERT", "SQL_TOKEN_LCASE", "SQL_TOKEN_LEFT",
  "SQL_TOKEN_LENGTH", "SQL_TOKEN_LOCATE", "SQL_TOKEN_LOCATE_2",
  "SQL_TOKEN_LTRIM", "SQL_TOKEN_POSITION", "SQL_TOKEN_REPEAT",
  "SQL_TOKEN_REPLACE", "SQL_TOKEN_RIGHT", "SQL_TOKEN_RTRIM",
  "SQL_TOKEN_SOUNDEX", "SQL_TOKEN_SPACE", "SQL_TOKEN_SUBSTRING",
  "SQL_TOKEN_UCASE", "SQL_TOKEN_CURRENT_DATE", "SQL_TOKEN_CURRENT_TIME",
  "SQL_TOKEN_CURRENT_TIMESTAMP", "SQL_TOKEN_CURDATE", "SQL_TOKEN_CURTIME",
  "SQL_TOKEN_DAYNAME", "SQL_TOKEN_DAYOFMONTH", "SQL_TOKEN_DAYOFWEEK",
  "SQL_TOKEN_DAYOFYEAR", "SQL_TOKEN_EXTRACT", "SQL_TOKEN_HOUR",
  "SQL_TOKEN_MINUTE", "SQL_TOKEN_MONTH", "SQL_TOKEN_MONTHNAME",
  "SQL_TOKEN_NOW", "SQL_TOKEN_QUARTER", "SQL_TOKEN_DATEDIFF",
  "SQL_TOKEN_SECOND", "SQL_TOKEN_TIMESTAMPADD", "SQL_TOKEN_TIMESTAMPDIFF",
  "SQL_TOKEN_TIMEVALUE", "SQL_TOKEN_WEEK", "SQL_TOKEN_YEAR",
  "SQL_TOKEN_ABS", "SQL_TOKEN_ACOS", "SQL_TOKEN_ASIN", "SQL_TOKEN_ATAN",
  "SQL_TOKEN_ATAN2", "SQL_TOKEN_CEILING", "SQL_TOKEN_COS", "SQL_TOKEN_COT",
  "SQL_TOKEN_DEGREES", "SQL_TOKEN_EXP", "SQL_TOKEN_FLOOR",
  "SQL_TOKEN_LOGF", "SQL_TOKEN_LOG", "SQL_TOKEN_LN", "SQL_TOKEN_LOG10",
  "SQL_TOKEN_MOD", "SQL_TOKEN_PI", "SQL_TOKEN_POWER", "SQL_TOKEN_RADIANS",
  "SQL_TOKEN_RAND", "SQL_TOKEN_ROUNDMAGIC", "SQL_TOKEN_ROUND",
  "SQL_TOKEN_SIGN", "SQL_TOKEN_SIN", "SQL_TOKEN_SQRT", "SQL_TOKEN_TAN",
  "SQL_TOKEN_TRUNCATE", "SQL_TOKEN_EVERY", "SQL_TOKEN_INTERSECTION",
  "SQL_TOKEN_FUSION", "SQL_TOKEN_COLLECT", "SQL_TOKEN_VAR_POP",
  "SQL_TOKEN_VAR_SAMP", "SQL_TOKEN_STDDEV_SAMP", "SQL_TOKEN_STDDEV_POP",
  "SQL_TOKEN_RANK", "SQL_TOKEN_DENSE_RANK", "SQL_TOKEN_PERCENT_RANK",
  "SQL_TOKEN_CUME_DIST", "SQL_TOKEN_PERCENTILE_CONT",
  "SQL_TOKEN_PERCENTILE_DISC", "SQL_TOKEN_WITHIN", "SQL_TOKEN_ARRAY_AGG",
  "SQL_TOKEN_CASE", "SQL_TOKEN_THEN", "SQL_TOKEN_END", "SQL_TOKEN_NULLIF",
  "SQL_TOKEN_COALESCE", "SQL_TOKEN_WHEN", "SQL_TOKEN_ELSE",
  "SQL_TOKEN_BEFORE", "SQL_TOKEN_AFTER", "SQL_TOKEN_INSTEAD",
  "SQL_TOKEN_EACH", "SQL_TOKEN_REFERENCING", "SQL_TOKEN_BEGIN",
  "SQL_TOKEN_ATOMIC", "SQL_TOKEN_TRIGGER", "SQL_TOKEN_ROW",
  "SQL_TOKEN_STATEMENT", "SQL_TOKEN_NEW", "SQL_TOKEN_OLD",
  "SQL_TOKEN_VALUE", "SQL_TOKEN_CURRENT_CATALOG",
  "SQL_TOKEN_CURRENT_DEFAULT_TRANSFORM_GROUP", "SQL_TOKEN_CURRENT_PATH",
  "SQL_TOKEN_CURRENT_ROLE", "SQL_TOKEN_CURRENT_SCHEMA",
  "SQL_TOKEN_CURRENT_USER", "SQL_TOKEN_SESSION_USER",
  "SQL_TOKEN_SYSTEM_USER", "SQL_TOKEN_VARCHAR", "SQL_TOKEN_VARBINARY",
  "SQL_TOKEN_VARYING", "SQL_TOKEN_OBJECT", "SQL_TOKEN_NCLOB",
  "SQL_TOKEN_NATIONAL", "SQL_TOKEN_LARGE", "SQL_TOKEN_CLOB",
  "SQL_TOKEN_BLOB", "SQL_TOKEN_BIGINT", "SQL_TOKEN_BINARY",
  "SQL_TOKEN_WITHOUT", "SQL_TOKEN_BOOLEAN", "SQL_TOKEN_INTERVAL",
  "SQL_TOKEN_OVER", "SQL_TOKEN_ROW_NUMBER", "SQL_TOKEN_NTILE",
  "SQL_TOKEN_LEAD", "SQL_TOKEN_LAG", "SQL_TOKEN_RESPECT",
  "SQL_TOKEN_IGNORE", "SQL_TOKEN_NULLS", "SQL_TOKEN_FIRST_VALUE",
  "SQL_TOKEN_LAST_VALUE", "SQL_TOKEN_NTH_VALUE", "SQL_TOKEN_FIRST",
  "SQL_TOKEN_LAST", "SQL_TOKEN_EXCLUDE", "SQL_TOKEN_OTHERS",
  "SQL_TOKEN_TIES", "SQL_TOKEN_FOLLOWING", "SQL_TOKEN_UNBOUNDED",
  "SQL_TOKEN_PRECEDING", "SQL_TOKEN_RANGE", "SQL_TOKEN_ROWS",
  "SQL_TOKEN_PARTITION", "SQL_TOKEN_WINDOW", "SQL_TOKEN_NO",
  "SQL_TOKEN_LIMIT", "SQL_TOKEN_OFFSET", "SQL_TOKEN_NEXT",
  "SQL_TOKEN_ONLY", "SQL_TOKEN_OR", "SQL_TOKEN_AND", "SQL_LESSEQ",
  "SQL_GREATEQ", "SQL_NOTEQUAL", "SQL_LESS", "SQL_GREAT", "SQL_EQUAL",
  "'+'", "'-'", "SQL_CONCAT", "'*'", "'/'", "'='",
  "SQL_TOKEN_INVALIDSYMBOL", "$accept", "sql_single_statement", "sql",
  "schema_element", "base_table_def", "base_table_element_commalist",
  "base_table_element", "column_def", "column_def_opt_list", "nil_fkt",
  "unique_spec", "column_def_opt", "table_constraint_def",
  "op_column_commalist", "column_commalist", "view_def",
  "opt_with_check_option", "opt_column_commalist", "privilege_def",
  "opt_with_grant_option", "privileges", "operation_commalist",
  "operation", "grantee_commalist", "grantee", "opt_order_by_clause",
  "ordering_spec_commalist", "ordering_spec", "opt_asc_desc", "sql_not",
  "manipulative_statement", "union_statement", "commit_statement",
  "delete_statement_searched", "fetch_statement", "insert_statement",
  "values_or_query_spec", "table_value_const_list",
  "row_value_constructor", "row_value_constructor_elem",
  "rollback_statement", "select_statement_into", "opt_all_distinct",
  "assignment_commalist", "assignment", "update_source",
  "update_statement_searched", "target_commalist", "target",
  "opt_where_clause", "query_term", "select_statement", "selection",
  "opt_result_offset_clause", "result_offset_clause",
  "opt_fetch_first_row_count", "first_or_next", "row_or_rows",
  "opt_fetch_first_clause", "fetch_first_clause", "offset_row_count",
  "fetch_first_row_count", "opt_limit_offset_clause", "opt_offset",
  "limit_offset_clause", "table_exp", "from_clause", "table_ref_commalist",
  "opt_as", "opt_row", "table_primary_as_range_column", "table_ref",
  "where_clause", "opt_group_by_clause", "column_ref_commalist",
  "opt_having_clause", "truth_value", "boolean_primary",
  "parenthesized_boolean_value_expression", "boolean_test",
  "boolean_factor", "boolean_term", "search_condition", "predicate",
  "comparison_predicate_part_2", "comparison_predicate", "comparison",
  "between_predicate_part_2", "between_predicate",
  "character_like_predicate_part_2", "other_like_predicate_part_2",
  "like_predicate", "opt_escape", "null_predicate_part_2", "test_for_null",
  "in_predicate_value", "in_predicate_part_2", "in_predicate",
  "quantified_comparison_predicate_part_2", "all_or_any_predicate",
  "any_all_some", "existence_test", "unique_test", "subquery",
  "scalar_exp_commalist", "select_sublist", "parameter_ref", "literal",
  "as_clause", "position_exp", "num_value_fct", "char_length_exp",
  "octet_length_exp", "bit_length_exp", "length_exp", "datetime_field",
  "extract_field", "time_zone_field", "extract_exp", "unsigned_value_spec",
  "general_value_spec", "set_fct_spec", "function_name0", "function_name1",
  "function_name2", "function_name12", "function_name23", "function_name3",
  "function_name", "string_function_1Argument",
  "string_function_2Argument", "string_function_3Argument",
  "string_function_4Argument", "string_function",
  "date_function_0Argument", "date_function_1Argument", "date_function",
  "numeric_function_0Argument", "numeric_function_1Argument",
  "numeric_function_2Argument", "numeric_function", "window_function",
  "window_function_type", "ntile_function",
  "dynamic_parameter_specification", "simple_value_specification",
  "number_of_tiles", "opt_lead_or_lag_function", "opt_null_treatment",
  "lead_or_lag_function", "lead_or_lag", "lead_or_lag_extent", "offset",
  "default_expression", "null_treatment", "first_or_last_value_function",
  "first_or_last_value", "opt_from_first_or_last", "nth_value_function",
  "nth_row", "from_first_or_last", "window_name",
  "window_name_or_specification", "in_line_window_specification",
  "opt_window_clause", "window_clause", "window_definition_list",
  "window_definition", "new_window_name", "window_specification",
  "opt_existing_window_name", "opt_window_partition_clause",
  "opt_window_frame_clause", "window_specification_details",
  "existing_window_name", "window_partition_clause",
  "window_partition_column_reference_list",
  "window_partition_column_reference", "opt_window_frame_exclusion",
  "window_frame_clause", "window_frame_units", "window_frame_extent",
  "window_frame_start", "window_frame_preceding", "window_frame_between",
  "window_frame_bound_1", "window_frame_bound_2", "window_frame_bound",
  "window_frame_following", "window_frame_exclusion", "op_parameter",
  "odbc_call_spec", "op_odbc_call_parameter", "odbc_parameter_commalist",
  "odbc_parameter", "odbc_fct_spec", "odbc_fct_type", "general_set_fct",
  "set_fct_type", "ordered_set_function", "hypothetical_set_function",
  "within_group_specification",
  "hypothetical_set_function_value_expression_list",
  "inverse_distribution_function",
  "inverse_distribution_function_argument",
  "inverse_distribution_function_type", "array_aggregate_function",
  "rank_function_type", "outer_join_type", "join_condition", "join_spec",
  "join_type", "cross_union", "qualified_join", "joined_table",
  "named_columns_join", "simple_table", "non_join_query_primary",
  "non_join_query_term", "query_primary", "non_join_query_exp", "all",
  "query_exp", "scalar_subquery", "cast_operand", "cast_target",
  "cast_spec", "value_exp_primary", "num_primary", "factor", "term",
  "num_value_exp", "datetime_primary", "datetime_value_fct", "time_zone",
  "time_zone_specifier", "datetime_factor", "datetime_term",
  "datetime_value_exp", "non_second_datetime_field", "start_field",
  "end_field", "single_datetime_field", "interval_qualifier",
  "function_arg_commalist2", "function_arg_commalist3",
  "function_arg_commalist4", "value_exp_commalist", "function_arg",
  "function_args_commalist", "value_exp", "string_value_exp",
  "char_value_exp", "concatenation", "char_primary", "collate_clause",
  "char_factor", "string_value_fct", "bit_value_fct", "bit_substring_fct",
  "bit_value_exp", "bit_factor", "bit_primary", "char_value_fct",
  "for_length", "char_substring_fct", "upper_lower", "fold",
  "form_conversion", "char_translation", "trim_fct", "trim_operands",
  "trim_spec", "derived_column", "table_node", "catalog_name",
  "schema_name", "table_name", "column_ref", "column_val", "data_type",
  "opt_char_set_spec", "opt_collate_clause", "predefined_type",
  "character_string_type", "opt_paren_precision", "paren_char_length",
  "opt_paren_char_large_length", "paren_character_large_object_length",
  "large_object_length", "opt_multiplier", "character_large_object_type",
  "national_character_string_type", "national_character_large_object_type",
  "binary_string_type", "binary_large_object_string_type", "numeric_type",
  "opt_paren_precision_scale", "exact_numeric_type",
  "approximate_numeric_type", "boolean_type", "datetime_type",
  "opt_with_or_without_time_zone", "interval_type", "column",
  "case_expression", "case_abbreviation", "case_specification",
  "simple_case", "searched_case", "simple_when_clause_list",
  "simple_when_clause", "when_operand_list", "when_operand",
  "searched_when_clause_list", "searched_when_clause", "else_clause",
  "result", "result_expression", "case_operand", "cursor", "parameter",
  "range_variable", "user", "trigger_definition", "op_referencing",
  "trigger_action_time", "trigger_event", "op_trigger_columnlist",
  "trigger_column_list", "triggered_action", "op_triggered_action_for",
  "trigger_for", "triggered_when_clause", "triggered_SQL_statement",
  "SQL_procedure_statement_list", "SQL_procedure_statement",
  "transition_table_or_variable_list", "transition_table_or_variable",
  "old_transition_table_name", "new_transition_table_name",
  "transition_table_name", "old_transition_variable_name",
  "new_transition_variable_name", "trigger_name", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-1114)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-736)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    2396,  1445,    84, -1114,    94,   123, -1114, -1114, -1114, -1114,
   -1114, -1114,  3030,   467, -1114, -1114,   148,   120,   183,   191,
      14, -1114, -1114, -1114,   200,   234, -1114,   272,   524,   291,
   -1114, -1114, -1114,   322,   204,    97, -1114, -1114,   434,   450,
   -1114,   234,   456, -1114, -1114, -1114,   494, -1114,   496,   503,
   -1114, -1114,   391, -1114, -1114, -1114, -1114, -1114, -1114,   510,
   -1114, -1114, -1114, -1114, -1114, -1114,   522, -1114, -1114, -1114,
   -1114, -1114, -1114, -1114, -1114, -1114, -1114,   528, -1114, -1114,
   -1114, -1114, -1114, -1114, -1114, -1114, -1114, -1114, -1114, -1114,
   -1114, -1114, -1114, -1114, -1114, -1114, -1114, -1114, -1114, -1114,
   -1114, -1114, -1114, -1114, -1114, -1114, -1114, -1114, -1114, -1114,
   -1114, -1114, -1114, -1114, -1114, -1114, -1114, -1114, -1114, -1114,
   -1114, -1114, -1114, -1114, -1114, -1114, -1114, -1114, -1114, -1114,
   -1114, -1114,   531,  4588,   537,   554, -1114, -1114, -1114, -1114,
   -1114, -1114, -1114, -1114, -1114,   561,   574, -1114, -1114, -1114,
   -1114,   585, -1114, -1114, -1114, -1114, -1114, -1114,  8079,  8079,
     501,   520, -1114, -1114, -1114, -1114,   372, -1114,   418, -1114,
   -1114, -1114, -1114,   171,   151, -1114, -1114, -1114, -1114,   507,
   -1114, -1114,   282,   301, -1114, -1114,  4889, -1114, -1114, -1114,
   -1114, -1114, -1114, -1114, -1114, -1114, -1114, -1114, -1114, -1114,
   -1114,   498, -1114, -1114, -1114, -1114, -1114, -1114, -1114, -1114,
   -1114, -1114,   610,   624,   630,   632,   642,   652,   656, -1114,
   -1114, -1114,   686, -1114, -1114, -1114, -1114, -1114, -1114, -1114,
   -1114, -1114,   415, -1114, -1114,   697, -1114,   699, -1114,   423,
     704, -1114, -1114, -1114,   726, -1114,   740, -1114, -1114, -1114,
   -1114, -1114,   -43,   -14,   698, -1114, -1114, -1114, -1114,   425,
   -1114,   437, -1114,   721, -1114, -1114, -1114, -1114, -1114, -1114,
     767, -1114, -1114, -1114, -1114, -1114, -1114, -1114, -1114, -1114,
   -1114, -1114, -1114, -1114,  1445,   770,   778, -1114,   700,    43,
      48, -1114, -1114,   702, -1114,    90,    23, -1114,   782,   477,
   -1114,  8551, -1114, -1114,   651,   786,   788,   789,  2713,   449,
   -1114, -1114, -1114,   810,    19,  7499, -1114,  7499,    20,   456,
     456,   812,   456,    93, -1114, -1114,   766,   733, -1114,   848,
   -1114,   848, -1114,   848,   752,   850, -1114,   757,  7499, -1114,
   -1114, -1114,  1154,  7499,  4298, -1114,   147,   742, -1114, -1114,
   -1114,  7499,  7499,  7499,   456,  7499,  7499,  5179,  7499,  5469,
    3347, -1114,   195, -1114,   612,  7499,  7499,   854,   552,  7499,
   -1114, -1114, -1114, -1114,  6049,   856,  7499,   832,  4889, -1114,
   -1114, -1114, -1114, -1114, -1114,   291,  3347,  3347, -1114, -1114,
   -1114, -1114, -1114,   234, -1114, -1114, -1114, -1114,   857,  6339,
    6339,  6339,  6339,  6339,  5759,  6339,   139,  7499,  7499,    97,
    7789,  6629,  7789,  7789,  7789,  7789,   772, -1114,  7499,   413,
     456, -1114,  7499,   859,  6049, -1114,   832, -1114, -1114,   832,
     832, -1114, -1114, -1114, -1114, -1114, -1114,   861,   456, -1114,
   -1114, -1114,  2713,    97, -1114,    48,   852,   853, -1114, -1114,
   -1114, -1114, -1114, -1114,   835,   425,   863,   731,   867,  6339,
     869,   848, -1114,   242,   732,    93,   448, -1114,    74, -1114,
   -1114, -1114,   456,   661, -1114,    26, -1114,   127,   865, -1114,
      33, -1114,   425,   736, -1114,  7499, -1114, -1114,    -6,   871,
    6919,   851,   855,    74,    29,    34,    40,   848,   471,   -12,
     532,    -3,   802, -1114, -1114,   804, -1114, -1114,   805,   809,
     814,   879,   815, -1114,   816, -1114, -1114,   425,   -28,  5469,
      23,  -130,  6339, -1114,   643,  3347,   638, -1114,   612,   539,
     425,   546,    42, -1114,   498, -1114, -1114,   882, -1114,    70,
     580,  5469, -1114, -1114,   128,    72, -1114,   781, -1114,   156,
   -1114,   282, -1114, -1114, -1114,   892,   266, -1114, -1114,   894,
     895, -1114,   635,   683,   899,   900, -1114,   688,   902,   904,
     873, -1114, -1114, -1114, -1114, -1114,   905,   425,    53,  6339,
     903,   -14, -1114,   109,   162, -1114, -1114,   -43,   -43, -1114,
   -1114, -1114, -1114, -1114,    60, -1114,   440, -1114,    93,    93,
      93,  7499,   901,  1154,   885,    77,   866,   866,   456, -1114,
     908,    73,   883, -1114, -1114,   817,   130,  3347, -1114, -1114,
      41,   909, -1114, -1114, -1114, -1114,   525, -1114,   791, -1114,
   -1114,   468,   448, -1114,   732,  7499,    74, -1114, -1114,   456,
     425,  7499, -1114,  7499,     4,   907, -1114,   907, -1114, -1114,
     125, -1114,   601, -1114, -1114, -1114,   785, -1114,  7499,  7499,
    7499, -1114,  7499,  7499,  7499,   881,   921,  6339,   425, -1114,
   -1114,     8, -1114,  4889,   923,   927,   928,   929,    50, -1114,
     695, -1114, -1114, -1114, -1114, -1114,   552,  6049,   723,   875,
    7499, -1114, -1114, -1114, -1114, -1114, -1114, -1114, -1114, -1114,
    7499,  7499,  7499, -1114,  6339, -1114,  6339,  6339, -1114, -1114,
    6339, -1114, -1114,  6339, -1114,   639,   934, -1114,   918,   939,
     275,   940,   707,   707,  7499, -1114, -1114,  6049, -1114, -1114,
     700, -1114,   700,   552, -1114,   872,   936,   937, -1114, -1114,
     137, -1114,   944,   944,   847,   949, -1114,   149,   944, -1114,
   -1114,   950,   950,   152,   950,   950,   951,    56,   951,   951,
   -1114,   160, -1114,   445,   952, -1114, -1114, -1114,   915, -1114,
     721, -1114, -1114, -1114, -1114, -1114, -1114, -1114, -1114, -1114,
     954,   956, -1114,   959,   877,   878, -1114,   566, -1114, -1114,
     963, -1114,   955,   781, -1114, -1114,   860, -1114,   868,   301,
     448, -1114,    74,    71,   163,   819,   962,    52,   136, -1114,
   -1114, -1114,   136,   107,   893, -1114, -1114,   967,   425,   425,
    7499,   946,    74, -1114,  7209, -1114,   425,   425,    65,    -9,
     111,    66,  3664, -1114, -1114,    96,  3981,  6339, -1114, -1114,
   -1114,   969, -1114, -1114,   958,   654,   162,   490,   425, -1114,
   -1114, -1114,   973,   974,   941,   884, -1114, -1114, -1114,   975,
     275,   694,   701, -1114, -1114, -1114,   910, -1114, -1114,   729,
   -1114,   498,   621, -1114, -1114,    79,    79,   961,   950,   720,
   -1114, -1114,   970, -1114, -1114, -1114,   971,   950,   724, -1114,
   -1114,   -57,   -57,   950,   725, -1114, -1114, -1114,   976, -1114,
   -1114,   168,    78, -1114, -1114,   950,   727, -1114, -1114, -1114,
   -1114, -1114,   944, -1114,   950,   874, -1114, -1114, -1114,   887,
     721, -1114, -1114, -1114, -1114,  3347,   989, -1114, -1114,    73,
      74, -1114,   880,    74, -1114,   946, -1114, -1114, -1114, -1114,
     101, -1114, -1114,   163,   641,   998,   468,   468,   917, -1114,
   -1114,    51, -1114, -1114,   911,   924, -1114,   981,  1010,   990,
   -1114, -1114,   977,   942, -1114,   425, -1114, -1114, -1114, -1114,
     425, -1114,  7499,  1011,  1017, -1114,  1018, -1114,   444,   114,
     373, -1114,  6049, -1114, -1114, -1114, -1114, -1114, -1114,   953,
    1012,  7499,  6339,  6339,    82,   342,  7499, -1114, -1114, -1114,
    1023,   707, -1114,   552, -1114, -1114,  1024, -1114,   951,   659,
    1025, -1114,   951,   912,   913, -1114, -1114, -1114,   951,   592,
    1026,   950,   760, -1114,   950, -1114, -1114,   951, -1114,   906,
     466,  1013, -1114,    83,    74, -1114,   679,   569,   995, -1114,
    1035, -1114,   790,    71,   966, -1114, -1114,  1031,    52,   468,
     960, -1114,   468, -1114,    74, -1114,  1010,  8324,  3347,   741,
     425, -1114, -1114,  3664, -1114, -1114, -1114, -1114,  7499, -1114,
     416,   275, -1114, -1114,   425, -1114,  1039,   287,  1042, -1114,
     721, -1114, -1114, -1114, -1114,   487, -1114,   425,   884, -1114,
   -1114, -1114, -1114, -1114,  1028, -1114, -1114,   914,   916, -1114,
   -1114, -1114, -1114, -1114, -1114, -1114, -1114, -1114,   951, -1114,
   -1114,   950, -1114, -1114, -1114, -1114,   734, -1114,   957,  1045,
     646,   456, -1114, -1114,   948,   483,   983, -1114,   964, -1114,
      52,   468,   631,   763, -1114,  1050, -1114, -1114,   301,   873,
     884, -1114, -1114, -1114, -1114, -1114,  6339,    82, -1114,   581,
     801,   761,   762,   768, -1114, -1114, -1114,  1053,  1062, -1114,
   -1114, -1114, -1114,   965, -1114,  3347, -1114, -1114, -1114,   498,
   -1114,  1064, -1114,    22,    27,   483, -1114,   818, -1114,   824,
   -1114,    52,  3347,  1067, -1114, -1114, -1114, -1114,  8324, -1114,
    1068, -1114,  1040,   773, -1114, -1114,  -101,   -85, -1114,   765,
   -1114, -1114, -1114, -1114, -1114,    15, -1114, -1114, -1114, -1114,
     456,    85,    74,  1048, -1114,  1048,  1048,  1048, -1114,   534,
    1071,  1762,   301,    74, -1114, -1114,   873,  1080,  1061,   779,
   -1114, -1114, -1114,   581,   830, -1114, -1114,   792,  1084, -1114,
     808,  1063,  1065,  1063,  1066, -1114, -1114, -1114,  3347, -1114,
     837, -1114, -1114, -1114,   811, -1114, -1114,   787,   756,  1027,
   -1114, -1114, -1114, -1114, -1114,    74, -1114, -1114, -1114, -1114,
   -1114, -1114, -1114, -1114, -1114, -1114,    86,  2396, -1114,  1070,
   -1114,  -141,   498,  -164, -1114, -1114,   813, -1114,  1088, -1114,
   -1114, -1114, -1114, -1114, -1114, -1114,   756, -1114,  2079,  -141,
   -1114,   498, -1114, -1114,   793, -1114
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
      70,    70,     0,   747,     0,   440,   586,   226,   222,   223,
     224,   225,    70,   712,   465,   460,     0,     0,     0,     0,
       0,   331,   329,    92,     0,     0,   258,     0,     0,    70,
     608,   461,   462,     0,     0,    95,   466,   463,     0,     0,
     259,     0,     0,   607,   257,   299,     0,   310,   714,     0,
     311,   312,   309,   300,   306,   298,   291,   313,   301,   713,
     305,   308,   307,   302,   314,   303,     0,   304,   544,   545,
     546,   315,   316,   322,   319,   318,   320,   715,   325,   326,
     321,   323,   317,   324,   292,   327,   332,   333,   330,   288,
     328,   335,   336,   337,   338,   353,   339,   340,   341,   342,
     348,   343,   289,   290,   350,   349,   354,   334,   355,   351,
     356,   352,   287,   344,   345,   346,   347,   357,   464,   473,
     472,   471,   470,   469,   468,   467,   486,   487,   488,   489,
     483,   484,     0,     0,     0,     0,   260,   261,   262,   263,
     264,   265,   266,   267,   268,     0,     0,   377,   378,   385,
     386,     0,   187,   188,   184,   183,   186,   185,     0,     0,
       0,     2,     5,     6,     7,     8,     0,     4,    79,    72,
      73,    74,    75,    70,   162,    76,    77,    78,    81,   164,
     166,   168,   170,   752,   160,   172,     0,   191,   173,   196,
     197,   179,   203,   177,   208,   178,   211,   174,   175,   176,
     519,   255,   236,   533,   243,   244,   245,   238,   237,   524,
     254,   526,     0,     0,     0,     0,     0,     0,     0,   282,
     285,   293,     0,   294,   280,   283,   295,   281,   284,   286,
     296,   529,     0,   362,   363,     0,   364,     0,   365,   269,
       0,   458,   474,   475,     0,   459,     0,   527,   531,   532,
     534,   537,   540,   578,   549,   543,   551,   552,   580,    91,
     579,   581,   583,   589,   582,   587,   592,   593,   591,   598,
       0,   599,   600,   601,   602,   525,   630,   528,   716,   717,
     721,   722,   256,     9,    70,     0,     0,   507,     0,    81,
       0,   508,   510,   513,   518,     0,    91,   746,     0,     0,
     452,     0,   453,   454,     0,     0,     0,     0,    70,     0,
      71,   309,   167,     0,     0,     0,    83,     0,    95,     0,
       0,     0,     0,     0,   215,   745,     0,     0,    53,    42,
      51,    42,    56,    42,     0,    48,    49,     0,     0,    93,
      96,    97,     0,     0,     0,   216,   629,     0,   625,   624,
     623,     0,     0,     0,     0,     0,   597,     0,     0,     0,
      70,   744,   739,   736,     0,     0,     0,     0,     0,     0,
     536,   535,     1,     3,     0,     0,     0,   516,     0,   190,
     194,   195,   202,   207,   210,    70,    70,    70,   213,   212,
     214,   182,    90,     0,   227,   228,   229,   230,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    95,
       0,     0,     0,     0,     0,     0,     0,   550,     0,     0,
       0,   590,     0,   518,     0,   753,   516,   161,   217,   516,
     516,   530,   748,   441,   297,   451,   269,     0,     0,    80,
     270,   450,    70,    95,   506,     0,     0,   712,   714,   713,
     715,   636,   631,   635,     0,   520,     0,   579,     0,     0,
       0,    42,   789,     0,   107,     0,     0,    47,     0,    55,
      54,    52,     0,     0,   201,     0,   111,     0,   112,   218,
     231,   220,     0,   579,   619,     0,   620,   621,   618,     0,
       0,     0,     0,     0,     0,     0,     0,    42,     0,   568,
       0,   568,     0,   595,   596,   329,   251,   252,   325,   326,
     321,   247,   328,   249,     0,   248,   246,   250,    61,     0,
       0,     0,     0,   737,     0,    70,   739,   725,     0,     0,
     568,     0,   568,   360,   368,   370,   369,     0,   367,     0,
       0,     0,   206,   204,   198,   198,   517,     0,   181,     0,
     169,   171,   209,   272,   742,     0,   743,   571,   741,     0,
       0,   575,     0,     0,     0,     0,   271,     0,     0,     0,
     406,   394,   395,   358,   396,   397,   371,   379,     0,     0,
       0,   482,   359,     0,   480,   538,   539,   541,   542,   548,
     547,   585,   584,   588,     0,   509,     0,    88,     0,     0,
       0,     0,   443,     0,     0,     0,     0,     0,     0,   456,
       0,     0,     0,   757,   758,     0,     0,    70,    84,   108,
       0,    85,   104,   106,   221,   712,     0,    38,     0,    50,
     241,     0,     0,   110,   107,     0,     0,   622,   233,     0,
     617,     0,   613,     0,     0,     0,   627,   629,   626,   628,
     107,    98,     0,   242,   239,   240,     0,   235,     0,     0,
       0,   606,     0,     0,     0,     0,     0,     0,   743,   740,
     724,    90,   731,     0,   191,   196,   203,   208,     0,   728,
       0,   726,   718,   720,   719,   366,     0,     0,     0,     0,
       0,   193,   192,    82,   157,   159,   156,   158,   165,   273,
       0,     0,     0,   274,     0,   278,     0,     0,   279,   275,
       0,   277,   276,     0,   413,   408,     0,   407,     0,     0,
     374,     0,   478,   478,     0,   609,    87,     0,   512,   511,
     515,   109,   514,   447,   442,     0,     0,   712,   632,   697,
     655,   705,   689,   689,   702,   700,   696,   655,   689,   701,
     695,   655,   655,   655,     0,     0,   658,     0,   658,   658,
     698,   655,   704,     0,     0,   521,   522,   637,   638,   654,
     640,   677,   644,   684,   645,   687,   688,   646,   647,   648,
       0,     0,   457,     0,     0,     0,    19,     0,    11,    13,
       0,    14,     0,     0,   759,   761,   763,   760,     0,   147,
       0,    43,     0,     0,     0,     0,   133,   134,   749,   503,
     504,   144,   140,     0,   148,   219,   232,     0,   616,   615,
       0,     0,     0,   103,     0,    86,   569,   570,     0,   603,
     579,     0,    70,   485,   738,   182,    70,     0,   723,   391,
     390,     0,   189,   205,     0,   199,   574,     0,   572,   565,
     576,   577,     0,     0,     0,    61,   409,   405,   380,   372,
     374,     0,     0,   384,   375,   455,     0,   481,   476,     0,
      89,   448,     0,   445,   449,     0,     0,     0,     0,     0,
     649,   656,     0,   694,   693,   703,     0,     0,     0,   673,
     692,   708,   708,     0,     0,   650,   653,   683,     0,   680,
     659,   655,   655,   670,   686,     0,     0,   681,   555,   556,
     557,   554,   689,   553,   655,     0,   564,   711,   523,     0,
     640,   641,   643,   611,   610,    70,     0,    20,    10,     0,
       0,    16,    40,     0,   762,     0,   105,    37,   751,    59,
      45,    57,    60,     0,   496,   144,     0,     0,     0,   492,
     497,   496,   490,   491,   498,     0,   137,     0,    35,     0,
     142,    94,     0,   154,   612,   614,   629,    99,   102,   100,
     101,   234,     0,     0,     0,   253,    62,    63,    67,    67,
       0,   730,     0,   732,   734,   735,   733,   729,   727,   387,
       0,     0,     0,     0,     0,   410,     0,   376,   382,   383,
       0,   478,   444,   447,   634,   633,     0,   651,   658,     0,
       0,   676,   658,     0,     0,   706,   707,   652,   658,   662,
       0,     0,     0,   671,     0,   672,   682,   658,   562,   561,
       0,     0,   642,     0,     0,    12,     0,    15,     0,    39,
     765,   764,   755,     0,     0,    44,   146,   144,   135,     0,
       0,   499,     0,   750,     0,   143,    35,     0,    70,   398,
     604,   605,   594,    70,    68,    69,    66,    65,     0,   180,
       0,   374,   388,   200,   573,   566,     0,   712,   414,   416,
     640,   422,   421,   412,   411,     0,   373,   381,    61,   477,
     446,   657,   668,   690,     0,   699,   679,     0,     0,   669,
     663,   664,   665,   666,   667,   661,   660,   674,   658,   675,
     685,   655,   559,   563,   639,    34,     0,    31,     0,    27,
       0,     0,    22,    17,     0,     0,   767,    58,     0,   145,
     500,     0,   496,     0,   141,   149,   151,   150,   155,     0,
      61,   399,    64,   392,   393,   389,     0,     0,   417,     0,
       0,     0,     0,   418,   423,   426,   424,     0,     0,   709,
     710,   678,   560,     0,    21,    70,    24,    25,    26,    23,
      18,    29,    41,   138,   138,   756,   778,     0,   754,   771,
      46,   501,    70,     0,   494,   502,   495,    36,     0,   404,
     400,   402,     0,   127,   567,   415,     0,     0,   432,     0,
     430,   434,   427,   425,   428,     0,   420,   419,   479,   691,
       0,     0,     0,   136,   139,   136,   136,   136,   779,     0,
       0,    70,   493,     0,   153,   152,     0,     0,     0,   113,
     128,   433,   435,     0,     0,   437,   438,     0,    32,    28,
       0,     0,     0,     0,     0,   769,   770,   768,    70,   772,
       0,   777,   766,   773,     0,   401,   403,   129,     0,   122,
     114,   429,   431,   436,   439,     0,    30,   786,   783,   785,
     788,   781,   782,   784,   787,   780,     0,    70,   505,     0,
     131,     0,   125,     0,   132,   123,     0,   163,     0,   775,
     130,   120,   121,   115,   118,   119,   116,    33,    70,     0,
     117,   126,   774,   776,     0,   124
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
   -1114, -1114,   122, -1114, -1114, -1114,   169, -1114, -1114, -1114,
      62, -1114, -1114,    44,  -913, -1114, -1114,  -238, -1114, -1114,
   -1114, -1114,   628, -1114,    54,  -738, -1114,    46,   124,   -22,
   -1114, -1114, -1114, -1114, -1114, -1114,   451, -1114,  -171,  -128,
   -1114, -1114,  -214, -1114,   283, -1114, -1114,   472,   306,  -516,
     224,     0,   508, -1114, -1114, -1114, -1114,  -189, -1114, -1114,
   -1114, -1114, -1114, -1114, -1114,   299, -1114, -1114,  -788,   -61,
   -1114,  -613, -1114, -1114, -1114, -1114, -1114, -1114, -1114,  1102,
     730,   735,     1,  -769, -1114, -1114,  -151,  -150, -1114,  -142,
     945, -1114,   572,  -138, -1114, -1114,  -137, -1114,   947, -1114,
   -1114, -1114, -1114,   -16, -1114,   484, -1114,  -357, -1114, -1114,
   -1114, -1114, -1114, -1114, -1114, -1114, -1114, -1114, -1114,  -999,
   -1114,  -295, -1114, -1114, -1114, -1114, -1114, -1114, -1114, -1114,
   -1114, -1114, -1114, -1114, -1114, -1114, -1114, -1114, -1114, -1114,
   -1114, -1114, -1114, -1114,   435,   438, -1114, -1114,  -784, -1114,
   -1114, -1114, -1114, -1114, -1114, -1114, -1114, -1114, -1114, -1114,
   -1114,  -322, -1114, -1114, -1114, -1114, -1114,  -103, -1114,  -102,
   -1114, -1114, -1114, -1114, -1114, -1114, -1114,   -21, -1114, -1114,
   -1114, -1114,    45, -1114, -1114, -1114, -1114,  -106, -1114, -1114,
   -1114, -1114, -1114, -1114,   126, -1114, -1114,  -293, -1114, -1114,
   -1114,  -663, -1114, -1114, -1114, -1114, -1114,  -291, -1114, -1114,
   -1114,   177, -1114, -1114,  -685, -1114, -1114,   533,   226, -1114,
    -265,  -212,   667, -1114,   820,   526, -1114,   764,   669,   417,
     420,   738, -1114,    16, -1114, -1114, -1114, -1114, -1114,  -707,
   -1114, -1114, -1114, -1114, -1114, -1114, -1114,  -316,  -371,  -129,
      13,  -296, -1114, -1114, -1114,   886,   737, -1114, -1114, -1114,
   -1114, -1114, -1114, -1114,   315, -1114, -1114, -1114, -1114, -1114,
   -1114, -1114, -1114, -1114,   -39, -1114,   340,  -476,  -921,  -544,
     358, -1114,  -829, -1114, -1114,  -710,  -240,  -705, -1114, -1114,
   -1114, -1114, -1114, -1114, -1114, -1114, -1114,  -671, -1114, -1114,
   -1114, -1114,   259, -1114,  -310, -1114, -1114, -1114, -1114, -1114,
   -1114,   625, -1114,   316,   794,  -283,   629,  -510, -1114, -1114,
   -1114,  -342, -1114, -1114, -1114, -1114, -1114, -1114, -1114, -1114,
   -1114, -1114, -1114, -1114, -1114, -1114, -1113, -1114,   -11, -1114,
   -1114,   -89, -1114, -1114, -1114
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,   160,  1251,   162,   163,   787,   788,   789,  1037,  1168,
     790,  1123,   791,  1055,   626,   164,  1039,   469,   165,  1045,
     334,   335,   336,   940,   941,   666,   976,   977,  1066,   166,
     167,   168,   169,   170,   171,   172,   287,   596,   173,   174,
     175,   176,   342,   650,   651,   969,   177,   621,   622,   618,
     288,   444,   477,  1259,  1260,  1299,  1296,  1293,  1284,  1285,
    1281,  1300,  1229,  1280,  1230,   633,   634,   806,   957,  1215,
     960,   944,   619,   963,  1135,  1059,   698,   179,  1249,   180,
     181,   182,   183,   184,   672,   185,   186,   187,   188,   189,
     190,   191,   691,   192,   193,   542,   194,   195,   196,   197,
     393,   198,   199,   200,   478,   479,   623,   201,   637,   202,
     203,   204,   205,   206,   207,   513,   514,   515,   208,   209,
     210,   211,   212,   213,   214,   215,   216,   217,   218,   219,
     220,   221,   222,   223,   224,   225,   226,   227,   228,   229,
     230,   231,   232,   233,   535,   536,   537,   719,   863,   234,
     235,   576,   859,  1086,   864,   236,   237,  1071,   238,   841,
    1072,  1189,   573,   574,  1140,  1141,  1190,  1191,  1192,   575,
     715,   855,  1083,   716,   717,   856,  1078,  1079,  1206,  1084,
    1085,  1153,  1198,  1155,  1156,  1199,  1261,  1200,  1201,  1207,
     304,   305,   734,   872,   873,   306,   307,   239,   240,   241,
     242,   867,   583,   243,   580,   244,   245,   246,   954,  1184,
    1185,   955,   809,   810,   811,  1186,   291,   292,   293,   729,
     294,   547,   295,   247,   454,   764,   248,   249,   250,   251,
     252,   253,   254,   255,   417,   590,   256,   257,   258,   516,
     915,  1113,   916,   917,   559,   564,   568,   584,   561,   562,
     259,   260,   261,   262,   263,   921,   264,   265,   266,   267,
     502,   503,   504,   268,   973,   269,   270,   271,   272,   273,
     274,   489,   490,   481,   812,   348,   349,   350,   275,   452,
     766,   920,   922,   767,   768,   880,   881,   899,   900,  1020,
    1105,   769,   770,   771,   772,   773,   774,   883,   775,   776,
     777,   778,  1015,   779,   276,   277,   278,   279,   280,   281,
     526,   527,   678,   679,   362,   363,   524,   557,   558,   364,
     326,   282,   958,   942,   283,  1126,   616,   798,   934,  1041,
    1178,  1179,  1247,  1221,  1252,  1288,  1253,  1175,  1176,  1272,
    1268,  1269,  1275,  1271,   463
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     178,   289,   290,   347,   453,   361,   435,   337,   436,   324,
     437,   534,   669,  -730,   296,   391,   649,  1036,   807,   423,
    1040,   457,   378,   379,   959,   345,   538,   431,   555,   560,
     630,   380,   565,   653,   569,   382,   383,   889,   654,   498,
     500,   891,   892,   895,   655,   447,   684,   483,   340,   529,
     531,   907,   427,   903,   904,   836,   914,   720,   392,   625,
     868,   738,   972,   979,   725,   636,  1234,   660,   641,   971,
     975,   662,   884,  1080,   665,   686,   997,   890,   820,   523,
     545,   877,   341,   689,   572,  1013,  1152,  1115,   610,  1239,
    1287,  1032,  1235,   470,   428,   471,   465,   938,   901,   625,
     625,  -180,   948,   737,   459,   625,  1043,   429,  1077,  -506,
     297,   667,   800,   723,  1291,  -603,   783,   995,   814,   945,
     298,  1116,   161,   286,   624,   340,   949,   949,  -506,  1294,
     822,   299,   950,   950,   823,   319,  1137,   690,  -496,   689,
     877,  1133,   570,  1213,  1295,   784,   951,  1064,  1216,   724,
    1197,   315,   877,   491,   320,   877,   429,   834,   627,   341,
     492,  1292,  -136,   877,  1289,   571,   943,   658,   956,   659,
     638,   877,   430,   448,   805,  1065,   -90,   423,   785,   939,
     387,   631,   972,   652,   449,  1303,   317,   448,   -90,   346,
     795,  1023,  1025,   690,   318,   579,   310,  1231,   449,  1203,
     423,   631,   450,   540,  1029,   786,   443,   548,   721,   902,
     632,   952,   952,  1232,   598,  1204,   450,   599,   600,   953,
     953,   430,  1014,   612,   694,   688,  1080,   448,   448,   603,
     -90,   448,   285,   448,  1197,   -90,   448,   323,   449,   449,
     -90,  1028,   449,  1044,   449,   523,   392,   449,   714,  -162,
     392,  1148,   695,   597,   423,    29,   450,   450,   945,   656,
     450,  1047,   450,   316,   796,   450,   617,  1225,   321,   300,
     301,   302,   303,   563,   322,   567,   443,  1214,   412,   413,
     460,   461,  1214,   464,   289,   290,   696,  1145,   797,   697,
     624,   837,   418,   313,   979,   453,   392,   296,   325,  1240,
     314,   792,   285,  1092,   414,   415,   700,  1096,   418,   445,
    1254,   418,  1236,  1099,   418,   497,   310,   418,  -162,  -162,
    1237,   296,  1110,  1112,   418,   338,   816,   988,   455,   534,
     455,  1004,  1005,   849,  1048,   850,   851,  -180,  1089,   852,
     451,   458,   853,   418,   538,   649,   418,   339,  1024,   418,
    1157,   475,  1286,   418,   418,   480,   482,   488,   387,   543,
     418,   521,   418,   549,   494,   495,   496,   830,   499,   501,
     517,   518,   520,   418,   673,   674,   871,   552,   530,   532,
     418,   593,   539,   675,   846,   418,   418,   676,   677,   482,
     418,   874,  -579,   387,   845,   387,   387,   671,   451,   602,
     451,  1162,  1193,  1161,   701,   702,   286,   878,   869,   374,
     374,   879,   556,   556,   556,   556,   556,   556,   556,   887,
     577,   578,   893,   888,   530,  1241,   894,  1242,  1243,  1244,
     905,   591,     6,   628,   906,   594,  1130,   343,  1021,  1132,
     360,   522,  1022,   445,   726,   727,  -532,  -532,  -532,  -532,
    -532,   375,   375,   344,     2,   296,     3,     4,   624,  1042,
      18,   376,  1068,   -90,   -90,   -90,   -90,   -90,   -90,   -90,
    -297,   804,   556,   313,   354,   657,   658,  1064,   659,   805,
     314,   -90,   346,   152,   153,   154,   155,   156,   157,   613,
     614,   615,   937,     2,   346,     3,     4,   351,   640,   352,
     908,   372,   835,   644,    30,  1065,   353,     7,     8,     9,
      10,    11,   652,   355,   896,   897,   842,   394,  1181,   395,
     396,   908,   397,   -90,  1149,   356,   521,   373,   -90,   801,
     802,   357,   520,   -90,   358,   668,   661,   658,  1150,   659,
     365,    38,    39,   682,   658,   392,   659,   693,    43,   377,
     683,   658,   327,   659,   530,    26,   870,   366,     2,   392,
       3,     4,   861,   862,   367,   453,   453,   765,   765,   781,
     928,   929,     7,     8,     9,    10,    11,   368,   152,   153,
     154,   155,   156,   157,   328,    66,   418,     2,   369,     3,
       4,   385,   556,   386,  1118,   300,   301,   302,   303,   392,
     817,     7,     8,     9,    10,    11,  1100,  1101,  1102,  1103,
    1104,   387,  1119,   398,   530,   808,   480,    40,   799,   792,
     627,  1075,  1076,   627,    44,  1002,  1003,   399,  1120,   909,
     910,   911,  1150,   400,   329,   401,   912,   330,  1007,   705,
     706,   913,   707,  1081,  1082,   402,   871,  1011,   480,    26,
     909,   910,   911,  1017,   818,   403,   819,  1111,   331,   404,
     332,   874,   913,  1093,  1094,  1026,     7,     8,     9,    10,
      11,   826,   827,   828,   785,   829,   482,   831,   423,  1121,
     668,   948,   333,  1117,   802,   982,   983,   708,   706,   405,
     707,   948,   711,   706,   984,   707,   991,   406,   985,   986,
     407,   786,   408,   482,   978,  -361,   949,   409,   981,  1143,
    1144,    40,   950,   530,   847,   848,   949,   556,    44,   556,
     556,   328,   950,   556,   627,   951,   556,   843,   658,   410,
     659,  1182,   416,  1001,   658,   951,   659,   530,  1163,   802,
    1173,  1174,  1166,   411,   627,   418,   136,   137,   138,   139,
     140,   141,   142,   143,   144,   419,   -90,   -90,   -90,   -90,
     -90,   -90,  1136,  1169,   436,   420,   437,  1187,   802,  1183,
     422,   329,   545,   424,   330,  1194,     7,     8,     9,    10,
      11,  1107,   425,  1167,  1109,   426,  1151,  -109,   808,  1245,
    1246,   952,   432,   932,   433,   331,   438,   332,   439,   953,
     440,   952,   152,   153,   154,   155,   156,   157,   441,   953,
     418,  1069,  1266,   802,   980,  1278,   802,  1297,   802,   333,
      68,    69,    70,   730,   732,   731,   731,   370,   371,   585,
     586,   646,   648,   965,   587,   588,   446,   970,   462,   467,
     136,   137,   138,   139,   140,   141,   142,   143,   144,   466,
     668,   468,   472,   474,   392,   473,   493,   525,   533,   541,
     546,   553,   589,   595,   601,   604,   605,   606,   607,   608,
     635,   609,   611,   617,   639,   642,   663,   645,  -555,  -556,
    1196,   647,  -327,  -557,   522,   670,   685,   739,  -554,  -553,
     664,   687,   346,  1224,   443,   436,   699,   437,   703,   571,
     704,  1282,   627,   709,   733,   710,   712,   722,   740,   713,
     718,   736,   782,   627,   800,   793,   794,   803,   824,   741,
     821,   832,   742,   743,   285,   833,  1033,   808,  -732,   744,
     808,   808,  -734,  -735,  -733,   978,   745,   838,   857,  1301,
     844,   858,   854,   860,   865,   866,   631,   882,   746,   875,
     876,   885,   886,   877,   898,   627,   918,   919,   923,   933,
     924,   747,   925,   748,   926,   927,   930,   947,   935,   946,
     962,   964,   966,   989,  -579,   749,   739,   990,   992,   993,
     996,   994,   750,   998,  1006,  1060,   665,  1000,   751,   752,
     999,  1008,  1034,  1009,  1010,  1012,  1018,   740,  1027,  1019,
    1030,  1031,  1046,  1049,  1074,   556,   556,  1053,   741,  1087,
    1052,   742,   743,  1054,  1051,  1061,  1056,  1057,   744,   753,
    1058,  1062,  1038,  1063,  1073,   745,  1088,  1070,  1091,  1095,
    1106,  1108,  -558,   808,  1097,  1098,   808,   746,  1124,  1114,
     802,  1125,  1128,  1129,  1146,  1139,  1131,  1147,  1165,  1172,
     747,  1158,   748,  1164,  1177,  1188,  1202,  1208,  1159,  1138,
    1160,  1203,  1204,  1205,   749,  1180,  1209,  1212,  1219,  1220,
    1223,   750,  1227,  1226,  1248,  1210,  1233,   751,   752,  1228,
     956,   482,  1171,   570,  1257,  1263,  1258,  1265,  1264,  1267,
    1277,  1270,  1274,  1290,  1279,  1298,  1283,  1127,  1035,  1122,
    1134,   629,  1305,  1067,   813,   967,   936,   825,   753,  1142,
    1304,   735,   961,  1217,   312,   808,   550,   692,   381,   815,
     384,   839,   551,  1255,   840,  1256,  1195,  1262,  1050,  1090,
    1154,   728,   620,   780,   754,   755,  1170,   456,   756,   757,
     544,   758,   759,   760,   761,   974,   762,   763,   581,   421,
     931,  1016,   987,   681,  1273,   680,   592,   359,   528,   556,
       2,     0,     3,     4,  1218,   309,  1211,     0,     0,     0,
       0,  1238,     0,     6,     7,     8,     9,    10,    11,     0,
      13,     0,     0,  1222,     0,    14,     0,     0,     0,     0,
      15,     0,     0,     0,     0,    16,     0,     0,     0,     0,
       0,    18,    19,     0,     0,     0,     0,     0,    21,    22,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   178,    26,   754,   755,     0,     0,   756,   757,     0,
     758,   759,   760,   761,     0,   762,   763,     0,     0,     0,
       0,     0,     0,     0,     0,    30,    31,    32,     0,  1276,
       0,     0,    33,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    36,     0,     0,    37,     0,     0,   178,     0,     0,
       0,     0,    38,    39,    40,     0,     0,     0,     0,    43,
       0,    44,     0,     0,     0,     0,     0,     0,   178,     0,
       0,     0,     0,     0,     0,    45,    46,    47,    48,    49,
      50,    51,   311,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   122,   123,   124,   125,   126,   127,   128,   129,
     130,   131,     0,   132,   133,     0,     0,   134,   135,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   136,   137,   138,   139,   140,   141,   142,
     143,   144,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   145,   146,   147,
     148,     0,     0,     0,   149,   150,   151,     0,   284,     0,
       0,     2,     0,     3,     4,     0,     5,     0,     0,     0,
       0,     0,     0,     0,     6,     7,     8,     9,    10,    11,
      12,    13,   158,   159,     0,   476,    14,     0,     0,     0,
       0,    15,     0,     0,     0,     0,    16,     0,     0,     0,
      17,     0,    18,    19,    20,     0,     0,     0,     0,    21,
      22,     0,     0,     0,    23,    24,     0,     0,     0,     0,
       0,     0,    25,    26,    27,     0,     0,     0,     0,     0,
       0,    28,     0,     0,     0,     0,     0,     0,     0,    29,
       0,     0,     0,     0,     0,     0,    30,    31,    32,     0,
       0,     0,     0,    33,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    34,     0,    35,     0,
       0,     0,    36,     0,     0,    37,     0,     0,     0,     0,
       0,     0,     0,    38,    39,    40,     0,    41,     0,    42,
      43,     0,    44,     0,   285,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    92,    93,    94,    95,    96,    97,    98,
      99,   100,   101,   102,   103,   104,   105,   106,   107,   108,
     109,   110,   111,   112,   113,   114,   115,   116,   117,   118,
     119,   120,   121,   122,   123,   124,   125,   126,   127,   128,
     129,   130,   131,     0,   132,   133,     0,     0,   134,   135,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   136,   137,   138,   139,   140,   141,
     142,   143,   144,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   145,   146,
     147,   148,     0,     0,     0,   149,   150,   151,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   152,   153,   154,
     155,   156,   157,   158,   159,     1,     0,     0,     2,     0,
       3,     4,     0,     5,     0,     0,     0,     0,     0,     0,
       0,     6,     7,     8,     9,    10,    11,    12,    13,     0,
       0,     0,     0,    14,     0,     0,     0,     0,    15,     0,
       0,     0,     0,    16,     0,     0,     0,    17,     0,    18,
      19,    20,     0,     0,     0,     0,    21,    22,     0,     0,
       0,    23,    24,     0,     0,     0,     0,     0,     0,    25,
      26,    27,     0,     0,     0,     0,     0,     0,    28,     0,
       0,     0,     0,     0,     0,     0,    29,     0,     0,     0,
       0,     0,     0,    30,    31,    32,     0,     0,     0,     0,
      33,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    34,     0,    35,     0,     0,     0,    36,
       0,     0,    37,     0,     0,     0,     0,     0,     0,     0,
      38,    39,    40,     0,    41,     0,    42,    43,     0,    44,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    45,    46,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    87,    88,    89,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   100,   101,
     102,   103,   104,   105,   106,   107,   108,   109,   110,   111,
     112,   113,   114,   115,   116,   117,   118,   119,   120,   121,
     122,   123,   124,   125,   126,   127,   128,   129,   130,   131,
       0,   132,   133,     0,     0,   134,   135,     0,     0,     0,
       0,     0,     0,     0,  1250,     0,     0,     0,     0,     0,
       0,   136,   137,   138,   139,   140,   141,   142,   143,   144,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   145,   146,   147,   148,     0,
       0,     0,   149,   150,   151,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   152,   153,   154,   155,   156,   157,
     158,   159,     1,     0,     0,     2,     0,     3,     4,     0,
       5,     0,     0,     0,     0,     0,     0,     0,     6,     7,
       8,     9,    10,    11,    12,    13,     0,     0,     0,     0,
      14,     0,     0,     0,     0,    15,     0,     0,     0,     0,
      16,     0,     0,     0,    17,     0,    18,    19,    20,     0,
       0,     0,     0,    21,    22,     0,     0,     0,    23,    24,
       0,     0,     0,     0,     0,     0,    25,    26,    27,     0,
       0,     0,     0,     0,     0,    28,     0,     0,     0,     0,
       0,     0,     0,    29,     0,     0,     0,     0,     0,     0,
      30,    31,    32,     0,     0,     0,     0,    33,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      34,     0,    35,     0,     0,     0,    36,     0,     0,    37,
       0,     0,     0,     0,     0,     0,     0,    38,    39,    40,
       0,    41,     0,    42,    43,     0,    44,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,     0,   132,   133,
       0,  1302,   134,   135,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   136,   137,
     138,   139,   140,   141,   142,   143,   144,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   145,   146,   147,   148,     0,     0,     0,   149,
     150,   151,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   152,   153,   154,   155,   156,   157,   158,   159,     1,
       0,     0,     2,     0,     3,     4,     0,     5,     0,     0,
       0,     0,     0,     0,     0,     6,     7,     8,     9,    10,
      11,    12,    13,     0,     0,     0,     0,    14,     0,     0,
       0,     0,    15,     0,     0,     0,     0,    16,     0,     0,
       0,    17,     0,    18,    19,    20,     0,     0,     0,     0,
      21,    22,     0,     0,     0,    23,    24,     0,     0,     0,
       0,     0,     0,    25,    26,    27,     0,     0,     0,     0,
       0,     0,    28,     0,     0,     0,     0,     0,     0,     0,
      29,     0,     0,     0,     0,     0,     0,    30,    31,    32,
       0,     0,     0,     0,    33,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    34,     0,    35,
       0,     0,     0,    36,     0,     0,    37,     0,     0,     0,
       0,     0,     0,     0,    38,    39,    40,     0,    41,     0,
      42,    43,     0,    44,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    82,    83,    84,    85,    86,    87,
      88,    89,    90,    91,    92,    93,    94,    95,    96,    97,
      98,    99,   100,   101,   102,   103,   104,   105,   106,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   121,   122,   123,   124,   125,   126,   127,
     128,   129,   130,   131,     0,   132,   133,     0,     0,   134,
     135,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   136,   137,   138,   139,   140,
     141,   142,   143,   144,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   145,
     146,   147,   148,     0,     0,     0,   149,   150,   151,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   152,   153,
     154,   155,   156,   157,   158,   159,   442,     0,     0,     2,
       0,     3,     4,     0,   309,     0,     0,     0,     0,     0,
       0,     0,     6,     7,     8,     9,    10,    11,    12,    13,
       0,     0,     0,     0,    14,     0,     0,     0,     0,    15,
       0,     0,     0,     0,    16,     0,     0,     0,     0,     0,
      18,    19,     0,     0,     0,     0,     0,    21,    22,     0,
       0,     0,    23,     0,     0,     0,     0,     0,     0,     0,
      25,    26,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    29,     0,     0,
       0,     0,     0,     0,    30,    31,    32,     0,     0,     0,
       0,    33,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   443,     0,     0,     0,
      36,     0,     0,    37,     0,     0,     0,     0,     0,     0,
       0,    38,    39,    40,     0,    41,     0,     0,    43,     0,
      44,     0,   285,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    45,    46,    47,    48,    49,    50,
      51,   311,    53,    54,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      91,    92,    93,    94,    95,    96,    97,    98,    99,   100,
     101,   102,   103,   104,   105,   106,   107,   108,   109,   110,
     111,   112,   113,   114,   115,   116,   117,   118,   119,   120,
     121,   122,   123,   124,   125,   126,   127,   128,   129,   130,
     131,     0,   132,   133,     0,     0,   134,   135,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   136,   137,   138,   139,   140,   141,   142,   143,
     144,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   145,   146,   147,   148,
       0,     0,     0,   149,   150,   151,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   152,   153,   154,   155,   156,
     157,   158,   159,   308,     0,     0,     2,     0,     3,     4,
       0,   309,     0,     0,     0,     0,     0,     0,     0,     6,
       7,     8,     9,    10,    11,   310,    13,     0,     0,     0,
       0,    14,     0,     0,     0,     0,    15,     0,     0,     0,
       0,    16,     0,     0,     0,     0,     0,    18,    19,     0,
       0,     0,     0,     0,    21,    22,     0,     0,     0,    23,
       0,     0,     0,     0,     0,     0,     0,    25,    26,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    29,     0,     0,     0,     0,     0,
       0,    30,    31,    32,     0,     0,     0,     0,    33,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    36,     0,     0,
      37,     0,     0,     0,     0,     0,     0,     0,    38,    39,
      40,     0,    41,     0,     0,    43,     0,    44,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    45,    46,    47,    48,    49,    50,    51,   311,    53,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    66,    67,    68,    69,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      84,    85,    86,    87,    88,    89,    90,    91,    92,    93,
      94,    95,    96,    97,    98,    99,   100,   101,   102,   103,
     104,   105,   106,   107,   108,   109,   110,   111,   112,   113,
     114,   115,   116,   117,   118,   119,   120,   121,   122,   123,
     124,   125,   126,   127,   128,   129,   130,   131,     0,   132,
     133,     0,     0,   134,   135,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   136,
     137,   138,   139,   140,   141,   142,   143,   144,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   145,   146,   147,   148,     0,     0,     0,
     149,   150,   151,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   152,   153,   154,   155,   156,   157,   158,   159,
     308,     0,     0,     2,     0,     3,     4,     0,   309,     0,
       0,     0,     0,     0,     0,     0,     6,     7,     8,     9,
      10,    11,    12,    13,     0,     0,     0,     0,    14,     0,
       0,     0,     0,    15,     0,     0,     0,     0,    16,     0,
       0,     0,     0,     0,    18,    19,     0,     0,     0,     0,
       0,    21,    22,     0,     0,     0,    23,     0,     0,     0,
       0,     0,     0,     0,    25,    26,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    29,     0,     0,     0,     0,     0,     0,    30,    31,
      32,     0,     0,     0,     0,    33,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    36,     0,     0,    37,     0,     0,
       0,     0,     0,     0,     0,    38,    39,    40,     0,    41,
       0,     0,    43,     0,    44,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    45,    46,
      47,    48,    49,    50,    51,   311,    53,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    82,    83,    84,    85,    86,
      87,    88,    89,    90,    91,    92,    93,    94,    95,    96,
      97,    98,    99,   100,   101,   102,   103,   104,   105,   106,
     107,   108,   109,   110,   111,   112,   113,   114,   115,   116,
     117,   118,   119,   120,   121,   122,   123,   124,   125,   126,
     127,   128,   129,   130,   131,     0,   132,   133,     0,     0,
     134,   135,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   136,   137,   138,   139,
     140,   141,   142,   143,   144,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     145,   146,   147,   148,     0,     0,     0,   149,   150,   151,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   152,
     153,   154,   155,   156,   157,   158,   159,   359,     0,     0,
       2,     0,     3,     4,     0,   309,     0,     0,     0,     0,
       0,     0,     0,     6,     7,     8,     9,    10,    11,   310,
      13,     0,     0,     0,     0,    14,     0,     0,     0,     0,
      15,     0,     0,     0,     0,    16,     0,     0,     0,     0,
       0,    18,    19,     0,     0,     0,     0,     0,    21,    22,
       0,     0,     0,    23,     0,     0,     0,     0,     0,     0,
       0,    25,    26,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    29,     0,
       0,     0,     0,     0,     0,    30,    31,    32,     0,     0,
       0,     0,    33,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    36,     0,     0,    37,     0,     0,     0,     0,     0,
       0,     0,    38,    39,    40,     0,    41,     0,     0,    43,
       0,    44,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    45,    46,    47,    48,    49,
      50,    51,   311,    53,    54,    55,    56,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   122,   123,   124,   125,   126,   127,   128,   129,
     130,   131,     0,   132,   133,     0,     0,   134,   135,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   136,   137,   138,   139,   140,   141,   142,
     143,   144,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   145,   146,   147,
     148,     0,     0,     0,   149,   150,   151,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   152,   153,   154,   155,
     156,   157,   158,   159,   359,     0,     0,     2,     0,     3,
       4,     0,   309,     0,     0,     0,     0,     0,     0,     0,
       6,     7,     8,     9,    10,    11,   310,    13,     0,     0,
       0,     0,    14,     0,     0,     0,     0,    15,     0,     0,
       0,     0,    16,     0,     0,     0,     0,     0,    18,    19,
       0,     0,     0,     0,     0,    21,    22,     0,     0,     0,
      23,     0,     0,     0,     0,     0,     0,     0,     0,    26,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    29,     0,     0,     0,     0,
       0,     0,    30,    31,    32,     0,     0,     0,     0,    33,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    36,     0,
       0,    37,     0,     0,     0,     0,     0,     0,     0,    38,
      39,    40,     0,     0,     0,     0,    43,     0,    44,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    45,    46,    47,    48,    49,    50,    51,   311,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    82,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   100,   101,   102,
     103,   104,   105,   106,   107,   108,   109,   110,   111,   112,
     113,   114,   115,   116,   117,   118,   119,   120,   121,   122,
     123,   124,   125,   126,   127,   128,   129,   130,   131,     0,
     132,   133,     0,     0,   134,   135,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     136,   137,   138,   139,   140,   141,   142,   143,   144,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   145,   146,   147,   148,     0,     0,
       0,   149,   150,   151,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   152,   153,   154,   155,   156,   157,   158,
     159,   359,     0,     0,     2,     0,     3,     4,     0,   309,
       0,     0,     0,     0,     0,     0,     0,     6,     7,     8,
       9,    10,    11,     0,    13,     0,     0,     0,     0,    14,
       0,     0,     0,     0,    15,     0,     0,   484,     0,    16,
       0,     0,     0,     0,     0,    18,    19,     0,     0,     0,
       0,     0,    21,    22,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    26,     0,     0,     0,
       0,     0,   485,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   486,     0,     0,    30,
      31,    32,     0,     0,     0,     0,    33,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    36,     0,     0,    37,     0,
       0,     0,     0,     0,     0,   487,    38,    39,    40,     0,
       0,     0,     0,    43,     0,    44,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    45,
      46,    47,    48,    49,    50,    51,   311,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    92,    93,    94,    95,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
     106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,   118,   119,   120,   121,   122,   123,   124,   125,
     126,   127,   128,   129,   130,   131,     0,   132,   133,     0,
       0,   134,   135,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   136,   137,   138,
     139,   140,   141,   142,   143,   144,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   145,   146,   147,   148,     0,     0,     0,   149,   150,
     151,   359,     0,     0,     2,     0,     3,     4,     0,   309,
       0,     0,     0,     0,     0,     0,     0,     6,     7,     8,
       9,    10,    11,     0,    13,     0,   158,   159,     0,    14,
       0,     0,     0,     0,    15,     0,     0,     0,     0,    16,
       0,     0,     0,     0,     0,    18,    19,     0,     0,     0,
       0,     0,    21,    22,     0,     0,     0,    23,     0,     0,
       0,     0,     0,     0,     0,     0,    26,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    30,
      31,    32,     0,     0,     0,     0,    33,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    36,     0,     0,    37,     0,
       0,     0,     0,     0,     0,     0,    38,    39,    40,     0,
       0,     0,     0,    43,     0,    44,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    45,
      46,    47,    48,    49,    50,    51,   311,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89,    90,    91,    92,    93,    94,    95,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
     106,   107,   108,   109,   110,   111,   112,   113,   114,   115,
     116,   117,   118,   119,   120,   121,   122,   123,   124,   125,
     126,   127,   128,   129,   130,   131,     0,   132,   133,     0,
       0,   134,   135,   360,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   136,   137,   138,
     139,   140,   141,   142,   143,   144,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   145,   146,   147,   148,     0,     0,     0,   149,   150,
     151,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   359,     0,     0,     2,     0,     3,     4,     0,
     309,     0,     0,     0,     0,     0,   158,   159,     6,     7,
       8,     9,    10,    11,     0,    13,     0,   388,     0,     0,
     389,     0,     0,     0,     0,    15,     0,     0,     0,     0,
      16,     0,     0,     0,     0,     0,    18,    19,     0,     0,
       0,     0,     0,    21,    22,     0,     0,     0,    23,     0,
       0,     0,     0,     0,     0,     0,     0,    26,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      30,    31,    32,     0,     0,     0,     0,    33,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   390,     0,     0,    37,
       0,     0,     0,     0,     0,     0,     0,    38,    39,    40,
       0,     0,     0,     0,    43,     0,    44,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      45,    46,    47,    48,    49,    50,    51,   311,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,     0,   132,   133,
       0,     0,   134,   135,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   136,   137,
     138,   139,   140,   141,   142,   143,   144,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   145,   146,   147,   148,     0,     0,     0,   149,
     150,   151,   359,     0,     0,     2,     0,     3,     4,     0,
     309,     0,     0,     0,     0,     0,     0,     0,     6,     7,
       8,     9,    10,    11,     0,    13,     0,   158,   159,     0,
      14,     0,     0,     0,     0,    15,     0,     0,     0,     0,
      16,     0,     0,     0,     0,     0,    18,    19,     0,     0,
       0,     0,     0,    21,   505,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    26,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      30,    31,    32,     0,     0,     0,     0,    33,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    36,     0,     0,    37,
       0,     0,     0,   506,   507,     0,     0,    38,    39,    40,
       0,     0,     0,     0,    43,     0,    44,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      45,    46,    47,    48,    49,    50,    51,   311,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,   508,   509,   510,    81,    82,    83,    84,
     511,    86,    87,    88,    89,   512,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,     0,   132,   133,
       0,     0,   134,   135,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   136,   137,
     138,   139,   140,   141,   142,   143,   144,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   145,   146,   147,   148,     0,     0,     0,   149,
     150,   151,   519,     0,     0,     2,     0,     3,     4,     0,
     309,     0,     0,     0,     0,     0,     0,     0,     6,     7,
       8,     9,    10,    11,     0,    13,     0,   158,   159,     0,
      14,     0,     0,     0,     0,    15,     0,     0,     0,     0,
      16,     0,     0,     0,     0,     0,    18,    19,     0,     0,
       0,     0,     0,    21,    22,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    26,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      30,    31,    32,     0,     0,     0,     0,    33,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   443,     0,     0,     0,    36,     0,     0,    37,
       0,     0,     0,     0,     0,     0,     0,    38,    39,    40,
       0,     0,     0,     0,    43,     0,    44,     0,   285,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      45,    46,    47,    48,    49,    50,    51,   311,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,     0,   132,   133,
       0,     0,   134,   135,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   136,   137,
     138,   139,   140,   141,   142,   143,   144,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   145,   146,   147,   148,     0,     0,     0,   149,
     150,   151,   359,   566,     0,     2,     0,     3,     4,     0,
     309,     0,     0,     0,     0,     0,     0,     0,     6,     7,
       8,     9,    10,    11,     0,    13,     0,   158,   159,     0,
      14,     0,     0,     0,     0,    15,     0,     0,     0,     0,
      16,     0,     0,     0,     0,     0,    18,    19,     0,     0,
       0,     0,     0,    21,    22,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    26,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      30,    31,    32,     0,     0,   554,     0,    33,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    36,     0,     0,    37,
       0,     0,     0,     0,     0,     0,     0,    38,    39,    40,
       0,     0,     0,     0,    43,     0,    44,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      45,    46,    47,    48,    49,    50,    51,   311,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,     0,   132,   133,
       0,     0,   134,   135,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   136,   137,
     138,   139,   140,   141,   142,   143,   144,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   145,   146,   147,   148,     0,     0,     0,   149,
     150,   151,   359,     0,     0,     2,     0,     3,     4,     0,
     309,     0,     0,     0,     0,     0,     0,     0,     6,     7,
       8,     9,    10,    11,     0,    13,     0,   158,   159,     0,
      14,     0,     0,     0,     0,    15,     0,     0,     0,     0,
      16,     0,     0,     0,     0,     0,    18,    19,     0,     0,
       0,     0,     0,    21,    22,     0,     0,     0,    23,     0,
       0,     0,     0,     0,     0,     0,     0,    26,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      30,    31,    32,     0,     0,     0,     0,    33,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    36,     0,     0,    37,
       0,     0,     0,     0,     0,     0,     0,    38,    39,    40,
       0,     0,     0,     0,    43,     0,    44,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      45,    46,    47,    48,    49,    50,    51,   311,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,     0,   132,   133,
       0,     0,   134,   135,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   136,   137,
     138,   139,   140,   141,   142,   143,   144,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   145,   146,   147,   148,     0,     0,     0,   149,
     150,   151,   359,     0,     0,     2,     0,     3,     4,     0,
     309,     0,     0,     0,     0,     0,     0,     0,     6,     7,
       8,     9,    10,    11,     0,    13,     0,   158,   159,     0,
      14,     0,     0,     0,     0,    15,     0,     0,     0,     0,
      16,     0,     0,     0,     0,     0,    18,    19,     0,     0,
       0,     0,     0,    21,    22,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    26,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      30,    31,    32,     0,     0,   554,     0,    33,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    36,     0,     0,    37,
       0,     0,     0,     0,     0,     0,     0,    38,    39,    40,
       0,     0,     0,     0,    43,     0,    44,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      45,    46,    47,    48,    49,    50,    51,   311,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,     0,   132,   133,
       0,     0,   134,   135,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   136,   137,
     138,   139,   140,   141,   142,   143,   144,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   145,   146,   147,   148,     0,     0,     0,   149,
     150,   151,   359,   582,     0,     2,     0,     3,     4,     0,
     309,     0,     0,     0,     0,     0,     0,     0,     6,     7,
       8,     9,    10,    11,     0,    13,     0,   158,   159,     0,
      14,     0,     0,     0,     0,    15,     0,     0,     0,     0,
      16,     0,     0,     0,     0,     0,    18,    19,     0,     0,
       0,     0,     0,    21,    22,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    26,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      30,    31,    32,     0,     0,     0,     0,    33,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    36,     0,     0,    37,
       0,     0,     0,     0,     0,     0,     0,    38,    39,    40,
       0,     0,     0,     0,    43,     0,    44,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      45,    46,    47,    48,    49,    50,    51,   311,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,     0,   132,   133,
       0,     0,   134,   135,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   136,   137,
     138,   139,   140,   141,   142,   143,   144,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   145,   146,   147,   148,     0,     0,     0,   149,
     150,   151,   359,     0,     0,     2,     0,     3,     4,     0,
     309,     0,     0,     0,     0,     0,     0,     0,     6,     7,
       8,     9,    10,    11,     0,    13,     0,   158,   159,     0,
      14,     0,     0,     0,     0,    15,     0,     0,     0,     0,
      16,     0,     0,     0,     0,     0,    18,    19,     0,     0,
       0,     0,     0,    21,    22,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    26,     0,     0,
       0,     0,     0,   643,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      30,    31,    32,     0,     0,     0,     0,    33,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    36,     0,     0,    37,
       0,     0,     0,     0,     0,     0,     0,    38,    39,    40,
       0,     0,     0,     0,    43,     0,    44,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      45,    46,    47,    48,    49,    50,    51,   311,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,     0,   132,   133,
       0,     0,   134,   135,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   136,   137,
     138,   139,   140,   141,   142,   143,   144,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   145,   146,   147,   148,     0,     0,     0,   149,
     150,   151,   359,     0,     0,     2,     0,     3,     4,     0,
     309,     0,     0,     0,     0,     0,     0,     0,     6,     7,
       8,     9,    10,    11,     0,    13,     0,   158,   159,     0,
      14,     0,     0,     0,     0,    15,     0,     0,     0,     0,
      16,     0,     0,     0,     0,     0,    18,    19,     0,     0,
       0,     0,     0,    21,    22,     0,     0,     0,   968,     0,
       0,     0,     0,     0,     0,     0,     0,    26,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      30,    31,    32,     0,     0,     0,     0,    33,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    36,     0,     0,    37,
       0,     0,     0,     0,     0,     0,     0,    38,    39,    40,
       0,     0,     0,     0,    43,     0,    44,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      45,    46,    47,    48,    49,    50,    51,   311,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,     0,   132,   133,
       0,     0,   134,   135,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   136,   137,
     138,   139,   140,   141,   142,   143,   144,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   145,   146,   147,   148,     0,     0,     0,   149,
     150,   151,   359,     0,     0,     2,     0,     3,     4,     0,
     309,     0,     0,     0,     0,     0,     0,     0,     6,     7,
       8,     9,    10,    11,     0,    13,     0,   158,   159,     0,
      14,     0,     0,     0,     0,    15,     0,     0,     0,     0,
      16,     0,     0,     0,     0,     0,    18,    19,     0,     0,
       0,     0,     0,    21,    22,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    26,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      30,    31,    32,     0,     0,     0,     0,    33,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    36,     0,     0,    37,
       0,     0,     0,     0,     0,     0,     0,    38,    39,    40,
       0,     0,     0,     0,    43,     0,    44,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      45,    46,    47,    48,    49,    50,    51,   311,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,     0,   132,   133,
       0,     0,   134,   135,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   136,   137,
     138,   139,   140,   141,   142,   143,   144,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   145,   146,   147,   148,     0,     0,     0,   149,
     150,   151,   359,     0,     0,     2,     0,     3,     4,     0,
     309,     0,     0,     0,     0,     0,     0,     0,     0,     7,
       8,     9,    10,    11,     0,    13,     0,   158,   159,     0,
      14,     0,     0,     0,     0,    15,     0,     0,     0,     0,
      16,     0,     0,     0,     0,     0,     0,    19,     0,     0,
       0,     0,     0,    21,    22,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    26,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    31,    32,     0,     0,     0,     0,    33,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    36,     0,     0,    37,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    40,
       0,     0,     0,     0,     0,     0,    44,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      45,    46,    47,    48,    49,    50,    51,   311,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,     0,    67,     0,     0,     0,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,     0,   132,   133,
       0,     0,   134,   135,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   136,   137,
     138,   139,   140,   141,   142,   143,   144,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   145,   146,   147,   148,     0,     0,     0,   149,
     150,   151,   359,     0,     0,     2,     0,     3,     4,     0,
     309,     0,     0,     0,     0,     0,     0,     0,     0,     7,
       8,     9,    10,    11,     0,    13,     0,   158,   159,     0,
      14,     0,     0,     0,     0,    15,     0,     0,     0,     0,
      16,     0,     0,     0,     0,     0,     0,    19,     0,     0,
       0,     0,     0,    21,    22,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    26,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    31,    32,     0,     0,     0,     0,    33,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    36,     0,     0,    37,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    40,
       0,     0,     0,     0,     0,     0,    44,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      45,    46,    47,    48,    49,    50,    51,   311,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,     0,    67,     0,     0,     0,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   127,   128,   129,   130,   131,     0,   132,   133,
       0,     0,   134,   135,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   309,     0,     0,   136,   137,
     138,   139,   140,   141,   142,   143,   144,     0,     0,     0,
      13,     0,     0,     0,     0,    14,     0,     0,     0,     0,
      15,     0,   145,   146,   147,   148,     0,     0,     0,   149,
     150,   151,    19,     0,     0,     0,     0,     0,    21,    22,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    31,    32,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    36,     0,     0,    37,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    45,     0,    47,   448,     0,
      50,    51,   311,    53,    54,    55,    56,    57,    58,   449,
      60,    61,    62,    63,    64,    65,     0,    67,     0,     0,
       0,    71,    72,    73,    74,    75,    76,   450,    78,    79,
      80,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   107,   108,   109,
     110,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   122,   123,   124,   125,   126,   127,   128,   129,
     130,   131,   309,   132,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   434,     0,     0,
       0,     0,    14,     0,     0,     0,     0,    15,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    19,
       0,     0,     0,     0,     0,    21,    22,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    31,    32,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    36,     0,
       0,    37,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    45,     0,    47,     0,     0,    50,    51,   311,
      53,    54,    55,    56,    57,    58,     0,    60,    61,    62,
      63,    64,    65,     0,    67,     0,     0,     0,    71,    72,
      73,    74,    75,    76,     0,    78,    79,    80,    81,    82,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   100,   101,   102,
     103,   104,   105,   106,   107,   108,   109,   110,   111,   112,
     113,   114,   115,   116,   117,   118,   119,   120,   121,   122,
     123,   124,   125,   126,   127,   128,   129,   130,   131,     0,
     132
};

static const yytype_int16 yycheck[] =
{
       0,     1,     1,    42,   314,   133,   301,    29,   301,    25,
     301,   368,   522,     5,     1,   186,   492,   930,   631,   284,
     933,   317,   173,   173,   812,    41,   368,     4,   399,   400,
       4,   173,   403,     4,   405,   173,   173,   747,     4,   355,
     356,   751,   752,   753,     4,    26,     4,   343,    28,   365,
     366,   761,     4,   758,   759,     5,   763,     4,   186,    26,
     723,   605,    71,   832,     4,    32,    51,    79,    74,     4,
       4,    74,   743,   994,   102,     5,   860,   748,    74,   362,
     376,     3,    62,    11,   406,   142,  1085,     4,   459,     4,
       4,   920,    77,   331,     4,   333,     3,    26,    42,    26,
      26,     5,    50,    26,   318,    26,     5,    66,    26,    66,
      26,   241,     5,     4,   255,     4,    43,   855,   634,   804,
      26,  1034,     0,     1,   466,    28,    75,    75,    85,   293,
       5,     8,    81,    81,   650,   121,  1057,    65,    86,    11,
       3,  1054,     3,   121,   308,    72,    94,    33,   121,    40,
    1149,     3,     3,     6,   140,     3,    66,   667,   468,    62,
      13,   302,    26,     3,  1277,    26,     3,     5,    32,     7,
     480,     3,   131,   154,    11,    61,    25,   442,   105,   108,
     310,    74,    71,   493,   165,  1298,     3,   154,    37,    26,
      60,   901,   902,    65,     3,   409,    25,   298,   165,   300,
     465,    74,   183,   374,   914,   132,   113,   378,   579,   153,
      83,   160,   160,   298,   426,   300,   183,   429,   430,   168,
     168,   131,   279,   461,    68,   541,  1147,   154,   154,   443,
      79,   154,   139,   154,  1233,    84,   154,     3,   165,   165,
      89,   912,   165,   142,   165,   528,   374,   165,   570,   241,
     378,  1080,    96,   424,   519,    84,   183,   183,   943,   497,
     183,   946,   183,   143,   134,   183,   141,  1188,   254,   146,
     147,   148,   149,   402,    74,   404,   113,   255,   321,   322,
     319,   320,   255,   322,   284,   284,   130,  1071,   158,   133,
     632,   241,   320,     6,  1063,   605,   424,   284,    26,  1212,
      13,   611,   139,  1008,   318,   319,    40,  1012,   320,   308,
    1223,   320,   297,  1018,   320,   354,    25,   320,   310,   311,
     305,   308,  1027,  1030,   320,     3,   636,   837,   315,   686,
     317,   875,   876,   704,   947,   706,   707,   241,  1001,   710,
     321,   321,   713,   320,   686,   821,   320,   143,   270,   320,
    1088,   338,  1265,   320,   320,   342,   343,   344,   310,   375,
     320,   360,   320,   385,   351,   352,   353,   663,   355,   356,
     357,   358,   359,   320,   525,   525,   733,   393,   365,   366,
     320,   420,   369,   525,   700,   320,   320,   525,   525,   376,
     320,   733,   320,   310,   690,   310,   310,   525,   321,   438,
     321,  1111,  1140,  1108,   138,   556,   284,   270,   724,    37,
      37,   274,   399,   400,   401,   402,   403,   404,   405,   270,
     407,   408,   270,   274,   411,  1213,   274,  1215,  1216,  1217,
     270,   418,    19,   472,   274,   422,  1049,     3,   270,  1052,
     245,   246,   274,   442,     4,     5,   318,   319,   320,   321,
     322,    79,    79,     3,     6,   442,     8,     9,   800,   935,
      47,    89,    89,   312,   313,   314,   315,   316,   317,    25,
       3,     3,   459,     6,    83,     4,     5,    33,     7,    11,
      13,    37,    26,   312,   313,   314,   315,   316,   317,   247,
     248,   249,   802,     6,    26,     8,     9,     3,   485,     3,
      55,     0,   673,   490,    91,    61,     3,    20,    21,    22,
      23,    24,   822,     3,   754,   755,   687,    19,  1131,    21,
      22,    55,    24,    79,    37,     3,   525,     7,    84,     4,
       5,     3,   519,    89,     3,   522,     4,     5,    51,     7,
       3,   128,   129,     4,     5,   673,     7,   547,   135,   131,
       4,     5,    28,     7,   541,    68,   727,     3,     6,   687,
       8,     9,   287,   288,     3,   875,   876,   606,   607,   608,
       4,     5,    20,    21,    22,    23,    24,     3,   312,   313,
     314,   315,   316,   317,    60,   172,   320,     6,     3,     8,
       9,    84,   579,   311,    25,   146,   147,   148,   149,   727,
     639,    20,    21,    22,    23,    24,    14,    15,    16,    17,
      18,   310,    43,     3,   601,   631,   603,   130,   617,   929,
     930,   992,   993,   933,   137,     4,     5,     3,    59,   184,
     185,   186,    51,     3,   110,     3,   191,   113,   878,     4,
       5,   196,     7,   301,   302,     3,  1003,   887,   635,    68,
     184,   185,   186,   893,   641,     3,   643,   191,   134,     3,
     136,  1003,   196,     4,     5,   905,    20,    21,    22,    23,
      24,   658,   659,   660,   105,   662,   663,   664,   943,   110,
     667,    50,   158,     4,     5,   836,   836,     4,     5,     3,
       7,    50,     4,     5,   836,     7,   847,   282,   836,   836,
       3,   132,     3,   690,   832,   282,    75,     3,   836,   293,
     294,   130,    81,   700,   701,   702,    75,   704,   137,   706,
     707,    60,    81,   710,  1034,    94,   713,     4,     5,     3,
       7,   100,    34,     4,     5,    94,     7,   724,     4,     5,
     257,   258,    96,     3,  1054,   320,   259,   260,   261,   262,
     263,   264,   265,   266,   267,   318,   312,   313,   314,   315,
     316,   317,  1057,  1120,  1057,    44,  1057,     4,     5,   138,
       3,   110,  1068,     3,   113,  1146,    20,    21,    22,    23,
      24,  1021,     4,   137,  1024,    85,   299,    85,   804,   255,
     256,   160,    10,   793,   317,   134,   145,   136,    12,   168,
      12,   160,   312,   313,   314,   315,   316,   317,    19,   168,
     320,   982,     4,     5,   836,     4,     5,     4,     5,   158,
     174,   175,   176,   599,   600,   599,   600,   158,   159,   412,
     413,   491,   492,   820,   414,   415,    26,   824,    26,   106,
     259,   260,   261,   262,   263,   264,   265,   266,   267,    83,
     837,     3,   100,    96,   982,     5,   114,   245,     4,     3,
      28,     4,    90,     4,     3,    13,    13,    32,     5,   138,
       5,     4,     3,   141,   138,     4,    74,    26,    74,    74,
     299,    26,     3,    74,   246,   242,     4,    21,    74,    74,
      74,   311,    26,  1188,   113,  1188,     4,  1188,     4,    26,
       5,  1258,  1212,     4,     3,     5,     4,     4,    42,     5,
       5,    26,     4,  1223,     5,    32,    99,   126,   317,    53,
      13,    40,    56,    57,   139,     4,   925,   943,     5,    63,
     946,   947,     5,     5,     5,  1063,    70,   242,     4,  1296,
      65,    23,   303,     4,     4,   238,    74,     3,    82,    13,
      13,   104,     3,     3,     3,  1265,     4,    42,     4,    99,
       4,    95,     3,    97,    87,    87,     3,     5,   100,   150,
      77,     4,    26,     4,   320,   109,    21,    19,     5,     5,
       5,    40,   116,   289,    23,   972,   102,    77,   122,   123,
     289,   271,     3,    23,    23,   271,   271,    42,   271,    23,
     126,   114,     4,    86,   991,   992,   993,    26,    53,   996,
      86,    56,    57,     3,   103,     4,    26,    40,    63,   153,
      78,     4,   142,     5,    12,    70,     3,    74,     4,     4,
       4,   271,   126,  1049,   122,   122,  1052,    82,    43,    26,
       5,   251,    76,    12,     5,   304,    86,     5,     3,   101,
      95,    23,    97,    96,    71,     5,   255,     4,   144,  1058,
     144,   300,   300,   295,   109,   101,     4,     3,   250,   245,
       3,   116,    32,     5,     3,   110,   311,   122,   123,   306,
      32,  1068,  1121,     3,    23,   255,   307,     3,   296,    26,
     253,    26,    26,    23,   307,     7,    69,  1043,   929,  1037,
    1056,   473,   309,   979,   632,   822,   800,   656,   153,  1063,
    1299,   603,   813,  1174,    12,  1131,   386,   545,   173,   635,
     173,   686,   387,  1226,   686,  1227,  1147,  1233,   951,  1003,
    1085,   598,   465,   607,   268,   269,  1120,   317,   272,   273,
     376,   275,   276,   277,   278,   830,   280,   281,   410,   263,
     792,   892,   836,   528,  1243,   526,   419,     3,   364,  1146,
       6,    -1,     8,     9,  1175,    11,  1165,    -1,    -1,    -1,
      -1,  1210,    -1,    19,    20,    21,    22,    23,    24,    -1,
      26,    -1,    -1,  1182,    -1,    31,    -1,    -1,    -1,    -1,
      36,    -1,    -1,    -1,    -1,    41,    -1,    -1,    -1,    -1,
      -1,    47,    48,    -1,    -1,    -1,    -1,    -1,    54,    55,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,  1221,    68,   268,   269,    -1,    -1,   272,   273,    -1,
     275,   276,   277,   278,    -1,   280,   281,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    91,    92,    93,    -1,  1248,
      -1,    -1,    98,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   117,    -1,    -1,   120,    -1,    -1,  1277,    -1,    -1,
      -1,    -1,   128,   129,   130,    -1,    -1,    -1,    -1,   135,
      -1,   137,    -1,    -1,    -1,    -1,    -1,    -1,  1298,    -1,
      -1,    -1,    -1,    -1,    -1,   151,   152,   153,   154,   155,
     156,   157,   158,   159,   160,   161,   162,   163,   164,   165,
     166,   167,   168,   169,   170,   171,   172,   173,   174,   175,
     176,   177,   178,   179,   180,   181,   182,   183,   184,   185,
     186,   187,   188,   189,   190,   191,   192,   193,   194,   195,
     196,   197,   198,   199,   200,   201,   202,   203,   204,   205,
     206,   207,   208,   209,   210,   211,   212,   213,   214,   215,
     216,   217,   218,   219,   220,   221,   222,   223,   224,   225,
     226,   227,   228,   229,   230,   231,   232,   233,   234,   235,
     236,   237,    -1,   239,   240,    -1,    -1,   243,   244,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   259,   260,   261,   262,   263,   264,   265,
     266,   267,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   283,   284,   285,
     286,    -1,    -1,    -1,   290,   291,   292,    -1,     3,    -1,
      -1,     6,    -1,     8,     9,    -1,    11,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    19,    20,    21,    22,    23,    24,
      25,    26,   318,   319,    -1,   321,    31,    -1,    -1,    -1,
      -1,    36,    -1,    -1,    -1,    -1,    41,    -1,    -1,    -1,
      45,    -1,    47,    48,    49,    -1,    -1,    -1,    -1,    54,
      55,    -1,    -1,    -1,    59,    60,    -1,    -1,    -1,    -1,
      -1,    -1,    67,    68,    69,    -1,    -1,    -1,    -1,    -1,
      -1,    76,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    84,
      -1,    -1,    -1,    -1,    -1,    -1,    91,    92,    93,    -1,
      -1,    -1,    -1,    98,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   111,    -1,   113,    -1,
      -1,    -1,   117,    -1,    -1,   120,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   128,   129,   130,    -1,   132,    -1,   134,
     135,    -1,   137,    -1,   139,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   151,   152,   153,   154,
     155,   156,   157,   158,   159,   160,   161,   162,   163,   164,
     165,   166,   167,   168,   169,   170,   171,   172,   173,   174,
     175,   176,   177,   178,   179,   180,   181,   182,   183,   184,
     185,   186,   187,   188,   189,   190,   191,   192,   193,   194,
     195,   196,   197,   198,   199,   200,   201,   202,   203,   204,
     205,   206,   207,   208,   209,   210,   211,   212,   213,   214,
     215,   216,   217,   218,   219,   220,   221,   222,   223,   224,
     225,   226,   227,   228,   229,   230,   231,   232,   233,   234,
     235,   236,   237,    -1,   239,   240,    -1,    -1,   243,   244,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   259,   260,   261,   262,   263,   264,
     265,   266,   267,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   283,   284,
     285,   286,    -1,    -1,    -1,   290,   291,   292,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   312,   313,   314,
     315,   316,   317,   318,   319,     3,    -1,    -1,     6,    -1,
       8,     9,    -1,    11,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    19,    20,    21,    22,    23,    24,    25,    26,    -1,
      -1,    -1,    -1,    31,    -1,    -1,    -1,    -1,    36,    -1,
      -1,    -1,    -1,    41,    -1,    -1,    -1,    45,    -1,    47,
      48,    49,    -1,    -1,    -1,    -1,    54,    55,    -1,    -1,
      -1,    59,    60,    -1,    -1,    -1,    -1,    -1,    -1,    67,
      68,    69,    -1,    -1,    -1,    -1,    -1,    -1,    76,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    84,    -1,    -1,    -1,
      -1,    -1,    -1,    91,    92,    93,    -1,    -1,    -1,    -1,
      98,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   111,    -1,   113,    -1,    -1,    -1,   117,
      -1,    -1,   120,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     128,   129,   130,    -1,   132,    -1,   134,   135,    -1,   137,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   151,   152,   153,   154,   155,   156,   157,
     158,   159,   160,   161,   162,   163,   164,   165,   166,   167,
     168,   169,   170,   171,   172,   173,   174,   175,   176,   177,
     178,   179,   180,   181,   182,   183,   184,   185,   186,   187,
     188,   189,   190,   191,   192,   193,   194,   195,   196,   197,
     198,   199,   200,   201,   202,   203,   204,   205,   206,   207,
     208,   209,   210,   211,   212,   213,   214,   215,   216,   217,
     218,   219,   220,   221,   222,   223,   224,   225,   226,   227,
     228,   229,   230,   231,   232,   233,   234,   235,   236,   237,
      -1,   239,   240,    -1,    -1,   243,   244,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   252,    -1,    -1,    -1,    -1,    -1,
      -1,   259,   260,   261,   262,   263,   264,   265,   266,   267,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   283,   284,   285,   286,    -1,
      -1,    -1,   290,   291,   292,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   312,   313,   314,   315,   316,   317,
     318,   319,     3,    -1,    -1,     6,    -1,     8,     9,    -1,
      11,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    19,    20,
      21,    22,    23,    24,    25,    26,    -1,    -1,    -1,    -1,
      31,    -1,    -1,    -1,    -1,    36,    -1,    -1,    -1,    -1,
      41,    -1,    -1,    -1,    45,    -1,    47,    48,    49,    -1,
      -1,    -1,    -1,    54,    55,    -1,    -1,    -1,    59,    60,
      -1,    -1,    -1,    -1,    -1,    -1,    67,    68,    69,    -1,
      -1,    -1,    -1,    -1,    -1,    76,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    84,    -1,    -1,    -1,    -1,    -1,    -1,
      91,    92,    93,    -1,    -1,    -1,    -1,    98,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     111,    -1,   113,    -1,    -1,    -1,   117,    -1,    -1,   120,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   128,   129,   130,
      -1,   132,    -1,   134,   135,    -1,   137,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     151,   152,   153,   154,   155,   156,   157,   158,   159,   160,
     161,   162,   163,   164,   165,   166,   167,   168,   169,   170,
     171,   172,   173,   174,   175,   176,   177,   178,   179,   180,
     181,   182,   183,   184,   185,   186,   187,   188,   189,   190,
     191,   192,   193,   194,   195,   196,   197,   198,   199,   200,
     201,   202,   203,   204,   205,   206,   207,   208,   209,   210,
     211,   212,   213,   214,   215,   216,   217,   218,   219,   220,
     221,   222,   223,   224,   225,   226,   227,   228,   229,   230,
     231,   232,   233,   234,   235,   236,   237,    -1,   239,   240,
      -1,   242,   243,   244,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   259,   260,
     261,   262,   263,   264,   265,   266,   267,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   283,   284,   285,   286,    -1,    -1,    -1,   290,
     291,   292,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   312,   313,   314,   315,   316,   317,   318,   319,     3,
      -1,    -1,     6,    -1,     8,     9,    -1,    11,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    19,    20,    21,    22,    23,
      24,    25,    26,    -1,    -1,    -1,    -1,    31,    -1,    -1,
      -1,    -1,    36,    -1,    -1,    -1,    -1,    41,    -1,    -1,
      -1,    45,    -1,    47,    48,    49,    -1,    -1,    -1,    -1,
      54,    55,    -1,    -1,    -1,    59,    60,    -1,    -1,    -1,
      -1,    -1,    -1,    67,    68,    69,    -1,    -1,    -1,    -1,
      -1,    -1,    76,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      84,    -1,    -1,    -1,    -1,    -1,    -1,    91,    92,    93,
      -1,    -1,    -1,    -1,    98,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   111,    -1,   113,
      -1,    -1,    -1,   117,    -1,    -1,   120,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   128,   129,   130,    -1,   132,    -1,
     134,   135,    -1,   137,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   151,   152,   153,
     154,   155,   156,   157,   158,   159,   160,   161,   162,   163,
     164,   165,   166,   167,   168,   169,   170,   171,   172,   173,
     174,   175,   176,   177,   178,   179,   180,   181,   182,   183,
     184,   185,   186,   187,   188,   189,   190,   191,   192,   193,
     194,   195,   196,   197,   198,   199,   200,   201,   202,   203,
     204,   205,   206,   207,   208,   209,   210,   211,   212,   213,
     214,   215,   216,   217,   218,   219,   220,   221,   222,   223,
     224,   225,   226,   227,   228,   229,   230,   231,   232,   233,
     234,   235,   236,   237,    -1,   239,   240,    -1,    -1,   243,
     244,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   259,   260,   261,   262,   263,
     264,   265,   266,   267,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   283,
     284,   285,   286,    -1,    -1,    -1,   290,   291,   292,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   312,   313,
     314,   315,   316,   317,   318,   319,     3,    -1,    -1,     6,
      -1,     8,     9,    -1,    11,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    19,    20,    21,    22,    23,    24,    25,    26,
      -1,    -1,    -1,    -1,    31,    -1,    -1,    -1,    -1,    36,
      -1,    -1,    -1,    -1,    41,    -1,    -1,    -1,    -1,    -1,
      47,    48,    -1,    -1,    -1,    -1,    -1,    54,    55,    -1,
      -1,    -1,    59,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      67,    68,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    84,    -1,    -1,
      -1,    -1,    -1,    -1,    91,    92,    93,    -1,    -1,    -1,
      -1,    98,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   113,    -1,    -1,    -1,
     117,    -1,    -1,   120,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   128,   129,   130,    -1,   132,    -1,    -1,   135,    -1,
     137,    -1,   139,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   151,   152,   153,   154,   155,   156,
     157,   158,   159,   160,   161,   162,   163,   164,   165,   166,
     167,   168,   169,   170,   171,   172,   173,   174,   175,   176,
     177,   178,   179,   180,   181,   182,   183,   184,   185,   186,
     187,   188,   189,   190,   191,   192,   193,   194,   195,   196,
     197,   198,   199,   200,   201,   202,   203,   204,   205,   206,
     207,   208,   209,   210,   211,   212,   213,   214,   215,   216,
     217,   218,   219,   220,   221,   222,   223,   224,   225,   226,
     227,   228,   229,   230,   231,   232,   233,   234,   235,   236,
     237,    -1,   239,   240,    -1,    -1,   243,   244,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   259,   260,   261,   262,   263,   264,   265,   266,
     267,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   283,   284,   285,   286,
      -1,    -1,    -1,   290,   291,   292,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   312,   313,   314,   315,   316,
     317,   318,   319,     3,    -1,    -1,     6,    -1,     8,     9,
      -1,    11,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    19,
      20,    21,    22,    23,    24,    25,    26,    -1,    -1,    -1,
      -1,    31,    -1,    -1,    -1,    -1,    36,    -1,    -1,    -1,
      -1,    41,    -1,    -1,    -1,    -1,    -1,    47,    48,    -1,
      -1,    -1,    -1,    -1,    54,    55,    -1,    -1,    -1,    59,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    67,    68,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    84,    -1,    -1,    -1,    -1,    -1,
      -1,    91,    92,    93,    -1,    -1,    -1,    -1,    98,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   117,    -1,    -1,
     120,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   128,   129,
     130,    -1,   132,    -1,    -1,   135,    -1,   137,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   151,   152,   153,   154,   155,   156,   157,   158,   159,
     160,   161,   162,   163,   164,   165,   166,   167,   168,   169,
     170,   171,   172,   173,   174,   175,   176,   177,   178,   179,
     180,   181,   182,   183,   184,   185,   186,   187,   188,   189,
     190,   191,   192,   193,   194,   195,   196,   197,   198,   199,
     200,   201,   202,   203,   204,   205,   206,   207,   208,   209,
     210,   211,   212,   213,   214,   215,   216,   217,   218,   219,
     220,   221,   222,   223,   224,   225,   226,   227,   228,   229,
     230,   231,   232,   233,   234,   235,   236,   237,    -1,   239,
     240,    -1,    -1,   243,   244,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   259,
     260,   261,   262,   263,   264,   265,   266,   267,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   283,   284,   285,   286,    -1,    -1,    -1,
     290,   291,   292,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   312,   313,   314,   315,   316,   317,   318,   319,
       3,    -1,    -1,     6,    -1,     8,     9,    -1,    11,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    19,    20,    21,    22,
      23,    24,    25,    26,    -1,    -1,    -1,    -1,    31,    -1,
      -1,    -1,    -1,    36,    -1,    -1,    -1,    -1,    41,    -1,
      -1,    -1,    -1,    -1,    47,    48,    -1,    -1,    -1,    -1,
      -1,    54,    55,    -1,    -1,    -1,    59,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    67,    68,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    84,    -1,    -1,    -1,    -1,    -1,    -1,    91,    92,
      93,    -1,    -1,    -1,    -1,    98,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   117,    -1,    -1,   120,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   128,   129,   130,    -1,   132,
      -1,    -1,   135,    -1,   137,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   151,   152,
     153,   154,   155,   156,   157,   158,   159,   160,   161,   162,
     163,   164,   165,   166,   167,   168,   169,   170,   171,   172,
     173,   174,   175,   176,   177,   178,   179,   180,   181,   182,
     183,   184,   185,   186,   187,   188,   189,   190,   191,   192,
     193,   194,   195,   196,   197,   198,   199,   200,   201,   202,
     203,   204,   205,   206,   207,   208,   209,   210,   211,   212,
     213,   214,   215,   216,   217,   218,   219,   220,   221,   222,
     223,   224,   225,   226,   227,   228,   229,   230,   231,   232,
     233,   234,   235,   236,   237,    -1,   239,   240,    -1,    -1,
     243,   244,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   259,   260,   261,   262,
     263,   264,   265,   266,   267,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     283,   284,   285,   286,    -1,    -1,    -1,   290,   291,   292,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   312,
     313,   314,   315,   316,   317,   318,   319,     3,    -1,    -1,
       6,    -1,     8,     9,    -1,    11,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    19,    20,    21,    22,    23,    24,    25,
      26,    -1,    -1,    -1,    -1,    31,    -1,    -1,    -1,    -1,
      36,    -1,    -1,    -1,    -1,    41,    -1,    -1,    -1,    -1,
      -1,    47,    48,    -1,    -1,    -1,    -1,    -1,    54,    55,
      -1,    -1,    -1,    59,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    67,    68,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    84,    -1,
      -1,    -1,    -1,    -1,    -1,    91,    92,    93,    -1,    -1,
      -1,    -1,    98,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   117,    -1,    -1,   120,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   128,   129,   130,    -1,   132,    -1,    -1,   135,
      -1,   137,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   151,   152,   153,   154,   155,
     156,   157,   158,   159,   160,   161,   162,   163,   164,   165,
     166,   167,   168,   169,   170,   171,   172,   173,   174,   175,
     176,   177,   178,   179,   180,   181,   182,   183,   184,   185,
     186,   187,   188,   189,   190,   191,   192,   193,   194,   195,
     196,   197,   198,   199,   200,   201,   202,   203,   204,   205,
     206,   207,   208,   209,   210,   211,   212,   213,   214,   215,
     216,   217,   218,   219,   220,   221,   222,   223,   224,   225,
     226,   227,   228,   229,   230,   231,   232,   233,   234,   235,
     236,   237,    -1,   239,   240,    -1,    -1,   243,   244,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   259,   260,   261,   262,   263,   264,   265,
     266,   267,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   283,   284,   285,
     286,    -1,    -1,    -1,   290,   291,   292,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   312,   313,   314,   315,
     316,   317,   318,   319,     3,    -1,    -1,     6,    -1,     8,
       9,    -1,    11,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      19,    20,    21,    22,    23,    24,    25,    26,    -1,    -1,
      -1,    -1,    31,    -1,    -1,    -1,    -1,    36,    -1,    -1,
      -1,    -1,    41,    -1,    -1,    -1,    -1,    -1,    47,    48,
      -1,    -1,    -1,    -1,    -1,    54,    55,    -1,    -1,    -1,
      59,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    68,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    84,    -1,    -1,    -1,    -1,
      -1,    -1,    91,    92,    93,    -1,    -1,    -1,    -1,    98,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   117,    -1,
      -1,   120,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   128,
     129,   130,    -1,    -1,    -1,    -1,   135,    -1,   137,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   151,   152,   153,   154,   155,   156,   157,   158,
     159,   160,   161,   162,   163,   164,   165,   166,   167,   168,
     169,   170,   171,   172,   173,   174,   175,   176,   177,   178,
     179,   180,   181,   182,   183,   184,   185,   186,   187,   188,
     189,   190,   191,   192,   193,   194,   195,   196,   197,   198,
     199,   200,   201,   202,   203,   204,   205,   206,   207,   208,
     209,   210,   211,   212,   213,   214,   215,   216,   217,   218,
     219,   220,   221,   222,   223,   224,   225,   226,   227,   228,
     229,   230,   231,   232,   233,   234,   235,   236,   237,    -1,
     239,   240,    -1,    -1,   243,   244,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     259,   260,   261,   262,   263,   264,   265,   266,   267,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   283,   284,   285,   286,    -1,    -1,
      -1,   290,   291,   292,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   312,   313,   314,   315,   316,   317,   318,
     319,     3,    -1,    -1,     6,    -1,     8,     9,    -1,    11,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    19,    20,    21,
      22,    23,    24,    -1,    26,    -1,    -1,    -1,    -1,    31,
      -1,    -1,    -1,    -1,    36,    -1,    -1,    39,    -1,    41,
      -1,    -1,    -1,    -1,    -1,    47,    48,    -1,    -1,    -1,
      -1,    -1,    54,    55,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    68,    -1,    -1,    -1,
      -1,    -1,    74,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    88,    -1,    -1,    91,
      92,    93,    -1,    -1,    -1,    -1,    98,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   117,    -1,    -1,   120,    -1,
      -1,    -1,    -1,    -1,    -1,   127,   128,   129,   130,    -1,
      -1,    -1,    -1,   135,    -1,   137,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   151,
     152,   153,   154,   155,   156,   157,   158,   159,   160,   161,
     162,   163,   164,   165,   166,   167,   168,   169,   170,   171,
     172,   173,   174,   175,   176,   177,   178,   179,   180,   181,
     182,   183,   184,   185,   186,   187,   188,   189,   190,   191,
     192,   193,   194,   195,   196,   197,   198,   199,   200,   201,
     202,   203,   204,   205,   206,   207,   208,   209,   210,   211,
     212,   213,   214,   215,   216,   217,   218,   219,   220,   221,
     222,   223,   224,   225,   226,   227,   228,   229,   230,   231,
     232,   233,   234,   235,   236,   237,    -1,   239,   240,    -1,
      -1,   243,   244,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   259,   260,   261,
     262,   263,   264,   265,   266,   267,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   283,   284,   285,   286,    -1,    -1,    -1,   290,   291,
     292,     3,    -1,    -1,     6,    -1,     8,     9,    -1,    11,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    19,    20,    21,
      22,    23,    24,    -1,    26,    -1,   318,   319,    -1,    31,
      -1,    -1,    -1,    -1,    36,    -1,    -1,    -1,    -1,    41,
      -1,    -1,    -1,    -1,    -1,    47,    48,    -1,    -1,    -1,
      -1,    -1,    54,    55,    -1,    -1,    -1,    59,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    68,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    91,
      92,    93,    -1,    -1,    -1,    -1,    98,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   117,    -1,    -1,   120,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   128,   129,   130,    -1,
      -1,    -1,    -1,   135,    -1,   137,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   151,
     152,   153,   154,   155,   156,   157,   158,   159,   160,   161,
     162,   163,   164,   165,   166,   167,   168,   169,   170,   171,
     172,   173,   174,   175,   176,   177,   178,   179,   180,   181,
     182,   183,   184,   185,   186,   187,   188,   189,   190,   191,
     192,   193,   194,   195,   196,   197,   198,   199,   200,   201,
     202,   203,   204,   205,   206,   207,   208,   209,   210,   211,
     212,   213,   214,   215,   216,   217,   218,   219,   220,   221,
     222,   223,   224,   225,   226,   227,   228,   229,   230,   231,
     232,   233,   234,   235,   236,   237,    -1,   239,   240,    -1,
      -1,   243,   244,   245,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   259,   260,   261,
     262,   263,   264,   265,   266,   267,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   283,   284,   285,   286,    -1,    -1,    -1,   290,   291,
     292,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,     3,    -1,    -1,     6,    -1,     8,     9,    -1,
      11,    -1,    -1,    -1,    -1,    -1,   318,   319,    19,    20,
      21,    22,    23,    24,    -1,    26,    -1,    28,    -1,    -1,
      31,    -1,    -1,    -1,    -1,    36,    -1,    -1,    -1,    -1,
      41,    -1,    -1,    -1,    -1,    -1,    47,    48,    -1,    -1,
      -1,    -1,    -1,    54,    55,    -1,    -1,    -1,    59,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    68,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      91,    92,    93,    -1,    -1,    -1,    -1,    98,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   117,    -1,    -1,   120,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   128,   129,   130,
      -1,    -1,    -1,    -1,   135,    -1,   137,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     151,   152,   153,   154,   155,   156,   157,   158,   159,   160,
     161,   162,   163,   164,   165,   166,   167,   168,   169,   170,
     171,   172,   173,   174,   175,   176,   177,   178,   179,   180,
     181,   182,   183,   184,   185,   186,   187,   188,   189,   190,
     191,   192,   193,   194,   195,   196,   197,   198,   199,   200,
     201,   202,   203,   204,   205,   206,   207,   208,   209,   210,
     211,   212,   213,   214,   215,   216,   217,   218,   219,   220,
     221,   222,   223,   224,   225,   226,   227,   228,   229,   230,
     231,   232,   233,   234,   235,   236,   237,    -1,   239,   240,
      -1,    -1,   243,   244,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   259,   260,
     261,   262,   263,   264,   265,   266,   267,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   283,   284,   285,   286,    -1,    -1,    -1,   290,
     291,   292,     3,    -1,    -1,     6,    -1,     8,     9,    -1,
      11,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    19,    20,
      21,    22,    23,    24,    -1,    26,    -1,   318,   319,    -1,
      31,    -1,    -1,    -1,    -1,    36,    -1,    -1,    -1,    -1,
      41,    -1,    -1,    -1,    -1,    -1,    47,    48,    -1,    -1,
      -1,    -1,    -1,    54,    55,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    68,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      91,    92,    93,    -1,    -1,    -1,    -1,    98,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   117,    -1,    -1,   120,
      -1,    -1,    -1,   124,   125,    -1,    -1,   128,   129,   130,
      -1,    -1,    -1,    -1,   135,    -1,   137,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     151,   152,   153,   154,   155,   156,   157,   158,   159,   160,
     161,   162,   163,   164,   165,   166,   167,   168,   169,   170,
     171,   172,   173,   174,   175,   176,   177,   178,   179,   180,
     181,   182,   183,   184,   185,   186,   187,   188,   189,   190,
     191,   192,   193,   194,   195,   196,   197,   198,   199,   200,
     201,   202,   203,   204,   205,   206,   207,   208,   209,   210,
     211,   212,   213,   214,   215,   216,   217,   218,   219,   220,
     221,   222,   223,   224,   225,   226,   227,   228,   229,   230,
     231,   232,   233,   234,   235,   236,   237,    -1,   239,   240,
      -1,    -1,   243,   244,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   259,   260,
     261,   262,   263,   264,   265,   266,   267,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   283,   284,   285,   286,    -1,    -1,    -1,   290,
     291,   292,     3,    -1,    -1,     6,    -1,     8,     9,    -1,
      11,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    19,    20,
      21,    22,    23,    24,    -1,    26,    -1,   318,   319,    -1,
      31,    -1,    -1,    -1,    -1,    36,    -1,    -1,    -1,    -1,
      41,    -1,    -1,    -1,    -1,    -1,    47,    48,    -1,    -1,
      -1,    -1,    -1,    54,    55,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    68,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      91,    92,    93,    -1,    -1,    -1,    -1,    98,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   113,    -1,    -1,    -1,   117,    -1,    -1,   120,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   128,   129,   130,
      -1,    -1,    -1,    -1,   135,    -1,   137,    -1,   139,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     151,   152,   153,   154,   155,   156,   157,   158,   159,   160,
     161,   162,   163,   164,   165,   166,   167,   168,   169,   170,
     171,   172,   173,   174,   175,   176,   177,   178,   179,   180,
     181,   182,   183,   184,   185,   186,   187,   188,   189,   190,
     191,   192,   193,   194,   195,   196,   197,   198,   199,   200,
     201,   202,   203,   204,   205,   206,   207,   208,   209,   210,
     211,   212,   213,   214,   215,   216,   217,   218,   219,   220,
     221,   222,   223,   224,   225,   226,   227,   228,   229,   230,
     231,   232,   233,   234,   235,   236,   237,    -1,   239,   240,
      -1,    -1,   243,   244,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   259,   260,
     261,   262,   263,   264,   265,   266,   267,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   283,   284,   285,   286,    -1,    -1,    -1,   290,
     291,   292,     3,     4,    -1,     6,    -1,     8,     9,    -1,
      11,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    19,    20,
      21,    22,    23,    24,    -1,    26,    -1,   318,   319,    -1,
      31,    -1,    -1,    -1,    -1,    36,    -1,    -1,    -1,    -1,
      41,    -1,    -1,    -1,    -1,    -1,    47,    48,    -1,    -1,
      -1,    -1,    -1,    54,    55,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    68,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      91,    92,    93,    -1,    -1,    96,    -1,    98,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   117,    -1,    -1,   120,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   128,   129,   130,
      -1,    -1,    -1,    -1,   135,    -1,   137,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     151,   152,   153,   154,   155,   156,   157,   158,   159,   160,
     161,   162,   163,   164,   165,   166,   167,   168,   169,   170,
     171,   172,   173,   174,   175,   176,   177,   178,   179,   180,
     181,   182,   183,   184,   185,   186,   187,   188,   189,   190,
     191,   192,   193,   194,   195,   196,   197,   198,   199,   200,
     201,   202,   203,   204,   205,   206,   207,   208,   209,   210,
     211,   212,   213,   214,   215,   216,   217,   218,   219,   220,
     221,   222,   223,   224,   225,   226,   227,   228,   229,   230,
     231,   232,   233,   234,   235,   236,   237,    -1,   239,   240,
      -1,    -1,   243,   244,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   259,   260,
     261,   262,   263,   264,   265,   266,   267,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   283,   284,   285,   286,    -1,    -1,    -1,   290,
     291,   292,     3,    -1,    -1,     6,    -1,     8,     9,    -1,
      11,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    19,    20,
      21,    22,    23,    24,    -1,    26,    -1,   318,   319,    -1,
      31,    -1,    -1,    -1,    -1,    36,    -1,    -1,    -1,    -1,
      41,    -1,    -1,    -1,    -1,    -1,    47,    48,    -1,    -1,
      -1,    -1,    -1,    54,    55,    -1,    -1,    -1,    59,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    68,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      91,    92,    93,    -1,    -1,    -1,    -1,    98,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   117,    -1,    -1,   120,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   128,   129,   130,
      -1,    -1,    -1,    -1,   135,    -1,   137,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     151,   152,   153,   154,   155,   156,   157,   158,   159,   160,
     161,   162,   163,   164,   165,   166,   167,   168,   169,   170,
     171,   172,   173,   174,   175,   176,   177,   178,   179,   180,
     181,   182,   183,   184,   185,   186,   187,   188,   189,   190,
     191,   192,   193,   194,   195,   196,   197,   198,   199,   200,
     201,   202,   203,   204,   205,   206,   207,   208,   209,   210,
     211,   212,   213,   214,   215,   216,   217,   218,   219,   220,
     221,   222,   223,   224,   225,   226,   227,   228,   229,   230,
     231,   232,   233,   234,   235,   236,   237,    -1,   239,   240,
      -1,    -1,   243,   244,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   259,   260,
     261,   262,   263,   264,   265,   266,   267,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   283,   284,   285,   286,    -1,    -1,    -1,   290,
     291,   292,     3,    -1,    -1,     6,    -1,     8,     9,    -1,
      11,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    19,    20,
      21,    22,    23,    24,    -1,    26,    -1,   318,   319,    -1,
      31,    -1,    -1,    -1,    -1,    36,    -1,    -1,    -1,    -1,
      41,    -1,    -1,    -1,    -1,    -1,    47,    48,    -1,    -1,
      -1,    -1,    -1,    54,    55,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    68,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      91,    92,    93,    -1,    -1,    96,    -1,    98,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   117,    -1,    -1,   120,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   128,   129,   130,
      -1,    -1,    -1,    -1,   135,    -1,   137,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     151,   152,   153,   154,   155,   156,   157,   158,   159,   160,
     161,   162,   163,   164,   165,   166,   167,   168,   169,   170,
     171,   172,   173,   174,   175,   176,   177,   178,   179,   180,
     181,   182,   183,   184,   185,   186,   187,   188,   189,   190,
     191,   192,   193,   194,   195,   196,   197,   198,   199,   200,
     201,   202,   203,   204,   205,   206,   207,   208,   209,   210,
     211,   212,   213,   214,   215,   216,   217,   218,   219,   220,
     221,   222,   223,   224,   225,   226,   227,   228,   229,   230,
     231,   232,   233,   234,   235,   236,   237,    -1,   239,   240,
      -1,    -1,   243,   244,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   259,   260,
     261,   262,   263,   264,   265,   266,   267,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   283,   284,   285,   286,    -1,    -1,    -1,   290,
     291,   292,     3,     4,    -1,     6,    -1,     8,     9,    -1,
      11,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    19,    20,
      21,    22,    23,    24,    -1,    26,    -1,   318,   319,    -1,
      31,    -1,    -1,    -1,    -1,    36,    -1,    -1,    -1,    -1,
      41,    -1,    -1,    -1,    -1,    -1,    47,    48,    -1,    -1,
      -1,    -1,    -1,    54,    55,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    68,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      91,    92,    93,    -1,    -1,    -1,    -1,    98,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   117,    -1,    -1,   120,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   128,   129,   130,
      -1,    -1,    -1,    -1,   135,    -1,   137,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     151,   152,   153,   154,   155,   156,   157,   158,   159,   160,
     161,   162,   163,   164,   165,   166,   167,   168,   169,   170,
     171,   172,   173,   174,   175,   176,   177,   178,   179,   180,
     181,   182,   183,   184,   185,   186,   187,   188,   189,   190,
     191,   192,   193,   194,   195,   196,   197,   198,   199,   200,
     201,   202,   203,   204,   205,   206,   207,   208,   209,   210,
     211,   212,   213,   214,   215,   216,   217,   218,   219,   220,
     221,   222,   223,   224,   225,   226,   227,   228,   229,   230,
     231,   232,   233,   234,   235,   236,   237,    -1,   239,   240,
      -1,    -1,   243,   244,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   259,   260,
     261,   262,   263,   264,   265,   266,   267,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   283,   284,   285,   286,    -1,    -1,    -1,   290,
     291,   292,     3,    -1,    -1,     6,    -1,     8,     9,    -1,
      11,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    19,    20,
      21,    22,    23,    24,    -1,    26,    -1,   318,   319,    -1,
      31,    -1,    -1,    -1,    -1,    36,    -1,    -1,    -1,    -1,
      41,    -1,    -1,    -1,    -1,    -1,    47,    48,    -1,    -1,
      -1,    -1,    -1,    54,    55,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    68,    -1,    -1,
      -1,    -1,    -1,    74,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      91,    92,    93,    -1,    -1,    -1,    -1,    98,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   117,    -1,    -1,   120,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   128,   129,   130,
      -1,    -1,    -1,    -1,   135,    -1,   137,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     151,   152,   153,   154,   155,   156,   157,   158,   159,   160,
     161,   162,   163,   164,   165,   166,   167,   168,   169,   170,
     171,   172,   173,   174,   175,   176,   177,   178,   179,   180,
     181,   182,   183,   184,   185,   186,   187,   188,   189,   190,
     191,   192,   193,   194,   195,   196,   197,   198,   199,   200,
     201,   202,   203,   204,   205,   206,   207,   208,   209,   210,
     211,   212,   213,   214,   215,   216,   217,   218,   219,   220,
     221,   222,   223,   224,   225,   226,   227,   228,   229,   230,
     231,   232,   233,   234,   235,   236,   237,    -1,   239,   240,
      -1,    -1,   243,   244,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   259,   260,
     261,   262,   263,   264,   265,   266,   267,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   283,   284,   285,   286,    -1,    -1,    -1,   290,
     291,   292,     3,    -1,    -1,     6,    -1,     8,     9,    -1,
      11,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    19,    20,
      21,    22,    23,    24,    -1,    26,    -1,   318,   319,    -1,
      31,    -1,    -1,    -1,    -1,    36,    -1,    -1,    -1,    -1,
      41,    -1,    -1,    -1,    -1,    -1,    47,    48,    -1,    -1,
      -1,    -1,    -1,    54,    55,    -1,    -1,    -1,    59,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    68,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      91,    92,    93,    -1,    -1,    -1,    -1,    98,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   117,    -1,    -1,   120,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   128,   129,   130,
      -1,    -1,    -1,    -1,   135,    -1,   137,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     151,   152,   153,   154,   155,   156,   157,   158,   159,   160,
     161,   162,   163,   164,   165,   166,   167,   168,   169,   170,
     171,   172,   173,   174,   175,   176,   177,   178,   179,   180,
     181,   182,   183,   184,   185,   186,   187,   188,   189,   190,
     191,   192,   193,   194,   195,   196,   197,   198,   199,   200,
     201,   202,   203,   204,   205,   206,   207,   208,   209,   210,
     211,   212,   213,   214,   215,   216,   217,   218,   219,   220,
     221,   222,   223,   224,   225,   226,   227,   228,   229,   230,
     231,   232,   233,   234,   235,   236,   237,    -1,   239,   240,
      -1,    -1,   243,   244,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   259,   260,
     261,   262,   263,   264,   265,   266,   267,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   283,   284,   285,   286,    -1,    -1,    -1,   290,
     291,   292,     3,    -1,    -1,     6,    -1,     8,     9,    -1,
      11,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    19,    20,
      21,    22,    23,    24,    -1,    26,    -1,   318,   319,    -1,
      31,    -1,    -1,    -1,    -1,    36,    -1,    -1,    -1,    -1,
      41,    -1,    -1,    -1,    -1,    -1,    47,    48,    -1,    -1,
      -1,    -1,    -1,    54,    55,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    68,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      91,    92,    93,    -1,    -1,    -1,    -1,    98,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   117,    -1,    -1,   120,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   128,   129,   130,
      -1,    -1,    -1,    -1,   135,    -1,   137,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     151,   152,   153,   154,   155,   156,   157,   158,   159,   160,
     161,   162,   163,   164,   165,   166,   167,   168,   169,   170,
     171,   172,   173,   174,   175,   176,   177,   178,   179,   180,
     181,   182,   183,   184,   185,   186,   187,   188,   189,   190,
     191,   192,   193,   194,   195,   196,   197,   198,   199,   200,
     201,   202,   203,   204,   205,   206,   207,   208,   209,   210,
     211,   212,   213,   214,   215,   216,   217,   218,   219,   220,
     221,   222,   223,   224,   225,   226,   227,   228,   229,   230,
     231,   232,   233,   234,   235,   236,   237,    -1,   239,   240,
      -1,    -1,   243,   244,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   259,   260,
     261,   262,   263,   264,   265,   266,   267,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   283,   284,   285,   286,    -1,    -1,    -1,   290,
     291,   292,     3,    -1,    -1,     6,    -1,     8,     9,    -1,
      11,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    20,
      21,    22,    23,    24,    -1,    26,    -1,   318,   319,    -1,
      31,    -1,    -1,    -1,    -1,    36,    -1,    -1,    -1,    -1,
      41,    -1,    -1,    -1,    -1,    -1,    -1,    48,    -1,    -1,
      -1,    -1,    -1,    54,    55,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    68,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    92,    93,    -1,    -1,    -1,    -1,    98,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   117,    -1,    -1,   120,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   130,
      -1,    -1,    -1,    -1,    -1,    -1,   137,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     151,   152,   153,   154,   155,   156,   157,   158,   159,   160,
     161,   162,   163,   164,   165,   166,   167,   168,   169,   170,
     171,    -1,   173,    -1,    -1,    -1,   177,   178,   179,   180,
     181,   182,   183,   184,   185,   186,   187,   188,   189,   190,
     191,   192,   193,   194,   195,   196,   197,   198,   199,   200,
     201,   202,   203,   204,   205,   206,   207,   208,   209,   210,
     211,   212,   213,   214,   215,   216,   217,   218,   219,   220,
     221,   222,   223,   224,   225,   226,   227,   228,   229,   230,
     231,   232,   233,   234,   235,   236,   237,    -1,   239,   240,
      -1,    -1,   243,   244,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   259,   260,
     261,   262,   263,   264,   265,   266,   267,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   283,   284,   285,   286,    -1,    -1,    -1,   290,
     291,   292,     3,    -1,    -1,     6,    -1,     8,     9,    -1,
      11,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    20,
      21,    22,    23,    24,    -1,    26,    -1,   318,   319,    -1,
      31,    -1,    -1,    -1,    -1,    36,    -1,    -1,    -1,    -1,
      41,    -1,    -1,    -1,    -1,    -1,    -1,    48,    -1,    -1,
      -1,    -1,    -1,    54,    55,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    68,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    92,    93,    -1,    -1,    -1,    -1,    98,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   117,    -1,    -1,   120,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   130,
      -1,    -1,    -1,    -1,    -1,    -1,   137,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     151,   152,   153,   154,   155,   156,   157,   158,   159,   160,
     161,   162,   163,   164,   165,   166,   167,   168,   169,   170,
     171,    -1,   173,    -1,    -1,    -1,   177,   178,   179,   180,
     181,   182,   183,   184,   185,   186,   187,   188,   189,   190,
     191,   192,   193,   194,   195,   196,   197,   198,   199,   200,
     201,   202,   203,   204,   205,   206,   207,   208,   209,   210,
     211,   212,   213,   214,   215,   216,   217,   218,   219,   220,
     221,   222,   223,   224,   225,   226,   227,   228,   229,   230,
     231,   232,   233,   234,   235,   236,   237,    -1,   239,   240,
      -1,    -1,   243,   244,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    11,    -1,    -1,   259,   260,
     261,   262,   263,   264,   265,   266,   267,    -1,    -1,    -1,
      26,    -1,    -1,    -1,    -1,    31,    -1,    -1,    -1,    -1,
      36,    -1,   283,   284,   285,   286,    -1,    -1,    -1,   290,
     291,   292,    48,    -1,    -1,    -1,    -1,    -1,    54,    55,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    92,    93,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   117,    -1,    -1,   120,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   151,    -1,   153,   154,    -1,
     156,   157,   158,   159,   160,   161,   162,   163,   164,   165,
     166,   167,   168,   169,   170,   171,    -1,   173,    -1,    -1,
      -1,   177,   178,   179,   180,   181,   182,   183,   184,   185,
     186,   187,   188,   189,   190,   191,   192,   193,   194,   195,
     196,   197,   198,   199,   200,   201,   202,   203,   204,   205,
     206,   207,   208,   209,   210,   211,   212,   213,   214,   215,
     216,   217,   218,   219,   220,   221,   222,   223,   224,   225,
     226,   227,   228,   229,   230,   231,   232,   233,   234,   235,
     236,   237,    11,   239,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    26,    -1,    -1,
      -1,    -1,    31,    -1,    -1,    -1,    -1,    36,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    48,
      -1,    -1,    -1,    -1,    -1,    54,    55,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    92,    93,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   117,    -1,
      -1,   120,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   151,    -1,   153,    -1,    -1,   156,   157,   158,
     159,   160,   161,   162,   163,   164,    -1,   166,   167,   168,
     169,   170,   171,    -1,   173,    -1,    -1,    -1,   177,   178,
     179,   180,   181,   182,    -1,   184,   185,   186,   187,   188,
     189,   190,   191,   192,   193,   194,   195,   196,   197,   198,
     199,   200,   201,   202,   203,   204,   205,   206,   207,   208,
     209,   210,   211,   212,   213,   214,   215,   216,   217,   218,
     219,   220,   221,   222,   223,   224,   225,   226,   227,   228,
     229,   230,   231,   232,   233,   234,   235,   236,   237,    -1,
     239
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int16 yystos[] =
{
       0,     3,     6,     8,     9,    11,    19,    20,    21,    22,
      23,    24,    25,    26,    31,    36,    41,    45,    47,    48,
      49,    54,    55,    59,    60,    67,    68,    69,    76,    84,
      91,    92,    93,    98,   111,   113,   117,   120,   128,   129,
     130,   132,   134,   135,   137,   151,   152,   153,   154,   155,
     156,   157,   158,   159,   160,   161,   162,   163,   164,   165,
     166,   167,   168,   169,   170,   171,   172,   173,   174,   175,
     176,   177,   178,   179,   180,   181,   182,   183,   184,   185,
     186,   187,   188,   189,   190,   191,   192,   193,   194,   195,
     196,   197,   198,   199,   200,   201,   202,   203,   204,   205,
     206,   207,   208,   209,   210,   211,   212,   213,   214,   215,
     216,   217,   218,   219,   220,   221,   222,   223,   224,   225,
     226,   227,   228,   229,   230,   231,   232,   233,   234,   235,
     236,   237,   239,   240,   243,   244,   259,   260,   261,   262,
     263,   264,   265,   266,   267,   283,   284,   285,   286,   290,
     291,   292,   312,   313,   314,   315,   316,   317,   318,   319,
     326,   327,   328,   329,   340,   343,   354,   355,   356,   357,
     358,   359,   360,   363,   364,   365,   366,   371,   376,   402,
     404,   405,   406,   407,   408,   410,   411,   412,   413,   414,
     415,   416,   418,   419,   421,   422,   423,   424,   426,   427,
     428,   432,   434,   435,   436,   437,   438,   439,   443,   444,
     445,   446,   447,   448,   449,   450,   451,   452,   453,   454,
     455,   456,   457,   458,   459,   460,   461,   462,   463,   464,
     465,   466,   467,   468,   474,   475,   480,   481,   483,   522,
     523,   524,   525,   528,   530,   531,   532,   548,   551,   552,
     553,   554,   555,   556,   557,   558,   561,   562,   563,   575,
     576,   577,   578,   579,   581,   582,   583,   584,   588,   590,
     591,   592,   593,   594,   595,   603,   629,   630,   631,   632,
     633,   634,   646,   649,     3,   139,   327,   361,   375,   376,
     407,   541,   542,   543,   545,   547,   575,    26,    26,     8,
     146,   147,   148,   149,   515,   516,   520,   521,     3,    11,
      25,   158,   404,     6,    13,     3,   143,     3,     3,   121,
     140,   254,    74,     3,   428,    26,   645,    28,    60,   110,
     113,   134,   136,   158,   345,   346,   347,   354,     3,   143,
      28,    62,   367,     3,     3,   428,    26,   599,   600,   601,
     602,     3,     3,     3,    83,     3,     3,     3,     3,     3,
     245,   364,   639,   640,   644,     3,     3,     3,     3,     3,
     553,   553,     0,     7,    37,    79,    89,   131,   411,   412,
     414,   415,   418,   421,   423,    84,   311,   310,    28,    31,
     117,   363,   364,   425,    19,    21,    22,    24,     3,     3,
       3,     3,     3,     3,     3,     3,   282,     3,     3,     3,
       3,     3,   321,   322,   318,   319,    34,   559,   320,   318,
      44,   580,     3,   545,     3,     4,    85,     4,     4,    66,
     131,     4,    10,   317,    26,   446,   522,   532,   145,    12,
      12,    19,     3,   113,   376,   407,    26,    26,   154,   165,
     183,   321,   604,   629,   549,   575,   549,   576,   321,   367,
     599,   599,    26,   669,   599,     3,    83,   106,     3,   342,
     342,   342,   100,     5,    96,   575,   321,   377,   429,   430,
     575,   598,   575,   576,    39,    74,    88,   127,   575,   596,
     597,     6,    13,   114,   575,   575,   575,   599,   572,   575,
     572,   575,   585,   586,   587,    55,   124,   125,   184,   185,
     186,   191,   196,   440,   441,   442,   564,   575,   575,     3,
     575,   407,   246,   640,   641,   245,   635,   636,   639,   572,
     575,   572,   575,     4,   432,   469,   470,   471,   646,   575,
     363,     3,   420,   428,   552,   576,    28,   546,   363,   354,
     405,   406,   428,     4,    96,   573,   575,   642,   643,   569,
     573,   573,   574,   574,   570,   573,     4,   574,   571,   573,
       3,    26,   486,   487,   488,   494,   476,   575,   575,   367,
     529,   556,     4,   527,   572,   554,   554,   555,   555,    90,
     560,   575,   581,   599,   575,     4,   362,   363,   546,   546,
     546,     3,   599,   367,    13,    13,    32,     5,   138,     4,
     573,     3,   342,   247,   248,   249,   651,   141,   374,   397,
     547,   372,   373,   431,   646,    26,   339,   629,   599,   347,
       4,    74,    83,   390,   391,     5,    32,   433,   629,   138,
     575,    74,     4,    74,   575,    26,   601,    26,   601,   602,
     368,   369,   629,     4,     4,     4,   342,     4,     5,     7,
      79,     4,    74,    74,    74,   102,   350,   241,   575,   642,
     242,   364,   409,   411,   412,   414,   418,   421,   637,   638,
     641,   636,     4,     4,     4,     4,     5,   311,   572,    11,
      65,   417,   417,   376,    68,    96,   130,   133,   401,     4,
      40,   138,   411,     4,     5,     4,     5,     7,     4,     4,
       5,     4,     4,     5,   486,   495,   498,   499,     5,   472,
       4,   573,     4,     4,    40,     4,     4,     5,   542,   544,
     375,   543,   375,     3,   517,   377,    26,    26,   604,    21,
      42,    53,    56,    57,    63,    70,    82,    95,    97,   109,
     116,   122,   123,   153,   268,   269,   272,   273,   275,   276,
     277,   278,   280,   281,   550,   599,   605,   608,   609,   616,
     617,   618,   619,   620,   621,   623,   624,   625,   626,   628,
     550,   599,     4,    43,    72,   105,   132,   330,   331,   332,
     335,   337,   629,    32,    99,    60,   134,   158,   652,   407,
       5,     4,     5,   126,     3,    11,   392,   396,   428,   537,
     538,   539,   599,   372,   374,   430,   629,   599,   575,   575,
      74,    13,     5,   374,   317,   361,   575,   575,   575,   575,
     576,   575,    40,     4,   642,   363,     5,   241,   242,   469,
     470,   484,   363,     4,    65,   576,   572,   575,   575,   573,
     573,   573,   573,   573,   303,   496,   500,     4,    23,   477,
       4,   287,   288,   473,   479,     4,   238,   526,   526,   572,
     363,   432,   518,   519,   646,    13,    13,     3,   270,   274,
     610,   611,     3,   622,   622,   104,     3,   270,   274,   610,
     622,   610,   610,   270,   274,   610,   611,   611,     3,   612,
     613,    42,   153,   612,   612,   270,   274,   610,    55,   184,
     185,   186,   191,   196,   564,   565,   567,   568,     4,    42,
     606,   580,   607,     4,     4,     3,    87,    87,     4,     5,
       3,   605,   376,    99,   653,   100,   373,   629,    26,   108,
     348,   349,   648,     3,   396,   539,   150,     5,    50,    75,
      81,    94,   160,   168,   533,   536,    32,   393,   647,   393,
     395,   390,    77,   398,     4,   575,    26,   369,    59,   370,
     575,     4,    71,   589,   589,     4,   351,   352,   364,   408,
     354,   364,   411,   412,   414,   418,   421,   638,   642,     4,
      19,   411,     5,     5,    40,   350,     5,   473,   289,   289,
      77,     4,     4,     5,   604,   604,    23,   611,   271,    23,
      23,   611,   271,   142,   279,   627,   627,   611,   271,    23,
     614,   270,   274,   610,   270,   610,   611,   271,   622,   610,
     126,   114,   607,   407,     3,   331,   339,   333,   142,   341,
     339,   654,   602,     5,   142,   344,     4,   539,   396,    86,
     536,   103,    86,    26,     3,   338,    26,    40,    78,   400,
     575,     4,     4,     5,    33,    61,   353,   353,    89,   363,
      74,   482,   485,    12,   575,   573,   573,    26,   501,   502,
     603,   301,   302,   497,   504,   505,   478,   575,     3,   526,
     519,     4,   612,     4,     5,     4,   612,   122,   122,   612,
      14,    15,    16,    17,    18,   615,     4,   611,   271,   611,
     612,   191,   564,   566,    26,     4,   339,     4,    25,    43,
      59,   110,   335,   336,    43,   251,   650,   349,    76,    12,
     396,    86,   396,   339,   338,   399,   446,   603,   407,   304,
     489,   490,   352,   293,   294,   473,     5,     5,   607,    37,
      51,   299,   444,   506,   507,   508,   509,   350,    23,   144,
     144,   612,   610,     4,    96,     3,    96,   137,   334,   432,
     558,   599,   101,   257,   258,   662,   663,    71,   655,   656,
     101,   396,   100,   138,   534,   535,   540,     4,     5,   486,
     491,   492,   493,   350,   573,   502,   299,   444,   507,   510,
     512,   513,   255,   300,   300,   295,   503,   514,     4,     4,
     110,   407,     3,   121,   255,   394,   121,   394,   663,   250,
     245,   658,   407,     3,   446,   603,     5,    32,   306,   387,
     389,   298,   298,   311,    51,    77,   297,   305,   599,     4,
     339,   393,   393,   393,   393,   255,   256,   657,     3,   403,
     252,   327,   659,   661,   339,   492,   494,    23,   307,   378,
     379,   511,   512,   255,   296,     3,     4,    26,   665,   666,
      26,   668,   664,   666,    26,   667,   407,   253,     4,   307,
     388,   385,   432,    69,   383,   384,   339,     4,   660,   661,
      23,   255,   302,   382,   293,   308,   381,     4,     7,   380,
     386,   432,   242,   661,   382,   309
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int16 yyr1[] =
{
       0,   325,   326,   326,   327,   327,   328,   328,   328,   328,
     329,   330,   330,   331,   331,   332,   333,   333,   334,   335,
     335,   336,   336,   336,   336,   336,   336,   336,   336,   336,
     336,   337,   337,   337,   337,   338,   338,   339,   339,   340,
     341,   341,   342,   342,   343,   344,   344,   345,   345,   346,
     346,   347,   347,   347,   347,   347,   347,   348,   348,   349,
     349,   350,   350,   351,   351,   352,   352,   353,   353,   353,
     354,   354,   355,   355,   355,   355,   355,   355,   355,   355,
     355,   356,   356,   357,   358,   359,   360,   361,   362,   362,
     363,   364,   364,   365,   366,   367,   367,   367,   368,   368,
     369,   370,   370,   371,   372,   372,   373,   374,   374,   375,
     376,   377,   377,   378,   378,   379,   380,   380,   381,   381,
     382,   382,   383,   383,   384,   385,   386,   387,   387,   388,
     388,   389,   390,   391,   392,   392,   393,   393,   394,   394,
     395,   395,   396,   396,   396,   396,   396,   397,   398,   398,
     399,   399,   399,   399,   400,   400,   401,   401,   401,   401,
     402,   402,   402,   403,   404,   404,   405,   405,   406,   406,
     407,   407,   408,   408,   408,   408,   408,   408,   408,   408,
     409,   410,   410,   411,   411,   411,   411,   411,   411,   412,
     413,   413,   414,   415,   416,   416,   416,   416,   417,   417,
     417,   418,   419,   419,   420,   420,   421,   422,   422,   423,
     424,   424,   425,   425,   425,   426,   427,   428,   429,   429,
     430,   431,   432,   432,   432,   432,   432,   432,   432,   432,
     432,   433,   433,   433,   434,   434,   435,   435,   435,   436,
     436,   437,   438,   439,   439,   439,   440,   440,   441,   441,
     441,   442,   442,   443,   444,   444,   445,   445,   445,   445,
     445,   445,   445,   445,   445,   445,   445,   445,   445,   446,
     446,   446,   446,   446,   446,   446,   446,   446,   446,   446,
     447,   447,   448,   448,   448,   449,   449,   450,   450,   450,
     450,   451,   451,   452,   453,   453,   453,   453,   454,   454,
     454,   454,   454,   454,   454,   455,   455,   455,   456,   457,
     458,   458,   458,   458,   458,   459,   459,   459,   460,   460,
     460,   460,   460,   460,   460,   460,   460,   460,   460,   460,
     460,   460,   461,   461,   462,   463,   463,   463,   463,   463,
     463,   463,   463,   463,   463,   463,   463,   463,   463,   463,
     463,   463,   463,   464,   464,   464,   465,   465,   466,   467,
     467,   467,   467,   467,   467,   467,   468,   469,   470,   471,
     471,   472,   472,   472,   473,   473,   474,   475,   475,   476,
     477,   478,   479,   479,   480,   481,   481,   482,   482,   483,
     484,   484,   485,   485,   486,   487,   487,   488,   489,   489,
     490,   491,   491,   492,   493,   494,   495,   495,   496,   496,
     497,   497,   498,   499,   500,   501,   501,   502,   503,   503,
     504,   505,   505,   506,   506,   507,   507,   507,   508,   509,
     510,   511,   512,   512,   512,   513,   514,   514,   514,   514,
     515,   515,   516,   517,   517,   518,   518,   519,   519,   519,
     520,   520,   521,   521,   521,   522,   522,   522,   522,   522,
     523,   523,   523,   523,   523,   523,   523,   523,   523,   523,
     523,   523,   523,   523,   524,   524,   525,   525,   526,   526,
     527,   528,   529,   530,   530,   531,   532,   532,   532,   532,
     533,   533,   533,   534,   535,   535,   536,   536,   536,   536,
     537,   538,   538,   538,   539,   540,   541,   541,   542,   542,
     543,   543,   544,   545,   545,   545,   546,   546,   547,   548,
     549,   550,   550,   551,   552,   552,   552,   552,   552,   552,
     552,   552,   553,   553,   554,   554,   554,   555,   555,   555,
     556,   556,   556,   557,   558,   558,   558,   559,   560,   561,
     561,   562,   563,   564,   564,   564,   564,   564,   565,   566,
     566,   567,   567,   568,   568,   569,   570,   571,   572,   572,
     572,   573,   573,   573,   573,   574,   574,   574,   575,   575,
     575,   576,   577,   577,   578,   578,   579,   579,   580,   581,
     581,   582,   582,   583,   584,   585,   586,   587,   588,   588,
     588,   588,   588,   589,   589,   590,   590,   591,   591,   592,
     593,   593,   594,   595,   596,   596,   596,   596,   596,   597,
     597,   597,   598,   599,   599,   599,   600,   600,   601,   602,
     603,   603,   603,   603,   603,   604,   604,   605,   606,   606,
     607,   607,   608,   608,   608,   608,   608,   608,   608,   609,
     609,   609,   609,   609,   609,   610,   610,   611,   612,   612,
     613,   614,   615,   615,   615,   615,   615,   615,   616,   616,
     616,   617,   617,   617,   617,   617,   617,   617,   618,   618,
     618,   619,   619,   619,   619,   620,   620,   621,   621,   622,
     622,   622,   623,   623,   623,   623,   623,   623,   623,   624,
     624,   624,   624,   624,   625,   626,   626,   626,   627,   627,
     627,   628,   629,   629,   629,   629,   630,   630,   631,   631,
     631,   632,   632,   633,   634,   635,   635,   636,   637,   637,
     638,   638,   638,   638,   638,   638,   639,   639,   640,   641,
     641,   642,   642,   643,   644,   645,   646,   646,   646,   647,
     647,   648,   327,   327,   649,   650,   650,   651,   651,   651,
     652,   652,   652,   653,   653,   654,   655,   656,   656,   657,
     657,   658,   658,   659,   659,   660,   660,   661,   662,   662,
     663,   663,   663,   663,   664,   665,   666,   667,   668,   669
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     2,     1,     1,     1,     1,     1,     1,
       6,     1,     3,     1,     1,     3,     0,     2,     1,     1,
       2,     2,     1,     2,     2,     2,     2,     1,     4,     2,
       5,     4,     7,    10,     4,     0,     3,     3,     1,     7,
       0,     3,     0,     3,     7,     0,     3,     2,     1,     1,
       3,     1,     2,     1,     2,     2,     1,     1,     3,     1,
       1,     0,     3,     1,     3,     2,     2,     0,     1,     1,
       0,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       3,     1,     4,     2,     4,     4,     5,     4,     1,     3,
       1,     1,     1,     2,     6,     0,     1,     1,     1,     3,
       3,     1,     1,     5,     1,     3,     1,     0,     1,     1,
       4,     1,     1,     0,     1,     3,     0,     1,     1,     1,
       1,     1,     0,     1,     5,     1,     1,     0,     1,     0,
       2,     3,     9,     2,     1,     3,     0,     1,     0,     1,
       0,     3,     2,     3,     1,     4,     3,     2,     0,     3,
       1,     1,     3,     3,     0,     2,     1,     1,     1,     1,
       1,     3,     1,     3,     1,     4,     1,     2,     1,     3,
       1,     3,     1,     1,     1,     1,     1,     1,     1,     1,
       2,     3,     2,     1,     1,     1,     1,     1,     1,     5,
       2,     1,     4,     4,     2,     2,     1,     1,     0,     2,
       4,     3,     2,     1,     1,     3,     3,     2,     1,     3,
       2,     1,     1,     1,     1,     2,     2,     3,     1,     3,
       1,     1,     1,     1,     1,     1,     1,     2,     2,     2,
       2,     0,     2,     1,     6,     4,     1,     1,     1,     4,
       4,     4,     4,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     6,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       3,     3,     3,     4,     4,     4,     4,     4,     4,     4,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     3,     3,
       3,     1,     1,     1,     1,     1,     4,     1,     1,     1,
       1,     0,     2,     4,     0,     1,     6,     1,     1,     1,
       1,     1,     2,     2,     5,     1,     1,     0,     1,     8,
       1,     1,     2,     2,     1,     1,     1,     1,     0,     1,
       2,     3,     1,     3,     1,     3,     0,     1,     0,     1,
       0,     1,     4,     1,     3,     3,     1,     2,     0,     1,
       3,     1,     1,     1,     1,     2,     1,     2,     2,     4,
       1,     1,     1,     2,     1,     2,     3,     2,     2,     3,
       0,     2,     4,     0,     3,     1,     3,     0,     1,     1,
       2,     2,     1,     1,     1,     5,     4,     5,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     5,     7,     0,     5,
       1,     5,     1,     1,     1,     5,     1,     1,     1,     1,
       1,     1,     1,     2,     1,     1,     0,     1,     1,     2,
       4,     5,     5,     1,     1,     4,     1,     1,     1,     3,
       1,     4,     1,     1,     4,     4,     0,     1,     1,     1,
       1,     1,     1,     6,     1,     1,     1,     1,     1,     1,
       3,     1,     1,     1,     1,     2,     2,     1,     3,     3,
       1,     3,     3,     1,     1,     1,     1,     2,     1,     1,
       2,     1,     1,     1,     1,     1,     1,     1,     2,     1,
       2,     2,     2,     3,     1,     3,     5,     7,     1,     3,
       3,     1,     3,     5,     3,     1,     3,     3,     1,     1,
       1,     1,     1,     1,     3,     3,     1,     1,     2,     1,
       2,     1,     1,     1,     7,     1,     1,     0,     1,     1,
       1,     1,     1,     0,     2,     7,     4,     1,     1,     4,
       6,     6,     6,     4,     4,     3,     3,     2,     1,     1,
       1,     1,     2,     1,     1,     1,     3,     3,     3,     1,
       1,     3,     5,     7,     7,     1,     1,     1,     0,     3,
       0,     1,     3,     2,     1,     1,     1,     1,     1,     2,
       2,     3,     3,     2,     1,     0,     1,     3,     0,     1,
       3,     2,     0,     1,     1,     1,     1,     1,     4,     4,
       2,     3,     3,     2,     4,     4,     3,     1,     5,     4,
       2,     2,     3,     2,     1,     4,     2,     1,     1,     0,
       3,     5,     2,     2,     2,     1,     1,     1,     1,     4,
       1,     1,     1,     2,     1,     1,     3,     3,     0,     3,
       3,     2,     1,     1,     1,     1,     1,     1,     4,     4,
       4,     1,     1,     5,     4,     1,     2,     4,     1,     3,
       1,     1,     1,     1,     1,     1,     1,     2,     4,     0,
       2,     1,     1,     1,     1,     1,     2,     1,     3,     0,
       2,     1,     1,     3,     9,     0,     2,     1,     1,     2,
       1,     1,     2,     0,     2,     1,     3,     0,     3,     1,
       1,     0,     2,     1,     5,     1,     3,     1,     1,     2,
       4,     4,     4,     4,     1,     1,     1,     1,     1,     1
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* sql_single_statement: sql  */
                { xxx_pGLOBAL_SQLPARSER->setParseTree( (yyvsp[0].pParseNode) ); }
    break;

  case 3: /* sql_single_statement: sql ';'  */
                { xxx_pGLOBAL_SQLPARSER->setParseTree( (yyvsp[-1].pParseNode) ); }
    break;

  case 5: /* sql: schema_element  */
           {
                (yyval.pParseNode) = SQL_NEW_RULE;
		(yyval.pParseNode)->append((yyvsp[0].pParseNode));
	   }
    break;

  case 10: /* base_table_def: SQL_TOKEN_CREATE SQL_TOKEN_TABLE table_node '(' base_table_element_commalist ')'  */
                {(yyval.pParseNode) = SQL_NEW_RULE;
		(yyval.pParseNode)->append((yyvsp[-5].pParseNode));
		(yyval.pParseNode)->append((yyvsp[-4].pParseNode));
		(yyval.pParseNode)->append((yyvsp[-3].pParseNode));
		(yyval.pParseNode)->append((yyvsp[-2].pParseNode) = newNode("(", SQL_NODE_PUNCTUATION));
		(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
		(yyval.pParseNode)->append((yyvsp[0].pParseNode) = newNode(")", SQL_NODE_PUNCTUATION));}
    break;

  case 11: /* base_table_element_commalist: base_table_element  */
                {(yyval.pParseNode) = SQL_NEW_COMMALISTRULE;
		(yyval.pParseNode)->append((yyvsp[0].pParseNode));}
    break;

  case 12: /* base_table_element_commalist: base_table_element_commalist ',' base_table_element  */
                {(yyvsp[-2].pParseNode)->append((yyvsp[0].pParseNode));
		(yyval.pParseNode) = (yyvsp[-2].pParseNode);}
    break;

  case 15: /* column_def: column data_type column_def_opt_list  */
                        {(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
			}
    break;

  case 16: /* column_def_opt_list: %empty  */
                                            {(yyval.pParseNode) = SQL_NEW_LISTRULE;}
    break;

  case 17: /* column_def_opt_list: column_def_opt_list column_def_opt  */
                        {(yyvsp[-1].pParseNode)->append((yyvsp[0].pParseNode));
			(yyval.pParseNode) = (yyvsp[-1].pParseNode);}
    break;

  case 20: /* unique_spec: SQL_TOKEN_PRIMARY SQL_TOKEN_KEY  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 21: /* column_def_opt: SQL_TOKEN_NOT SQL_TOKEN_NULL  */
                        {(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));}
    break;

  case 23: /* column_def_opt: SQL_TOKEN_DEFAULT literal  */
                        {(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));}
    break;

  case 24: /* column_def_opt: SQL_TOKEN_DEFAULT SQL_TOKEN_NULL  */
                        {(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));}
    break;

  case 25: /* column_def_opt: SQL_TOKEN_DEFAULT SQL_TOKEN_USER  */
                        {(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));}
    break;

  case 26: /* column_def_opt: SQL_TOKEN_DEFAULT nil_fkt  */
                        {
				(yyval.pParseNode) = SQL_NEW_RULE;
				(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
				(yyval.pParseNode)->append((yyvsp[0].pParseNode));
			}
    break;

  case 28: /* column_def_opt: SQL_TOKEN_CHECK '(' search_condition ')'  */
                        {(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-3].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode) = newNode("(", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode) = newNode(")", SQL_NODE_PUNCTUATION));}
    break;

  case 29: /* column_def_opt: SQL_TOKEN_REFERENCES table_node  */
                        {(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));}
    break;

  case 30: /* column_def_opt: SQL_TOKEN_REFERENCES table_node '(' column_commalist ')'  */
                        {(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-4].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-3].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode) = newNode("(", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode) = newNode(")", SQL_NODE_PUNCTUATION));}
    break;

  case 31: /* table_constraint_def: unique_spec '(' column_commalist ')'  */
                        {(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-3].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode) = newNode("(", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode) = newNode(")", SQL_NODE_PUNCTUATION));}
    break;

  case 32: /* table_constraint_def: SQL_TOKEN_FOREIGN SQL_TOKEN_KEY '(' column_commalist ')' SQL_TOKEN_REFERENCES table_node  */
                        {(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-6].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-5].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-4].pParseNode) = newNode("(", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[-3].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode) = newNode(")", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));}
    break;

  case 33: /* table_constraint_def: SQL_TOKEN_FOREIGN SQL_TOKEN_KEY '(' column_commalist ')' SQL_TOKEN_REFERENCES table_node '(' column_commalist ')'  */
                        {(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-9].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-8].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-7].pParseNode) = newNode("(", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[-6].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-5].pParseNode) = newNode(")", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[-4].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-3].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode) = newNode("(", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode) = newNode(")", SQL_NODE_PUNCTUATION));}
    break;

  case 34: /* table_constraint_def: SQL_TOKEN_CHECK '(' search_condition ')'  */
                        {(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-3].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode) = newNode("(", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode) = newNode(")", SQL_NODE_PUNCTUATION));}
    break;

  case 35: /* op_column_commalist: %empty  */
                    {(yyval.pParseNode) = SQL_NEW_RULE;}
    break;

  case 36: /* op_column_commalist: '(' column_commalist ')'  */
                {(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode) = newNode("(", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode) = newNode(")", SQL_NODE_PUNCTUATION));
		}
    break;

  case 37: /* column_commalist: column_commalist ',' column  */
                        {(yyvsp[-2].pParseNode)->append((yyvsp[0].pParseNode));
			(yyval.pParseNode) = (yyvsp[-2].pParseNode);}
    break;

  case 38: /* column_commalist: column  */
                        {(yyval.pParseNode) = SQL_NEW_COMMALISTRULE;
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));}
    break;

  case 39: /* view_def: SQL_TOKEN_CREATE SQL_TOKEN_VIEW table_node opt_column_commalist SQL_TOKEN_AS select_statement opt_with_check_option  */
                        {(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-6].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-5].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-4].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-3].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));}
    break;

  case 40: /* opt_with_check_option: %empty  */
                                    {(yyval.pParseNode) = SQL_NEW_RULE;}
    break;

  case 41: /* opt_with_check_option: SQL_TOKEN_WITH SQL_TOKEN_CHECK SQL_TOKEN_OPTION  */
                        {(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));}
    break;

  case 42: /* opt_column_commalist: %empty  */
                                    {(yyval.pParseNode) = SQL_NEW_RULE;}
    break;

  case 43: /* opt_column_commalist: '(' column_commalist ')'  */
                        {(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode) = newNode("(", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode) = newNode(")", SQL_NODE_PUNCTUATION));}
    break;

  case 44: /* privilege_def: SQL_TOKEN_GRANT privileges SQL_TOKEN_ON table_node SQL_TOKEN_TO grantee_commalist opt_with_grant_option  */
                        {(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-6].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-5].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-4].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-3].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));}
    break;

  case 45: /* opt_with_grant_option: %empty  */
                                    {(yyval.pParseNode) = SQL_NEW_RULE;}
    break;

  case 46: /* opt_with_grant_option: SQL_TOKEN_WITH SQL_TOKEN_GRANT SQL_TOKEN_OPTION  */
                        {(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));}
    break;

  case 47: /* privileges: SQL_TOKEN_ALL SQL_TOKEN_PRIVILEGES  */
                        {(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));}
    break;

  case 49: /* operation_commalist: operation  */
                        {(yyval.pParseNode) = SQL_NEW_COMMALISTRULE;
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));}
    break;

  case 50: /* operation_commalist: operation_commalist ',' operation  */
                        {(yyvsp[-2].pParseNode)->append((yyvsp[0].pParseNode));
			(yyval.pParseNode) = (yyvsp[-2].pParseNode);}
    break;

  case 52: /* operation: SQL_TOKEN_INSERT opt_column_commalist  */
                        {(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));}
    break;

  case 54: /* operation: SQL_TOKEN_UPDATE opt_column_commalist  */
                        {(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));}
    break;

  case 55: /* operation: SQL_TOKEN_REFERENCES opt_column_commalist  */
                        {(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));}
    break;

  case 57: /* grantee_commalist: grantee  */
                        {(yyval.pParseNode) = SQL_NEW_COMMALISTRULE;
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));}
    break;

  case 58: /* grantee_commalist: grantee_commalist ',' grantee  */
                        {(yyvsp[-2].pParseNode)->append((yyvsp[0].pParseNode));
			(yyval.pParseNode) = (yyvsp[-2].pParseNode);}
    break;

  case 61: /* opt_order_by_clause: %empty  */
                                    {(yyval.pParseNode) = SQL_NEW_RULE;}
    break;

  case 62: /* opt_order_by_clause: SQL_TOKEN_ORDER SQL_TOKEN_BY ordering_spec_commalist  */
                        {(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));}
    break;

  case 63: /* ordering_spec_commalist: ordering_spec  */
                        {(yyval.pParseNode) = SQL_NEW_COMMALISTRULE;
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));}
    break;

  case 64: /* ordering_spec_commalist: ordering_spec_commalist ',' ordering_spec  */
                        {(yyvsp[-2].pParseNode)->append((yyvsp[0].pParseNode));
			(yyval.pParseNode) = (yyvsp[-2].pParseNode);}
    break;

  case 65: /* ordering_spec: predicate opt_asc_desc  */
                        {(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));}
    break;

  case 66: /* ordering_spec: row_value_constructor_elem opt_asc_desc  */
                        {(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));}
    break;

  case 67: /* opt_asc_desc: %empty  */
                {(yyval.pParseNode) = SQL_NEW_RULE;}
    break;

  case 70: /* sql_not: %empty  */
        {(yyval.pParseNode) = SQL_NEW_RULE;}
    break;

  case 80: /* manipulative_statement: '{' odbc_call_spec '}'  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode) = newNode("{", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode) = newNode("}", SQL_NODE_PUNCTUATION));
		}
    break;

  case 82: /* union_statement: union_statement SQL_TOKEN_UNION all select_statement  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-3].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 83: /* commit_statement: SQL_TOKEN_COMMIT SQL_TOKEN_WORK  */
                        {(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));}
    break;

  case 84: /* delete_statement_searched: SQL_TOKEN_DELETE SQL_TOKEN_FROM table_node opt_where_clause  */
                        {(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-3].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));}
    break;

  case 85: /* fetch_statement: SQL_TOKEN_FETCH cursor SQL_TOKEN_INTO target_commalist  */
                        {(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-3].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));}
    break;

  case 86: /* insert_statement: SQL_TOKEN_INSERT SQL_TOKEN_INTO table_node opt_column_commalist values_or_query_spec  */
                        {(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-4].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-3].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));}
    break;

  case 87: /* values_or_query_spec: SQL_TOKEN_VALUES '(' table_value_const_list ')'  */
                {(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-3].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode) = newNode("(", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode) = newNode(")", SQL_NODE_PUNCTUATION));
		}
    break;

  case 88: /* table_value_const_list: row_value_constructor  */
                        {(yyval.pParseNode) = SQL_NEW_COMMALISTRULE;
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));}
    break;

  case 89: /* table_value_const_list: table_value_const_list ',' row_value_constructor  */
                        {(yyvsp[-2].pParseNode)->append((yyvsp[0].pParseNode));
			(yyval.pParseNode) = (yyvsp[-2].pParseNode);}
    break;

  case 93: /* rollback_statement: SQL_TOKEN_ROLLBACK SQL_TOKEN_WORK  */
                        {(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));}
    break;

  case 94: /* select_statement_into: SQL_TOKEN_SELECT opt_all_distinct selection SQL_TOKEN_INTO target_commalist table_exp  */
                        {(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-5].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-4].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-3].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode)); }
    break;

  case 95: /* opt_all_distinct: %empty  */
                        {(yyval.pParseNode) = SQL_NEW_RULE;}
    break;

  case 98: /* assignment_commalist: assignment  */
                        {(yyval.pParseNode) = SQL_NEW_COMMALISTRULE;
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));}
    break;

  case 99: /* assignment_commalist: assignment_commalist ',' assignment  */
                        {(yyvsp[-2].pParseNode)->append((yyvsp[0].pParseNode));
			(yyval.pParseNode) = (yyvsp[-2].pParseNode);}
    break;

  case 100: /* assignment: column SQL_EQUAL update_source  */
                        {(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));}
    break;

  case 103: /* update_statement_searched: SQL_TOKEN_UPDATE table_node SQL_TOKEN_SET assignment_commalist opt_where_clause  */
                        {(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-4].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-3].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));}
    break;

  case 104: /* target_commalist: target  */
                        {(yyval.pParseNode) = SQL_NEW_COMMALISTRULE;
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));}
    break;

  case 105: /* target_commalist: target_commalist ',' target  */
                        {(yyvsp[-2].pParseNode)->append((yyvsp[0].pParseNode));
			(yyval.pParseNode) = (yyvsp[-2].pParseNode);}
    break;

  case 107: /* opt_where_clause: %empty  */
                                        {(yyval.pParseNode) = SQL_NEW_RULE;}
    break;

  case 109: /* query_term: non_join_query_term  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 110: /* select_statement: SQL_TOKEN_SELECT opt_all_distinct selection table_exp  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-3].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 111: /* selection: '*'  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[0].pParseNode) = newNode("*", SQL_NODE_PUNCTUATION));
		}
    break;

  case 113: /* opt_result_offset_clause: %empty  */
                            {(yyval.pParseNode) = SQL_NEW_RULE;}
    break;

  case 115: /* result_offset_clause: SQL_TOKEN_OFFSET offset_row_count row_or_rows  */
        {
		(yyval.pParseNode) = SQL_NEW_RULE;
		(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
		(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
		(yyval.pParseNode)->append((yyvsp[0].pParseNode));
	}
    break;

  case 116: /* opt_fetch_first_row_count: %empty  */
                            {(yyval.pParseNode) = SQL_NEW_RULE;}
    break;

  case 122: /* opt_fetch_first_clause: %empty  */
                            {(yyval.pParseNode) = SQL_NEW_RULE;}
    break;

  case 124: /* fetch_first_clause: SQL_TOKEN_FETCH first_or_next opt_fetch_first_row_count row_or_rows SQL_TOKEN_ONLY  */
        {
		(yyval.pParseNode) = SQL_NEW_RULE;
		(yyval.pParseNode)->append((yyvsp[-4].pParseNode));
		(yyval.pParseNode)->append((yyvsp[-3].pParseNode));
		(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
		(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
		(yyval.pParseNode)->append((yyvsp[0].pParseNode));
	}
    break;

  case 127: /* opt_limit_offset_clause: %empty  */
                            {(yyval.pParseNode) = SQL_NEW_RULE;}
    break;

  case 129: /* opt_offset: %empty  */
                            {(yyval.pParseNode) = SQL_NEW_RULE;}
    break;

  case 130: /* opt_offset: SQL_TOKEN_OFFSET SQL_TOKEN_INTNUM  */
        {
		(yyval.pParseNode) = SQL_NEW_RULE;
		(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
		(yyval.pParseNode)->append((yyvsp[0].pParseNode));
	}
    break;

  case 131: /* limit_offset_clause: SQL_TOKEN_LIMIT SQL_TOKEN_INTNUM opt_offset  */
        {
		(yyval.pParseNode) = SQL_NEW_RULE;
		(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
		(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
		(yyval.pParseNode)->append((yyvsp[0].pParseNode));
	}
    break;

  case 132: /* table_exp: from_clause opt_where_clause opt_group_by_clause opt_having_clause opt_window_clause opt_order_by_clause opt_limit_offset_clause opt_result_offset_clause opt_fetch_first_clause  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-8].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-7].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-6].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-5].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-4].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-3].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 133: /* from_clause: SQL_TOKEN_FROM table_ref_commalist  */
                        {(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));}
    break;

  case 134: /* table_ref_commalist: table_ref  */
                        {(yyval.pParseNode) = SQL_NEW_COMMALISTRULE;
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));}
    break;

  case 135: /* table_ref_commalist: table_ref_commalist ',' table_ref  */
                        {(yyvsp[-2].pParseNode)->append((yyvsp[0].pParseNode));
			(yyval.pParseNode) = (yyvsp[-2].pParseNode);}
    break;

  case 136: /* opt_as: %empty  */
                            {(yyval.pParseNode) = SQL_NEW_RULE;}
    break;

  case 138: /* opt_row: %empty  */
                            {(yyval.pParseNode) = SQL_NEW_RULE;}
    break;

  case 140: /* table_primary_as_range_column: %empty  */
        {(yyval.pParseNode) = SQL_NEW_RULE;}
    break;

  case 141: /* table_primary_as_range_column: opt_as SQL_TOKEN_NAME op_column_commalist  */
                {(yyval.pParseNode) = SQL_NEW_RULE;
		    (yyval.pParseNode)->append((yyvsp[-2].pParseNode));
		    (yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 142: /* table_ref: table_node table_primary_as_range_column  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 143: /* table_ref: subquery range_variable op_column_commalist  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
		    (yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 145: /* table_ref: '{' SQL_TOKEN_OJ joined_table '}'  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-3].pParseNode) = newNode("{", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode) = newNode("}", SQL_NODE_PUNCTUATION));
		}
    break;

  case 146: /* table_ref: '(' joined_table ')'  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode) = newNode("(", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode) = newNode(")", SQL_NODE_PUNCTUATION));
		}
    break;

  case 147: /* where_clause: SQL_TOKEN_WHERE search_condition  */
                        {(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));}
    break;

  case 148: /* opt_group_by_clause: %empty  */
                                 {(yyval.pParseNode) = SQL_NEW_RULE;}
    break;

  case 149: /* opt_group_by_clause: SQL_TOKEN_GROUP SQL_TOKEN_BY column_ref_commalist  */
                        {(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));}
    break;

  case 150: /* column_ref_commalist: column_ref  */
                        {(yyval.pParseNode) = SQL_NEW_COMMALISTRULE;
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));}
    break;

  case 151: /* column_ref_commalist: set_fct_spec  */
                {(yyval.pParseNode) = SQL_NEW_COMMALISTRULE;
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));}
    break;

  case 152: /* column_ref_commalist: column_ref_commalist ',' column_ref  */
                        {(yyvsp[-2].pParseNode)->append((yyvsp[0].pParseNode));
			(yyval.pParseNode) = (yyvsp[-2].pParseNode);}
    break;

  case 153: /* column_ref_commalist: column_ref_commalist ',' set_fct_spec  */
                        {(yyvsp[-2].pParseNode)->append((yyvsp[0].pParseNode));
			(yyval.pParseNode) = (yyvsp[-2].pParseNode);}
    break;

  case 154: /* opt_having_clause: %empty  */
                                            {(yyval.pParseNode) = SQL_NEW_RULE;}
    break;

  case 155: /* opt_having_clause: SQL_TOKEN_HAVING search_condition  */
                        {(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));}
    break;

  case 161: /* boolean_primary: '(' search_condition ')'  */
                { // boolean_primary: rule 2
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode) = newNode("(", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode) = newNode(")", SQL_NODE_PUNCTUATION));
		}
    break;

  case 162: /* boolean_primary: row_value_constructor_elem  */
                {
    		if(xxx_pGLOBAL_SQLPARSER->inPredicateCheck())// boolean_primary: rule 3
			{
			    (yyval.pParseNode) = SQL_NEW_RULE;
			    sal_Int16 nErg = xxx_pGLOBAL_SQLPARSER->buildComparsionRule((yyval.pParseNode),(yyvsp[0].pParseNode));
			    if(nErg == 1)
			    {
				    OSQLParseNode* pTemp = (yyval.pParseNode);
				    (yyval.pParseNode) = pTemp->removeAt((sal_uInt32)0);
				    delete pTemp;
			    }
			    else
			    {
				    delete (yyval.pParseNode);
				    if(nErg)
					    YYERROR;
				    else
					    YYABORT;
			    }
			}
			else
				YYERROR;
		}
    break;

  case 163: /* parenthesized_boolean_value_expression: '(' search_condition ')'  */
        { // boolean_primary: rule 2
		(yyval.pParseNode) = SQL_NEW_RULE;
		(yyval.pParseNode)->append((yyvsp[-2].pParseNode) = newNode("(", SQL_NODE_PUNCTUATION));
		(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
		(yyval.pParseNode)->append((yyvsp[0].pParseNode) = newNode(")", SQL_NODE_PUNCTUATION));
	}
    break;

  case 165: /* boolean_test: boolean_primary SQL_TOKEN_IS sql_not truth_value  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-3].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 167: /* boolean_factor: SQL_TOKEN_NOT boolean_test  */
                { // boolean_factor: rule 1
		    (yyval.pParseNode) = SQL_NEW_RULE;
		    (yyval.pParseNode)->append((yyvsp[-1].pParseNode));
		    (yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 169: /* boolean_term: boolean_term SQL_TOKEN_AND boolean_factor  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE; // boolean_term: rule 1
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 171: /* search_condition: search_condition SQL_TOKEN_OR boolean_term  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE; // search_condition
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 180: /* comparison_predicate_part_2: comparison row_value_constructor  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE; // comparison_predicate: rule 1
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 181: /* comparison_predicate: row_value_constructor comparison row_value_constructor  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE; // comparison_predicate: rule 1
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 182: /* comparison_predicate: comparison row_value_constructor  */
                {
			if(xxx_pGLOBAL_SQLPARSER->inPredicateCheck()) // comparison_predicate: rule 2
			{
				(yyval.pParseNode) = SQL_NEW_RULE;
				sal_Int16 nErg = xxx_pGLOBAL_SQLPARSER->buildPredicateRule((yyval.pParseNode),(yyvsp[0].pParseNode),(yyvsp[-1].pParseNode));
				if(nErg == 1)
				{
					OSQLParseNode* pTemp = (yyval.pParseNode);
					(yyval.pParseNode) = pTemp->removeAt((sal_uInt32)0);
					delete pTemp;
				}
				else
				{
					delete (yyval.pParseNode);
					YYABORT;
				}
			}
			else
			{
				YYERROR;
			}
		}
    break;

  case 189: /* between_predicate_part_2: sql_not SQL_TOKEN_BETWEEN row_value_constructor SQL_TOKEN_AND row_value_constructor  */
                {
			if (xxx_pGLOBAL_SQLPARSER->inPredicateCheck()) // between_predicate: rule 2 
			{
				(yyval.pParseNode) = SQL_NEW_RULE;
				
				sal_Int16 nErg = xxx_pGLOBAL_SQLPARSER->buildPredicateRule((yyval.pParseNode),(yyvsp[-2].pParseNode),(yyvsp[-3].pParseNode),(yyvsp[0].pParseNode));
				if(nErg == 1)
				{
					OSQLParseNode* pTemp = (yyval.pParseNode);
					(yyval.pParseNode) = pTemp->removeAt((sal_uInt32)0);
					OSQLParseNode* pColumnRef = (yyval.pParseNode)->removeAt((sal_uInt32)0);
					(yyval.pParseNode)->insert(0,(yyvsp[-4].pParseNode));
					OSQLParseNode* pBetween_predicate = new OSQLInternalNode(aEmptyString, SQL_NODE_RULE,OSQLParser::RuleID(OSQLParseNode::between_predicate));
					pBetween_predicate->append(pColumnRef);
					pBetween_predicate->append((yyval.pParseNode));
					(yyval.pParseNode) = pBetween_predicate;
					
					delete pTemp;
					delete (yyvsp[-1].pParseNode);
				}
				else
				{
					delete (yyval.pParseNode);
					YYABORT;
				}
			}
			else
			{
				(yyval.pParseNode) = SQL_NEW_RULE; // between_predicate: rule 1
				(yyval.pParseNode)->append((yyvsp[-4].pParseNode));
				(yyval.pParseNode)->append((yyvsp[-3].pParseNode));
				(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
				(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
				(yyval.pParseNode)->append((yyvsp[0].pParseNode));
			}
		}
    break;

  case 190: /* between_predicate: row_value_constructor between_predicate_part_2  */
                {	
			(yyval.pParseNode) = SQL_NEW_RULE; // between_predicate: rule 1
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 192: /* character_like_predicate_part_2: sql_not SQL_TOKEN_LIKE string_value_exp opt_escape  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE; // like_predicate: rule 1
			(yyval.pParseNode)->append((yyvsp[-3].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 193: /* other_like_predicate_part_2: sql_not SQL_TOKEN_LIKE value_exp_primary opt_escape  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE; // like_predicate: rule 1
			(yyval.pParseNode)->append((yyvsp[-3].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 194: /* like_predicate: row_value_constructor character_like_predicate_part_2  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE; // like_predicate: rule 1
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 195: /* like_predicate: row_value_constructor other_like_predicate_part_2  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;  // like_predicate: rule 3
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 196: /* like_predicate: character_like_predicate_part_2  */
                {
			if (xxx_pGLOBAL_SQLPARSER->inPredicateCheck())  // like_predicate: rule 5
			{
				OSQLParseNode* pColumnRef = newNode(aEmptyString, SQL_NODE_RULE,OSQLParser::RuleID(OSQLParseNode::column_ref));
				pColumnRef->append(newNode(xxx_pGLOBAL_SQLPARSER->getFieldName(),SQL_NODE_NAME));

				(yyval.pParseNode) = SQL_NEW_RULE;
				(yyval.pParseNode)->append(pColumnRef);
				(yyval.pParseNode)->append((yyvsp[0].pParseNode));
				OSQLParseNode* p2nd = (yyvsp[0].pParseNode)->removeAt(2);
				OSQLParseNode* p3rd = (yyvsp[0].pParseNode)->removeAt(2);
				if ( !xxx_pGLOBAL_SQLPARSER->buildLikeRule((yyvsp[0].pParseNode),p2nd,p3rd) )
				{
					delete (yyval.pParseNode);
					YYABORT;
				}
				(yyvsp[0].pParseNode)->append(p3rd);
			}
			else
				YYERROR;
		}
    break;

  case 197: /* like_predicate: other_like_predicate_part_2  */
                {
			if (xxx_pGLOBAL_SQLPARSER->inPredicateCheck()) // like_predicate: rule 6
			{
				OSQLParseNode* pColumnRef = newNode(aEmptyString, SQL_NODE_RULE,OSQLParser::RuleID(OSQLParseNode::column_ref));
				pColumnRef->append(newNode(xxx_pGLOBAL_SQLPARSER->getFieldName(),SQL_NODE_NAME));

				(yyval.pParseNode) = SQL_NEW_RULE;
				(yyval.pParseNode)->append(pColumnRef);
				(yyval.pParseNode)->append((yyvsp[0].pParseNode));
				OSQLParseNode* p2nd = (yyvsp[0].pParseNode)->removeAt(2);
				OSQLParseNode* p3rd = (yyvsp[0].pParseNode)->removeAt(2);
				if ( !xxx_pGLOBAL_SQLPARSER->buildLikeRule((yyvsp[0].pParseNode),p2nd,p3rd) )
				{
					delete (yyval.pParseNode);
					YYABORT;
				}
				(yyvsp[0].pParseNode)->append(p3rd);
			}
			else
				YYERROR;
		}
    break;

  case 198: /* opt_escape: %empty  */
                                            {(yyval.pParseNode) = SQL_NEW_RULE;}
    break;

  case 199: /* opt_escape: SQL_TOKEN_ESCAPE string_value_exp  */
                        {(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));}
    break;

  case 200: /* opt_escape: '{' SQL_TOKEN_ESCAPE SQL_TOKEN_STRING '}'  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-3].pParseNode) = newNode("{", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode) = newNode("}", SQL_NODE_PUNCTUATION));
		}
    break;

  case 201: /* null_predicate_part_2: SQL_TOKEN_IS sql_not SQL_TOKEN_NULL  */
        {
		(yyval.pParseNode) = SQL_NEW_RULE; // test_for_null: rule 1
		(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
		(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
		(yyval.pParseNode)->append((yyvsp[0].pParseNode));
	}
    break;

  case 202: /* test_for_null: row_value_constructor null_predicate_part_2  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE; // test_for_null: rule 1
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 203: /* test_for_null: null_predicate_part_2  */
                {
			if (xxx_pGLOBAL_SQLPARSER->inPredicateCheck())// test_for_null: rule 2
			{
				OSQLParseNode* pColumnRef = newNode(aEmptyString, SQL_NODE_RULE,OSQLParser::RuleID(OSQLParseNode::column_ref));
				pColumnRef->append(newNode(xxx_pGLOBAL_SQLPARSER->getFieldName(),SQL_NODE_NAME));

				(yyval.pParseNode) = SQL_NEW_RULE;
				(yyval.pParseNode)->append(pColumnRef);
				(yyval.pParseNode)->append((yyvsp[0].pParseNode));
			}
			else
				YYERROR;
		}
    break;

  case 204: /* in_predicate_value: subquery  */
                {(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 205: /* in_predicate_value: '(' value_exp_commalist ')'  */
                {(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode) = newNode("(", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode) = newNode(")", SQL_NODE_PUNCTUATION));
		}
    break;

  case 206: /* in_predicate_part_2: sql_not SQL_TOKEN_IN in_predicate_value  */
        {
		(yyval.pParseNode) = SQL_NEW_RULE;// in_predicate: rule 1
		(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
		(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
		(yyval.pParseNode)->append((yyvsp[0].pParseNode));
	}
    break;

  case 207: /* in_predicate: row_value_constructor in_predicate_part_2  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;// in_predicate: rule 1
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 208: /* in_predicate: in_predicate_part_2  */
                {
			if ( xxx_pGLOBAL_SQLPARSER->inPredicateCheck() )// in_predicate: rule 2
			{
				OSQLParseNode* pColumnRef = newNode(aEmptyString, SQL_NODE_RULE,OSQLParser::RuleID(OSQLParseNode::column_ref));
				pColumnRef->append(newNode(xxx_pGLOBAL_SQLPARSER->getFieldName(),SQL_NODE_NAME));

				(yyval.pParseNode) = SQL_NEW_RULE;
				(yyval.pParseNode)->append(pColumnRef);
				(yyval.pParseNode)->append((yyvsp[0].pParseNode));
			}
			else
				YYERROR;
		}
    break;

  case 209: /* quantified_comparison_predicate_part_2: comparison any_all_some subquery  */
        {
		(yyval.pParseNode) = SQL_NEW_RULE;
		(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
		(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
		(yyval.pParseNode)->append((yyvsp[0].pParseNode));
	}
    break;

  case 210: /* all_or_any_predicate: row_value_constructor quantified_comparison_predicate_part_2  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 211: /* all_or_any_predicate: quantified_comparison_predicate_part_2  */
                {
			if(xxx_pGLOBAL_SQLPARSER->inPredicateCheck())
			{
				OSQLParseNode* pColumnRef = newNode(aEmptyString, SQL_NODE_RULE,OSQLParser::RuleID(OSQLParseNode::column_ref));
				pColumnRef->append(newNode(xxx_pGLOBAL_SQLPARSER->getFieldName(),SQL_NODE_NAME));

				(yyval.pParseNode) = SQL_NEW_RULE;
				(yyval.pParseNode)->append(pColumnRef);
				(yyval.pParseNode)->append((yyvsp[0].pParseNode));
			}
			else
				YYERROR;
		}
    break;

  case 215: /* existence_test: SQL_TOKEN_EXISTS subquery  */
                        {(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));}
    break;

  case 216: /* unique_test: SQL_TOKEN_UNIQUE subquery  */
                {(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));}
    break;

  case 217: /* subquery: '(' query_exp ')'  */
                        {(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode) = newNode("(", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode) = newNode(")", SQL_NODE_PUNCTUATION));}
    break;

  case 218: /* scalar_exp_commalist: select_sublist  */
                {
			(yyval.pParseNode) = SQL_NEW_COMMALISTRULE;
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 219: /* scalar_exp_commalist: scalar_exp_commalist ',' select_sublist  */
                {
			(yyvsp[-2].pParseNode)->append((yyvsp[0].pParseNode));
			(yyval.pParseNode) = (yyvsp[-2].pParseNode);
		}
    break;

  case 227: /* literal: literal SQL_TOKEN_STRING  */
                {
			if (xxx_pGLOBAL_SQLPARSER->inPredicateCheck())
			{
				(yyval.pParseNode) = SQL_NEW_RULE;
				(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
				(yyval.pParseNode)->append((yyvsp[0].pParseNode));
				xxx_pGLOBAL_SQLPARSER->reduceLiteral((yyval.pParseNode), sal_True);
			}
			else
				YYERROR;
		}
    break;

  case 228: /* literal: literal SQL_TOKEN_INT  */
                {
			if (xxx_pGLOBAL_SQLPARSER->inPredicateCheck())
			{
				(yyval.pParseNode) = SQL_NEW_RULE;
				(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
				(yyval.pParseNode)->append((yyvsp[0].pParseNode));
				xxx_pGLOBAL_SQLPARSER->reduceLiteral((yyval.pParseNode), sal_True);
			}
			else
				YYERROR;
		}
    break;

  case 229: /* literal: literal SQL_TOKEN_REAL_NUM  */
                {
			if (xxx_pGLOBAL_SQLPARSER->inPredicateCheck())
			{
				(yyval.pParseNode) = SQL_NEW_RULE;
				(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
				(yyval.pParseNode)->append((yyvsp[0].pParseNode));
				xxx_pGLOBAL_SQLPARSER->reduceLiteral((yyval.pParseNode), sal_True);
			}
			else
				YYERROR;
		}
    break;

  case 230: /* literal: literal SQL_TOKEN_APPROXNUM  */
                {
			if (xxx_pGLOBAL_SQLPARSER->inPredicateCheck())
			{
				(yyval.pParseNode) = SQL_NEW_RULE;
				(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
				(yyval.pParseNode)->append((yyvsp[0].pParseNode));
				xxx_pGLOBAL_SQLPARSER->reduceLiteral((yyval.pParseNode), sal_True);
			}
			else
				YYERROR;
		}
    break;

  case 231: /* as_clause: %empty  */
                            {(yyval.pParseNode) = SQL_NEW_RULE;}
    break;

  case 232: /* as_clause: SQL_TOKEN_AS column  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 234: /* position_exp: SQL_TOKEN_POSITION '(' value_exp SQL_TOKEN_IN value_exp ')'  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-5].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-4].pParseNode) = newNode("(", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[-3].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode) = newNode(")", SQL_NODE_PUNCTUATION));
		}
    break;

  case 235: /* position_exp: SQL_TOKEN_POSITION '(' value_exp_commalist ')'  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-3].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode) = newNode("(", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode) = newNode(")", SQL_NODE_PUNCTUATION));
		}
    break;

  case 239: /* char_length_exp: SQL_TOKEN_CHAR_LENGTH '(' value_exp ')'  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-3].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode) = newNode("(", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode) = newNode(")", SQL_NODE_PUNCTUATION));
		}
    break;

  case 240: /* char_length_exp: SQL_TOKEN_SQL_TOKEN_INTNUM '(' value_exp ')'  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-3].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode) = newNode("(", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode) = newNode(")", SQL_NODE_PUNCTUATION));
		}
    break;

  case 241: /* octet_length_exp: SQL_TOKEN_OCTET_LENGTH '(' value_exp ')'  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-3].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode) = newNode("(", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode) = newNode(")", SQL_NODE_PUNCTUATION));
		}
    break;

  case 242: /* bit_length_exp: SQL_TOKEN_BIT_LENGTH '(' value_exp ')'  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-3].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode) = newNode("(", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode) = newNode(")", SQL_NODE_PUNCTUATION));
		}
    break;

  case 243: /* length_exp: char_length_exp  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 244: /* length_exp: octet_length_exp  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 245: /* length_exp: bit_length_exp  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 246: /* datetime_field: non_second_datetime_field  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 247: /* datetime_field: SQL_TOKEN_SECOND  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 251: /* time_zone_field: SQL_TOKEN_TIMEZONE_HOUR  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 252: /* time_zone_field: SQL_TOKEN_TIMEZONE_MINUTE  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 253: /* extract_exp: SQL_TOKEN_EXTRACT '(' extract_field SQL_TOKEN_FROM value_exp ')'  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-5].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-4].pParseNode) = newNode("(", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[-3].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode) = newNode(")", SQL_NODE_PUNCTUATION));
		}
    break;

  case 270: /* set_fct_spec: '{' odbc_fct_spec '}'  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode) = newNode("{", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode) = newNode("}", SQL_NODE_PUNCTUATION));
		}
    break;

  case 271: /* set_fct_spec: function_name '(' ')'  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode) = newNode("(", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode) = newNode(")", SQL_NODE_PUNCTUATION));
		}
    break;

  case 272: /* set_fct_spec: function_name0 '(' ')'  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode) = newNode("(", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode) = newNode(")", SQL_NODE_PUNCTUATION));
		}
    break;

  case 273: /* set_fct_spec: function_name1 '(' function_arg ')'  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-3].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode) = newNode("(", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode) = newNode(")", SQL_NODE_PUNCTUATION));
		}
    break;

  case 274: /* set_fct_spec: function_name2 '(' function_arg_commalist2 ')'  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-3].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode) = newNode("(", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode) = newNode(")", SQL_NODE_PUNCTUATION));
		}
    break;

  case 275: /* set_fct_spec: function_name3 '(' function_arg_commalist3 ')'  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-3].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode) = newNode("(", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode) = newNode(")", SQL_NODE_PUNCTUATION));
		}
    break;

  case 276: /* set_fct_spec: string_function_4Argument '(' function_arg_commalist4 ')'  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-3].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode) = newNode("(", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode) = newNode(")", SQL_NODE_PUNCTUATION));
		}
    break;

  case 277: /* set_fct_spec: function_name '(' function_args_commalist ')'  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-3].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode) = newNode("(", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode) = newNode(")", SQL_NODE_PUNCTUATION));
		}
    break;

  case 278: /* set_fct_spec: function_name12 '(' function_args_commalist ')'  */
                {
			if ( (yyvsp[-1].pParseNode)->count() == 1 || (yyvsp[-1].pParseNode)->count() == 2 )
			{
				(yyval.pParseNode) = SQL_NEW_RULE;
				(yyval.pParseNode)->append((yyvsp[-3].pParseNode));
				(yyval.pParseNode)->append((yyvsp[-2].pParseNode) = newNode("(", SQL_NODE_PUNCTUATION));
				(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
				(yyval.pParseNode)->append((yyvsp[0].pParseNode) = newNode(")", SQL_NODE_PUNCTUATION));
			}
			else
				YYERROR;
		}
    break;

  case 279: /* set_fct_spec: function_name23 '(' function_args_commalist ')'  */
                {
			if ( (yyvsp[-1].pParseNode)->count() == 2 || (yyvsp[-1].pParseNode)->count() == 3)
			{
				(yyval.pParseNode) = SQL_NEW_RULE;
				(yyval.pParseNode)->append((yyvsp[-3].pParseNode));
				(yyval.pParseNode)->append((yyvsp[-2].pParseNode) = newNode("(", SQL_NODE_PUNCTUATION));
				(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
				(yyval.pParseNode)->append((yyvsp[0].pParseNode) = newNode(")", SQL_NODE_PUNCTUATION));
			}
			else
				YYERROR;
		}
    break;

  case 358: /* window_function: window_function_type SQL_TOKEN_OVER window_name_or_specification  */
        {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
	}
    break;

  case 359: /* window_function_type: rank_function_type '(' ')'  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode) = newNode("(", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode) = newNode(")", SQL_NODE_PUNCTUATION));
		}
    break;

  case 360: /* window_function_type: SQL_TOKEN_ROW_NUMBER '(' ')'  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode) = newNode("(", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode) = newNode(")", SQL_NODE_PUNCTUATION));
		}
    break;

  case 366: /* ntile_function: SQL_TOKEN_NTILE '(' number_of_tiles ')'  */
        {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-3].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode) = newNode("(", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode) = newNode(")", SQL_NODE_PUNCTUATION));
	}
    break;

  case 371: /* opt_lead_or_lag_function: %empty  */
                         {(yyval.pParseNode) = SQL_NEW_RULE;}
    break;

  case 372: /* opt_lead_or_lag_function: ',' offset  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode) = newNode(",", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 373: /* opt_lead_or_lag_function: ',' offset ',' default_expression  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-3].pParseNode) = newNode(",", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode) = newNode(",", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 374: /* opt_null_treatment: %empty  */
                                 {(yyval.pParseNode) = SQL_NEW_RULE;}
    break;

  case 376: /* lead_or_lag_function: lead_or_lag '(' lead_or_lag_extent opt_lead_or_lag_function ')' opt_null_treatment  */
        {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-5].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-4].pParseNode) = newNode("(", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[-3].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode) = newNode(")", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
	}
    break;

  case 384: /* first_or_last_value_function: first_or_last_value '(' value_exp ')' opt_null_treatment  */
        {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-4].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-3].pParseNode) = newNode("(", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode) = newNode(")", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
	}
    break;

  case 387: /* opt_from_first_or_last: %empty  */
                                 {(yyval.pParseNode) = SQL_NEW_RULE;}
    break;

  case 389: /* nth_value_function: SQL_TOKEN_NTH_VALUE '(' value_exp ',' nth_row ')' opt_from_first_or_last opt_null_treatment  */
        {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-7].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-6].pParseNode) = newNode("(", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[-5].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-4].pParseNode) = newNode(",", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[-3].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode) = newNode(")", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
	}
    break;

  case 392: /* from_first_or_last: SQL_TOKEN_FROM SQL_TOKEN_FIRST  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 393: /* from_first_or_last: SQL_TOKEN_FROM SQL_TOKEN_LAST  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 398: /* opt_window_clause: %empty  */
                                 {(yyval.pParseNode) = SQL_NEW_RULE;}
    break;

  case 400: /* window_clause: SQL_TOKEN_WINDOW window_definition_list  */
        {
		(yyval.pParseNode) = SQL_NEW_RULE;
		(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
		(yyval.pParseNode)->append((yyvsp[0].pParseNode));
	}
    break;

  case 401: /* window_definition_list: window_definition_list ',' window_definition  */
                        {(yyvsp[-2].pParseNode)->append((yyvsp[0].pParseNode));
			(yyval.pParseNode) = (yyvsp[-2].pParseNode);}
    break;

  case 402: /* window_definition_list: window_definition  */
                        {(yyval.pParseNode) = SQL_NEW_COMMALISTRULE;
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));}
    break;

  case 403: /* window_definition: new_window_name SQL_TOKEN_AS window_specification  */
        {
		(yyval.pParseNode) = SQL_NEW_RULE;
		(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
		(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
		(yyval.pParseNode)->append((yyvsp[0].pParseNode));
	}
    break;

  case 405: /* window_specification: '(' window_specification_details ')'  */
        {
		(yyval.pParseNode) = SQL_NEW_RULE;
		(yyval.pParseNode)->append((yyvsp[-2].pParseNode) = newNode("(", SQL_NODE_PUNCTUATION));
		(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
		(yyval.pParseNode)->append((yyvsp[0].pParseNode) = newNode(")", SQL_NODE_PUNCTUATION));
	}
    break;

  case 406: /* opt_existing_window_name: %empty  */
                                 {(yyval.pParseNode) = SQL_NEW_RULE;}
    break;

  case 408: /* opt_window_partition_clause: %empty  */
                         {(yyval.pParseNode) = SQL_NEW_RULE;}
    break;

  case 410: /* opt_window_frame_clause: %empty  */
                         {(yyval.pParseNode) = SQL_NEW_RULE;}
    break;

  case 414: /* window_partition_clause: SQL_TOKEN_PARTITION SQL_TOKEN_BY window_partition_column_reference_list  */
        {
		(yyval.pParseNode) = SQL_NEW_RULE;
		(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
		(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
		(yyval.pParseNode)->append((yyvsp[0].pParseNode));
	}
    break;

  case 415: /* window_partition_column_reference_list: window_partition_column_reference_list ',' window_partition_column_reference  */
                        {(yyvsp[-2].pParseNode)->append((yyvsp[0].pParseNode));
			(yyval.pParseNode) = (yyvsp[-2].pParseNode);}
    break;

  case 416: /* window_partition_column_reference_list: window_partition_column_reference  */
                        {(yyval.pParseNode) = SQL_NEW_COMMALISTRULE;
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));}
    break;

  case 417: /* window_partition_column_reference: column_ref opt_collate_clause  */
        {
		(yyval.pParseNode) = SQL_NEW_RULE;
		(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
		(yyval.pParseNode)->append((yyvsp[0].pParseNode));
	}
    break;

  case 418: /* opt_window_frame_exclusion: %empty  */
                         {(yyval.pParseNode) = SQL_NEW_RULE;}
    break;

  case 420: /* window_frame_clause: window_frame_units window_frame_extent opt_window_frame_exclusion  */
        {
		(yyval.pParseNode) = SQL_NEW_RULE;
		(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
		(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
		(yyval.pParseNode)->append((yyvsp[0].pParseNode));
	}
    break;

  case 425: /* window_frame_start: SQL_TOKEN_UNBOUNDED SQL_TOKEN_PRECEDING  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 427: /* window_frame_start: SQL_TOKEN_CURRENT SQL_TOKEN_ROW  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 428: /* window_frame_preceding: unsigned_value_spec SQL_TOKEN_PRECEDING  */
        {
		(yyval.pParseNode) = SQL_NEW_RULE;
		(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
		(yyval.pParseNode)->append((yyvsp[0].pParseNode));
	}
    break;

  case 429: /* window_frame_between: SQL_TOKEN_BETWEEN window_frame_bound_1 SQL_TOKEN_AND window_frame_bound_2  */
        {
		(yyval.pParseNode) = SQL_NEW_RULE;
		(yyval.pParseNode)->append((yyvsp[-3].pParseNode));
		(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
		(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
		(yyval.pParseNode)->append((yyvsp[0].pParseNode));
	}
    break;

  case 433: /* window_frame_bound: SQL_TOKEN_UNBOUNDED SQL_TOKEN_FOLLOWING  */
        {
		(yyval.pParseNode) = SQL_NEW_RULE;
		(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
		(yyval.pParseNode)->append((yyvsp[0].pParseNode));
	}
    break;

  case 435: /* window_frame_following: unsigned_value_spec SQL_TOKEN_FOLLOWING  */
        {
		(yyval.pParseNode) = SQL_NEW_RULE;
		(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
		(yyval.pParseNode)->append((yyvsp[0].pParseNode));
	}
    break;

  case 436: /* window_frame_exclusion: SQL_TOKEN_EXCLUDE SQL_TOKEN_CURRENT SQL_TOKEN_ROW  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 437: /* window_frame_exclusion: SQL_TOKEN_EXCLUDE SQL_TOKEN_GROUP  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 438: /* window_frame_exclusion: SQL_TOKEN_EXCLUDE SQL_TOKEN_TIES  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 439: /* window_frame_exclusion: SQL_TOKEN_EXCLUDE SQL_TOKEN_NO SQL_TOKEN_OTHERS  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 440: /* op_parameter: %empty  */
                {(yyval.pParseNode) = SQL_NEW_RULE;}
    break;

  case 441: /* op_parameter: '?' SQL_EQUAL  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode) = newNode("?", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 442: /* odbc_call_spec: op_parameter SQL_TOKEN_CALL table_node op_odbc_call_parameter  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-3].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 443: /* op_odbc_call_parameter: %empty  */
                {(yyval.pParseNode) = SQL_NEW_RULE;}
    break;

  case 444: /* op_odbc_call_parameter: '(' odbc_parameter_commalist ')'  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode) = newNode("(", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode) = newNode(")", SQL_NODE_PUNCTUATION));
		}
    break;

  case 445: /* odbc_parameter_commalist: odbc_parameter  */
                {(yyval.pParseNode) = SQL_NEW_COMMALISTRULE;
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));}
    break;

  case 446: /* odbc_parameter_commalist: odbc_parameter_commalist ',' odbc_parameter  */
                {
			(yyvsp[-2].pParseNode)->append((yyvsp[0].pParseNode));
			(yyval.pParseNode) = (yyvsp[-2].pParseNode);
		}
    break;

  case 447: /* odbc_parameter: %empty  */
                        {(yyval.pParseNode) = SQL_NEW_RULE;}
    break;

  case 450: /* odbc_fct_spec: odbc_fct_type SQL_TOKEN_STRING  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 451: /* odbc_fct_spec: SQL_TOKEN_FN set_fct_spec  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 455: /* general_set_fct: set_fct_type '(' opt_all_distinct function_arg ')'  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-4].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-3].pParseNode) = newNode("(", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode) = newNode(")", SQL_NODE_PUNCTUATION));
		}
    break;

  case 456: /* general_set_fct: SQL_TOKEN_COUNT '(' '*' ')'  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-3].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode) = newNode("(", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode) = newNode("*", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode) = newNode(")", SQL_NODE_PUNCTUATION));
		}
    break;

  case 457: /* general_set_fct: SQL_TOKEN_COUNT '(' opt_all_distinct function_arg ')'  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-4].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-3].pParseNode) = newNode("(", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode) = newNode(")", SQL_NODE_PUNCTUATION));
		}
    break;

  case 476: /* hypothetical_set_function: rank_function_type '(' hypothetical_set_function_value_expression_list ')' within_group_specification  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-4].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-3].pParseNode) = newNode("(", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode) = newNode(")", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 477: /* hypothetical_set_function: rank_function_type '(' hypothetical_set_function_value_expression_list SQL_TOKEN_BY value_exp_commalist ')' within_group_specification  */
        {
		(yyval.pParseNode) = SQL_NEW_RULE;
		(yyval.pParseNode)->append((yyvsp[-6].pParseNode));
		(yyval.pParseNode)->append((yyvsp[-5].pParseNode) = newNode("(", SQL_NODE_PUNCTUATION));
		(yyval.pParseNode)->append((yyvsp[-4].pParseNode));
		(yyval.pParseNode)->append((yyvsp[-3].pParseNode));
		(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
		(yyval.pParseNode)->append((yyvsp[-1].pParseNode) = newNode(")", SQL_NODE_PUNCTUATION));
		(yyval.pParseNode)->append((yyvsp[0].pParseNode));
	}
    break;

  case 478: /* within_group_specification: %empty  */
        {
		(yyval.pParseNode) = SQL_NEW_RULE;
	}
    break;

  case 479: /* within_group_specification: SQL_TOKEN_WITHIN SQL_TOKEN_GROUP '(' opt_order_by_clause ')'  */
        {
		(yyval.pParseNode) = SQL_NEW_RULE;
		(yyval.pParseNode)->append((yyvsp[-4].pParseNode));
		(yyval.pParseNode)->append((yyvsp[-3].pParseNode));
		(yyval.pParseNode)->append((yyvsp[-2].pParseNode) = newNode("(", SQL_NODE_PUNCTUATION));
		(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
		(yyval.pParseNode)->append((yyvsp[0].pParseNode) = newNode(")", SQL_NODE_PUNCTUATION));
	}
    break;

  case 481: /* inverse_distribution_function: inverse_distribution_function_type '(' inverse_distribution_function_argument ')' within_group_specification  */
        {
		(yyval.pParseNode) = SQL_NEW_RULE;
		(yyval.pParseNode)->append((yyvsp[-4].pParseNode));
		(yyval.pParseNode)->append((yyvsp[-3].pParseNode) = newNode("(", SQL_NODE_PUNCTUATION));
		(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
		(yyval.pParseNode)->append((yyvsp[-1].pParseNode) = newNode(")", SQL_NODE_PUNCTUATION));
	}
    break;

  case 485: /* array_aggregate_function: SQL_TOKEN_ARRAY_AGG '(' value_exp opt_order_by_clause ')'  */
        {
		(yyval.pParseNode) = SQL_NEW_RULE;
		(yyval.pParseNode)->append((yyvsp[-4].pParseNode));
		(yyval.pParseNode)->append((yyvsp[-3].pParseNode) = newNode("(", SQL_NODE_PUNCTUATION));
		(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
		(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
		(yyval.pParseNode)->append((yyvsp[0].pParseNode) = newNode(")", SQL_NODE_PUNCTUATION));
	}
    break;

  case 490: /* outer_join_type: SQL_TOKEN_LEFT  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 491: /* outer_join_type: SQL_TOKEN_RIGHT  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 492: /* outer_join_type: SQL_TOKEN_FULL  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 493: /* join_condition: SQL_TOKEN_ON search_condition  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 496: /* join_type: %empty  */
                                {(yyval.pParseNode) = SQL_NEW_RULE;}
    break;

  case 497: /* join_type: SQL_TOKEN_INNER  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 499: /* join_type: outer_join_type SQL_TOKEN_OUTER  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 500: /* cross_union: table_ref SQL_TOKEN_CROSS SQL_TOKEN_JOIN table_ref  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-3].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 501: /* qualified_join: table_ref SQL_TOKEN_NATURAL join_type SQL_TOKEN_JOIN table_ref  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-4].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-3].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 502: /* qualified_join: table_ref join_type SQL_TOKEN_JOIN table_ref join_spec  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-4].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-3].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 505: /* named_columns_join: SQL_TOKEN_USING '(' column_commalist ')'  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-3].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode) = newNode("(", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode) = newNode(")", SQL_NODE_PUNCTUATION));
		}
    break;

  case 509: /* non_join_query_primary: '(' non_join_query_exp ')'  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode) = newNode("(", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode) = newNode(")", SQL_NODE_PUNCTUATION));
		}
    break;

  case 511: /* non_join_query_term: query_term SQL_TOKEN_INTERSECT all query_primary  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-3].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 514: /* non_join_query_exp: query_exp SQL_TOKEN_UNION all query_term  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-3].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 515: /* non_join_query_exp: query_exp SQL_TOKEN_EXCEPT all query_term  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-3].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 516: /* all: %empty  */
                   {(yyval.pParseNode) = SQL_NEW_RULE;}
    break;

  case 523: /* cast_spec: SQL_TOKEN_CAST '(' cast_operand SQL_TOKEN_AS cast_target ')'  */
          {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-5].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-4].pParseNode) = newNode("(", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[-3].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode) = newNode(")", SQL_NODE_PUNCTUATION));
		}
    break;

  case 530: /* value_exp_primary: '(' value_exp ')'  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode) = newNode("(", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode) = newNode(")", SQL_NODE_PUNCTUATION));
		}
    break;

  case 535: /* factor: '-' num_primary  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode) = newNode("-", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 536: /* factor: '+' num_primary  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode) = newNode("+", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 538: /* term: term '*' factor  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode) = newNode("*", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 539: /* term: term '/' factor  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode) = newNode("/", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 541: /* num_value_exp: num_value_exp '+' term  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode) = newNode("+", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 542: /* num_value_exp: num_value_exp '-' term  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode) = newNode("-", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 543: /* datetime_primary: datetime_value_fct  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 544: /* datetime_value_fct: SQL_TOKEN_CURRENT_DATE  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 545: /* datetime_value_fct: SQL_TOKEN_CURRENT_TIME  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 546: /* datetime_value_fct: SQL_TOKEN_CURRENT_TIMESTAMP  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 547: /* time_zone: SQL_TOKEN_AT time_zone_specifier  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 548: /* time_zone_specifier: SQL_TOKEN_LOCAL  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 549: /* datetime_factor: datetime_primary  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 550: /* datetime_factor: datetime_primary time_zone  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 551: /* datetime_term: datetime_factor  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 552: /* datetime_value_exp: datetime_term  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 558: /* start_field: non_second_datetime_field opt_paren_precision  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 560: /* end_field: SQL_TOKEN_SECOND opt_paren_precision  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 561: /* single_datetime_field: non_second_datetime_field opt_paren_precision  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 562: /* single_datetime_field: SQL_TOKEN_SECOND opt_paren_precision_scale  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 563: /* interval_qualifier: start_field SQL_TOKEN_TO end_field  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 565: /* function_arg_commalist2: function_arg ',' function_arg  */
                        {(yyval.pParseNode) = SQL_NEW_COMMALISTRULE;
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));}
    break;

  case 566: /* function_arg_commalist3: function_arg ',' function_arg ',' function_arg  */
                {
			(yyval.pParseNode) = SQL_NEW_COMMALISTRULE;
			(yyval.pParseNode)->append((yyvsp[-4].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 567: /* function_arg_commalist4: function_arg ',' function_arg ',' function_arg ',' function_arg  */
                {
			(yyval.pParseNode) = SQL_NEW_COMMALISTRULE;
			(yyval.pParseNode)->append((yyvsp[-6].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-4].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 568: /* value_exp_commalist: value_exp  */
                        {(yyval.pParseNode) = SQL_NEW_COMMALISTRULE;
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));}
    break;

  case 569: /* value_exp_commalist: value_exp_commalist ',' value_exp  */
                        {(yyvsp[-2].pParseNode)->append((yyvsp[0].pParseNode));
			(yyval.pParseNode) = (yyvsp[-2].pParseNode);}
    break;

  case 570: /* value_exp_commalist: value_exp_commalist ';' value_exp  */
                {
			if (xxx_pGLOBAL_SQLPARSER->inPredicateCheck())
			{
				(yyvsp[-2].pParseNode)->append((yyvsp[0].pParseNode));
				(yyval.pParseNode) = (yyvsp[-2].pParseNode);
			}
			else
				YYERROR;
		}
    break;

  case 572: /* function_arg: value_exp comparison value_exp  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 573: /* function_arg: value_exp SQL_TOKEN_USING value_exp comparison value_exp  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-4].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-3].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
		}
    break;

  case 574: /* function_arg: value_exp SQL_TOKEN_BY value_exp_commalist  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 575: /* function_args_commalist: function_arg  */
                        {(yyval.pParseNode) = SQL_NEW_COMMALISTRULE;
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));}
    break;

  case 576: /* function_args_commalist: function_args_commalist ',' function_arg  */
                        {(yyvsp[-2].pParseNode)->append((yyvsp[0].pParseNode));
			(yyval.pParseNode) = (yyvsp[-2].pParseNode);}
    break;

  case 577: /* function_args_commalist: function_args_commalist ';' function_arg  */
                {
			if (xxx_pGLOBAL_SQLPARSER->inPredicateCheck())
			{
				(yyvsp[-2].pParseNode)->append((yyvsp[0].pParseNode));
				(yyval.pParseNode) = (yyvsp[-2].pParseNode);
			}
			else
				YYERROR;
		}
    break;

  case 580: /* value_exp: datetime_value_exp  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 584: /* concatenation: char_value_exp '+' char_factor  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode) = newNode("+", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 585: /* concatenation: value_exp SQL_CONCAT value_exp  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 588: /* collate_clause: SQL_TOKEN_COLLATE table_node  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 590: /* char_factor: char_primary collate_clause  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 593: /* bit_value_fct: bit_substring_fct  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 594: /* bit_substring_fct: SQL_TOKEN_SUBSTRING '(' bit_value_exp SQL_TOKEN_FROM string_value_exp for_length ')'  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-6].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-5].pParseNode) = newNode("(", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[-4].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-3].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode) = newNode(")", SQL_NODE_PUNCTUATION));
		}
    break;

  case 595: /* bit_value_exp: bit_factor  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 596: /* bit_factor: bit_primary  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 597: /* bit_primary: %empty  */
        {(yyval.pParseNode) = SQL_NEW_RULE;}
    break;

  case 600: /* char_value_fct: form_conversion  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 601: /* char_value_fct: char_translation  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 602: /* char_value_fct: trim_fct  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 603: /* for_length: %empty  */
                {(yyval.pParseNode) = SQL_NEW_RULE;}
    break;

  case 604: /* for_length: SQL_TOKEN_FOR value_exp  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 605: /* char_substring_fct: SQL_TOKEN_SUBSTRING '(' value_exp SQL_TOKEN_FROM value_exp for_length ')'  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-6].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-5].pParseNode) = newNode("(", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[-4].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-3].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode) = newNode(")", SQL_NODE_PUNCTUATION));
		}
    break;

  case 606: /* char_substring_fct: SQL_TOKEN_SUBSTRING '(' value_exp_commalist ')'  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-3].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode) = newNode("(", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode) = newNode(")", SQL_NODE_PUNCTUATION));
		}
    break;

  case 609: /* fold: upper_lower '(' value_exp ')'  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-3].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode) = newNode("(", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode) = newNode(")", SQL_NODE_PUNCTUATION));
		}
    break;

  case 610: /* form_conversion: SQL_TOKEN_CONVERT '(' string_value_exp SQL_TOKEN_USING table_node ')'  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-5].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-4].pParseNode) = newNode("(", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[-3].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode) = newNode(")", SQL_NODE_PUNCTUATION));
		}
    break;

  case 611: /* form_conversion: SQL_TOKEN_CONVERT '(' cast_operand ',' cast_target ')'  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-5].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-4].pParseNode) = newNode("(", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[-3].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-4].pParseNode) = newNode(",", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode) = newNode(")", SQL_NODE_PUNCTUATION));
		}
    break;

  case 612: /* char_translation: SQL_TOKEN_TRANSLATE '(' string_value_exp SQL_TOKEN_USING table_node ')'  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-5].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-4].pParseNode) = newNode("(", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[-3].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode) = newNode(")", SQL_NODE_PUNCTUATION));
		}
    break;

  case 613: /* trim_fct: SQL_TOKEN_TRIM '(' trim_operands ')'  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-3].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode) = newNode("(", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode) = newNode(")", SQL_NODE_PUNCTUATION));
		}
    break;

  case 614: /* trim_operands: trim_spec value_exp SQL_TOKEN_FROM value_exp  */
        {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-3].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 615: /* trim_operands: trim_spec SQL_TOKEN_FROM value_exp  */
        {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 616: /* trim_operands: value_exp SQL_TOKEN_FROM value_exp  */
        {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 617: /* trim_operands: SQL_TOKEN_FROM value_exp  */
            {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 622: /* derived_column: value_exp as_clause  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 626: /* catalog_name: SQL_TOKEN_NAME '.' schema_name  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode) = newNode(".", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 627: /* catalog_name: SQL_TOKEN_NAME ':' schema_name  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode) = newNode(":", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 628: /* schema_name: SQL_TOKEN_NAME '.' table_name  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode) = newNode(".", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 629: /* table_name: SQL_TOKEN_NAME  */
                        {(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));}
    break;

  case 630: /* column_ref: column  */
                        {(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));}
    break;

  case 631: /* column_ref: SQL_TOKEN_NAME '.' column_val  */
                        {(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode) = newNode(".", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
			}
    break;

  case 632: /* column_ref: SQL_TOKEN_NAME '.' SQL_TOKEN_NAME '.' column_val  */
                        {(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-4].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-3].pParseNode) = newNode(".", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode) = newNode(".", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));}
    break;

  case 633: /* column_ref: SQL_TOKEN_NAME '.' SQL_TOKEN_NAME '.' SQL_TOKEN_NAME '.' column_val  */
                        {(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-6].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-5].pParseNode)= newNode(".", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[-4].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-3].pParseNode) = newNode(".", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode) = newNode(".", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
			}
    break;

  case 634: /* column_ref: SQL_TOKEN_NAME ':' SQL_TOKEN_NAME '.' SQL_TOKEN_NAME '.' column_val  */
                        {(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-6].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-5].pParseNode)= newNode(":", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[-4].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-3].pParseNode) = newNode(".", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode) = newNode(".", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
			}
    break;

  case 635: /* column_val: column  */
                {(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));}
    break;

  case 636: /* column_val: '*'  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[0].pParseNode) = newNode("*", SQL_NODE_PUNCTUATION));
		}
    break;

  case 638: /* opt_char_set_spec: %empty  */
        {(yyval.pParseNode) = SQL_NEW_RULE;}
    break;

  case 639: /* opt_char_set_spec: SQL_TOKEN_CHARACTER SQL_TOKEN_SET SQL_TOKEN_NAME  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 640: /* opt_collate_clause: %empty  */
        {(yyval.pParseNode) = SQL_NEW_RULE;}
    break;

  case 642: /* predefined_type: character_string_type opt_char_set_spec opt_collate_clause  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 643: /* predefined_type: national_character_string_type opt_collate_clause  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 649: /* character_string_type: SQL_TOKEN_CHARACTER opt_paren_precision  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 650: /* character_string_type: SQL_TOKEN_CHAR opt_paren_precision  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 651: /* character_string_type: SQL_TOKEN_CHARACTER SQL_TOKEN_VARYING paren_char_length  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 652: /* character_string_type: SQL_TOKEN_CHAR SQL_TOKEN_VARYING paren_char_length  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 653: /* character_string_type: SQL_TOKEN_VARCHAR paren_char_length  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 655: /* opt_paren_precision: %empty  */
                {(yyval.pParseNode) = SQL_NEW_RULE;}
    break;

  case 657: /* paren_char_length: '(' SQL_TOKEN_INTNUM ')'  */
        {
		(yyval.pParseNode) = SQL_NEW_RULE;
		(yyval.pParseNode)->append((yyvsp[-2].pParseNode) = newNode("(", SQL_NODE_PUNCTUATION));
		(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
		(yyval.pParseNode)->append((yyvsp[0].pParseNode) = newNode(")", SQL_NODE_PUNCTUATION));
	}
    break;

  case 658: /* opt_paren_char_large_length: %empty  */
                {(yyval.pParseNode) = SQL_NEW_RULE;}
    break;

  case 660: /* paren_character_large_object_length: '(' large_object_length ')'  */
        {
		(yyval.pParseNode) = SQL_NEW_RULE;
		(yyval.pParseNode)->append((yyvsp[-2].pParseNode) = newNode("(", SQL_NODE_PUNCTUATION));
		(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
		(yyval.pParseNode)->append((yyvsp[0].pParseNode) = newNode(")", SQL_NODE_PUNCTUATION));
	}
    break;

  case 661: /* large_object_length: SQL_TOKEN_INTNUM opt_multiplier  */
        {
		(yyval.pParseNode) = SQL_NEW_RULE;
		(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
		(yyval.pParseNode)->append((yyvsp[0].pParseNode));
	}
    break;

  case 662: /* opt_multiplier: %empty  */
        {(yyval.pParseNode) = SQL_NEW_RULE;}
    break;

  case 663: /* opt_multiplier: 'K'  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[0].pParseNode) = newNode("K", SQL_NODE_PUNCTUATION));
		}
    break;

  case 664: /* opt_multiplier: 'M'  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[0].pParseNode) = newNode("M", SQL_NODE_PUNCTUATION));
		}
    break;

  case 665: /* opt_multiplier: 'G'  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[0].pParseNode) = newNode("G", SQL_NODE_PUNCTUATION));
		}
    break;

  case 666: /* opt_multiplier: 'T'  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[0].pParseNode) = newNode("T", SQL_NODE_PUNCTUATION));
		}
    break;

  case 667: /* opt_multiplier: 'P'  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[0].pParseNode) = newNode("P", SQL_NODE_PUNCTUATION));
		}
    break;

  case 668: /* character_large_object_type: SQL_TOKEN_CHARACTER SQL_TOKEN_LARGE SQL_TOKEN_OBJECT opt_paren_char_large_length  */
        {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-3].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 669: /* character_large_object_type: SQL_TOKEN_CHAR SQL_TOKEN_LARGE SQL_TOKEN_OBJECT opt_paren_char_large_length  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-3].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 670: /* character_large_object_type: SQL_TOKEN_CLOB opt_paren_char_large_length  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 671: /* national_character_string_type: SQL_TOKEN_NATIONAL SQL_TOKEN_CHARACTER opt_paren_precision  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 672: /* national_character_string_type: SQL_TOKEN_NATIONAL SQL_TOKEN_CHAR opt_paren_precision  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 673: /* national_character_string_type: SQL_TOKEN_NCHAR opt_paren_precision  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 674: /* national_character_string_type: SQL_TOKEN_NATIONAL SQL_TOKEN_CHARACTER SQL_TOKEN_VARYING paren_char_length  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-3].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 675: /* national_character_string_type: SQL_TOKEN_NATIONAL SQL_TOKEN_CHAR SQL_TOKEN_VARYING paren_char_length  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-3].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 676: /* national_character_string_type: SQL_TOKEN_NCHAR SQL_TOKEN_VARYING paren_char_length  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 678: /* national_character_large_object_type: SQL_TOKEN_NATIONAL SQL_TOKEN_CHARACTER SQL_TOKEN_LARGE SQL_TOKEN_OBJECT opt_paren_char_large_length  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-4].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-3].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 679: /* national_character_large_object_type: SQL_TOKEN_NCHAR SQL_TOKEN_LARGE SQL_TOKEN_OBJECT opt_paren_char_large_length  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-3].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 680: /* national_character_large_object_type: SQL_TOKEN_NCLOB opt_paren_char_large_length  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 681: /* binary_string_type: SQL_TOKEN_BINARY opt_paren_precision  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 682: /* binary_string_type: SQL_TOKEN_BINARY SQL_TOKEN_VARYING paren_char_length  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 683: /* binary_string_type: SQL_TOKEN_VARBINARY paren_char_length  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 685: /* binary_large_object_string_type: SQL_TOKEN_BINARY SQL_TOKEN_LARGE SQL_TOKEN_OBJECT opt_paren_char_large_length  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-3].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 686: /* binary_large_object_string_type: SQL_TOKEN_BLOB opt_paren_char_large_length  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 689: /* opt_paren_precision_scale: %empty  */
        {(yyval.pParseNode) = SQL_NEW_RULE;}
    break;

  case 690: /* opt_paren_precision_scale: '(' SQL_TOKEN_INTNUM ')'  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode) = newNode("(", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode) = newNode(")", SQL_NODE_PUNCTUATION));
		}
    break;

  case 691: /* opt_paren_precision_scale: '(' SQL_TOKEN_INTNUM ',' SQL_TOKEN_INTNUM ')'  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-4].pParseNode) = newNode("(", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[-3].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode) = newNode(",", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode) = newNode(")", SQL_NODE_PUNCTUATION));
		}
    break;

  case 692: /* exact_numeric_type: SQL_TOKEN_NUMERIC opt_paren_precision_scale  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 693: /* exact_numeric_type: SQL_TOKEN_DECIMAL opt_paren_precision_scale  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 694: /* exact_numeric_type: SQL_TOKEN_DEC opt_paren_precision_scale  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 699: /* approximate_numeric_type: SQL_TOKEN_FLOAT '(' SQL_TOKEN_INTNUM ')'  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-3].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode) = newNode("(", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode) = newNode(")", SQL_NODE_PUNCTUATION));
		}
    break;

  case 703: /* approximate_numeric_type: SQL_TOKEN_DOUBLE SQL_TOKEN_PRECISION  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 706: /* datetime_type: SQL_TOKEN_TIME opt_paren_precision opt_with_or_without_time_zone  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 707: /* datetime_type: SQL_TOKEN_TIMESTAMP opt_paren_precision opt_with_or_without_time_zone  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 708: /* opt_with_or_without_time_zone: %empty  */
                {(yyval.pParseNode) = SQL_NEW_RULE;}
    break;

  case 709: /* opt_with_or_without_time_zone: SQL_TOKEN_WITH SQL_TOKEN_TIME SQL_TOKEN_ZONE  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 710: /* opt_with_or_without_time_zone: SQL_TOKEN_WITHOUT SQL_TOKEN_TIME SQL_TOKEN_ZONE  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 711: /* interval_type: SQL_TOKEN_INTERVAL interval_qualifier  */
        {
		(yyval.pParseNode) = SQL_NEW_RULE;
		(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
		(yyval.pParseNode)->append((yyvsp[0].pParseNode));
	}
    break;

  case 713: /* column: SQL_TOKEN_POSITION  */
                {
			sal_uInt32 nNod = (yyval.pParseNode)->getRuleID();
			delete (yyval.pParseNode);
			(yyval.pParseNode) = newNode(xxx_pGLOBAL_SQLPARSER->TokenIDToStr(nNod), SQL_NODE_NAME);
		}
    break;

  case 714: /* column: SQL_TOKEN_CHAR_LENGTH  */
                {
			sal_uInt32 nNod = (yyval.pParseNode)->getRuleID();
			delete (yyval.pParseNode);
			(yyval.pParseNode) = newNode(xxx_pGLOBAL_SQLPARSER->TokenIDToStr(nNod), SQL_NODE_NAME);
		}
    break;

  case 715: /* column: SQL_TOKEN_EXTRACT  */
                {
			sal_uInt32 nNod = (yyval.pParseNode)->getRuleID();
			delete (yyval.pParseNode);
			(yyval.pParseNode) = newNode(xxx_pGLOBAL_SQLPARSER->TokenIDToStr(nNod), SQL_NODE_NAME);
		}
    break;

  case 718: /* case_abbreviation: SQL_TOKEN_NULLIF '(' value_exp_commalist ')'  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-3].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode) = newNode("(", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode) = newNode(")", SQL_NODE_PUNCTUATION));
		}
    break;

  case 719: /* case_abbreviation: SQL_TOKEN_COALESCE '(' value_exp ')'  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-3].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode) = newNode("(", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode) = newNode(")", SQL_NODE_PUNCTUATION));
		}
    break;

  case 720: /* case_abbreviation: SQL_TOKEN_COALESCE '(' value_exp_commalist ')'  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-3].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode) = newNode("(", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode) = newNode(")", SQL_NODE_PUNCTUATION));
		}
    break;

  case 723: /* simple_case: SQL_TOKEN_CASE case_operand simple_when_clause_list else_clause SQL_TOKEN_END  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-4].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-3].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 724: /* searched_case: SQL_TOKEN_CASE searched_when_clause_list else_clause SQL_TOKEN_END  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-3].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 725: /* simple_when_clause_list: simple_when_clause  */
                {
			(yyval.pParseNode) = SQL_NEW_LISTRULE;
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 726: /* simple_when_clause_list: searched_when_clause_list simple_when_clause  */
                {
			(yyvsp[-1].pParseNode)->append((yyvsp[0].pParseNode));
			(yyval.pParseNode) = (yyvsp[-1].pParseNode);
		}
    break;

  case 727: /* simple_when_clause: SQL_TOKEN_WHEN when_operand_list SQL_TOKEN_THEN result  */
        {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-3].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 728: /* when_operand_list: when_operand  */
                {(yyval.pParseNode) = SQL_NEW_COMMALISTRULE;
		(yyval.pParseNode)->append((yyvsp[0].pParseNode));}
    break;

  case 729: /* when_operand_list: when_operand_list ',' when_operand  */
                {(yyvsp[-2].pParseNode)->append((yyvsp[0].pParseNode));
		(yyval.pParseNode) = (yyvsp[-2].pParseNode);}
    break;

  case 736: /* searched_when_clause_list: searched_when_clause  */
                {
			(yyval.pParseNode) = SQL_NEW_LISTRULE;
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 737: /* searched_when_clause_list: searched_when_clause_list searched_when_clause  */
                {
			(yyvsp[-1].pParseNode)->append((yyvsp[0].pParseNode));
			(yyval.pParseNode) = (yyvsp[-1].pParseNode);
		}
    break;

  case 738: /* searched_when_clause: SQL_TOKEN_WHEN search_condition SQL_TOKEN_THEN result  */
        {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-3].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 739: /* else_clause: %empty  */
                {(yyval.pParseNode) = SQL_NEW_RULE;}
    break;

  case 740: /* else_clause: SQL_TOKEN_ELSE result  */
                {
			(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 745: /* cursor: SQL_TOKEN_NAME  */
                        {(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));}
    break;

  case 746: /* parameter: ':' SQL_TOKEN_NAME  */
                        {(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode) = newNode(":", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));}
    break;

  case 747: /* parameter: '?'  */
                        {(yyval.pParseNode) = SQL_NEW_RULE; // test
			(yyval.pParseNode)->append((yyvsp[0].pParseNode) = newNode("?", SQL_NODE_PUNCTUATION));}
    break;

  case 748: /* parameter: '[' SQL_TOKEN_NAME ']'  */
                        {(yyval.pParseNode) = SQL_NEW_RULE;
			(yyval.pParseNode)->append((yyvsp[-2].pParseNode) = newNode("[", SQL_NODE_PUNCTUATION));
			(yyval.pParseNode)->append((yyvsp[-1].pParseNode));
			(yyval.pParseNode)->append((yyvsp[0].pParseNode) = newNode("]", SQL_NODE_PUNCTUATION));}
    break;

  case 749: /* range_variable: %empty  */
        {(yyval.pParseNode) = SQL_NEW_RULE;}
    break;

  case 750: /* range_variable: opt_as SQL_TOKEN_NAME  */
        {(yyval.pParseNode) = SQL_NEW_RULE;
        (yyval.pParseNode)->append((yyvsp[-1].pParseNode));
        (yyval.pParseNode)->append((yyvsp[0].pParseNode));
        }
    break;

  case 752: /* sql: search_condition  */
                {
			if (xxx_pGLOBAL_SQLPARSER->inPredicateCheck()) // sql: rule 1
			{
				(yyval.pParseNode) = (yyvsp[0].pParseNode);
				if ( SQL_ISRULE((yyval.pParseNode),search_condition) )
				{
					(yyval.pParseNode)->insert(0,newNode("(", SQL_NODE_PUNCTUATION));
					(yyval.pParseNode)->append(newNode(")", SQL_NODE_PUNCTUATION));
				}
			}
			else
				YYERROR;
		}
    break;

  case 754: /* trigger_definition: SQL_TOKEN_CREATE SQL_TOKEN_TRIGGER trigger_name trigger_action_time trigger_event SQL_TOKEN_ON table_name op_referencing triggered_action  */
        {
		(yyval.pParseNode) = SQL_NEW_RULE;
        (yyval.pParseNode)->append((yyvsp[-8].pParseNode));
        (yyval.pParseNode)->append((yyvsp[-7].pParseNode));
        (yyval.pParseNode)->append((yyvsp[-6].pParseNode));
        (yyval.pParseNode)->append((yyvsp[-5].pParseNode));
        (yyval.pParseNode)->append((yyvsp[-4].pParseNode));
        (yyval.pParseNode)->append((yyvsp[-3].pParseNode));
        (yyval.pParseNode)->append((yyvsp[-2].pParseNode));
        (yyval.pParseNode)->append((yyvsp[-1].pParseNode));
        (yyval.pParseNode)->append((yyvsp[0].pParseNode));
    }
    break;

  case 755: /* op_referencing: %empty  */
        {
		(yyval.pParseNode) = SQL_NEW_RULE;
	}
    break;

  case 756: /* op_referencing: SQL_TOKEN_REFERENCING transition_table_or_variable_list  */
        {
		(yyval.pParseNode) = SQL_NEW_RULE;
        (yyval.pParseNode)->append((yyvsp[-1].pParseNode));
        (yyval.pParseNode)->append((yyvsp[0].pParseNode));
    }
    break;

  case 759: /* trigger_action_time: SQL_TOKEN_INSTEAD SQL_TOKEN_OF  */
        {
		(yyval.pParseNode) = SQL_NEW_RULE;
        (yyval.pParseNode)->append((yyvsp[-1].pParseNode));
        (yyval.pParseNode)->append((yyvsp[0].pParseNode));
    }
    break;

  case 762: /* trigger_event: SQL_TOKEN_UPDATE op_trigger_columnlist  */
        {
		(yyval.pParseNode) = SQL_NEW_RULE;
        (yyval.pParseNode)->append((yyvsp[-1].pParseNode));
        (yyval.pParseNode)->append((yyvsp[0].pParseNode));
    }
    break;

  case 763: /* op_trigger_columnlist: %empty  */
        {
		(yyval.pParseNode) = SQL_NEW_RULE;
	}
    break;

  case 764: /* op_trigger_columnlist: SQL_TOKEN_OF trigger_column_list  */
        {
		(yyval.pParseNode) = SQL_NEW_RULE;
        (yyval.pParseNode)->append((yyvsp[-1].pParseNode));
        (yyval.pParseNode)->append((yyvsp[0].pParseNode));
    }
    break;

  case 766: /* triggered_action: op_triggered_action_for triggered_when_clause triggered_SQL_statement  */
        {
		(yyval.pParseNode) = SQL_NEW_RULE;
        (yyval.pParseNode)->append((yyvsp[-2].pParseNode));
        (yyval.pParseNode)->append((yyvsp[-1].pParseNode));
        (yyval.pParseNode)->append((yyvsp[0].pParseNode));
    }
    break;

  case 767: /* op_triggered_action_for: %empty  */
                {
		(yyval.pParseNode) = SQL_NEW_RULE;
		}
    break;

  case 768: /* op_triggered_action_for: SQL_TOKEN_FOR SQL_TOKEN_EACH trigger_for  */
        {
		(yyval.pParseNode) = SQL_NEW_RULE;
        (yyval.pParseNode)->append((yyvsp[-2].pParseNode));
        (yyval.pParseNode)->append((yyvsp[-1].pParseNode));
        (yyval.pParseNode)->append((yyvsp[0].pParseNode));
    }
    break;

  case 771: /* triggered_when_clause: %empty  */
        {
		(yyval.pParseNode) = SQL_NEW_RULE;
	}
    break;

  case 772: /* triggered_when_clause: SQL_TOKEN_WHEN parenthesized_boolean_value_expression  */
        {
		(yyval.pParseNode) = SQL_NEW_RULE;
        (yyval.pParseNode)->append((yyvsp[-1].pParseNode));
        (yyval.pParseNode)->append((yyvsp[0].pParseNode));
    }
    break;

  case 774: /* triggered_SQL_statement: SQL_TOKEN_BEGIN SQL_TOKEN_ATOMIC SQL_procedure_statement_list ';' SQL_TOKEN_END  */
        {
		(yyval.pParseNode) = SQL_NEW_RULE;
        (yyval.pParseNode)->append((yyvsp[-4].pParseNode));
        (yyval.pParseNode)->append((yyvsp[-3].pParseNode));
        (yyval.pParseNode)->append((yyvsp[-2].pParseNode));
        (yyval.pParseNode)->append((yyvsp[-1].pParseNode) = newNode(";", SQL_NODE_PUNCTUATION));
        (yyval.pParseNode)->append((yyvsp[0].pParseNode));
    }
    break;

  case 775: /* SQL_procedure_statement_list: SQL_procedure_statement  */
                {
			(yyval.pParseNode) = SQL_NEW_LISTRULE;
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 776: /* SQL_procedure_statement_list: SQL_procedure_statement_list ';' SQL_procedure_statement  */
                {
			(yyvsp[-2].pParseNode)->append((yyvsp[0].pParseNode));
			(yyval.pParseNode) = (yyvsp[-2].pParseNode);
		}
    break;

  case 778: /* transition_table_or_variable_list: transition_table_or_variable  */
                {
			(yyval.pParseNode) = SQL_NEW_LISTRULE;
			(yyval.pParseNode)->append((yyvsp[0].pParseNode));
		}
    break;

  case 779: /* transition_table_or_variable_list: transition_table_or_variable_list transition_table_or_variable  */
                {
			(yyvsp[-1].pParseNode)->append((yyvsp[0].pParseNode));
			(yyval.pParseNode) = (yyvsp[-1].pParseNode);
		}
    break;

  case 780: /* transition_table_or_variable: SQL_TOKEN_OLD opt_row opt_as old_transition_variable_name  */
        {
		(yyval.pParseNode) = SQL_NEW_RULE;
        (yyval.pParseNode)->append((yyvsp[-3].pParseNode));
        (yyval.pParseNode)->append((yyvsp[-2].pParseNode));
        (yyval.pParseNode)->append((yyvsp[-1].pParseNode));
        (yyval.pParseNode)->append((yyvsp[0].pParseNode));
    }
    break;

  case 781: /* transition_table_or_variable: SQL_TOKEN_NEW opt_row opt_as new_transition_variable_name  */
        {
		(yyval.pParseNode) = SQL_NEW_RULE;
        (yyval.pParseNode)->append((yyvsp[-3].pParseNode));
        (yyval.pParseNode)->append((yyvsp[-2].pParseNode));
        (yyval.pParseNode)->append((yyvsp[-1].pParseNode));
        (yyval.pParseNode)->append((yyvsp[0].pParseNode));
    }
    break;

  case 782: /* transition_table_or_variable: SQL_TOKEN_OLD SQL_TOKEN_TABLE opt_as old_transition_table_name  */
        {
		(yyval.pParseNode) = SQL_NEW_RULE;
        (yyval.pParseNode)->append((yyvsp[-3].pParseNode));
        (yyval.pParseNode)->append((yyvsp[-2].pParseNode));
        (yyval.pParseNode)->append((yyvsp[-1].pParseNode));
        (yyval.pParseNode)->append((yyvsp[0].pParseNode));
    }
    break;

  case 783: /* transition_table_or_variable: SQL_TOKEN_NEW SQL_TOKEN_TABLE opt_as new_transition_table_name  */
        {
		(yyval.pParseNode) = SQL_NEW_RULE;
        (yyval.pParseNode)->append((yyvsp[-3].pParseNode));
        (yyval.pParseNode)->append((yyvsp[-2].pParseNode));
        (yyval.pParseNode)->append((yyvsp[-1].pParseNode));
        (yyval.pParseNode)->append((yyvsp[0].pParseNode));
    }
    break;



      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
    }

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}




using namespace ::com::sun::star::sdbc;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::i18n;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::util;
using namespace ::osl;
using namespace ::dbtools;
//	using namespace connectivity;

//============================================================
//= a helper for static ascii pseudo-unicode strings
//============================================================
// string constants
struct _ConstAsciiString_
{
	sal_Int32 length;
	sal_Char  const* str;

	operator rtl::OUString () const { return rtl::OUString(str, length, RTL_TEXTENCODING_ASCII_US); }
//	operator ::rtl::OUString () const { return ::rtl::OUString(str, length, RTL_TEXTENCODING_ASCII_US); }
	operator const sal_Char * () const { return str; }
	operator ::rtl::OString() const { return str; }
};

#define IMPLEMENT_CONSTASCII_STRING( name, string ) \
	_ConstAsciiString_ const name = { sizeof(string)-1, string }

IMPLEMENT_CONSTASCII_STRING(ERROR_STR_GENERAL, "Syntax error in SQL expression");
IMPLEMENT_CONSTASCII_STRING(ERROR_STR_VALUE_NO_LIKE, "The value #1 can not be used with LIKE.");
IMPLEMENT_CONSTASCII_STRING(ERROR_STR_FIELD_NO_LIKE, "LIKE can not be used with this field.");
IMPLEMENT_CONSTASCII_STRING(ERROR_STR_INVALID_COMPARE, "The entered criterion can not be compared with this field.");
IMPLEMENT_CONSTASCII_STRING(ERROR_STR_INVALID_DATE_COMPARE, "The field can not be compared with a date.");
IMPLEMENT_CONSTASCII_STRING(ERROR_STR_INVALID_REAL_COMPARE,	"The field can not be compared with a floating point number.");
IMPLEMENT_CONSTASCII_STRING(ERROR_STR_INVALID_INT_COMPARE,	"The field can not be compared with a number.");
IMPLEMENT_CONSTASCII_STRING(ERROR_STR_INVALID_TABLE,	"The database does not contain a table named \"#\".");
IMPLEMENT_CONSTASCII_STRING(ERROR_STR_INVALID_TABLE_OR_QUERY,   "The database does contain neither a table nor a query named \"#\".");
IMPLEMENT_CONSTASCII_STRING(ERROR_STR_INVALID_COLUMN,	"The column \"#1\" is unknown in the table \"#2\".");
IMPLEMENT_CONSTASCII_STRING(ERROR_STR_INVALID_TABLE_EXIST,	"The database already contains a table or view with name \"#\".");
IMPLEMENT_CONSTASCII_STRING(ERROR_STR_INVALID_QUERY_EXIST,	"The database already contains a query with name \"#\".");

IMPLEMENT_CONSTASCII_STRING(KEY_STR_LIKE, "LIKE");
IMPLEMENT_CONSTASCII_STRING(KEY_STR_NOT, "NOT");
IMPLEMENT_CONSTASCII_STRING(KEY_STR_NULL, "NULL");
IMPLEMENT_CONSTASCII_STRING(KEY_STR_TRUE, "True");
IMPLEMENT_CONSTASCII_STRING(KEY_STR_FALSE, "False");
IMPLEMENT_CONSTASCII_STRING(KEY_STR_IS, "IS");
IMPLEMENT_CONSTASCII_STRING(KEY_STR_BETWEEN, "BETWEEN");
IMPLEMENT_CONSTASCII_STRING(KEY_STR_OR, "OR");
IMPLEMENT_CONSTASCII_STRING(KEY_STR_AND, "AND");
IMPLEMENT_CONSTASCII_STRING(KEY_STR_AVG, "AVG");
IMPLEMENT_CONSTASCII_STRING(KEY_STR_COUNT, "COUNT");
IMPLEMENT_CONSTASCII_STRING(KEY_STR_MAX, "MAX");
IMPLEMENT_CONSTASCII_STRING(KEY_STR_MIN, "MIN");
IMPLEMENT_CONSTASCII_STRING(KEY_STR_SUM, "SUM");
IMPLEMENT_CONSTASCII_STRING(KEY_STR_EVERY, "EVERY");
IMPLEMENT_CONSTASCII_STRING(KEY_STR_ANY, "ANY");
IMPLEMENT_CONSTASCII_STRING(KEY_STR_SOME, "SOME");
IMPLEMENT_CONSTASCII_STRING(KEY_STR_STDDEV_POP, "STDDEV_POP");
IMPLEMENT_CONSTASCII_STRING(KEY_STR_STDDEV_SAMP, "STDDEV_SAMP");
IMPLEMENT_CONSTASCII_STRING(KEY_STR_VAR_SAMP, "VAR_SAMP");
IMPLEMENT_CONSTASCII_STRING(KEY_STR_VAR_POP, "VAR_POP");
IMPLEMENT_CONSTASCII_STRING(KEY_STR_COLLECT, "COLLECT");
IMPLEMENT_CONSTASCII_STRING(KEY_STR_FUSION, "FUSION");
IMPLEMENT_CONSTASCII_STRING(KEY_STR_INTERSECTION, "INTERSECTION");

IMPLEMENT_CONSTASCII_STRING(FIELD_STR_NULLDATE, "NullDate");

IMPLEMENT_CONSTASCII_STRING(STR_SQL_TOKEN, "SQL_TOKEN_");

//==========================================================================
//= OParseContext
//==========================================================================
//-----------------------------------------------------------------------------
OParseContext::OParseContext()
{
}

//-----------------------------------------------------------------------------
OParseContext::~OParseContext()
{
}

//-----------------------------------------------------------------------------
::rtl::OUString OParseContext::getErrorMessage(ErrorCode _eCode) const
{
	::rtl::OUString aMsg;
	switch (_eCode)
	{
		case ERROR_GENERAL:					aMsg = ERROR_STR_GENERAL; break;
		case ERROR_VALUE_NO_LIKE:			aMsg = ERROR_STR_VALUE_NO_LIKE; break;
		case ERROR_FIELD_NO_LIKE:			aMsg = ERROR_STR_FIELD_NO_LIKE; break;
		case ERROR_INVALID_COMPARE:			aMsg = ERROR_STR_INVALID_COMPARE; break;
		case ERROR_INVALID_INT_COMPARE:		aMsg = ERROR_STR_INVALID_INT_COMPARE; break;
		case ERROR_INVALID_DATE_COMPARE:	aMsg = ERROR_STR_INVALID_DATE_COMPARE; break;
		case ERROR_INVALID_REAL_COMPARE:	aMsg = ERROR_STR_INVALID_REAL_COMPARE; break;
		case ERROR_INVALID_TABLE:			aMsg = ERROR_STR_INVALID_TABLE; break;
		case ERROR_INVALID_TABLE_OR_QUERY:  aMsg = ERROR_STR_INVALID_TABLE_OR_QUERY; break;
		case ERROR_INVALID_COLUMN:			aMsg = ERROR_STR_INVALID_COLUMN; break;
		case ERROR_INVALID_TABLE_EXIST:		aMsg = ERROR_STR_INVALID_TABLE_EXIST; break;
		case ERROR_INVALID_QUERY_EXIST:		aMsg = ERROR_STR_INVALID_QUERY_EXIST; break;
        default:
            OSL_ENSURE( false, "OParseContext::getErrorMessage: unknown error code!" );
            break;
	}
	return aMsg;
}

//-----------------------------------------------------------------------------
::rtl::OString OParseContext::getIntlKeywordAscii(InternationalKeyCode _eKey) const
{
	::rtl::OString aKeyword;
	switch (_eKey)
	{
		case KEY_LIKE:		aKeyword = KEY_STR_LIKE; break;
		case KEY_NOT:		aKeyword = KEY_STR_NOT; break;
		case KEY_NULL:		aKeyword = KEY_STR_NULL; break;
		case KEY_TRUE:		aKeyword = KEY_STR_TRUE; break;
		case KEY_FALSE:		aKeyword = KEY_STR_FALSE; break;
		case KEY_IS:		aKeyword = KEY_STR_IS; break;
		case KEY_BETWEEN:	aKeyword = KEY_STR_BETWEEN; break;
		case KEY_OR:		aKeyword = KEY_STR_OR; break;
		case KEY_AND:		aKeyword = KEY_STR_AND; break;
		case KEY_AVG:		aKeyword = KEY_STR_AVG; break;
		case KEY_COUNT:		aKeyword = KEY_STR_COUNT; break;
		case KEY_MAX:		aKeyword = KEY_STR_MAX; break;
		case KEY_MIN:		aKeyword = KEY_STR_MIN; break;
		case KEY_SUM:		aKeyword = KEY_STR_SUM; break;
        case KEY_EVERY:     aKeyword = KEY_STR_EVERY; break;
        case KEY_ANY:       aKeyword = KEY_STR_ANY; break;
        case KEY_SOME:      aKeyword = KEY_STR_SOME; break;
        case KEY_STDDEV_POP: aKeyword = KEY_STR_STDDEV_POP; break;
        case KEY_STDDEV_SAMP: aKeyword = KEY_STR_STDDEV_SAMP; break;
        case KEY_VAR_SAMP:  aKeyword = KEY_STR_VAR_SAMP; break;
        case KEY_VAR_POP:   aKeyword = KEY_STR_VAR_POP; break;
        case KEY_COLLECT:   aKeyword = KEY_STR_COLLECT; break;
        case KEY_FUSION:    aKeyword = KEY_STR_FUSION; break;
        case KEY_INTERSECTION:aKeyword = KEY_STR_INTERSECTION; break;
        case KEY_NONE:      break;
        default:
            OSL_ENSURE( false, "OParseContext::getIntlKeywordAscii: unknown key!" );
            break;
	}
	return aKeyword;
}

//-----------------------------------------------------------------------------
IParseContext::InternationalKeyCode OParseContext::getIntlKeyCode(const ::rtl::OString& rToken) const
{
	static IParseContext::InternationalKeyCode Intl_TokenID[] =
	{
		KEY_LIKE, KEY_NOT, KEY_NULL, KEY_TRUE,
		KEY_FALSE, KEY_IS, KEY_BETWEEN, KEY_OR,
		KEY_AND, KEY_AVG, KEY_COUNT, KEY_MAX,
		KEY_MIN, KEY_SUM, KEY_EVERY,KEY_ANY,KEY_SOME,
        KEY_STDDEV_POP,KEY_STDDEV_SAMP,KEY_VAR_SAMP,
        KEY_VAR_POP,KEY_COLLECT,KEY_FUSION,KEY_INTERSECTION
	};

	sal_uInt32 nCount = sizeof Intl_TokenID / sizeof Intl_TokenID[0];
	for (sal_uInt32 i = 0; i < nCount; i++)
	{
		::rtl::OString aKey = getIntlKeywordAscii(Intl_TokenID[i]);
		if (rToken.equalsIgnoreAsciiCase(aKey))
			return Intl_TokenID[i];
	}

	return KEY_NONE;
}

//------------------------------------------------------------------------------
static Locale& impl_getLocaleInstance( )
{
	static Locale s_aLocale(
		::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "en" ) ),
		::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( "US" ) ),
		::rtl::OUString( )
	);
	return s_aLocale;
}

//------------------------------------------------------------------------------
void OParseContext::setDefaultLocale( const ::com::sun::star::lang::Locale& _rLocale )
{
	impl_getLocaleInstance() = _rLocale;
}

//------------------------------------------------------------------------------
Locale OParseContext::getPreferredLocale( ) const
{
	return getDefaultLocale();
}

//------------------------------------------------------------------------------
const Locale& OParseContext::getDefaultLocale()
{
	return impl_getLocaleInstance();
}

//==========================================================================
//= misc
//==========================================================================
// Der (leider globale) yylval fuer die Uebergabe von
// Werten vom Scanner an den Parser. Die globale Variable
// wird nur kurzzeitig verwendet, der Parser liest die Variable
// sofort nach dem Scanner-Aufruf in eine gleichnamige eigene
// Member-Variable.

const double fMilliSecondsPerDay = 86400000.0;

//------------------------------------------------------------------------------


//------------------------------------------------------------------
::rtl::OUString ConvertLikeToken(const OSQLParseNode* pTokenNode, const OSQLParseNode* pEscapeNode, sal_Bool bInternational)
{
	::rtl::OUStringBuffer aMatchStr;
	if (pTokenNode->isToken())
	{
		sal_Unicode cEscape = 0;
		if (pEscapeNode->count())
			cEscape = pEscapeNode->getChild(1)->getTokenValue().toChar();

		// Platzhalter austauschen
		aMatchStr = pTokenNode->getTokenValue();
		const sal_Int32 nLen = aMatchStr.getLength();
		::rtl::OUStringBuffer sSearch,sReplace;
		if ( bInternational )
		{
		    sSearch.appendAscii("%_",2);
		    sReplace.appendAscii("*?",2);
		}
		else
		{
		    sSearch.appendAscii("*?",2);
		    sReplace.appendAscii("%_",2);
		}
		
		for (sal_Int32 i = 0; i < nLen; i++)
		{
			const sal_Unicode c = aMatchStr.charAt(i);
			if (c == sSearch.charAt(0) || c == sSearch.charAt(1))
			{
				if (i > 0 && aMatchStr.charAt(i-1) == cEscape)
					continue;
				else
				{
					const sal_Unicode cCharacter = sReplace.charAt( (c == sSearch.charAt(0)) ? 0 : 1);
					aMatchStr.setCharAt(i , cCharacter);
				}
			}
		}
	}
	return aMatchStr.makeStringAndClear();
}

//==========================================================================
//= OSQLParser
//==========================================================================

sal_uInt32			    OSQLParser::s_nRuleIDs[OSQLParseNode::rule_count + 1];
OSQLParser::RuleIDMap   OSQLParser::s_aReverseRuleIDLookup;
OParseContext		    OSQLParser::s_aDefaultContext;

sal_Int32			OSQLParser::s_nRefCount	= 0;
//	::osl::Mutex		OSQLParser::s_aMutex;
OSQLScanner*		OSQLParser::s_pScanner = 0;
OSQLParseNodesGarbageCollector*		OSQLParser::s_pGarbageCollector = 0;
::com::sun::star::uno::Reference< ::com::sun::star::i18n::XLocaleData>		OSQLParser::s_xLocaleData = NULL;
//-----------------------------------------------------------------------------
void setParser(OSQLParser* _pParser)
{
	xxx_pGLOBAL_SQLPARSER = _pParser;
}
// -------------------------------------------------------------------------
void OSQLParser::setParseTree(OSQLParseNode * pNewParseTree)
{
	::osl::MutexGuard aGuard(getMutex());
	m_pParseTree = pNewParseTree;
}
//-----------------------------------------------------------------------------
OSQLParseNode* OSQLParser::parseTree(::rtl::OUString& rErrorMessage,
									 const ::rtl::OUString& rStatement,
								     sal_Bool bInternational)
{


	// Guard the parsing
	::osl::MutexGuard aGuard(getMutex());
	// must be reset
	setParser(this);

	// defines how to scan
	s_pScanner->SetRule(s_pScanner->GetSQLRule()); // initial
	s_pScanner->prepareScan(rStatement, m_pContext, bInternational);

	SQLyylval.pParseNode = NULL;
	//	SQLyypvt = NULL;
	m_pParseTree = NULL;
	m_sErrorMessage = ::rtl::OUString();

	// ... und den Parser anwerfen ...
	if (SQLyyparse() != 0)
	{
		// only set the error message, if it's not already set
		if (!m_sErrorMessage.getLength())
			m_sErrorMessage = s_pScanner->getErrorMessage();
		if (!m_sErrorMessage.getLength())
			m_sErrorMessage = m_pContext->getErrorMessage(IParseContext::ERROR_GENERAL);

		rErrorMessage = m_sErrorMessage;

		// clear the garbage collector
		(*s_pGarbageCollector)->clearAndDelete();
		return NULL;
	}
	else
	{
		(*s_pGarbageCollector)->clear();

		// Das Ergebnis liefern (den Root Parse Node):

		//	OSL_ENSURE(Sdbyyval.pParseNode != NULL,"OSQLParser: Parser hat keinen ParseNode geliefert");
		//	return Sdbyyval.pParseNode;
		// geht nicht wegen Bug in MKS YACC-erzeugtem Code (es wird ein falscher ParseNode
		// geliefert).

		// Stattdessen setzt die Parse-Routine jetzt den Member pParseTree
		// - einfach diesen zurueckliefern:
		OSL_ENSURE(m_pParseTree != NULL,"OSQLParser: Parser hat keinen ParseTree geliefert");
		return m_pParseTree;
	}
}
//-----------------------------------------------------------------------------
::rtl::OString OSQLParser::TokenIDToStr(sal_uInt32 nTokenID, const IParseContext* pContext)
{
	::rtl::OString aStr;
	if (pContext)
	{
		IParseContext::InternationalKeyCode eKeyCode = IParseContext::KEY_NONE;
		switch( nTokenID )
		{
			case SQL_TOKEN_LIKE: eKeyCode = IParseContext::KEY_LIKE; break;
			case SQL_TOKEN_NOT: eKeyCode = IParseContext::KEY_NOT; break;
			case SQL_TOKEN_NULL: eKeyCode = IParseContext::KEY_NULL; break;
			case SQL_TOKEN_TRUE: eKeyCode = IParseContext::KEY_TRUE; break;
			case SQL_TOKEN_FALSE: eKeyCode = IParseContext::KEY_FALSE; break;
			case SQL_TOKEN_IS: eKeyCode = IParseContext::KEY_IS; break;
			case SQL_TOKEN_BETWEEN: eKeyCode = IParseContext::KEY_BETWEEN; break;
			case SQL_TOKEN_OR: eKeyCode = IParseContext::KEY_OR; break;
			case SQL_TOKEN_AND: eKeyCode = IParseContext::KEY_AND; break;
			case SQL_TOKEN_AVG: eKeyCode = IParseContext::KEY_AVG; break;
			case SQL_TOKEN_COUNT: eKeyCode = IParseContext::KEY_COUNT; break;
			case SQL_TOKEN_MAX: eKeyCode = IParseContext::KEY_MAX; break;
			case SQL_TOKEN_MIN: eKeyCode = IParseContext::KEY_MIN; break;
			case SQL_TOKEN_SUM: eKeyCode = IParseContext::KEY_SUM; break;
		}
		if ( eKeyCode != IParseContext::KEY_NONE )
		    aStr = pContext->getIntlKeywordAscii(eKeyCode);
	}

	if (!aStr.getLength())
	{
		aStr = yytname[YYTRANSLATE(nTokenID)];
		if(!aStr.compareTo("SQL_TOKEN_",10))
			aStr = aStr.copy(10);
	}
	return aStr;
}

//-----------------------------------------------------------------------------
::rtl::OUString OSQLParser::RuleIDToStr(sal_uInt32 nRuleID)
{
	OSL_ENSURE(nRuleID < (sizeof yytname/sizeof yytname[0]), "OSQLParser::RuleIDToStr: Invalid nRuleId!");
	return ::rtl::OUString::createFromAscii(yytname[nRuleID]);
}

//-----------------------------------------------------------------------------
sal_uInt32 OSQLParser::StrToRuleID(const ::rtl::OString & rValue)
{
	// In yysvar nach dem angegebenen Namen suchen, den Index zurueckliefern
	// (oder 0, wenn nicht gefunden)
	static sal_uInt32 nLen = sizeof(yytname)/sizeof(yytname[0]);
	for (sal_uInt32 i = YYTRANSLATE(SQL_TOKEN_INVALIDSYMBOL); i < (nLen-1); i++)
	{
		if (yytname && rValue == yytname[i])
			return i;
	}

	// Nicht gefunden
	return 0;
}

//-----------------------------------------------------------------------------
OSQLParseNode::Rule OSQLParser::RuleIDToRule( sal_uInt32 _nRule )
{
    return s_aReverseRuleIDLookup[ _nRule ];
}

//-----------------------------------------------------------------------------
sal_uInt32 OSQLParser::RuleID(OSQLParseNode::Rule eRule)
{
	return s_nRuleIDs[(sal_uInt16)eRule];
}
// -------------------------------------------------------------------------
sal_Int16 OSQLParser::buildNode(OSQLParseNode*& pAppend,OSQLParseNode* pCompare,OSQLParseNode* pLiteral,OSQLParseNode* pLiteral2)
{
	OSQLParseNode* pColumnRef = new OSQLInternalNode(aEmptyString, SQL_NODE_RULE,OSQLParser::RuleID(OSQLParseNode::column_ref));
	pColumnRef->append(new OSQLInternalNode(m_sFieldName,SQL_NODE_NAME));
	OSQLParseNode* pComp = NULL;
	if ( SQL_ISTOKEN( pCompare, BETWEEN) && pLiteral2 )
		pComp = new OSQLInternalNode(aEmptyString, SQL_NODE_RULE,OSQLParser::RuleID(OSQLParseNode::between_predicate_part_2));
	else
		pComp = new OSQLInternalNode(aEmptyString, SQL_NODE_RULE,OSQLParser::RuleID(OSQLParseNode::comparison_predicate));
	
	pComp->append(pColumnRef);
	pComp->append(pCompare);
	pComp->append(pLiteral);
	if ( pLiteral2 )
	{
		pComp->append(new OSQLInternalNode(aEmptyString, SQL_NODE_KEYWORD,SQL_TOKEN_AND));
		pComp->append(pLiteral2);		
	}
	pAppend->append(pComp);
	return 1;
}
//-----------------------------------------------------------------------------
sal_Int16 OSQLParser::buildStringNodes(OSQLParseNode*& pLiteral)
{
	if(!pLiteral)
		return 1;

	if(SQL_ISRULE(pLiteral,set_fct_spec) || SQL_ISRULE(pLiteral,general_set_fct) || SQL_ISRULE(pLiteral,column_ref)
		|| SQL_ISRULE(pLiteral,subquery))
		return 1; // here I have a function that I can't transform into a string

	if(pLiteral->getNodeType() == SQL_NODE_INTNUM || pLiteral->getNodeType() == SQL_NODE_APPROXNUM || pLiteral->getNodeType() == SQL_NODE_ACCESS_DATE)
	{
		OSQLParseNode* pParent = pLiteral->getParent();

		OSQLParseNode* pNewNode = new OSQLInternalNode(pLiteral->getTokenValue(), SQL_NODE_STRING);
		pParent->replace(pLiteral, pNewNode);
		delete pLiteral;
		pLiteral = NULL;
		return 1;
	}

	for(sal_uInt32 i=0;i<pLiteral->count();++i)
	{
		OSQLParseNode* pChild = pLiteral->getChild(i);
		buildStringNodes(pChild);
	}
	if(SQL_ISRULE(pLiteral,term) || SQL_ISRULE(pLiteral,value_exp_primary))
	{
		m_sErrorMessage = m_pContext->getErrorMessage(IParseContext::ERROR_INVALID_COMPARE);
		return 0;
	}
	return 1;
}
//-----------------------------------------------------------------------------
sal_Int16 OSQLParser::buildComparsionRule(OSQLParseNode*& pAppend,OSQLParseNode* pLiteral)
{
	OSQLParseNode* pComp = new OSQLInternalNode(::rtl::OUString::createFromAscii("="), SQL_NODE_EQUAL);
	return buildPredicateRule(pAppend,pLiteral,pComp);
}


//-----------------------------------------------------------------------------
void OSQLParser::reduceLiteral(OSQLParseNode*& pLiteral, sal_Bool bAppendBlank)
{
	OSL_ENSURE(pLiteral->isRule(), "This is no ::com::sun::star::chaos::Rule");
	OSL_ENSURE(pLiteral->count() == 2, "OSQLParser::ReduceLiteral() Invalid count");
	OSQLParseNode* pTemp = pLiteral;
	::rtl::OUStringBuffer aValue(pLiteral->getChild(0)->getTokenValue());
	if (bAppendBlank)
	{
		aValue.appendAscii(" ");
	}
	
	aValue.append(pLiteral->getChild(1)->getTokenValue());

	pLiteral = new OSQLInternalNode(aValue.makeStringAndClear(),SQL_NODE_STRING);
	delete pTemp;
}

// -------------------------------------------------------------------------
void OSQLParser::error( const sal_Char* fmt)
{
	if(!m_sErrorMessage.getLength())
	{
		::rtl::OUString sStr(fmt,strlen(fmt),RTL_TEXTENCODING_UTF8);
		::rtl::OUString sSQL_TOKEN(::rtl::OUString::createFromAscii("SQL_TOKEN_"));

		sal_Int32 nPos1 = sStr.indexOf(sSQL_TOKEN);
		if(nPos1 != -1)
		{
			::rtl::OUString sFirst  = sStr.copy(0,nPos1);
			sal_Int32 nPos2 = sStr.indexOf(sSQL_TOKEN,nPos1+1);
			if(nPos2 != -1)
			{
				::rtl::OUString sSecond = sStr.copy(nPos1+sSQL_TOKEN.getLength(),nPos2-nPos1-sSQL_TOKEN.getLength());
				sFirst  += sSecond;
				sFirst  += sStr.copy(nPos2+sSQL_TOKEN.getLength());
			}
			else
				sFirst += sStr.copy(nPos1+sSQL_TOKEN.getLength());

			m_sErrorMessage = sFirst;
		}
		else
			m_sErrorMessage = sStr;

		::rtl::OUString aError = s_pScanner->getErrorMessage();
		if(aError.getLength())
		{
			m_sErrorMessage += ::rtl::OUString::createFromAscii(", ");
			m_sErrorMessage += aError;
		}
	}
}
// -------------------------------------------------------------------------
int OSQLParser::SQLlex()
{
	return s_pScanner->SQLlex();
}

#if defined __SUNPRO_CC
#pragma enable_warn
#elif defined _MSC_VER
#pragma warning(pop)
#endif
