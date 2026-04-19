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

#ifndef YY_SQLYY_SQLBISON_HXX_INCLUDED
# define YY_SQLYY_SQLBISON_HXX_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int SQLyydebug;
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
    SQL_TOKEN_STRING = 258,        /* SQL_TOKEN_STRING  */
    SQL_TOKEN_ACCESS_DATE = 259,   /* SQL_TOKEN_ACCESS_DATE  */
    SQL_TOKEN_INT = 260,           /* SQL_TOKEN_INT  */
    SQL_TOKEN_REAL_NUM = 261,      /* SQL_TOKEN_REAL_NUM  */
    SQL_TOKEN_INTNUM = 262,        /* SQL_TOKEN_INTNUM  */
    SQL_TOKEN_APPROXNUM = 263,     /* SQL_TOKEN_APPROXNUM  */
    SQL_TOKEN_NOT = 264,           /* SQL_TOKEN_NOT  */
    SQL_TOKEN_NAME = 265,          /* SQL_TOKEN_NAME  */
    SQL_TOKEN_UMINUS = 266,        /* SQL_TOKEN_UMINUS  */
    SQL_TOKEN_ALL = 267,           /* SQL_TOKEN_ALL  */
    SQL_TOKEN_ALTER = 268,         /* SQL_TOKEN_ALTER  */
    SQL_TOKEN_AMMSC = 269,         /* SQL_TOKEN_AMMSC  */
    SQL_TOKEN_ANY = 270,           /* SQL_TOKEN_ANY  */
    SQL_TOKEN_AS = 271,            /* SQL_TOKEN_AS  */
    SQL_TOKEN_ASC = 272,           /* SQL_TOKEN_ASC  */
    SQL_TOKEN_AT = 273,            /* SQL_TOKEN_AT  */
    SQL_TOKEN_AUTHORIZATION = 274, /* SQL_TOKEN_AUTHORIZATION  */
    SQL_TOKEN_AVG = 275,           /* SQL_TOKEN_AVG  */
    SQL_TOKEN_BETWEEN = 276,       /* SQL_TOKEN_BETWEEN  */
    SQL_TOKEN_BIT = 277,           /* SQL_TOKEN_BIT  */
    SQL_TOKEN_BOTH = 278,          /* SQL_TOKEN_BOTH  */
    SQL_TOKEN_BY = 279,            /* SQL_TOKEN_BY  */
    SQL_TOKEN_CAST = 280,          /* SQL_TOKEN_CAST  */
    SQL_TOKEN_CHARACTER = 281,     /* SQL_TOKEN_CHARACTER  */
    SQL_TOKEN_CHECK = 282,         /* SQL_TOKEN_CHECK  */
    SQL_TOKEN_COLLATE = 283,       /* SQL_TOKEN_COLLATE  */
    SQL_TOKEN_COMMIT = 284,        /* SQL_TOKEN_COMMIT  */
    SQL_TOKEN_CONTINUE = 285,      /* SQL_TOKEN_CONTINUE  */
    SQL_TOKEN_CONVERT = 286,       /* SQL_TOKEN_CONVERT  */
    SQL_TOKEN_COUNT = 287,         /* SQL_TOKEN_COUNT  */
    SQL_TOKEN_CREATE = 288,        /* SQL_TOKEN_CREATE  */
    SQL_TOKEN_CROSS = 289,         /* SQL_TOKEN_CROSS  */
    SQL_TOKEN_CURRENT = 290,       /* SQL_TOKEN_CURRENT  */
    SQL_TOKEN_CURSOR = 291,        /* SQL_TOKEN_CURSOR  */
    SQL_TOKEN_DATE = 292,          /* SQL_TOKEN_DATE  */
    SQL_TOKEN_DATEVALUE = 293,     /* SQL_TOKEN_DATEVALUE  */
    SQL_TOKEN_DAY = 294,           /* SQL_TOKEN_DAY  */
    SQL_TOKEN_DEC = 295,           /* SQL_TOKEN_DEC  */
    SQL_TOKEN_DECIMAL = 296,       /* SQL_TOKEN_DECIMAL  */
    SQL_TOKEN_DECLARE = 297,       /* SQL_TOKEN_DECLARE  */
    SQL_TOKEN_DEFAULT = 298,       /* SQL_TOKEN_DEFAULT  */
    SQL_TOKEN_DELETE = 299,        /* SQL_TOKEN_DELETE  */
    SQL_TOKEN_DESC = 300,          /* SQL_TOKEN_DESC  */
    SQL_TOKEN_DISTINCT = 301,      /* SQL_TOKEN_DISTINCT  */
    SQL_TOKEN_DOUBLE = 302,        /* SQL_TOKEN_DOUBLE  */
    SQL_TOKEN_DROP = 303,          /* SQL_TOKEN_DROP  */
    SQL_TOKEN_ESCAPE = 304,        /* SQL_TOKEN_ESCAPE  */
    SQL_TOKEN_EXCEPT = 305,        /* SQL_TOKEN_EXCEPT  */
    SQL_TOKEN_EXISTS = 306,        /* SQL_TOKEN_EXISTS  */
    SQL_TOKEN_FALSE = 307,         /* SQL_TOKEN_FALSE  */
    SQL_TOKEN_FETCH = 308,         /* SQL_TOKEN_FETCH  */
    SQL_TOKEN_FLOAT = 309,         /* SQL_TOKEN_FLOAT  */
    SQL_TOKEN_FOR = 310,           /* SQL_TOKEN_FOR  */
    SQL_TOKEN_FOREIGN = 311,       /* SQL_TOKEN_FOREIGN  */
    SQL_TOKEN_FOUND = 312,         /* SQL_TOKEN_FOUND  */
    SQL_TOKEN_FROM = 313,          /* SQL_TOKEN_FROM  */
    SQL_TOKEN_FULL = 314,          /* SQL_TOKEN_FULL  */
    SQL_TOKEN_GRANT = 315,         /* SQL_TOKEN_GRANT  */
    SQL_TOKEN_GROUP = 316,         /* SQL_TOKEN_GROUP  */
    SQL_TOKEN_HAVING = 317,        /* SQL_TOKEN_HAVING  */
    SQL_TOKEN_IN = 318,            /* SQL_TOKEN_IN  */
    SQL_TOKEN_INDICATOR = 319,     /* SQL_TOKEN_INDICATOR  */
    SQL_TOKEN_INNER = 320,         /* SQL_TOKEN_INNER  */
    SQL_TOKEN_INTEGER = 321,       /* SQL_TOKEN_INTEGER  */
    SQL_TOKEN_INTO = 322,          /* SQL_TOKEN_INTO  */
    SQL_TOKEN_IS = 323,            /* SQL_TOKEN_IS  */
    SQL_TOKEN_INTERSECT = 324,     /* SQL_TOKEN_INTERSECT  */
    SQL_TOKEN_JOIN = 325,          /* SQL_TOKEN_JOIN  */
    SQL_TOKEN_KEY = 326,           /* SQL_TOKEN_KEY  */
    SQL_TOKEN_LEADING = 327,       /* SQL_TOKEN_LEADING  */
    SQL_TOKEN_LIKE = 328,          /* SQL_TOKEN_LIKE  */
    SQL_TOKEN_LOCAL = 329,         /* SQL_TOKEN_LOCAL  */
    SQL_TOKEN_LOWER = 330,         /* SQL_TOKEN_LOWER  */
    SQL_TOKEN_MAX = 331,           /* SQL_TOKEN_MAX  */
    SQL_TOKEN_MIN = 332,           /* SQL_TOKEN_MIN  */
    SQL_TOKEN_NATURAL = 333,       /* SQL_TOKEN_NATURAL  */
    SQL_TOKEN_NCHAR = 334,         /* SQL_TOKEN_NCHAR  */
    SQL_TOKEN_NULL = 335,          /* SQL_TOKEN_NULL  */
    SQL_TOKEN_NUMERIC = 336,       /* SQL_TOKEN_NUMERIC  */
    SQL_TOKEN_OCTET_LENGTH = 337,  /* SQL_TOKEN_OCTET_LENGTH  */
    SQL_TOKEN_OF = 338,            /* SQL_TOKEN_OF  */
    SQL_TOKEN_ON = 339,            /* SQL_TOKEN_ON  */
    SQL_TOKEN_OPTION = 340,        /* SQL_TOKEN_OPTION  */
    SQL_TOKEN_ORDER = 341,         /* SQL_TOKEN_ORDER  */
    SQL_TOKEN_OUTER = 342,         /* SQL_TOKEN_OUTER  */
    SQL_TOKEN_PRECISION = 343,     /* SQL_TOKEN_PRECISION  */
    SQL_TOKEN_PRIMARY = 344,       /* SQL_TOKEN_PRIMARY  */
    SQL_TOKEN_PRIVILEGES = 345,    /* SQL_TOKEN_PRIVILEGES  */
    SQL_TOKEN_PROCEDURE = 346,     /* SQL_TOKEN_PROCEDURE  */
    SQL_TOKEN_PUBLIC = 347,        /* SQL_TOKEN_PUBLIC  */
    SQL_TOKEN_REAL = 348,          /* SQL_TOKEN_REAL  */
    SQL_TOKEN_REFERENCES = 349,    /* SQL_TOKEN_REFERENCES  */
    SQL_TOKEN_ROLLBACK = 350,      /* SQL_TOKEN_ROLLBACK  */
    SQL_TOKEN_SCHEMA = 351,        /* SQL_TOKEN_SCHEMA  */
    SQL_TOKEN_SELECT = 352,        /* SQL_TOKEN_SELECT  */
    SQL_TOKEN_SET = 353,           /* SQL_TOKEN_SET  */
    SQL_TOKEN_SIZE = 354,          /* SQL_TOKEN_SIZE  */
    SQL_TOKEN_SMALLINT = 355,      /* SQL_TOKEN_SMALLINT  */
    SQL_TOKEN_SOME = 356,          /* SQL_TOKEN_SOME  */
    SQL_TOKEN_SQLCODE = 357,       /* SQL_TOKEN_SQLCODE  */
    SQL_TOKEN_SQLERROR = 358,      /* SQL_TOKEN_SQLERROR  */
    SQL_TOKEN_SUM = 359,           /* SQL_TOKEN_SUM  */
    SQL_TOKEN_TABLE = 360,         /* SQL_TOKEN_TABLE  */
    SQL_TOKEN_TIME = 361,          /* SQL_TOKEN_TIME  */
    SQL_TOKEN_TIMESTAMP = 362,     /* SQL_TOKEN_TIMESTAMP  */
    SQL_TOKEN_TIMEZONE_HOUR = 363, /* SQL_TOKEN_TIMEZONE_HOUR  */
    SQL_TOKEN_TIMEZONE_MINUTE = 364, /* SQL_TOKEN_TIMEZONE_MINUTE  */
    SQL_TOKEN_TO = 365,            /* SQL_TOKEN_TO  */
    SQL_TOKEN_TRAILING = 366,      /* SQL_TOKEN_TRAILING  */
    SQL_TOKEN_TRANSLATE = 367,     /* SQL_TOKEN_TRANSLATE  */
    SQL_TOKEN_TRIM = 368,          /* SQL_TOKEN_TRIM  */
    SQL_TOKEN_TRUE = 369,          /* SQL_TOKEN_TRUE  */
    SQL_TOKEN_UNION = 370,         /* SQL_TOKEN_UNION  */
    SQL_TOKEN_UNIQUE = 371,        /* SQL_TOKEN_UNIQUE  */
    SQL_TOKEN_UNKNOWN = 372,       /* SQL_TOKEN_UNKNOWN  */
    SQL_TOKEN_UPDATE = 373,        /* SQL_TOKEN_UPDATE  */
    SQL_TOKEN_UPPER = 374,         /* SQL_TOKEN_UPPER  */
    SQL_TOKEN_USAGE = 375,         /* SQL_TOKEN_USAGE  */
    SQL_TOKEN_USER = 376,          /* SQL_TOKEN_USER  */
    SQL_TOKEN_USING = 377,         /* SQL_TOKEN_USING  */
    SQL_TOKEN_VALUES = 378,        /* SQL_TOKEN_VALUES  */
    SQL_TOKEN_VIEW = 379,          /* SQL_TOKEN_VIEW  */
    SQL_TOKEN_WHERE = 380,         /* SQL_TOKEN_WHERE  */
    SQL_TOKEN_WITH = 381,          /* SQL_TOKEN_WITH  */
    SQL_TOKEN_WORK = 382,          /* SQL_TOKEN_WORK  */
    SQL_TOKEN_ZONE = 383,          /* SQL_TOKEN_ZONE  */
    SQL_TOKEN_CALL = 384,          /* SQL_TOKEN_CALL  */
    SQL_TOKEN_D = 385,             /* SQL_TOKEN_D  */
    SQL_TOKEN_FN = 386,            /* SQL_TOKEN_FN  */
    SQL_TOKEN_T = 387,             /* SQL_TOKEN_T  */
    SQL_TOKEN_TS = 388,            /* SQL_TOKEN_TS  */
    SQL_TOKEN_OJ = 389,            /* SQL_TOKEN_OJ  */
    SQL_TOKEN_ASCII = 390,         /* SQL_TOKEN_ASCII  */
    SQL_TOKEN_BIT_LENGTH = 391,    /* SQL_TOKEN_BIT_LENGTH  */
    SQL_TOKEN_CHAR = 392,          /* SQL_TOKEN_CHAR  */
    SQL_TOKEN_CHAR_LENGTH = 393,   /* SQL_TOKEN_CHAR_LENGTH  */
    SQL_TOKEN_SQL_TOKEN_INTNUM = 394, /* SQL_TOKEN_SQL_TOKEN_INTNUM  */
    SQL_TOKEN_CONCAT = 395,        /* SQL_TOKEN_CONCAT  */
    SQL_TOKEN_DIFFERENCE = 396,    /* SQL_TOKEN_DIFFERENCE  */
    SQL_TOKEN_INSERT = 397,        /* SQL_TOKEN_INSERT  */
    SQL_TOKEN_LCASE = 398,         /* SQL_TOKEN_LCASE  */
    SQL_TOKEN_LEFT = 399,          /* SQL_TOKEN_LEFT  */
    SQL_TOKEN_LENGTH = 400,        /* SQL_TOKEN_LENGTH  */
    SQL_TOKEN_LOCATE = 401,        /* SQL_TOKEN_LOCATE  */
    SQL_TOKEN_LOCATE_2 = 402,      /* SQL_TOKEN_LOCATE_2  */
    SQL_TOKEN_LTRIM = 403,         /* SQL_TOKEN_LTRIM  */
    SQL_TOKEN_POSITION = 404,      /* SQL_TOKEN_POSITION  */
    SQL_TOKEN_REPEAT = 405,        /* SQL_TOKEN_REPEAT  */
    SQL_TOKEN_REPLACE = 406,       /* SQL_TOKEN_REPLACE  */
    SQL_TOKEN_RIGHT = 407,         /* SQL_TOKEN_RIGHT  */
    SQL_TOKEN_RTRIM = 408,         /* SQL_TOKEN_RTRIM  */
    SQL_TOKEN_SOUNDEX = 409,       /* SQL_TOKEN_SOUNDEX  */
    SQL_TOKEN_SPACE = 410,         /* SQL_TOKEN_SPACE  */
    SQL_TOKEN_SUBSTRING = 411,     /* SQL_TOKEN_SUBSTRING  */
    SQL_TOKEN_UCASE = 412,         /* SQL_TOKEN_UCASE  */
    SQL_TOKEN_CURRENT_DATE = 413,  /* SQL_TOKEN_CURRENT_DATE  */
    SQL_TOKEN_CURRENT_TIME = 414,  /* SQL_TOKEN_CURRENT_TIME  */
    SQL_TOKEN_CURRENT_TIMESTAMP = 415, /* SQL_TOKEN_CURRENT_TIMESTAMP  */
    SQL_TOKEN_CURDATE = 416,       /* SQL_TOKEN_CURDATE  */
    SQL_TOKEN_CURTIME = 417,       /* SQL_TOKEN_CURTIME  */
    SQL_TOKEN_DAYNAME = 418,       /* SQL_TOKEN_DAYNAME  */
    SQL_TOKEN_DAYOFMONTH = 419,    /* SQL_TOKEN_DAYOFMONTH  */
    SQL_TOKEN_DAYOFWEEK = 420,     /* SQL_TOKEN_DAYOFWEEK  */
    SQL_TOKEN_DAYOFYEAR = 421,     /* SQL_TOKEN_DAYOFYEAR  */
    SQL_TOKEN_EXTRACT = 422,       /* SQL_TOKEN_EXTRACT  */
    SQL_TOKEN_HOUR = 423,          /* SQL_TOKEN_HOUR  */
    SQL_TOKEN_MINUTE = 424,        /* SQL_TOKEN_MINUTE  */
    SQL_TOKEN_MONTH = 425,         /* SQL_TOKEN_MONTH  */
    SQL_TOKEN_MONTHNAME = 426,     /* SQL_TOKEN_MONTHNAME  */
    SQL_TOKEN_NOW = 427,           /* SQL_TOKEN_NOW  */
    SQL_TOKEN_QUARTER = 428,       /* SQL_TOKEN_QUARTER  */
    SQL_TOKEN_DATEDIFF = 429,      /* SQL_TOKEN_DATEDIFF  */
    SQL_TOKEN_SECOND = 430,        /* SQL_TOKEN_SECOND  */
    SQL_TOKEN_TIMESTAMPADD = 431,  /* SQL_TOKEN_TIMESTAMPADD  */
    SQL_TOKEN_TIMESTAMPDIFF = 432, /* SQL_TOKEN_TIMESTAMPDIFF  */
    SQL_TOKEN_TIMEVALUE = 433,     /* SQL_TOKEN_TIMEVALUE  */
    SQL_TOKEN_WEEK = 434,          /* SQL_TOKEN_WEEK  */
    SQL_TOKEN_YEAR = 435,          /* SQL_TOKEN_YEAR  */
    SQL_TOKEN_ABS = 436,           /* SQL_TOKEN_ABS  */
    SQL_TOKEN_ACOS = 437,          /* SQL_TOKEN_ACOS  */
    SQL_TOKEN_ASIN = 438,          /* SQL_TOKEN_ASIN  */
    SQL_TOKEN_ATAN = 439,          /* SQL_TOKEN_ATAN  */
    SQL_TOKEN_ATAN2 = 440,         /* SQL_TOKEN_ATAN2  */
    SQL_TOKEN_CEILING = 441,       /* SQL_TOKEN_CEILING  */
    SQL_TOKEN_COS = 442,           /* SQL_TOKEN_COS  */
    SQL_TOKEN_COT = 443,           /* SQL_TOKEN_COT  */
    SQL_TOKEN_DEGREES = 444,       /* SQL_TOKEN_DEGREES  */
    SQL_TOKEN_EXP = 445,           /* SQL_TOKEN_EXP  */
    SQL_TOKEN_FLOOR = 446,         /* SQL_TOKEN_FLOOR  */
    SQL_TOKEN_LOGF = 447,          /* SQL_TOKEN_LOGF  */
    SQL_TOKEN_LOG = 448,           /* SQL_TOKEN_LOG  */
    SQL_TOKEN_LN = 449,            /* SQL_TOKEN_LN  */
    SQL_TOKEN_LOG10 = 450,         /* SQL_TOKEN_LOG10  */
    SQL_TOKEN_MOD = 451,           /* SQL_TOKEN_MOD  */
    SQL_TOKEN_PI = 452,            /* SQL_TOKEN_PI  */
    SQL_TOKEN_POWER = 453,         /* SQL_TOKEN_POWER  */
    SQL_TOKEN_RADIANS = 454,       /* SQL_TOKEN_RADIANS  */
    SQL_TOKEN_RAND = 455,          /* SQL_TOKEN_RAND  */
    SQL_TOKEN_ROUNDMAGIC = 456,    /* SQL_TOKEN_ROUNDMAGIC  */
    SQL_TOKEN_ROUND = 457,         /* SQL_TOKEN_ROUND  */
    SQL_TOKEN_SIGN = 458,          /* SQL_TOKEN_SIGN  */
    SQL_TOKEN_SIN = 459,           /* SQL_TOKEN_SIN  */
    SQL_TOKEN_SQRT = 460,          /* SQL_TOKEN_SQRT  */
    SQL_TOKEN_TAN = 461,           /* SQL_TOKEN_TAN  */
    SQL_TOKEN_TRUNCATE = 462,      /* SQL_TOKEN_TRUNCATE  */
    SQL_TOKEN_EVERY = 463,         /* SQL_TOKEN_EVERY  */
    SQL_TOKEN_INTERSECTION = 464,  /* SQL_TOKEN_INTERSECTION  */
    SQL_TOKEN_FUSION = 465,        /* SQL_TOKEN_FUSION  */
    SQL_TOKEN_COLLECT = 466,       /* SQL_TOKEN_COLLECT  */
    SQL_TOKEN_VAR_POP = 467,       /* SQL_TOKEN_VAR_POP  */
    SQL_TOKEN_VAR_SAMP = 468,      /* SQL_TOKEN_VAR_SAMP  */
    SQL_TOKEN_STDDEV_SAMP = 469,   /* SQL_TOKEN_STDDEV_SAMP  */
    SQL_TOKEN_STDDEV_POP = 470,    /* SQL_TOKEN_STDDEV_POP  */
    SQL_TOKEN_RANK = 471,          /* SQL_TOKEN_RANK  */
    SQL_TOKEN_DENSE_RANK = 472,    /* SQL_TOKEN_DENSE_RANK  */
    SQL_TOKEN_PERCENT_RANK = 473,  /* SQL_TOKEN_PERCENT_RANK  */
    SQL_TOKEN_CUME_DIST = 474,     /* SQL_TOKEN_CUME_DIST  */
    SQL_TOKEN_PERCENTILE_CONT = 475, /* SQL_TOKEN_PERCENTILE_CONT  */
    SQL_TOKEN_PERCENTILE_DISC = 476, /* SQL_TOKEN_PERCENTILE_DISC  */
    SQL_TOKEN_WITHIN = 477,        /* SQL_TOKEN_WITHIN  */
    SQL_TOKEN_ARRAY_AGG = 478,     /* SQL_TOKEN_ARRAY_AGG  */
    SQL_TOKEN_CASE = 479,          /* SQL_TOKEN_CASE  */
    SQL_TOKEN_THEN = 480,          /* SQL_TOKEN_THEN  */
    SQL_TOKEN_END = 481,           /* SQL_TOKEN_END  */
    SQL_TOKEN_NULLIF = 482,        /* SQL_TOKEN_NULLIF  */
    SQL_TOKEN_COALESCE = 483,      /* SQL_TOKEN_COALESCE  */
    SQL_TOKEN_WHEN = 484,          /* SQL_TOKEN_WHEN  */
    SQL_TOKEN_ELSE = 485,          /* SQL_TOKEN_ELSE  */
    SQL_TOKEN_BEFORE = 486,        /* SQL_TOKEN_BEFORE  */
    SQL_TOKEN_AFTER = 487,         /* SQL_TOKEN_AFTER  */
    SQL_TOKEN_INSTEAD = 488,       /* SQL_TOKEN_INSTEAD  */
    SQL_TOKEN_EACH = 489,          /* SQL_TOKEN_EACH  */
    SQL_TOKEN_REFERENCING = 490,   /* SQL_TOKEN_REFERENCING  */
    SQL_TOKEN_BEGIN = 491,         /* SQL_TOKEN_BEGIN  */
    SQL_TOKEN_ATOMIC = 492,        /* SQL_TOKEN_ATOMIC  */
    SQL_TOKEN_TRIGGER = 493,       /* SQL_TOKEN_TRIGGER  */
    SQL_TOKEN_ROW = 494,           /* SQL_TOKEN_ROW  */
    SQL_TOKEN_STATEMENT = 495,     /* SQL_TOKEN_STATEMENT  */
    SQL_TOKEN_NEW = 496,           /* SQL_TOKEN_NEW  */
    SQL_TOKEN_OLD = 497,           /* SQL_TOKEN_OLD  */
    SQL_TOKEN_VALUE = 498,         /* SQL_TOKEN_VALUE  */
    SQL_TOKEN_CURRENT_CATALOG = 499, /* SQL_TOKEN_CURRENT_CATALOG  */
    SQL_TOKEN_CURRENT_DEFAULT_TRANSFORM_GROUP = 500, /* SQL_TOKEN_CURRENT_DEFAULT_TRANSFORM_GROUP  */
    SQL_TOKEN_CURRENT_PATH = 501,  /* SQL_TOKEN_CURRENT_PATH  */
    SQL_TOKEN_CURRENT_ROLE = 502,  /* SQL_TOKEN_CURRENT_ROLE  */
    SQL_TOKEN_CURRENT_SCHEMA = 503, /* SQL_TOKEN_CURRENT_SCHEMA  */
    SQL_TOKEN_CURRENT_USER = 504,  /* SQL_TOKEN_CURRENT_USER  */
    SQL_TOKEN_SESSION_USER = 505,  /* SQL_TOKEN_SESSION_USER  */
    SQL_TOKEN_SYSTEM_USER = 506,   /* SQL_TOKEN_SYSTEM_USER  */
    SQL_TOKEN_VARCHAR = 507,       /* SQL_TOKEN_VARCHAR  */
    SQL_TOKEN_VARBINARY = 508,     /* SQL_TOKEN_VARBINARY  */
    SQL_TOKEN_VARYING = 509,       /* SQL_TOKEN_VARYING  */
    SQL_TOKEN_OBJECT = 510,        /* SQL_TOKEN_OBJECT  */
    SQL_TOKEN_NCLOB = 511,         /* SQL_TOKEN_NCLOB  */
    SQL_TOKEN_NATIONAL = 512,      /* SQL_TOKEN_NATIONAL  */
    SQL_TOKEN_LARGE = 513,         /* SQL_TOKEN_LARGE  */
    SQL_TOKEN_CLOB = 514,          /* SQL_TOKEN_CLOB  */
    SQL_TOKEN_BLOB = 515,          /* SQL_TOKEN_BLOB  */
    SQL_TOKEN_BIGINT = 516,        /* SQL_TOKEN_BIGINT  */
    SQL_TOKEN_BINARY = 517,        /* SQL_TOKEN_BINARY  */
    SQL_TOKEN_WITHOUT = 518,       /* SQL_TOKEN_WITHOUT  */
    SQL_TOKEN_BOOLEAN = 519,       /* SQL_TOKEN_BOOLEAN  */
    SQL_TOKEN_INTERVAL = 520,      /* SQL_TOKEN_INTERVAL  */
    SQL_TOKEN_OVER = 521,          /* SQL_TOKEN_OVER  */
    SQL_TOKEN_ROW_NUMBER = 522,    /* SQL_TOKEN_ROW_NUMBER  */
    SQL_TOKEN_NTILE = 523,         /* SQL_TOKEN_NTILE  */
    SQL_TOKEN_LEAD = 524,          /* SQL_TOKEN_LEAD  */
    SQL_TOKEN_LAG = 525,           /* SQL_TOKEN_LAG  */
    SQL_TOKEN_RESPECT = 526,       /* SQL_TOKEN_RESPECT  */
    SQL_TOKEN_IGNORE = 527,        /* SQL_TOKEN_IGNORE  */
    SQL_TOKEN_NULLS = 528,         /* SQL_TOKEN_NULLS  */
    SQL_TOKEN_FIRST_VALUE = 529,   /* SQL_TOKEN_FIRST_VALUE  */
    SQL_TOKEN_LAST_VALUE = 530,    /* SQL_TOKEN_LAST_VALUE  */
    SQL_TOKEN_NTH_VALUE = 531,     /* SQL_TOKEN_NTH_VALUE  */
    SQL_TOKEN_FIRST = 532,         /* SQL_TOKEN_FIRST  */
    SQL_TOKEN_LAST = 533,          /* SQL_TOKEN_LAST  */
    SQL_TOKEN_EXCLUDE = 534,       /* SQL_TOKEN_EXCLUDE  */
    SQL_TOKEN_OTHERS = 535,        /* SQL_TOKEN_OTHERS  */
    SQL_TOKEN_TIES = 536,          /* SQL_TOKEN_TIES  */
    SQL_TOKEN_FOLLOWING = 537,     /* SQL_TOKEN_FOLLOWING  */
    SQL_TOKEN_UNBOUNDED = 538,     /* SQL_TOKEN_UNBOUNDED  */
    SQL_TOKEN_PRECEDING = 539,     /* SQL_TOKEN_PRECEDING  */
    SQL_TOKEN_RANGE = 540,         /* SQL_TOKEN_RANGE  */
    SQL_TOKEN_ROWS = 541,          /* SQL_TOKEN_ROWS  */
    SQL_TOKEN_PARTITION = 542,     /* SQL_TOKEN_PARTITION  */
    SQL_TOKEN_WINDOW = 543,        /* SQL_TOKEN_WINDOW  */
    SQL_TOKEN_NO = 544,            /* SQL_TOKEN_NO  */
    SQL_TOKEN_LIMIT = 545,         /* SQL_TOKEN_LIMIT  */
    SQL_TOKEN_OFFSET = 546,        /* SQL_TOKEN_OFFSET  */
    SQL_TOKEN_NEXT = 547,          /* SQL_TOKEN_NEXT  */
    SQL_TOKEN_ONLY = 548,          /* SQL_TOKEN_ONLY  */
    SQL_TOKEN_OR = 549,            /* SQL_TOKEN_OR  */
    SQL_TOKEN_AND = 550,           /* SQL_TOKEN_AND  */
    SQL_LESSEQ = 551,              /* SQL_LESSEQ  */
    SQL_GREATEQ = 552,             /* SQL_GREATEQ  */
    SQL_NOTEQUAL = 553,            /* SQL_NOTEQUAL  */
    SQL_LESS = 554,                /* SQL_LESS  */
    SQL_GREAT = 555,               /* SQL_GREAT  */
    SQL_EQUAL = 556,               /* SQL_EQUAL  */
    SQL_CONCAT = 557,              /* SQL_CONCAT  */
    SQL_TOKEN_INVALIDSYMBOL = 558  /* SQL_TOKEN_INVALIDSYMBOL  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{

	connectivity::OSQLParseNode * pParseNode;


};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE SQLyylval;


int SQLyyparse (void);


#endif /* !YY_SQLYY_SQLBISON_HXX_INCLUDED  */
