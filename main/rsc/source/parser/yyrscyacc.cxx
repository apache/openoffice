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




/* First part of user prologue.  */
#line 24 "yyrscyacc.y"

#if defined __GNUC__
#pragma GCC system_header
#elif defined __SUNPRO_CC
#pragma disable_warn
#endif

#line 79 "yyrscyacc.cxx"

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

#include "yyrscyacc.hxx"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_NUMBER = 3,                     /* NUMBER  */
  YYSYMBOL_SYMBOL = 4,                     /* SYMBOL  */
  YYSYMBOL_RSCDEFINE = 5,                  /* RSCDEFINE  */
  YYSYMBOL_STRING = 6,                     /* STRING  */
  YYSYMBOL_INCLUDE_STRING = 7,             /* INCLUDE_STRING  */
  YYSYMBOL_CHARACTER = 8,                  /* CHARACTER  */
  YYSYMBOL_BOOLEAN = 9,                    /* BOOLEAN  */
  YYSYMBOL_LINE = 10,                      /* LINE  */
  YYSYMBOL_AUTO_ID = 11,                   /* AUTO_ID  */
  YYSYMBOL_NOT = 12,                       /* NOT  */
  YYSYMBOL_XSCALE = 13,                    /* XSCALE  */
  YYSYMBOL_YSCALE = 14,                    /* YSCALE  */
  YYSYMBOL_RGB = 15,                       /* RGB  */
  YYSYMBOL_GEOMETRY = 16,                  /* GEOMETRY  */
  YYSYMBOL_POSITION = 17,                  /* POSITION  */
  YYSYMBOL_DIMENSION = 18,                 /* DIMENSION  */
  YYSYMBOL_INZOOMOUTPUTSIZE = 19,          /* INZOOMOUTPUTSIZE  */
  YYSYMBOL_FLOATINGPOS = 20,               /* FLOATINGPOS  */
  YYSYMBOL_DEFINE = 21,                    /* DEFINE  */
  YYSYMBOL_INCLUDE = 22,                   /* INCLUDE  */
  YYSYMBOL_MACROTARGET = 23,               /* MACROTARGET  */
  YYSYMBOL_DEFAULT = 24,                   /* DEFAULT  */
  YYSYMBOL_CLASSNAME = 25,                 /* CLASSNAME  */
  YYSYMBOL_VARNAME = 26,                   /* VARNAME  */
  YYSYMBOL_CONSTNAME = 27,                 /* CONSTNAME  */
  YYSYMBOL_CLASS = 28,                     /* CLASS  */
  YYSYMBOL_EXTENDABLE = 29,                /* EXTENDABLE  */
  YYSYMBOL_WRITEIFSET = 30,                /* WRITEIFSET  */
  YYSYMBOL_31_ = 31,                       /* '|'  */
  YYSYMBOL_32_ = 32,                       /* '&'  */
  YYSYMBOL_LEFTSHIFT = 33,                 /* LEFTSHIFT  */
  YYSYMBOL_RIGHTSHIFT = 34,                /* RIGHTSHIFT  */
  YYSYMBOL_35_ = 35,                       /* '+'  */
  YYSYMBOL_36_ = 36,                       /* '-'  */
  YYSYMBOL_37_ = 37,                       /* '*'  */
  YYSYMBOL_38_ = 38,                       /* '/'  */
  YYSYMBOL_UNARYMINUS = 39,                /* UNARYMINUS  */
  YYSYMBOL_UNARYPLUS = 40,                 /* UNARYPLUS  */
  YYSYMBOL_41_ = 41,                       /* ','  */
  YYSYMBOL_42_ = 42,                       /* '('  */
  YYSYMBOL_43_ = 43,                       /* ')'  */
  YYSYMBOL_44_ = 44,                       /* '#'  */
  YYSYMBOL_45_ = 45,                       /* ';'  */
  YYSYMBOL_46_ = 46,                       /* '{'  */
  YYSYMBOL_47_ = 47,                       /* '}'  */
  YYSYMBOL_48_ = 48,                       /* ':'  */
  YYSYMBOL_49_ = 49,                       /* '['  */
  YYSYMBOL_50_ = 50,                       /* ']'  */
  YYSYMBOL_51_ = 51,                       /* '<'  */
  YYSYMBOL_52_ = 52,                       /* '='  */
  YYSYMBOL_53_ = 53,                       /* '>'  */
  YYSYMBOL_YYACCEPT = 54,                  /* $accept  */
  YYSYMBOL_resource_definitions = 55,      /* resource_definitions  */
  YYSYMBOL_resource_definition = 56,       /* resource_definition  */
  YYSYMBOL_new_class_definition_header = 57, /* new_class_definition_header  */
  YYSYMBOL_new_class_definition_body = 58, /* new_class_definition_body  */
  YYSYMBOL_property_definition = 59,       /* property_definition  */
  YYSYMBOL_type_flags = 60,                /* type_flags  */
  YYSYMBOL_type = 61,                      /* type  */
  YYSYMBOL_type_base = 62,                 /* type_base  */
  YYSYMBOL_class_definition = 63,          /* class_definition  */
  YYSYMBOL_class_header = 64,              /* class_header  */
  YYSYMBOL_copy_ref = 65,                  /* copy_ref  */
  YYSYMBOL_class_header_body = 66,         /* class_header_body  */
  YYSYMBOL_class_body = 67,                /* class_body  */
  YYSYMBOL_var_definitions = 68,           /* var_definitions  */
  YYSYMBOL_xy_mapmode = 69,                /* xy_mapmode  */
  YYSYMBOL_wh_mapmode = 70,                /* wh_mapmode  */
  YYSYMBOL_xywh_mapmode = 71,              /* xywh_mapmode  */
  YYSYMBOL_var_definition = 72,            /* var_definition  */
  YYSYMBOL_var_header_class = 73,          /* var_header_class  */
  YYSYMBOL_var_header = 74,                /* var_header  */
  YYSYMBOL_tupel_header0 = 75,             /* tupel_header0  */
  YYSYMBOL_tupel_header1 = 76,             /* tupel_header1  */
  YYSYMBOL_tupel_header2 = 77,             /* tupel_header2  */
  YYSYMBOL_tupel_header3 = 78,             /* tupel_header3  */
  YYSYMBOL_tupel_body = 79,                /* tupel_body  */
  YYSYMBOL_var_list_header = 80,           /* var_list_header  */
  YYSYMBOL_list_body = 81,                 /* list_body  */
  YYSYMBOL_list_header = 82,               /* list_header  */
  YYSYMBOL_list = 83,                      /* list  */
  YYSYMBOL_var_bodysimple = 84,            /* var_bodysimple  */
  YYSYMBOL_var_bodycomplex = 85,           /* var_bodycomplex  */
  YYSYMBOL_var_body = 86,                  /* var_body  */
  YYSYMBOL_string_multiline = 87,          /* string_multiline  */
  YYSYMBOL_long_expression = 88,           /* long_expression  */
  YYSYMBOL_macro_expression = 89,          /* macro_expression  */
  YYSYMBOL_id_expression = 90,             /* id_expression  */
  YYSYMBOL_DUMMY_NUMBER = 91,              /* DUMMY_NUMBER  */
  YYSYMBOL_line_number = 92                /* line_number  */
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
#define YYFINAL  9
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   321

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  54
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  39
/* YYNRULES -- Number of rules.  */
#define YYNRULES  117
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  259

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   289


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,    44,     2,     2,    32,     2,
      42,    43,    37,    35,    41,    36,     2,    38,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    48,    45,
      51,    52,    53,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    49,     2,    50,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    46,    31,    47,     2,     2,     2,     2,
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
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    33,    34,    39,    40
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   124,   124,   125,   126,   139,   140,   174,   178,   181,
     184,   192,   193,   197,   210,   218,   219,   223,   230,   238,
     242,   247,   253,   257,   279,   283,   293,   300,   316,   325,
     329,   336,   344,   352,   360,   368,   376,   386,   387,   388,
     395,   396,   400,   404,   408,   412,   416,   421,   425,   426,
     430,   431,   443,   459,   464,   469,   476,   484,   490,   496,
     503,   508,   515,   523,   542,   570,   602,   616,   640,   668,
     684,   700,   716,   730,   738,   755,   763,   773,   774,   775,
     776,   777,   781,   818,   835,   852,   869,   886,   890,   891,
     892,   893,   895,   900,   901,   906,   910,   920,   930,   935,
     940,   954,   958,   969,   980,   991,  1008,  1019,  1030,  1034,
    1045,  1059,  1060,  1077,  1081,  1086,  1096,  1106
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
  "\"end of file\"", "error", "\"invalid token\"", "NUMBER", "SYMBOL",
  "RSCDEFINE", "STRING", "INCLUDE_STRING", "CHARACTER", "BOOLEAN", "LINE",
  "AUTO_ID", "NOT", "XSCALE", "YSCALE", "RGB", "GEOMETRY", "POSITION",
  "DIMENSION", "INZOOMOUTPUTSIZE", "FLOATINGPOS", "DEFINE", "INCLUDE",
  "MACROTARGET", "DEFAULT", "CLASSNAME", "VARNAME", "CONSTNAME", "CLASS",
  "EXTENDABLE", "WRITEIFSET", "'|'", "'&'", "LEFTSHIFT", "RIGHTSHIFT",
  "'+'", "'-'", "'*'", "'/'", "UNARYMINUS", "UNARYPLUS", "','", "'('",
  "')'", "'#'", "';'", "'{'", "'}'", "':'", "'['", "']'", "'<'", "'='",
  "'>'", "$accept", "resource_definitions", "resource_definition",
  "new_class_definition_header", "new_class_definition_body",
  "property_definition", "type_flags", "type", "type_base",
  "class_definition", "class_header", "copy_ref", "class_header_body",
  "class_body", "var_definitions", "xy_mapmode", "wh_mapmode",
  "xywh_mapmode", "var_definition", "var_header_class", "var_header",
  "tupel_header0", "tupel_header1", "tupel_header2", "tupel_header3",
  "tupel_body", "var_list_header", "list_body", "list_header", "list",
  "var_bodysimple", "var_bodycomplex", "var_body", "string_multiline",
  "long_expression", "macro_expression", "id_expression", "DUMMY_NUMBER",
  "line_number", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-138)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-16)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
      -3,    11,    13,  -138,  -138,    11,    11,    11,   237,  -138,
     102,    26,    89,  -138,    87,   -16,     9,  -138,  -138,  -138,
    -138,   224,    11,    11,    11,    11,    11,    11,    11,    11,
    -138,  -138,   159,   237,    44,    11,    11,    42,    36,   131,
     134,  -138,    49,  -138,  -138,   192,  -138,    77,  -138,   244,
     250,    90,    90,   108,   108,  -138,  -138,    11,    54,    69,
     162,  -138,    16,    72,   111,   115,    11,    11,  -138,  -138,
      82,    97,    33,    95,    98,   104,   105,   107,   109,   114,
     116,    70,  -138,   118,   207,  -138,     4,    88,  -138,  -138,
      54,    11,    54,   129,   144,  -138,  -138,  -138,   237,   237,
     127,    49,  -138,  -138,  -138,  -138,    14,   125,   133,   137,
     138,   156,   158,   164,   -10,    34,    15,   161,  -138,  -138,
    -138,  -138,   147,  -138,   169,  -138,  -138,  -138,  -138,  -138,
    -138,   154,    77,   237,    54,  -138,  -138,  -138,  -138,  -138,
    -138,   139,    11,    11,    11,  -138,   160,  -138,   171,  -138,
     173,   174,    11,   177,    11,   150,   153,  -138,  -138,  -138,
    -138,    88,  -138,  -138,   187,   237,   188,   189,    11,    11,
      11,    11,   190,    11,   193,   183,   185,   146,   195,  -138,
      11,    11,    11,   197,   200,   201,   202,    11,   203,    11,
     161,   161,  -138,   204,    62,   205,  -138,   194,   209,   220,
     223,    11,    11,    11,    11,   222,    11,   246,  -138,  -138,
    -138,   221,  -138,  -138,  -138,    88,   245,   247,    11,   252,
     248,   251,   253,   254,   255,   256,  -138,   257,  -138,  -138,
     260,    11,   259,   261,   262,  -138,   263,  -138,   242,   264,
     269,  -138,  -138,  -138,  -138,  -138,    88,  -138,    11,   266,
     270,   265,   267,  -138,    88,  -138,   271,   268,  -138
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       2,     0,     0,    99,    98,     0,     0,     0,     4,     1,
      36,     0,     0,     3,     0,     0,    27,    28,     5,   101,
     100,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      30,    29,     0,   112,    33,     0,     0,   117,     0,     0,
       0,    12,    21,    10,    95,     0,    26,    39,   108,   107,
     106,   109,   110,   102,   103,   104,   105,     0,    34,     0,
       0,   111,     0,     0,   114,     0,     0,     0,     8,     9,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    38,     0,     0,    40,     0,     0,    48,    96,
      35,     0,    32,     0,     0,   113,   116,   115,     6,     7,
       0,    21,    25,    24,    19,    20,     0,    22,     0,     0,
       0,    47,    43,    45,     0,     0,     0,    66,    50,    37,
      41,    52,     0,    85,     0,    87,    83,    76,    69,    93,
      94,     0,    86,    82,    31,    13,    14,    11,    16,    17,
      18,     0,     0,     0,     0,    46,     0,    42,     0,    44,
       0,     0,     0,     0,     0,     0,     0,    63,    51,    84,
      81,     0,    49,    23,     0,    97,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    74,     0,    73,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      68,    67,    88,     0,     0,     0,    80,    70,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    65,    64,
      79,     0,    75,    78,    89,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    77,     0,    53,    54,
       0,     0,     0,     0,     0,    60,     0,    62,    71,     0,
       0,    57,    58,    59,    61,    90,     0,    55,     0,     0,
       0,    72,     0,    91,     0,    56,     0,     0,    92
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -138,  -138,  -138,  -138,   145,  -138,  -138,  -138,  -138,     1,
    -138,   214,  -116,   211,  -138,  -138,  -138,  -138,   216,  -138,
    -138,  -138,  -138,  -138,  -138,  -137,  -138,  -138,  -138,  -138,
     128,   120,   228,    -9,  -100,    -1,   -24,  -138,     0
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_uint8 yydefgoto[] =
{
       0,     2,    13,    14,    70,    71,    72,   106,   107,    83,
      16,    32,    17,    46,    84,   148,   150,   146,    85,    86,
      87,   161,   215,   246,   254,   178,   194,   211,   160,   177,
     129,   130,   179,   132,   164,   165,    34,    96,    61
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
       8,   157,    18,    15,    19,    20,    21,    47,    58,    33,
      44,    62,    63,     9,     3,    44,     4,   151,   139,   155,
       1,    49,    50,    51,    52,    53,    54,    55,    56,    43,
      35,    33,   152,    90,    33,    33,    92,   102,    10,    65,
     140,    11,   156,   166,   167,    88,     5,     6,    64,   121,
      45,    36,   172,     7,   174,    45,    33,    12,   103,    33,
      59,   153,   104,   105,    93,    98,    99,   134,   183,   184,
     185,   186,    37,   188,   208,   209,   154,    47,   227,    38,
     198,   199,   200,    89,    88,    30,   133,   205,    59,   207,
      33,     3,    37,     4,    44,    31,   -15,   123,    59,    38,
     124,   219,   220,   221,   222,     3,   224,     4,   127,   249,
      39,    40,   125,   128,    95,   126,    59,   256,   230,   116,
      94,    97,   117,     5,     6,    26,    27,    28,    29,   100,
       7,   240,    41,    42,   127,    66,    67,     5,     6,   128,
      68,    69,   101,    30,     7,    28,    29,   108,   250,     3,
     109,     4,    44,    31,   135,   123,   110,   111,   124,   112,
     133,   113,     3,   118,     4,     3,   114,     4,   115,   136,
     125,    10,   137,   126,   141,   142,   133,   196,   193,   143,
     144,     5,     6,   145,    57,   147,    10,    91,     7,   163,
      59,   149,   158,   192,     5,     6,   159,     5,     6,   162,
     175,     7,   168,   176,     7,    73,    74,    75,    76,    77,
      78,    79,    80,   169,   133,   170,   171,    10,    81,   173,
      73,    74,    75,    76,    77,    78,    79,    80,   180,   181,
     182,   187,    10,    81,   189,   190,    59,   191,   201,    82,
     197,   202,   203,   204,   206,   133,   138,   214,    60,   210,
     213,    59,   216,   133,   119,    22,    23,    24,    25,    26,
      27,    28,    29,   217,   218,   223,   226,    48,    22,    23,
      24,    25,    26,    27,    28,    29,    23,    24,    25,    26,
      27,    28,    29,    24,    25,    26,    27,    28,    29,   225,
     228,   232,   229,   231,   233,   245,   234,   122,   236,   235,
     120,   237,   238,   239,   241,   195,   242,   243,   244,   247,
     248,   251,   255,   252,   212,   131,   257,     0,   253,     0,
       0,   258
};

static const yytype_int16 yycheck[] =
{
       1,   117,     2,     2,     5,     6,     7,    16,    32,    10,
       6,    35,    36,     0,     3,     6,     5,    27,     4,     4,
      23,    22,    23,    24,    25,    26,    27,    28,    29,    45,
       4,    32,    42,    57,    35,    36,    60,     4,    25,     3,
      26,    28,    27,   143,   144,    45,    35,    36,     6,    45,
      46,    25,   152,    42,   154,    46,    57,    44,    25,    60,
      44,    27,    29,    30,    48,    66,    67,    91,   168,   169,
     170,   171,     3,   173,   190,   191,    42,    86,   215,    10,
     180,   181,   182,     6,    84,    41,    87,   187,    44,   189,
      91,     3,     3,     5,     6,    51,    47,     9,    44,    10,
      12,   201,   202,   203,   204,     3,   206,     5,    46,   246,
      21,    22,    24,    51,     3,    27,    44,   254,   218,    49,
      48,     6,    52,    35,    36,    35,    36,    37,    38,    47,
      42,   231,    45,    46,    46,     4,     5,    35,    36,    51,
       6,     7,    45,    41,    42,    37,    38,    52,   248,     3,
      52,     5,     6,    51,    25,     9,    52,    52,    12,    52,
     161,    52,     3,    45,     5,     3,    52,     5,    52,    25,
      24,    25,    45,    27,    49,    42,   177,   177,   177,    42,
      42,    35,    36,    27,    25,    27,    25,    25,    42,    50,
      44,    27,    45,    47,    35,    36,    27,    35,    36,    45,
      50,    42,    42,    50,    42,    13,    14,    15,    16,    17,
      18,    19,    20,    42,   215,    42,    42,    25,    26,    42,
      13,    14,    15,    16,    17,    18,    19,    20,    41,    41,
      41,    41,    25,    26,    41,    52,    44,    52,    41,    47,
      45,    41,    41,    41,    41,   246,   101,    53,    34,    45,
      45,    44,    43,   254,    47,    31,    32,    33,    34,    35,
      36,    37,    38,    43,    41,    43,    45,    43,    31,    32,
      33,    34,    35,    36,    37,    38,    32,    33,    34,    35,
      36,    37,    38,    33,    34,    35,    36,    37,    38,    43,
      45,    43,    45,    41,    43,    53,    43,    86,    43,    45,
      84,    45,    45,    43,    45,   177,    45,    45,    45,    45,
      41,    45,    45,    43,   194,    87,    45,    -1,    53,    -1,
      -1,    53
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,    23,    55,     3,     5,    35,    36,    42,    89,     0,
      25,    28,    44,    56,    57,    63,    64,    66,    92,    89,
      89,    89,    31,    32,    33,    34,    35,    36,    37,    38,
      41,    51,    65,    89,    90,     4,    25,     3,    10,    21,
      22,    45,    46,    45,     6,    46,    67,    87,    43,    89,
      89,    89,    89,    89,    89,    89,    89,    25,    90,    44,
      65,    92,    90,    90,     6,     3,     4,     5,     6,     7,
      58,    59,    60,    13,    14,    15,    16,    17,    18,    19,
      20,    26,    47,    63,    68,    72,    73,    74,    92,     6,
      90,    25,    90,    48,    48,     3,    91,     6,    89,    89,
      47,    45,     4,    25,    29,    30,    61,    62,    52,    52,
      52,    52,    52,    52,    52,    52,    49,    52,    45,    47,
      72,    45,    67,     9,    12,    24,    27,    46,    51,    84,
      85,    86,    87,    89,    90,    25,    25,    45,    58,     4,
      26,    49,    42,    42,    42,    27,    71,    27,    69,    27,
      70,    27,    42,    27,    42,     4,    27,    66,    45,    27,
      82,    75,    45,    50,    88,    89,    88,    88,    42,    42,
      42,    42,    88,    42,    88,    50,    50,    83,    79,    86,
      41,    41,    41,    88,    88,    88,    88,    41,    88,    41,
      52,    52,    47,    63,    80,    84,    92,    45,    88,    88,
      88,    41,    41,    41,    41,    88,    41,    88,    66,    66,
      45,    81,    85,    45,    53,    76,    43,    43,    41,    88,
      88,    88,    88,    43,    88,    43,    45,    79,    45,    45,
      88,    41,    43,    43,    43,    45,    43,    45,    45,    43,
      88,    45,    45,    45,    45,    53,    77,    45,    41,    79,
      88,    45,    43,    53,    78,    45,    79,    45,    53
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    54,    55,    55,    55,    56,    56,    56,    56,    56,
      56,    56,    56,    57,    57,    58,    58,    59,    59,    60,
      60,    60,    61,    61,    62,    62,    63,    63,    64,    65,
      65,    66,    66,    66,    66,    66,    66,    67,    67,    67,
      68,    68,    69,    69,    70,    70,    71,    71,    72,    72,
      72,    72,    72,    72,    72,    72,    72,    72,    72,    72,
      72,    72,    72,    73,    73,    73,    74,    74,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    83,    83,
      83,    83,    84,    84,    84,    84,    84,    84,    85,    85,
      85,    85,    85,    86,    86,    87,    87,    88,    89,    89,
      89,    89,    89,    89,    89,    89,    89,    89,    89,    89,
      89,    90,    90,    91,    91,    92,    92,    92
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     0,     2,     2,     1,     4,     4,     3,     3,
       2,     5,     2,     5,     5,     0,     3,     3,     3,     2,
       2,     0,     1,     3,     1,     1,     2,     1,     1,     1,
       1,     5,     4,     2,     3,     4,     1,     3,     2,     1,
       1,     2,     1,     0,     1,     0,     1,     0,     1,     3,
       2,     3,     2,     8,     8,    10,    13,     9,     9,     9,
       8,     9,     8,     3,     6,     6,     2,     5,     5,     0,
       0,     0,     0,     1,     0,     1,     0,     4,     3,     3,
       2,     0,     1,     1,     2,     1,     1,     1,     4,     5,
       8,    11,    14,     1,     1,     1,     2,     1,     1,     1,
       2,     2,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     2,     1,     1,     0,     4,     4,     2
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
  case 4: /* resource_definitions: MACROTARGET macro_expression  */
#line 127 "yyrscyacc.y"
        {
		RscExpType		aExpType;
		sal_Int32			lValue;

		aExpType.cType = RSCEXP_NOTHING;
		pExp = new RscExpression( aExpType, '+', (yyvsp[0].macrostruct) );
		if( !pExp->Evaluate( &lValue ) )
			pTC->pEH->Error( ERR_ZERODIVISION, NULL, RscId() );
	}
#line 1350 "yyrscyacc.cxx"
    break;

  case 6: /* resource_definition: '#' DEFINE SYMBOL macro_expression  */
#line 141 "yyrscyacc.y"
        {
		sal_Bool		bError = sal_False;

		if( (yyvsp[0].macrostruct).IsNumber() ){
			if( !pTC->aFileTab.NewDef( pFI->GetFileIndex(),
									   ByteString( (yyvsp[-1].string) ),
									   (yyvsp[0].macrostruct).GetLong(), LIST_APPEND ) )
				bError = sal_True;
		}
		else if( (yyvsp[0].macrostruct).IsDefinition() ){
			RscExpType		aExpType;
			RscExpression * pExpr;

			aExpType.cType = RSCEXP_NOTHING;
			aExpType.SetLong( 0 );
			aExpType.cType = RSCEXP_LONG;
			pExpr = new RscExpression( aExpType, '+', (yyvsp[0].macrostruct) );

			if( !pTC->aFileTab.NewDef( pFI->GetFileIndex(),
									   ByteString( (yyvsp[-1].string) ), pExpr, LIST_APPEND ) )
				bError = sal_True;
		}
		else if( (yyvsp[0].macrostruct).IsExpression() ){
			if( !pTC->aFileTab.NewDef( pFI->GetFileIndex(),
									   ByteString( (yyvsp[-1].string) ), (yyvsp[0].macrostruct).aExp.pExp,
									   LIST_APPEND ) )
				bError = sal_True;
		}

		if( bError ){
			pTC->pEH->Error( ERR_DECLAREDEFINE, NULL, RscId(), (yyvsp[-1].string) );
		}
	}
#line 1388 "yyrscyacc.cxx"
    break;

  case 7: /* resource_definition: '#' DEFINE RSCDEFINE macro_expression  */
#line 175 "yyrscyacc.y"
        {
		pTC->pEH->Error( ERR_DOUBLEDEFINE, NULL, RscId(), (yyvsp[-1].defineele)->GetName().GetBuffer() );
	}
#line 1396 "yyrscyacc.cxx"
    break;

  case 8: /* resource_definition: '#' INCLUDE STRING  */
#line 179 "yyrscyacc.y"
        {
	}
#line 1403 "yyrscyacc.cxx"
    break;

  case 9: /* resource_definition: '#' INCLUDE INCLUDE_STRING  */
#line 182 "yyrscyacc.y"
        {
	}
#line 1410 "yyrscyacc.cxx"
    break;

  case 10: /* resource_definition: class_definition ';'  */
#line 185 "yyrscyacc.y"
        {
#ifdef D40
			void * pMem;
			pMem = rtl_allocateMemory( 20000 );
			rtl_freeMemory( pMem );
#endif
	}
#line 1422 "yyrscyacc.cxx"
    break;

  case 13: /* new_class_definition_header: CLASS SYMBOL id_expression ':' CLASSNAME  */
#line 198 "yyrscyacc.y"
        {
		sal_Int32	lType;

		(yyvsp[-2].macrostruct).Evaluate( &lType );

		// Klasse anlegen
		Atom nId = pHS->getID( (yyvsp[-3].string) );
		pCurClass = new RscClass( nId, lType, (yyvsp[0].pClass) );
		nCurMask = 1;
		pTC->aNmTb.Put( nId, CLASSNAME, pCurClass );
		pTC->GetRoot()->Insert( pCurClass );
	}
#line 1439 "yyrscyacc.cxx"
    break;

  case 14: /* new_class_definition_header: CLASS CLASSNAME id_expression ':' CLASSNAME  */
#line 211 "yyrscyacc.y"
        {
		pCurClass = (yyvsp[-3].pClass);
		nCurMask = 1;
	}
#line 1448 "yyrscyacc.cxx"
    break;

  case 17: /* property_definition: type_flags type SYMBOL  */
#line 224 "yyrscyacc.y"
  {
	// Variable anlegen
	Atom nId = pTC->aNmTb.Put( (yyvsp[0].string), VARNAME );
	pCurClass->SetVariable( nId, (yyvsp[-1].pClass), NULL, (yyvsp[-2].ushort), nCurMask );
	nCurMask <<= 1;
  }
#line 1459 "yyrscyacc.cxx"
    break;

  case 18: /* property_definition: type_flags type VARNAME  */
#line 231 "yyrscyacc.y"
  {
	pCurClass->SetVariable( (yyvsp[0].varid), (yyvsp[-1].pClass), NULL, (yyvsp[-2].ushort), nCurMask );
	nCurMask <<= 1;
  }
#line 1468 "yyrscyacc.cxx"
    break;

  case 19: /* type_flags: type_flags EXTENDABLE  */
#line 239 "yyrscyacc.y"
  {
	(yyval.ushort) = (yyvsp[-1].ushort) | VAR_EXTENDABLE;
  }
#line 1476 "yyrscyacc.cxx"
    break;

  case 20: /* type_flags: type_flags WRITEIFSET  */
#line 243 "yyrscyacc.y"
  {
	(yyval.ushort) = (yyvsp[-1].ushort) | VAR_SVDYNAMIC;
  }
#line 1484 "yyrscyacc.cxx"
    break;

  case 21: /* type_flags: %empty  */
#line 247 "yyrscyacc.y"
  {
	(yyval.ushort) = 0;
  }
#line 1492 "yyrscyacc.cxx"
    break;

  case 22: /* type: type_base  */
#line 254 "yyrscyacc.y"
  {
	(yyval.pClass) = (yyvsp[0].pClass);
  }
#line 1500 "yyrscyacc.cxx"
    break;

  case 23: /* type: type_base '[' ']'  */
#line 258 "yyrscyacc.y"
  {
	if( (yyvsp[-2].pClass) )
	{
		ByteString aTypeName = pHS->getString( (yyvsp[-2].pClass)->GetId() );
		aTypeName += "[]";
		(yyval.pClass) = pTC->SearchType( pHS->getID( aTypeName.GetBuffer(), true ) );
		if( !(yyval.pClass) )
		{
			RscCont * pCont;
			pCont = new RscCont( pHS->getID( aTypeName.GetBuffer() ), RSC_NOTYPE );
			pCont->SetTypeClass( (yyvsp[-2].pClass) );
			pTC->InsertType( pCont );
			(yyval.pClass) = pCont;
		}
	}
	else
		(yyval.pClass) = NULL;
  }
#line 1523 "yyrscyacc.cxx"
    break;

  case 24: /* type_base: CLASSNAME  */
#line 280 "yyrscyacc.y"
  {
	(yyval.pClass) = (yyvsp[0].pClass);
  }
#line 1531 "yyrscyacc.cxx"
    break;

  case 25: /* type_base: SYMBOL  */
#line 284 "yyrscyacc.y"
  {
	RscTop * pType = pTC->SearchType( pHS->getID( (yyvsp[0].string), true ) );
	if( !pType )
		pTC->pEH->Error( ERR_NOTYPE, pCurClass, RscId() );
	(yyval.pClass) = pType;
  }
#line 1542 "yyrscyacc.cxx"
    break;

  case 26: /* class_definition: class_header class_body  */
#line 294 "yyrscyacc.y"
  {
	if( TYPE_REF == (yyvsp[-1].header).nTyp )
		pTC->pEH->Error( ERR_REFNOTALLOWED, S.Top().pClass,
						 RscId( (yyvsp[-1].header).nName1 ) );
	S.Pop();
  }
#line 1553 "yyrscyacc.cxx"
    break;

  case 27: /* class_definition: class_header  */
#line 301 "yyrscyacc.y"
  {
	ERRTYPE aError;
	RscId aRscId( (yyvsp[0].header).nName1 );

	if( TYPE_NOTHING == (yyvsp[0].header).nTyp && aRscId.IsId() )
		aError = S.Top().pClass->SetRef( S.Top(), aRscId );
	else if( TYPE_COPY == (yyvsp[0].header).nTyp )
		aError = ERR_COPYNOTALLOWED;
	if( aError.IsError() || aError.IsWarning() )
		pTC->pEH->Error( aError, S.Top().pClass, aRscId );
	S.Pop();
  }
#line 1570 "yyrscyacc.cxx"
    break;

  case 28: /* class_header: class_header_body  */
#line 317 "yyrscyacc.y"
        {
		if( !DoClassHeader( &(yyvsp[0].header), sal_False ) )
			return( ERR_ERROR );
		(yyval.header) = (yyvsp[0].header);
	}
#line 1580 "yyrscyacc.cxx"
    break;

  case 29: /* copy_ref: '<'  */
#line 326 "yyrscyacc.y"
        {
		(yyval.copyref) = TYPE_COPY;
	}
#line 1588 "yyrscyacc.cxx"
    break;

  case 30: /* copy_ref: ','  */
#line 330 "yyrscyacc.y"
        {
		(yyval.copyref) = TYPE_REF;
	}
#line 1596 "yyrscyacc.cxx"
    break;

  case 31: /* class_header_body: CLASSNAME id_expression copy_ref CLASSNAME id_expression  */
#line 337 "yyrscyacc.y"
        {
		(yyval.header).pClass = (yyvsp[-4].pClass);
		(yyval.header).nName1 = (yyvsp[-3].macrostruct);
		(yyval.header).nTyp = (yyvsp[-2].copyref);
		(yyval.header).pRefClass = (yyvsp[-1].pClass);
		(yyval.header).nName2 = (yyvsp[0].macrostruct);
	}
#line 1608 "yyrscyacc.cxx"
    break;

  case 32: /* class_header_body: CLASSNAME id_expression copy_ref id_expression  */
#line 345 "yyrscyacc.y"
        {
		(yyval.header).pClass = (yyvsp[-3].pClass);
		(yyval.header).nName1 = (yyvsp[-2].macrostruct);
		(yyval.header).nTyp = (yyvsp[-1].copyref);
		(yyval.header).pRefClass = NULL;
		(yyval.header).nName2 = (yyvsp[0].macrostruct);
	}
#line 1620 "yyrscyacc.cxx"
    break;

  case 33: /* class_header_body: CLASSNAME id_expression  */
#line 353 "yyrscyacc.y"
        {
		(yyval.header).pClass = (yyvsp[-1].pClass);
		(yyval.header).nName1 = (yyvsp[0].macrostruct);
		(yyval.header).nTyp = TYPE_NOTHING;
		(yyval.header).pRefClass = NULL;
		(yyval.header).nName2.cType = RSCEXP_NOTHING;
	}
#line 1632 "yyrscyacc.cxx"
    break;

  case 34: /* class_header_body: CLASSNAME copy_ref id_expression  */
#line 361 "yyrscyacc.y"
        {
		(yyval.header).pClass = (yyvsp[-2].pClass);
		(yyval.header).nName1.cType = RSCEXP_NOTHING;
		(yyval.header).nTyp = (yyvsp[-1].copyref);
		(yyval.header).pRefClass = NULL;
		(yyval.header).nName2 = (yyvsp[0].macrostruct);
	}
#line 1644 "yyrscyacc.cxx"
    break;

  case 35: /* class_header_body: CLASSNAME copy_ref CLASSNAME id_expression  */
#line 369 "yyrscyacc.y"
        {
		(yyval.header).pClass = (yyvsp[-3].pClass);
		(yyval.header).nName1.cType = RSCEXP_NOTHING;
		(yyval.header).nTyp = (yyvsp[-2].copyref);
		(yyval.header).pRefClass = (yyvsp[-1].pClass);
		(yyval.header).nName2 = (yyvsp[0].macrostruct);
	}
#line 1656 "yyrscyacc.cxx"
    break;

  case 36: /* class_header_body: CLASSNAME  */
#line 377 "yyrscyacc.y"
        {
		(yyval.header).pClass = (yyvsp[0].pClass);
		(yyval.header).nName1.cType = RSCEXP_NOTHING;
		(yyval.header).nTyp = TYPE_NOTHING;
		(yyval.header).nName2.cType = RSCEXP_NOTHING;
	}
#line 1667 "yyrscyacc.cxx"
    break;

  case 39: /* class_body: string_multiline  */
#line 389 "yyrscyacc.y"
        {
		SetString( S.Top(), "TEXT", (yyvsp[0].string) );
	}
#line 1675 "yyrscyacc.cxx"
    break;

  case 42: /* xy_mapmode: CONSTNAME  */
#line 401 "yyrscyacc.y"
        {
		SetConst( S.Top(), "_XYMAPMODE", (yyvsp[0].constname).hashid, (yyvsp[0].constname).nValue );
	}
#line 1683 "yyrscyacc.cxx"
    break;

  case 44: /* wh_mapmode: CONSTNAME  */
#line 409 "yyrscyacc.y"
        {
		SetConst( S.Top(), "_WHMAPMODE", (yyvsp[0].constname).hashid, (yyvsp[0].constname).nValue );
	}
#line 1691 "yyrscyacc.cxx"
    break;

  case 46: /* xywh_mapmode: CONSTNAME  */
#line 417 "yyrscyacc.y"
        {
		SetConst( S.Top(), "_XYMAPMODE", (yyvsp[0].constname).hashid, (yyvsp[0].constname).nValue );
		SetConst( S.Top(), "_WHMAPMODE", (yyvsp[0].constname).hashid, (yyvsp[0].constname).nValue );
	}
#line 1700 "yyrscyacc.cxx"
    break;

  case 49: /* var_definition: var_header var_body ';'  */
#line 427 "yyrscyacc.y"
        {
		S.Pop();
	}
#line 1708 "yyrscyacc.cxx"
    break;

  case 51: /* var_definition: var_header_class class_body ';'  */
#line 432 "yyrscyacc.y"
        {
		if( TYPE_REF == (yyvsp[-2].header).nTyp )
			pTC->pEH->Error( ERR_REFNOTALLOWED, S.Top().pClass,
							 RscId( (yyvsp[-2].header).nName1 ) );

		if( S.Top().pClass->GetCount( S.Top() ) )
			pTC->pEH->Error( WRN_SUBINMEMBER, S.Top().pClass,
							 RscId(  (yyvsp[-2].header).nName1 ) );

		S.Pop();
	}
#line 1724 "yyrscyacc.cxx"
    break;

  case 52: /* var_definition: var_header_class ';'  */
#line 444 "yyrscyacc.y"
        {
		ERRTYPE aError;
		RscId aRscId( (yyvsp[-1].header).nName1 );

		if( TYPE_NOTHING == (yyvsp[-1].header).nTyp && aRscId.IsId() )
			aError = S.Top().pClass->SetRef( S.Top(), aRscId );
		else if( TYPE_COPY == (yyvsp[-1].header).nTyp )
			aError = ERR_COPYNOTALLOWED;
		if( S.Top().pClass->GetCount( S.Top() ) )
			aError = WRN_SUBINMEMBER;
		if( aError.IsError() || aError.IsWarning() )
			pTC->pEH->Error( aError, S.Top().pClass, aRscId );

		S.Pop();
	}
#line 1744 "yyrscyacc.cxx"
    break;

  case 53: /* var_definition: XSCALE '=' '(' long_expression ',' long_expression ')' ';'  */
#line 460 "yyrscyacc.y"
        {
		SetNumber( S.Top(), "_XNUMERATOR", (yyvsp[-4].value) );
		SetNumber( S.Top(), "_XDENOMINATOR", (yyvsp[-2].value) );
	}
#line 1753 "yyrscyacc.cxx"
    break;

  case 54: /* var_definition: YSCALE '=' '(' long_expression ',' long_expression ')' ';'  */
#line 465 "yyrscyacc.y"
        {
		SetNumber( S.Top(), "_YNUMERATOR", (yyvsp[-4].value) );
		SetNumber( S.Top(), "_YDENOMINATOR", (yyvsp[-2].value) );
	}
#line 1762 "yyrscyacc.cxx"
    break;

  case 55: /* var_definition: RGB '=' '(' long_expression ',' long_expression ',' long_expression ')' ';'  */
#line 471 "yyrscyacc.y"
        {
		SetNumber( S.Top(), "RED", (yyvsp[-6].value) );
		SetNumber( S.Top(), "GREEN", (yyvsp[-4].value) );
		SetNumber( S.Top(), "BLUE", (yyvsp[-2].value) );
	}
#line 1772 "yyrscyacc.cxx"
    break;

  case 56: /* var_definition: GEOMETRY '=' xywh_mapmode '(' long_expression ',' long_expression ',' long_expression ',' long_expression ')' ';'  */
#line 478 "yyrscyacc.y"
        {
		SetNumber( S.Top(), "_X", (yyvsp[-8].value) );
		SetNumber( S.Top(), "_Y", (yyvsp[-6].value) );
		SetNumber( S.Top(), "_WIDTH", (yyvsp[-4].value) );
		SetNumber( S.Top(), "_HEIGHT", (yyvsp[-2].value) );
	}
#line 1783 "yyrscyacc.cxx"
    break;

  case 57: /* var_definition: POSITION '=' xy_mapmode '(' long_expression ',' long_expression ')' ';'  */
#line 486 "yyrscyacc.y"
        {
		SetNumber( S.Top(), "_X", (yyvsp[-4].value) );
		SetNumber( S.Top(), "_Y", (yyvsp[-2].value) );
	}
#line 1792 "yyrscyacc.cxx"
    break;

  case 58: /* var_definition: DIMENSION '=' wh_mapmode '(' long_expression ',' long_expression ')' ';'  */
#line 492 "yyrscyacc.y"
        {
		SetNumber( S.Top(), "_WIDTH", (yyvsp[-4].value) );
		SetNumber( S.Top(), "_HEIGHT", (yyvsp[-2].value) );
	}
#line 1801 "yyrscyacc.cxx"
    break;

  case 59: /* var_definition: INZOOMOUTPUTSIZE '=' CONSTNAME '(' long_expression ',' long_expression ')' ';'  */
#line 498 "yyrscyacc.y"
        {
		SetConst( S.Top(), "_ZOOMINMAPMODE", (yyvsp[-6].constname).hashid, (yyvsp[-6].constname).nValue );
		SetNumber( S.Top(), "_ZOOMINWIDTH", (yyvsp[-4].value) );
		SetNumber( S.Top(), "_ZOOMINHEIGHT", (yyvsp[-2].value) );
	}
#line 1811 "yyrscyacc.cxx"
    break;

  case 60: /* var_definition: INZOOMOUTPUTSIZE '=' '(' long_expression ',' long_expression ')' ';'  */
#line 504 "yyrscyacc.y"
        {
		SetNumber( S.Top(), "_ZOOMINWIDTH", (yyvsp[-4].value) );
		SetNumber( S.Top(), "_ZOOMINHEIGHT", (yyvsp[-2].value) );
	}
#line 1820 "yyrscyacc.cxx"
    break;

  case 61: /* var_definition: FLOATINGPOS '=' CONSTNAME '(' long_expression ',' long_expression ')' ';'  */
#line 510 "yyrscyacc.y"
        {
		SetConst( S.Top(),	"_FLOATINGPOSMAPMODE", (yyvsp[-6].constname).hashid, (yyvsp[-6].constname).nValue );
		SetNumber( S.Top(), "_FLOATINGPOSX", (yyvsp[-4].value) );
		SetNumber( S.Top(), "_FLOATINGPOSY", (yyvsp[-2].value) );
	}
#line 1830 "yyrscyacc.cxx"
    break;

  case 62: /* var_definition: FLOATINGPOS '=' '(' long_expression ',' long_expression ')' ';'  */
#line 516 "yyrscyacc.y"
        {
		SetNumber( S.Top(), "_FLOATINGPOSX", (yyvsp[-4].value) );
		SetNumber( S.Top(), "_FLOATINGPOSY", (yyvsp[-2].value) );
	}
#line 1839 "yyrscyacc.cxx"
    break;

  case 63: /* var_header_class: VARNAME '=' class_header_body  */
#line 524 "yyrscyacc.y"
        {
		RSCINST aInst;

		aInst = S.Top().pClass->GetVariable( S.Top(), (yyvsp[-2].varid), RSCINST(), sal_False, (yyvsp[0].header).pClass );

		if( aInst.pData )
			S.Push( aInst );
		else
		{
			pTC->pEH->Error( ERR_NOVARIABLENAME, S.Top().pClass, RscId(),
							 pHS->getString( (yyvsp[-2].varid) )  );
			return( ERR_ERROR );
		};

		if( !DoClassHeader( &(yyvsp[0].header), sal_True ) )
			return( ERR_ERROR );
		(yyval.header) = (yyvsp[0].header);
	}
#line 1862 "yyrscyacc.cxx"
    break;

  case 64: /* var_header_class: VARNAME '[' CONSTNAME ']' '=' class_header_body  */
#line 543 "yyrscyacc.y"
        {
		RSCINST aInst;

		aInst = S.Top().pClass->GetVariable( S.Top(), (yyvsp[-5].varid), RSCINST() );

		if( aInst.pData )
		{
			ERRTYPE aError;
			RSCINST aIdxInst;

			aError = aInst.pClass->GetArrayEle( aInst, (yyvsp[-3].constname).hashid, NULL, &aIdxInst );
			if( aError.IsError() || aError.IsWarning() )
				 pTC->pEH->Error( aError, S.Top().pClass, RscId() );
			if( aError.IsError() )
				return( ERR_ERROR );
			S.Push( aIdxInst );
		}
		else
		{
			pTC->pEH->Error( ERR_NOVARIABLENAME, S.Top().pClass, RscId(),
							 pHS->getString( (yyvsp[-5].varid) )  );
			return( ERR_ERROR );
		};
		if( !DoClassHeader( &(yyvsp[0].header), sal_True ) )
			return( ERR_ERROR );
		(yyval.header) = (yyvsp[0].header);
	}
#line 1894 "yyrscyacc.cxx"
    break;

  case 65: /* var_header_class: VARNAME '[' SYMBOL ']' '=' class_header_body  */
#line 571 "yyrscyacc.y"
        {
		RSCINST aInst;

		aInst = S.Top().pClass->GetVariable( S.Top(), (yyvsp[-5].varid), RSCINST() );

		if( aInst.pData )
		{
            long nNewLang = pTC->AddLanguage( (yyvsp[-3].string) );
			ERRTYPE aError;
			RSCINST aIdxInst;

			aError = aInst.pClass->GetArrayEle( aInst, nNewLang, NULL, &aIdxInst );
			if( aError.IsError() || aError.IsWarning() )
				 pTC->pEH->Error( aError, S.Top().pClass, RscId() );
			if( aError.IsError() )
				return( ERR_ERROR );
			S.Push( aIdxInst );
		}
		else
		{
			pTC->pEH->Error( ERR_NOVARIABLENAME, S.Top().pClass, RscId(),
							 pHS->getString( (yyvsp[-5].varid) )  );
			return( ERR_ERROR );
		};
		if( !DoClassHeader( &(yyvsp[0].header), sal_True ) )
			return( ERR_ERROR );
		(yyval.header) = (yyvsp[0].header);
	}
#line 1927 "yyrscyacc.cxx"
    break;

  case 66: /* var_header: VARNAME '='  */
#line 603 "yyrscyacc.y"
        {
		RSCINST aInst;

		aInst = S.Top().pClass->GetVariable( S.Top(), (yyvsp[-1].varid), RSCINST() );

		if( aInst.pData )
			S.Push( aInst );
		else{
			pTC->pEH->Error( ERR_NOVARIABLENAME, S.Top().pClass, RscId(),
							 pHS->getString( (yyvsp[-1].varid) )  );
			return( ERR_ERROR );
		};
	}
#line 1945 "yyrscyacc.cxx"
    break;

  case 67: /* var_header: VARNAME '[' CONSTNAME ']' '='  */
#line 617 "yyrscyacc.y"
        {
		RSCINST aInst;

		aInst = S.Top().pClass->GetVariable( S.Top(), (yyvsp[-4].varid), RSCINST() );

		if( aInst.pData )
		{
			ERRTYPE aError;
			RSCINST aIdxInst;

			aError = aInst.pClass->GetArrayEle( aInst, (yyvsp[-2].constname).hashid, NULL, &aIdxInst );
			if( aError.IsError() || aError.IsWarning() )
				 pTC->pEH->Error( aError, S.Top().pClass, RscId() );
			if( aError.IsError() )
				return( ERR_ERROR );
			S.Push( aIdxInst );
		}
		else{
			pTC->pEH->Error( ERR_NOVARIABLENAME, S.Top().pClass, RscId(),
							 pHS->getString( (yyvsp[-4].varid) )  );
			return( ERR_ERROR );
		};
	}
#line 1973 "yyrscyacc.cxx"
    break;

  case 68: /* var_header: VARNAME '[' SYMBOL ']' '='  */
#line 641 "yyrscyacc.y"
        {
		RSCINST aInst;

		aInst = S.Top().pClass->GetVariable( S.Top(), (yyvsp[-4].varid), RSCINST() );

		if( aInst.pData )
		{
            long nNewLang = pTC->AddLanguage( (yyvsp[-2].string) );
			ERRTYPE aError;
			RSCINST aIdxInst;

			aError = aInst.pClass->GetArrayEle( aInst, nNewLang, NULL, &aIdxInst );
			if( aError.IsError() || aError.IsWarning() )
				 pTC->pEH->Error( aError, S.Top().pClass, RscId() );
			if( aError.IsError() )
				return( ERR_ERROR );
			S.Push( aIdxInst );
		}
		else{
			pTC->pEH->Error( ERR_NOVARIABLENAME, S.Top().pClass, RscId(),
							 pHS->getString( (yyvsp[-4].varid) )  );
			return( ERR_ERROR );
		};
	}
#line 2002 "yyrscyacc.cxx"
    break;

  case 69: /* tupel_header0: %empty  */
#line 668 "yyrscyacc.y"
        {
		RSCINST aInst;

		aInst = S.Top().pClass->GetTupelVar( S.Top(), 0, RSCINST() );
		if( aInst.pData )
			S.Push( aInst );
		else
		{
			pTC->pEH->Error( ERR_NOTUPELNAME, S.Top().pClass, RscId() );
			return( ERR_ERROR );
		};
	}
#line 2019 "yyrscyacc.cxx"
    break;

  case 70: /* tupel_header1: %empty  */
#line 684 "yyrscyacc.y"
        {
		RSCINST aInst;

		aInst = S.Top().pClass->GetTupelVar( S.Top(), 1, RSCINST() );
		if( aInst.pData )
			S.Push( aInst );
		else
		{
			pTC->pEH->Error( ERR_NOTUPELNAME, S.Top().pClass, RscId() );
			return( ERR_ERROR );
		};
	}
#line 2036 "yyrscyacc.cxx"
    break;

  case 71: /* tupel_header2: %empty  */
#line 700 "yyrscyacc.y"
        {
		RSCINST aInst;

		aInst = S.Top().pClass->GetTupelVar( S.Top(), 2, RSCINST() );
		if( aInst.pData )
			S.Push( aInst );
		else
		{
			pTC->pEH->Error( ERR_NOTUPELNAME, S.Top().pClass, RscId() );
			return( ERR_ERROR );
		};
	}
#line 2053 "yyrscyacc.cxx"
    break;

  case 72: /* tupel_header3: %empty  */
#line 716 "yyrscyacc.y"
        {
		RSCINST aInst;

		aInst = S.Top().pClass->GetTupelVar( S.Top(), 3, RSCINST() );
		if( !aInst.pData )
		{
			pTC->pEH->Error( ERR_NOTUPELNAME, S.Top().pClass, RscId() );
			return( ERR_ERROR );
		};
		S.Push( aInst );
	}
#line 2069 "yyrscyacc.cxx"
    break;

  case 73: /* tupel_body: var_body  */
#line 731 "yyrscyacc.y"
        {
		S.Pop();
	}
#line 2077 "yyrscyacc.cxx"
    break;

  case 74: /* var_list_header: %empty  */
#line 738 "yyrscyacc.y"
        {
		ERRTYPE aError;
		RSCINST aInst;

		aError = S.Top().pClass->GetElement( S.Top(), RscId(),
											NULL, RSCINST(), &aInst );
		if( aError.IsError() || aError.IsWarning() )
			pTC->pEH->Error( aError, S.Top().pClass, RscId() );
		if( aError.IsError() )
		{ // unbedingt Instanz auf den Stack bringen
			aInst = S.Top().pClass->Create( NULL, RSCINST() );
		}
		S.Push( aInst );
	}
#line 2096 "yyrscyacc.cxx"
    break;

  case 75: /* list_body: var_bodycomplex  */
#line 756 "yyrscyacc.y"
        {
		S.Pop();
	}
#line 2104 "yyrscyacc.cxx"
    break;

  case 76: /* list_header: %empty  */
#line 763 "yyrscyacc.y"
        {
		sal_uInt32 nCount = S.Top().pClass->GetCount( S.Top() );
		sal_uInt32 i;

		for( i = nCount; i > 0; i-- )
			S.Top().pClass->DeletePos( S.Top(), i -1 );
	}
#line 2116 "yyrscyacc.cxx"
    break;

  case 82: /* var_bodysimple: macro_expression  */
#line 782 "yyrscyacc.y"
        {
		sal_Int32	l;
		ERRTYPE aError;

		if( !(yyvsp[0].macrostruct).Evaluate( &l ) )
			pTC->pEH->Error( ERR_ZERODIVISION, NULL, RscId() );
		else
		{
			aError = S.Top().pClass->SetRef( S.Top(), RscId( (yyvsp[0].macrostruct) ) );
			if( aError.IsError() )
			{
				aError.Clear();
				aError = S.Top().pClass->SetNumber( S.Top(), l );
			}
			if( aError.IsError() )
			{ // Aufwaertskompatible, Tupel probieren
				RSCINST aInst = GetFirstTupelEle( S.Top() );
				if( aInst.pData )
				{
					aError.Clear(); // Fehler zuruecksetzen
					aError = aInst.pClass->SetRef( aInst, RscId( (yyvsp[0].macrostruct) ) );
					if( aError.IsError() )
					{
						aError.Clear();
						aError = aInst.pClass->SetNumber( aInst, l );
					}
				}
			}
		}

		if( (yyvsp[0].macrostruct).IsExpression() )
			delete (yyvsp[0].macrostruct).aExp.pExp;

		if( aError.IsError() || aError.IsWarning() )
			pTC->pEH->Error( aError, S.Top().pClass, RscId() );
	}
#line 2157 "yyrscyacc.cxx"
    break;

  case 83: /* var_bodysimple: CONSTNAME  */
#line 819 "yyrscyacc.y"
        {
		ERRTYPE aError;
		aError = S.Top().pClass->SetConst( S.Top(), (yyvsp[0].constname).hashid, (yyvsp[0].constname).nValue );
		if( aError.IsError() )
		{ // Aufwaertskompatible, Tupel probieren
			RSCINST aInst = GetFirstTupelEle( S.Top() );
			if( aInst.pData )
			{
				aError.Clear(); // Fehler zuruecksetzen
				aError = aInst.pClass->SetConst( aInst, (yyvsp[0].constname).hashid, (yyvsp[0].constname).nValue );
			}
		}

		if( aError.IsError() || aError.IsWarning() )
			pTC->pEH->Error( aError, S.Top().pClass, RscId() );
	}
#line 2178 "yyrscyacc.cxx"
    break;

  case 84: /* var_bodysimple: NOT CONSTNAME  */
#line 836 "yyrscyacc.y"
        {
		ERRTYPE aError;
		aError = S.Top().pClass->SetNotConst( S.Top(), (yyvsp[0].constname).hashid );
		if( aError.IsError() )
		{ // Aufwaertskompatible, Tupel probieren
			RSCINST aInst = GetFirstTupelEle( S.Top() );
			if( aInst.pData )
			{
				aError.Clear(); // Fehler zuruecksetzen
				aError = aInst.pClass->SetNotConst( aInst, (yyvsp[0].constname).hashid );
			}
		}

		if( aError.IsError() || aError.IsWarning() )
			pTC->pEH->Error( aError, S.Top().pClass, RscId() );
	}
#line 2199 "yyrscyacc.cxx"
    break;

  case 85: /* var_bodysimple: BOOLEAN  */
#line 853 "yyrscyacc.y"
        {
		ERRTYPE aError;
		aError = S.Top().pClass->SetBool( S.Top(), (yyvsp[0].svbool) );
		if( aError.IsError() )
		{ // Aufwaertskompatible, Tupel probieren
			RSCINST aInst = GetFirstTupelEle( S.Top() );
			if( aInst.pData )
			{
				aError.Clear(); // Fehler zuruecksetzen
				aError = aInst.pClass->SetBool( aInst, (yyvsp[0].svbool) );
			}
		}

		if( aError.IsError() || aError.IsWarning() )
			pTC->pEH->Error( aError, S.Top().pClass, RscId() );
	}
#line 2220 "yyrscyacc.cxx"
    break;

  case 86: /* var_bodysimple: string_multiline  */
#line 870 "yyrscyacc.y"
        {
		ERRTYPE aError;
		aError = S.Top().pClass->SetString( S.Top(), (yyvsp[0].string) );
		if( aError.IsError() )
		{ // Aufwaertskompatible, Tupel probieren
			RSCINST aInst = GetFirstTupelEle( S.Top() );
			if( aInst.pData )
			{
				aError.Clear(); // Fehler zuruecksetzen
				aError = aInst.pClass->SetString( aInst, (yyvsp[0].string) );
			}
		}

		if( aError.IsError() || aError.IsWarning() )
			pTC->pEH->Error( aError, S.Top().pClass, RscId() );
	}
#line 2241 "yyrscyacc.cxx"
    break;

  case 95: /* string_multiline: STRING  */
#line 907 "yyrscyacc.y"
        {
		(yyval.string) = (yyvsp[0].string);
	}
#line 2249 "yyrscyacc.cxx"
    break;

  case 96: /* string_multiline: string_multiline STRING  */
#line 911 "yyrscyacc.y"
        {
        rtl::OStringBuffer aBuf( 256 );
        aBuf.append( (yyvsp[-1].string) );
        aBuf.append( (yyvsp[0].string) );
        (yyval.string) = (char*)pStringContainer->putString( aBuf.getStr() );
	}
#line 2260 "yyrscyacc.cxx"
    break;

  case 97: /* long_expression: macro_expression  */
#line 921 "yyrscyacc.y"
        {
		if( !(yyvsp[0].macrostruct).Evaluate( &(yyval.value) ) )
			pTC->pEH->Error( ERR_ZERODIVISION, NULL, RscId() );
		if( (yyvsp[0].macrostruct).IsExpression() )
			delete (yyvsp[0].macrostruct).aExp.pExp;
	}
#line 2271 "yyrscyacc.cxx"
    break;

  case 98: /* macro_expression: RSCDEFINE  */
#line 931 "yyrscyacc.y"
        {
		(yyval.macrostruct).cType = RSCEXP_DEF;
		(yyval.macrostruct).aExp.pDef = (yyvsp[0].defineele);
	}
#line 2280 "yyrscyacc.cxx"
    break;

  case 99: /* macro_expression: NUMBER  */
#line 936 "yyrscyacc.y"
        {
		(yyval.macrostruct).cType = RSCEXP_LONG;
		(yyval.macrostruct).SetLong( (yyvsp[0].value) );
	}
#line 2289 "yyrscyacc.cxx"
    break;

  case 100: /* macro_expression: '-' macro_expression  */
#line 941 "yyrscyacc.y"
        {
		if( (yyvsp[0].macrostruct).IsNumber() ){
			(yyval.macrostruct).cType = (yyvsp[0].macrostruct).cType;
			(yyval.macrostruct).SetLong( - (yyvsp[0].macrostruct).GetLong() );
		}
		else{
			RscExpType aLeftExp;

			aLeftExp.cType = RSCEXP_NOTHING;
			(yyval.macrostruct).cType = RSCEXP_EXP;
			(yyval.macrostruct).aExp.pExp = new RscExpression( aLeftExp, '-', (yyvsp[0].macrostruct) );
		}
	}
#line 2307 "yyrscyacc.cxx"
    break;

  case 101: /* macro_expression: '+' macro_expression  */
#line 955 "yyrscyacc.y"
        {
		(yyval.macrostruct) = (yyvsp[0].macrostruct);
	}
#line 2315 "yyrscyacc.cxx"
    break;

  case 102: /* macro_expression: macro_expression '+' macro_expression  */
#line 959 "yyrscyacc.y"
        {
		if( (yyvsp[-2].macrostruct).IsNumber() && (yyvsp[0].macrostruct).IsNumber() ){
			(yyval.macrostruct).cType = RSCEXP_LONG;
			(yyval.macrostruct).SetLong( (yyvsp[-2].macrostruct).GetLong() + (yyvsp[0].macrostruct).GetLong() );
		}
		else{
			(yyval.macrostruct).cType = RSCEXP_EXP;
			(yyval.macrostruct).aExp.pExp = new RscExpression( (yyvsp[-2].macrostruct), '+', (yyvsp[0].macrostruct) );
		}
	}
#line 2330 "yyrscyacc.cxx"
    break;

  case 103: /* macro_expression: macro_expression '-' macro_expression  */
#line 970 "yyrscyacc.y"
        {
		if( (yyvsp[-2].macrostruct).IsNumber() && (yyvsp[0].macrostruct).IsNumber() ){
			(yyval.macrostruct).cType = RSCEXP_LONG;
			(yyval.macrostruct).SetLong( (yyvsp[-2].macrostruct).GetLong() - (yyvsp[0].macrostruct).GetLong() );
		}
		else{
			(yyval.macrostruct).cType = RSCEXP_EXP;
			(yyval.macrostruct).aExp.pExp = new RscExpression( (yyvsp[-2].macrostruct), '-', (yyvsp[0].macrostruct) );
		}
	}
#line 2345 "yyrscyacc.cxx"
    break;

  case 104: /* macro_expression: macro_expression '*' macro_expression  */
#line 981 "yyrscyacc.y"
        {
		if( (yyvsp[-2].macrostruct).IsNumber() && (yyvsp[0].macrostruct).IsNumber() ){
			(yyval.macrostruct).cType = RSCEXP_LONG;
			(yyval.macrostruct).SetLong( (yyvsp[-2].macrostruct).GetLong() * (yyvsp[0].macrostruct).GetLong() );
		}
		else{
			(yyval.macrostruct).cType = RSCEXP_EXP;
			(yyval.macrostruct).aExp.pExp = new RscExpression( (yyvsp[-2].macrostruct), '*', (yyvsp[0].macrostruct) );
		}
	}
#line 2360 "yyrscyacc.cxx"
    break;

  case 105: /* macro_expression: macro_expression '/' macro_expression  */
#line 992 "yyrscyacc.y"
        {
		if( (yyvsp[-2].macrostruct).IsNumber() && (yyvsp[0].macrostruct).IsNumber() ){
			if( 0 == (yyvsp[0].macrostruct).GetLong() ){
				(yyval.macrostruct).cType = RSCEXP_EXP;
				(yyval.macrostruct).aExp.pExp = new RscExpression( (yyvsp[-2].macrostruct), '/', (yyvsp[0].macrostruct) );
			}
			else{
				(yyval.macrostruct).cType = RSCEXP_LONG;
				(yyval.macrostruct).SetLong( (yyvsp[-2].macrostruct).GetLong() / (yyvsp[0].macrostruct).GetLong() );
			}
		}
		else{
			(yyval.macrostruct).cType = RSCEXP_EXP;
			(yyval.macrostruct).aExp.pExp = new RscExpression( (yyvsp[-2].macrostruct), '/', (yyvsp[0].macrostruct) );
		}
	}
#line 2381 "yyrscyacc.cxx"
    break;

  case 106: /* macro_expression: macro_expression '&' macro_expression  */
#line 1009 "yyrscyacc.y"
        {
		if( (yyvsp[-2].macrostruct).IsNumber() && (yyvsp[0].macrostruct).IsNumber() ){
			(yyval.macrostruct).cType = RSCEXP_LONG;
			(yyval.macrostruct).SetLong( (yyvsp[-2].macrostruct).GetLong() & (yyvsp[0].macrostruct).GetLong() );
		}
		else{
			(yyval.macrostruct).cType = RSCEXP_EXP;
			(yyval.macrostruct).aExp.pExp = new RscExpression( (yyvsp[-2].macrostruct), '&', (yyvsp[0].macrostruct) );
		}
	}
#line 2396 "yyrscyacc.cxx"
    break;

  case 107: /* macro_expression: macro_expression '|' macro_expression  */
#line 1020 "yyrscyacc.y"
        {
		if( (yyvsp[-2].macrostruct).IsNumber() && (yyvsp[0].macrostruct).IsNumber() ){
			(yyval.macrostruct).cType = RSCEXP_LONG;
			(yyval.macrostruct).SetLong( (yyvsp[-2].macrostruct).GetLong() | (yyvsp[0].macrostruct).GetLong() );
		}
		else{
			(yyval.macrostruct).cType = RSCEXP_EXP;
			(yyval.macrostruct).aExp.pExp = new RscExpression( (yyvsp[-2].macrostruct), '|', (yyvsp[0].macrostruct) );
		}
	}
#line 2411 "yyrscyacc.cxx"
    break;

  case 108: /* macro_expression: '(' macro_expression ')'  */
#line 1031 "yyrscyacc.y"
        {
		(yyval.macrostruct) = (yyvsp[-1].macrostruct);
	}
#line 2419 "yyrscyacc.cxx"
    break;

  case 109: /* macro_expression: macro_expression LEFTSHIFT macro_expression  */
#line 1035 "yyrscyacc.y"
        {
		if( (yyvsp[-2].macrostruct).IsNumber() && (yyvsp[0].macrostruct).IsNumber() ){
			(yyval.macrostruct).cType = RSCEXP_LONG;
			(yyval.macrostruct).SetLong( (yyvsp[-2].macrostruct).GetLong() << (yyvsp[0].macrostruct).GetLong() );
		}
		else{
			(yyval.macrostruct).cType = RSCEXP_EXP;
			(yyval.macrostruct).aExp.pExp = new RscExpression( (yyvsp[-2].macrostruct), 'l', (yyvsp[0].macrostruct) );
		}
	}
#line 2434 "yyrscyacc.cxx"
    break;

  case 110: /* macro_expression: macro_expression RIGHTSHIFT macro_expression  */
#line 1046 "yyrscyacc.y"
        {
		if( (yyvsp[-2].macrostruct).IsNumber() && (yyvsp[0].macrostruct).IsNumber() ){
			(yyval.macrostruct).cType = RSCEXP_LONG;
			(yyval.macrostruct).SetLong( (yyvsp[-2].macrostruct).GetLong() >> (yyvsp[0].macrostruct).GetLong() );
		}
		else{
			(yyval.macrostruct).cType = RSCEXP_EXP;
			(yyval.macrostruct).aExp.pExp = new RscExpression( (yyvsp[-2].macrostruct), 'r', (yyvsp[0].macrostruct) );
		}
	}
#line 2449 "yyrscyacc.cxx"
    break;

  case 112: /* id_expression: macro_expression  */
#line 1061 "yyrscyacc.y"
        {  // pExpession auswerten und loeschen
		if( RSCEXP_EXP == (yyvsp[0].macrostruct).cType ){
			sal_Int32	lValue;

			if( !(yyvsp[0].macrostruct).Evaluate( &lValue ) )
				pTC->pEH->Error( ERR_ZERODIVISION, NULL, RscId() );
			delete (yyvsp[0].macrostruct).aExp.pExp;
			(yyval.macrostruct).cType = RSCEXP_LONG;
			(yyval.macrostruct).SetLong( lValue );
		}
		else
			(yyval.macrostruct) = (yyvsp[0].macrostruct);
	}
#line 2467 "yyrscyacc.cxx"
    break;

  case 113: /* DUMMY_NUMBER: NUMBER  */
#line 1078 "yyrscyacc.y"
        {
	}
#line 2474 "yyrscyacc.cxx"
    break;

  case 114: /* DUMMY_NUMBER: %empty  */
#line 1081 "yyrscyacc.y"
        {
	}
#line 2481 "yyrscyacc.cxx"
    break;

  case 115: /* line_number: '#' LINE NUMBER STRING  */
#line 1087 "yyrscyacc.y"
        {
		RscFile * pFName;

		pFI->SetLineNo( (yyvsp[-1].value) );
		pFI->SetFileIndex( pTC->aFileTab.NewCodeFile( ByteString( (yyvsp[0].string) ) ) );
		pFName = pTC->aFileTab.Get( pFI->GetFileIndex() );
		pFName->bLoaded = sal_True;
		pFName->bScanned = sal_True;
	}
#line 2495 "yyrscyacc.cxx"
    break;

  case 116: /* line_number: '#' NUMBER STRING DUMMY_NUMBER  */
#line 1097 "yyrscyacc.y"
        {
		RscFile * pFName;

		pFI->SetLineNo( (yyvsp[-2].value) );
		pFI->SetFileIndex( pTC->aFileTab.NewCodeFile( ByteString( (yyvsp[-1].string) ) ) );
		pFName = pTC->aFileTab.Get( pFI->GetFileIndex() );
		pFName->bLoaded = sal_True;
		pFName->bScanned = sal_True;
	}
#line 2509 "yyrscyacc.cxx"
    break;

  case 117: /* line_number: '#' NUMBER  */
#line 1107 "yyrscyacc.y"
        {
		pFI->SetLineNo( (yyvsp[0].value) );
	}
#line 2517 "yyrscyacc.cxx"
    break;


#line 2521 "yyrscyacc.cxx"

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

