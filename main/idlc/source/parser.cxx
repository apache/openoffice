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
#line 28 "main/idlc/source/parser.y"

#include <string.h>

#ifndef _IDLC_IDLC_HXX_
#include <idlc/idlc.hxx>
#endif
#ifndef _IDLC_ERRORHANDLER_HXX_
#include <idlc/errorhandler.hxx>
#endif
#ifndef _IDLC_FEHELPER_HXX_
#include <idlc/fehelper.hxx>
#endif
#ifndef _IDLC_EXPRESSION_HXX_
#include <idlc/astexpression.hxx>
#endif
#ifndef _IDLC_ASTCONSTANTS_HXX_
#include <idlc/astconstants.hxx>
#endif
#ifndef _IDLC_ASTCONSTANT_HXX_
#include <idlc/astconstant.hxx>
#endif
#ifndef _IDLC_ASTARRAY_HXX_
#include <idlc/astarray.hxx>
#endif
#ifndef _IDLC_ASTBASETYPE_HXX_
#include <idlc/astbasetype.hxx>
#endif
#ifndef _IDLC_ASTTYPEDEF_HXX_
#include <idlc/asttypedef.hxx>
#endif
#ifndef _IDLC_ASTEXCEPTION_HXX_
#include <idlc/astexception.hxx>
#endif
#ifndef _IDLC_ASTMEMBER_HXX_
#include <idlc/astmember.hxx>
#endif
#ifndef _IDLC_ASTENUM_HXX_
#include <idlc/astenum.hxx>
#endif
#ifndef _IDLC_ASTSEQUENCE_HXX_
#include <idlc/astsequence.hxx>
#endif
#ifndef _IDLC_ASTATTRIBUTE_HXX_
#include <idlc/astattribute.hxx>
#endif
#ifndef _IDLC_ASTOPERATION_HXX_
#include <idlc/astoperation.hxx>
#endif
#ifndef _IDLC_ASTPARAMETER_HXX_
#include <idlc/astparameter.hxx>
#endif
#ifndef _IDLC_ASTINTERFACEMEMBER_HXX_
#include <idlc/astinterfacemember.hxx>
#endif
#ifndef _IDLC_ASTSERVICEMEMBER_HXX_
#include <idlc/astservicemember.hxx>
#endif
#ifndef _IDLC_ASTOBSERVES_HXX_
#include <idlc/astobserves.hxx>
#endif
#ifndef _IDLC_ASTNEEDS_HXX_
#include <idlc/astneeds.hxx>
#endif
#ifndef _IDLC_ASTUNION_HXX_
#include <idlc/astunion.hxx>
#endif
#include "idlc/aststructinstance.hxx"

#include "attributeexceptions.hxx"

#include "rtl/strbuf.hxx"

#include <algorithm>
#include <vector>
    
using namespace ::rtl;

#define YYDEBUG 1
#define YYERROR_VERBOSE 1

extern int yylex(void);
void yyerror(char const *);

void checkIdentifier(::rtl::OString* id)
{
    static short check = 0;
    if (check == 0) {
        if (idlc()->getOptions()->isValid("-cid"))
            check = 1;
        else
            check = 2;
    }

    if ( id->indexOf('_') >= 0 )
        if ( (id->pData->buffer[0] >= 97 && id->pData->buffer[0] <= 122)
             || id->pData->buffer[0] == '_') {
            if (check == 1) {
                ::rtl::OStringBuffer msg(25 + id->getLength());
                msg.append("mismatched identifier '");
                msg.append(*id);
                msg.append("'");
                idlc()->error()->syntaxError(idlc()->getParseState(),
                                         idlc()->getLineNumber(),
                                         msg.getStr());
            }
            else
                idlc()->error()->warning0(WIDL_WRONG_NAMING_CONV, id->getStr());
        }
}

void reportDoubleMemberDeclarations(
    AstInterface::DoubleMemberDeclarations const & doubleMembers)
{
    for (AstInterface::DoubleMemberDeclarations::const_iterator i(
             doubleMembers.begin());
         i != doubleMembers.end(); ++i)
    {
        idlc()->error()->error2(EIDL_DOUBLE_MEMBER, i->first, i->second);
    }
}

void addInheritedInterface(
    AstInterface * ifc, rtl::OString const & name, bool optional,
    rtl::OUString const & documentation)
{
    AstDeclaration * decl = ifc->lookupByName(name);
    AstDeclaration const * resolved = resolveTypedefs(decl);
    if (resolved != 0 && resolved->getNodeType() == NT_interface) {
        if (idlc()->error()->checkPublished(decl)) {
            if (!static_cast< AstInterface const * >(resolved)->isDefined()) {
                idlc()->error()->inheritanceError(
                    NT_interface, &ifc->getScopedName(), decl);
            } else {
                AstInterface::DoubleDeclarations doubleDecls(
                    ifc->checkInheritedInterfaceClashes(
                        static_cast< AstInterface const * >(resolved),
                        optional));
                if (doubleDecls.interfaces.empty()
                    && doubleDecls.members.empty())
                {
                    ifc->addInheritedInterface(
                        static_cast< AstType * >(decl), optional,
                        documentation);
                } else {
                    for (AstInterface::DoubleInterfaceDeclarations::iterator i(
                             doubleDecls.interfaces.begin());
                         i != doubleDecls.interfaces.end(); ++i)
                    {
                        idlc()->error()->error1(
                            EIDL_DOUBLE_INHERITANCE, *i);
                    }
                    reportDoubleMemberDeclarations(doubleDecls.members);
                }
            }
        }
    } else {
        idlc()->error()->lookupError(
            EIDL_INTERFACEMEMBER_LOOKUP, name, scopeAsDecl(ifc));
    }
}

AstDeclaration const * createNamedType(
    rtl::OString const * scopedName, DeclList const * typeArgs)
{
    AstDeclaration * decl = idlc()->scopes()->topNonNull()->lookupByName(
        *scopedName);
    AstDeclaration const * resolved = resolveTypedefs(decl);
    if (decl == 0) {
        idlc()->error()->lookupError(*scopedName);
    } else if (!idlc()->error()->checkPublished(decl)) {
        decl = 0;
    } else if (resolved->getNodeType() == NT_struct) {
        if (static_cast< AstStruct const * >(resolved)->getTypeParameterCount()
            != (typeArgs == 0 ? 0 : typeArgs->size()))
        {
            idlc()->error()->error0(EIDL_WRONG_NUMBER_OF_TYPE_ARGUMENTS);
            decl = 0;
        } else if (typeArgs != 0) {
            AstScope * global = idlc()->scopes()->bottom();
            AstDeclaration * inst = new AstStructInstance(
                static_cast< AstType * >(decl), typeArgs, global);
            decl = global->addDeclaration(inst);
            if (decl != inst) {
                delete inst;
            }
        }
    } else if (decl->isType()) {
        if (typeArgs != 0) {
            idlc()->error()->error0(EIDL_WRONG_NUMBER_OF_TYPE_ARGUMENTS);
            decl = 0;
        }
    } else {
        idlc()->error()->noTypeError(decl);
        decl = 0;
    }
    delete scopedName;
    delete typeArgs;
    return decl;
}

bool includes(AstDeclaration const * type1, AstDeclaration const * type2) {
    OSL_ASSERT(type2 != 0);
    if (type1 != 0) {
        if (type1->getNodeType() == NT_instantiated_struct) {
            AstStructInstance const * inst
                = static_cast< AstStructInstance const * >(type1);
            if (inst->getTypeTemplate() == type2) {
                return true;
            }
            for (DeclList::const_iterator i(inst->getTypeArgumentsBegin());
                 i != inst->getTypeArgumentsEnd(); ++i)
            {
                if (includes(*i, type2)) {
                    return true;
                }
            }
        } else if (type1 == type2) {
            return true;
        }
    }
    return false;
}

// Suppress any warnings from generated code:
#if defined __GNUC__
#pragma GCC system_header
#elif defined __SUNPRO_CC
#pragma disable_warn
#elif defined _MSC_VER
#pragma warning(push, 1)
#pragma warning(disable: 4273 4701 4706)
#endif

#line 305 "main/idlc/source/parser.cxx"

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

#include "parser.hxx"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_IDL_IDENTIFIER = 3,             /* IDL_IDENTIFIER  */
  YYSYMBOL_IDL_ATTRIBUTE = 4,              /* IDL_ATTRIBUTE  */
  YYSYMBOL_IDL_BOUND = 5,                  /* IDL_BOUND  */
  YYSYMBOL_IDL_CASE = 6,                   /* IDL_CASE  */
  YYSYMBOL_IDL_CONST = 7,                  /* IDL_CONST  */
  YYSYMBOL_IDL_CONSTANTS = 8,              /* IDL_CONSTANTS  */
  YYSYMBOL_IDL_CONSTRAINED = 9,            /* IDL_CONSTRAINED  */
  YYSYMBOL_IDL_DEFAULT = 10,               /* IDL_DEFAULT  */
  YYSYMBOL_IDL_ENUM = 11,                  /* IDL_ENUM  */
  YYSYMBOL_IDL_EXCEPTION = 12,             /* IDL_EXCEPTION  */
  YYSYMBOL_IDL_INTERFACE = 13,             /* IDL_INTERFACE  */
  YYSYMBOL_IDL_MAYBEAMBIGUOUS = 14,        /* IDL_MAYBEAMBIGUOUS  */
  YYSYMBOL_IDL_MAYBEDEFAULT = 15,          /* IDL_MAYBEDEFAULT  */
  YYSYMBOL_IDL_MAYBEVOID = 16,             /* IDL_MAYBEVOID  */
  YYSYMBOL_IDL_MODULE = 17,                /* IDL_MODULE  */
  YYSYMBOL_IDL_NEEDS = 18,                 /* IDL_NEEDS  */
  YYSYMBOL_IDL_OBSERVES = 19,              /* IDL_OBSERVES  */
  YYSYMBOL_IDL_OPTIONAL = 20,              /* IDL_OPTIONAL  */
  YYSYMBOL_IDL_PROPERTY = 21,              /* IDL_PROPERTY  */
  YYSYMBOL_IDL_RAISES = 22,                /* IDL_RAISES  */
  YYSYMBOL_IDL_READONLY = 23,              /* IDL_READONLY  */
  YYSYMBOL_IDL_REMOVEABLE = 24,            /* IDL_REMOVEABLE  */
  YYSYMBOL_IDL_SERVICE = 25,               /* IDL_SERVICE  */
  YYSYMBOL_IDL_SEQUENCE = 26,              /* IDL_SEQUENCE  */
  YYSYMBOL_IDL_SINGLETON = 27,             /* IDL_SINGLETON  */
  YYSYMBOL_IDL_STRUCT = 28,                /* IDL_STRUCT  */
  YYSYMBOL_IDL_SWITCH = 29,                /* IDL_SWITCH  */
  YYSYMBOL_IDL_TYPEDEF = 30,               /* IDL_TYPEDEF  */
  YYSYMBOL_IDL_TRANSIENT = 31,             /* IDL_TRANSIENT  */
  YYSYMBOL_IDL_UNION = 32,                 /* IDL_UNION  */
  YYSYMBOL_IDL_ANY = 33,                   /* IDL_ANY  */
  YYSYMBOL_IDL_CHAR = 34,                  /* IDL_CHAR  */
  YYSYMBOL_IDL_BOOLEAN = 35,               /* IDL_BOOLEAN  */
  YYSYMBOL_IDL_BYTE = 36,                  /* IDL_BYTE  */
  YYSYMBOL_IDL_DOUBLE = 37,                /* IDL_DOUBLE  */
  YYSYMBOL_IDL_FLOAT = 38,                 /* IDL_FLOAT  */
  YYSYMBOL_IDL_HYPER = 39,                 /* IDL_HYPER  */
  YYSYMBOL_IDL_LONG = 40,                  /* IDL_LONG  */
  YYSYMBOL_IDL_SHORT = 41,                 /* IDL_SHORT  */
  YYSYMBOL_IDL_VOID = 42,                  /* IDL_VOID  */
  YYSYMBOL_IDL_STRING = 43,                /* IDL_STRING  */
  YYSYMBOL_IDL_TYPE = 44,                  /* IDL_TYPE  */
  YYSYMBOL_IDL_UNSIGNED = 45,              /* IDL_UNSIGNED  */
  YYSYMBOL_IDL_TRUE = 46,                  /* IDL_TRUE  */
  YYSYMBOL_IDL_FALSE = 47,                 /* IDL_FALSE  */
  YYSYMBOL_IDL_IN = 48,                    /* IDL_IN  */
  YYSYMBOL_IDL_OUT = 49,                   /* IDL_OUT  */
  YYSYMBOL_IDL_INOUT = 50,                 /* IDL_INOUT  */
  YYSYMBOL_IDL_ONEWAY = 51,                /* IDL_ONEWAY  */
  YYSYMBOL_IDL_GET = 52,                   /* IDL_GET  */
  YYSYMBOL_IDL_SET = 53,                   /* IDL_SET  */
  YYSYMBOL_IDL_PUBLISHED = 54,             /* IDL_PUBLISHED  */
  YYSYMBOL_IDL_ELLIPSIS = 55,              /* IDL_ELLIPSIS  */
  YYSYMBOL_IDL_LEFTSHIFT = 56,             /* IDL_LEFTSHIFT  */
  YYSYMBOL_IDL_RIGHTSHIFT = 57,            /* IDL_RIGHTSHIFT  */
  YYSYMBOL_IDL_SCOPESEPARATOR = 58,        /* IDL_SCOPESEPARATOR  */
  YYSYMBOL_IDL_INTEGER_LITERAL = 59,       /* IDL_INTEGER_LITERAL  */
  YYSYMBOL_IDL_INTEGER_ULITERAL = 60,      /* IDL_INTEGER_ULITERAL  */
  YYSYMBOL_IDL_FLOATING_PT_LITERAL = 61,   /* IDL_FLOATING_PT_LITERAL  */
  YYSYMBOL_62_ = 62,                       /* ';'  */
  YYSYMBOL_63_ = 63,                       /* '{'  */
  YYSYMBOL_64_ = 64,                       /* '}'  */
  YYSYMBOL_65_ = 65,                       /* ':'  */
  YYSYMBOL_66_ = 66,                       /* '['  */
  YYSYMBOL_67_ = 67,                       /* ']'  */
  YYSYMBOL_68_ = 68,                       /* ','  */
  YYSYMBOL_69_ = 69,                       /* '('  */
  YYSYMBOL_70_ = 70,                       /* ')'  */
  YYSYMBOL_71_ = 71,                       /* '='  */
  YYSYMBOL_72_ = 72,                       /* '|'  */
  YYSYMBOL_73_ = 73,                       /* '^'  */
  YYSYMBOL_74_ = 74,                       /* '&'  */
  YYSYMBOL_75_ = 75,                       /* '+'  */
  YYSYMBOL_76_ = 76,                       /* '-'  */
  YYSYMBOL_77_ = 77,                       /* '*'  */
  YYSYMBOL_78_ = 78,                       /* '/'  */
  YYSYMBOL_79_ = 79,                       /* '%'  */
  YYSYMBOL_80_ = 80,                       /* '~'  */
  YYSYMBOL_81_ = 81,                       /* '<'  */
  YYSYMBOL_82_ = 82,                       /* '>'  */
  YYSYMBOL_YYACCEPT = 83,                  /* $accept  */
  YYSYMBOL_start = 84,                     /* start  */
  YYSYMBOL_definitions = 85,               /* definitions  */
  YYSYMBOL_definition = 86,                /* definition  */
  YYSYMBOL_87_1 = 87,                      /* $@1  */
  YYSYMBOL_opt_published = 88,             /* opt_published  */
  YYSYMBOL_publishable_definition = 89,    /* publishable_definition  */
  YYSYMBOL_90_2 = 90,                      /* $@2  */
  YYSYMBOL_91_3 = 91,                      /* $@3  */
  YYSYMBOL_92_4 = 92,                      /* $@4  */
  YYSYMBOL_93_5 = 93,                      /* $@5  */
  YYSYMBOL_94_6 = 94,                      /* $@6  */
  YYSYMBOL_95_7 = 95,                      /* $@7  */
  YYSYMBOL_96_8 = 96,                      /* $@8  */
  YYSYMBOL_module_dcl = 97,                /* module_dcl  */
  YYSYMBOL_98_9 = 98,                      /* $@9  */
  YYSYMBOL_99_10 = 99,                     /* $@10  */
  YYSYMBOL_100_11 = 100,                   /* $@11  */
  YYSYMBOL_101_12 = 101,                   /* $@12  */
  YYSYMBOL_interface = 102,                /* interface  */
  YYSYMBOL_interface_decl = 103,           /* interface_decl  */
  YYSYMBOL_104_13 = 104,                   /* $@13  */
  YYSYMBOL_forward_dcl = 105,              /* forward_dcl  */
  YYSYMBOL_interface_dcl = 106,            /* interface_dcl  */
  YYSYMBOL_107_14 = 107,                   /* $@14  */
  YYSYMBOL_108_15 = 108,                   /* $@15  */
  YYSYMBOL_109_16 = 109,                   /* $@16  */
  YYSYMBOL_interfaceheader = 110,          /* interfaceheader  */
  YYSYMBOL_inheritance_spec = 111,         /* inheritance_spec  */
  YYSYMBOL_112_17 = 112,                   /* $@17  */
  YYSYMBOL_exports = 113,                  /* exports  */
  YYSYMBOL_export = 114,                   /* export  */
  YYSYMBOL_115_18 = 115,                   /* $@18  */
  YYSYMBOL_116_19 = 116,                   /* $@19  */
  YYSYMBOL_117_20 = 117,                   /* $@20  */
  YYSYMBOL_attribute = 118,                /* attribute  */
  YYSYMBOL_119_21 = 119,                   /* $@21  */
  YYSYMBOL_120_22 = 120,                   /* $@22  */
  YYSYMBOL_flag_header = 121,              /* flag_header  */
  YYSYMBOL_opt_attrflags = 122,            /* opt_attrflags  */
  YYSYMBOL_opt_attrflag = 123,             /* opt_attrflag  */
  YYSYMBOL_opt_attribute_block = 124,      /* opt_attribute_block  */
  YYSYMBOL_attribute_block_rest = 125,     /* attribute_block_rest  */
  YYSYMBOL_opt_attribute_raises = 126,     /* opt_attribute_raises  */
  YYSYMBOL_opt_attribute_get_raises = 127, /* opt_attribute_get_raises  */
  YYSYMBOL_attribute_get_raises = 128,     /* attribute_get_raises  */
  YYSYMBOL_opt_attribute_set_raises = 129, /* opt_attribute_set_raises  */
  YYSYMBOL_attribute_set_raises = 130,     /* attribute_set_raises  */
  YYSYMBOL_131_23 = 131,                   /* $@23  */
  YYSYMBOL_operation = 132,                /* operation  */
  YYSYMBOL_133_24 = 133,                   /* $@24  */
  YYSYMBOL_134_25 = 134,                   /* $@25  */
  YYSYMBOL_135_26 = 135,                   /* $@26  */
  YYSYMBOL_136_27 = 136,                   /* $@27  */
  YYSYMBOL_137_28 = 137,                   /* $@28  */
  YYSYMBOL_operation_head = 138,           /* operation_head  */
  YYSYMBOL_139_29 = 139,                   /* $@29  */
  YYSYMBOL_op_type_spec = 140,             /* op_type_spec  */
  YYSYMBOL_parameters = 141,               /* parameters  */
  YYSYMBOL_142_30 = 142,                   /* $@30  */
  YYSYMBOL_parameter = 143,                /* parameter  */
  YYSYMBOL_144_31 = 144,                   /* $@31  */
  YYSYMBOL_145_32 = 145,                   /* $@32  */
  YYSYMBOL_direction = 146,                /* direction  */
  YYSYMBOL_opt_rest = 147,                 /* opt_rest  */
  YYSYMBOL_opt_raises = 148,               /* opt_raises  */
  YYSYMBOL_raises = 149,                   /* raises  */
  YYSYMBOL_150_33 = 150,                   /* $@33  */
  YYSYMBOL_151_34 = 151,                   /* $@34  */
  YYSYMBOL_exception_list = 152,           /* exception_list  */
  YYSYMBOL_exception_name = 153,           /* exception_name  */
  YYSYMBOL_interface_inheritance_decl = 154, /* interface_inheritance_decl  */
  YYSYMBOL_155_35 = 155,                   /* $@35  */
  YYSYMBOL_optional_inherited_interface = 156, /* optional_inherited_interface  */
  YYSYMBOL_constants_exports = 157,        /* constants_exports  */
  YYSYMBOL_constants_export = 158,         /* constants_export  */
  YYSYMBOL_159_36 = 159,                   /* $@36  */
  YYSYMBOL_const_dcl = 160,                /* const_dcl  */
  YYSYMBOL_161_37 = 161,                   /* $@37  */
  YYSYMBOL_162_38 = 162,                   /* $@38  */
  YYSYMBOL_163_39 = 163,                   /* $@39  */
  YYSYMBOL_164_40 = 164,                   /* $@40  */
  YYSYMBOL_constants_dcl = 165,            /* constants_dcl  */
  YYSYMBOL_166_41 = 166,                   /* $@41  */
  YYSYMBOL_167_42 = 167,                   /* $@42  */
  YYSYMBOL_168_43 = 168,                   /* $@43  */
  YYSYMBOL_169_44 = 169,                   /* $@44  */
  YYSYMBOL_expression = 170,               /* expression  */
  YYSYMBOL_const_expr = 171,               /* const_expr  */
  YYSYMBOL_or_expr = 172,                  /* or_expr  */
  YYSYMBOL_xor_expr = 173,                 /* xor_expr  */
  YYSYMBOL_and_expr = 174,                 /* and_expr  */
  YYSYMBOL_shift_expr = 175,               /* shift_expr  */
  YYSYMBOL_add_expr = 176,                 /* add_expr  */
  YYSYMBOL_mult_expr = 177,                /* mult_expr  */
  YYSYMBOL_unary_expr = 178,               /* unary_expr  */
  YYSYMBOL_primary_expr = 179,             /* primary_expr  */
  YYSYMBOL_literal = 180,                  /* literal  */
  YYSYMBOL_positive_int_expr = 181,        /* positive_int_expr  */
  YYSYMBOL_const_type = 182,               /* const_type  */
  YYSYMBOL_exception_header = 183,         /* exception_header  */
  YYSYMBOL_184_45 = 184,                   /* $@45  */
  YYSYMBOL_185_46 = 185,                   /* $@46  */
  YYSYMBOL_exception_dcl = 186,            /* exception_dcl  */
  YYSYMBOL_187_47 = 187,                   /* $@47  */
  YYSYMBOL_188_48 = 188,                   /* $@48  */
  YYSYMBOL_189_49 = 189,                   /* $@49  */
  YYSYMBOL_property = 190,                 /* property  */
  YYSYMBOL_191_50 = 191,                   /* $@50  */
  YYSYMBOL_service_exports = 192,          /* service_exports  */
  YYSYMBOL_service_export = 193,           /* service_export  */
  YYSYMBOL_service_interface_header = 194, /* service_interface_header  */
  YYSYMBOL_service_service_header = 195,   /* service_service_header  */
  YYSYMBOL_service_dcl = 196,              /* service_dcl  */
  YYSYMBOL_197_51 = 197,                   /* $@51  */
  YYSYMBOL_198_52 = 198,                   /* $@52  */
  YYSYMBOL_service_dfn = 199,              /* service_dfn  */
  YYSYMBOL_service_interface_dfn = 200,    /* service_interface_dfn  */
  YYSYMBOL_201_53 = 201,                   /* $@53  */
  YYSYMBOL_opt_service_body = 202,         /* opt_service_body  */
  YYSYMBOL_service_body = 203,             /* service_body  */
  YYSYMBOL_constructors = 204,             /* constructors  */
  YYSYMBOL_constructor = 205,              /* constructor  */
  YYSYMBOL_206_54 = 206,                   /* $@54  */
  YYSYMBOL_207_55 = 207,                   /* $@55  */
  YYSYMBOL_singleton_dcl = 208,            /* singleton_dcl  */
  YYSYMBOL_209_56 = 209,                   /* $@56  */
  YYSYMBOL_210_57 = 210,                   /* $@57  */
  YYSYMBOL_singleton_dfn = 211,            /* singleton_dfn  */
  YYSYMBOL_singleton_interface_dfn = 212,  /* singleton_interface_dfn  */
  YYSYMBOL_service_obsolete_dfn = 213,     /* service_obsolete_dfn  */
  YYSYMBOL_214_58 = 214,                   /* $@58  */
  YYSYMBOL_215_59 = 215,                   /* $@59  */
  YYSYMBOL_type_dcl = 216,                 /* type_dcl  */
  YYSYMBOL_217_60 = 217,                   /* $@60  */
  YYSYMBOL_type_declarator = 218,          /* type_declarator  */
  YYSYMBOL_219_61 = 219,                   /* $@61  */
  YYSYMBOL_at_least_one_declarator = 220,  /* at_least_one_declarator  */
  YYSYMBOL_declarators = 221,              /* declarators  */
  YYSYMBOL_222_62 = 222,                   /* $@62  */
  YYSYMBOL_declarator = 223,               /* declarator  */
  YYSYMBOL_simple_declarator = 224,        /* simple_declarator  */
  YYSYMBOL_complex_declarator = 225,       /* complex_declarator  */
  YYSYMBOL_array_declarator = 226,         /* array_declarator  */
  YYSYMBOL_227_63 = 227,                   /* $@63  */
  YYSYMBOL_at_least_one_array_dim = 228,   /* at_least_one_array_dim  */
  YYSYMBOL_array_dims = 229,               /* array_dims  */
  YYSYMBOL_array_dim = 230,                /* array_dim  */
  YYSYMBOL_231_64 = 231,                   /* $@64  */
  YYSYMBOL_232_65 = 232,                   /* $@65  */
  YYSYMBOL_at_least_one_scoped_name = 233, /* at_least_one_scoped_name  */
  YYSYMBOL_scoped_names = 234,             /* scoped_names  */
  YYSYMBOL_235_66 = 235,                   /* $@66  */
  YYSYMBOL_scoped_name = 236,              /* scoped_name  */
  YYSYMBOL_237_67 = 237,                   /* $@67  */
  YYSYMBOL_238_68 = 238,                   /* $@68  */
  YYSYMBOL_type_spec = 239,                /* type_spec  */
  YYSYMBOL_simple_type_spec = 240,         /* simple_type_spec  */
  YYSYMBOL_fundamental_type = 241,         /* fundamental_type  */
  YYSYMBOL_opt_type_args = 242,            /* opt_type_args  */
  YYSYMBOL_type_args = 243,                /* type_args  */
  YYSYMBOL_type_arg = 244,                 /* type_arg  */
  YYSYMBOL_base_type_spec = 245,           /* base_type_spec  */
  YYSYMBOL_integer_type = 246,             /* integer_type  */
  YYSYMBOL_signed_int = 247,               /* signed_int  */
  YYSYMBOL_unsigned_int = 248,             /* unsigned_int  */
  YYSYMBOL_floating_pt_type = 249,         /* floating_pt_type  */
  YYSYMBOL_char_type = 250,                /* char_type  */
  YYSYMBOL_byte_type = 251,                /* byte_type  */
  YYSYMBOL_boolean_type = 252,             /* boolean_type  */
  YYSYMBOL_any_type = 253,                 /* any_type  */
  YYSYMBOL_type_type = 254,                /* type_type  */
  YYSYMBOL_string_type = 255,              /* string_type  */
  YYSYMBOL_template_type_spec = 256,       /* template_type_spec  */
  YYSYMBOL_constructed_type_spec = 257,    /* constructed_type_spec  */
  YYSYMBOL_array_type = 258,               /* array_type  */
  YYSYMBOL_259_69 = 259,                   /* $@69  */
  YYSYMBOL_sequence_type_spec = 260,       /* sequence_type_spec  */
  YYSYMBOL_261_70 = 261,                   /* $@70  */
  YYSYMBOL_262_71 = 262,                   /* $@71  */
  YYSYMBOL_263_72 = 263,                   /* $@72  */
  YYSYMBOL_struct_type = 264,              /* struct_type  */
  YYSYMBOL_265_73 = 265,                   /* $@73  */
  YYSYMBOL_266_74 = 266,                   /* $@74  */
  YYSYMBOL_267_75 = 267,                   /* $@75  */
  YYSYMBOL_structure_header = 268,         /* structure_header  */
  YYSYMBOL_269_76 = 269,                   /* $@76  */
  YYSYMBOL_270_77 = 270,                   /* $@77  */
  YYSYMBOL_opt_type_params = 271,          /* opt_type_params  */
  YYSYMBOL_type_params = 272,              /* type_params  */
  YYSYMBOL_at_least_one_member = 273,      /* at_least_one_member  */
  YYSYMBOL_members = 274,                  /* members  */
  YYSYMBOL_member = 275,                   /* member  */
  YYSYMBOL_276_78 = 276,                   /* $@78  */
  YYSYMBOL_277_79 = 277,                   /* $@79  */
  YYSYMBOL_type_or_parameter = 278,        /* type_or_parameter  */
  YYSYMBOL_enum_type = 279,                /* enum_type  */
  YYSYMBOL_280_80 = 280,                   /* $@80  */
  YYSYMBOL_281_81 = 281,                   /* $@81  */
  YYSYMBOL_282_82 = 282,                   /* $@82  */
  YYSYMBOL_283_83 = 283,                   /* $@83  */
  YYSYMBOL_at_least_one_enumerator = 284,  /* at_least_one_enumerator  */
  YYSYMBOL_enumerators = 285,              /* enumerators  */
  YYSYMBOL_286_84 = 286,                   /* $@84  */
  YYSYMBOL_enumerator = 287,               /* enumerator  */
  YYSYMBOL_union_type = 288,               /* union_type  */
  YYSYMBOL_289_85 = 289,                   /* $@85  */
  YYSYMBOL_290_86 = 290,                   /* $@86  */
  YYSYMBOL_291_87 = 291,                   /* $@87  */
  YYSYMBOL_292_88 = 292,                   /* $@88  */
  YYSYMBOL_293_89 = 293,                   /* $@89  */
  YYSYMBOL_294_90 = 294,                   /* $@90  */
  YYSYMBOL_295_91 = 295,                   /* $@91  */
  YYSYMBOL_296_92 = 296,                   /* $@92  */
  YYSYMBOL_switch_type_spec = 297,         /* switch_type_spec  */
  YYSYMBOL_at_least_one_case_branch = 298, /* at_least_one_case_branch  */
  YYSYMBOL_case_branches = 299,            /* case_branches  */
  YYSYMBOL_case_branch = 300,              /* case_branch  */
  YYSYMBOL_301_93 = 301,                   /* $@93  */
  YYSYMBOL_at_least_one_case_label = 302,  /* at_least_one_case_label  */
  YYSYMBOL_case_labels = 303,              /* case_labels  */
  YYSYMBOL_case_label = 304,               /* case_label  */
  YYSYMBOL_305_94 = 305,                   /* $@94  */
  YYSYMBOL_306_95 = 306,                   /* $@95  */
  YYSYMBOL_307_96 = 307,                   /* $@96  */
  YYSYMBOL_element_spec = 308,             /* element_spec  */
  YYSYMBOL_309_97 = 309,                   /* $@97  */
  YYSYMBOL_310_98 = 310,                   /* $@98  */
  YYSYMBOL_identifier = 311                /* identifier  */
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
#define YYFINAL  11
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   809

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  83
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  229
/* YYNRULES -- Number of rules.  */
#define YYNRULES  362
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  520

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   316


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
       2,     2,     2,     2,     2,     2,     2,    79,    74,     2,
      69,    70,    77,    75,    68,    76,     2,    78,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    65,    62,
      81,    71,    82,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    66,     2,    67,    73,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    63,    72,    64,    80,     2,     2,     2,
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
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   415,   415,   418,   419,   423,   425,   424,   432,   440,
     441,   446,   445,   454,   453,   462,   461,   470,   469,   478,
     477,   486,   485,   494,   493,   505,   510,   545,   549,   504,
     563,   564,   569,   568,   581,   620,   700,   704,   619,   725,
     733,   744,   743,   752,   758,   759,   764,   763,   772,   771,
     780,   779,   792,   796,   790,   829,   837,   844,   851,   856,
     861,   866,   871,   876,   881,   886,   891,   896,   901,   906,
     914,   916,   925,   926,   938,   944,   951,   960,   961,   965,
     975,   976,   981,   980,  1000,  1004,  1042,  1046,  1050,   998,
    1078,  1076,  1087,  1093,  1094,  1101,  1104,  1102,  1108,  1109,
    1120,  1124,  1117,  1184,  1193,  1197,  1201,  1208,  1213,  1219,
    1221,  1228,  1232,  1227,  1244,  1249,  1257,  1280,  1278,  1300,
    1301,  1305,  1306,  1311,  1310,  1318,  1322,  1326,  1331,  1317,
    1358,  1362,  1367,  1391,  1357,  1404,  1406,  1409,  1410,  1417,
    1418,  1425,  1426,  1433,  1434,  1438,  1445,  1446,  1450,  1457,
    1458,  1462,  1466,  1473,  1474,  1478,  1482,  1488,  1497,  1498,
    1505,  1509,  1513,  1517,  1521,  1528,  1541,  1542,  1543,  1544,
    1545,  1546,  1577,  1581,  1576,  1596,  1616,  1620,  1595,  1634,
    1632,  1696,  1704,  1705,  1709,  1759,  1801,  1843,  1885,  1893,
    1898,  1909,  1914,  1926,  1930,  1925,  1959,  1960,  1965,  1964,
    1989,  1990,  1994,  2000,  2001,  2006,  2018,  2005,  2034,  2038,
    2033,  2067,  2068,  2072,  2091,  2097,  2090,  2112,  2111,  2116,
    2117,  2118,  2123,  2122,  2177,  2195,  2193,  2213,  2219,  2220,
    2224,  2243,  2251,  2250,  2264,  2280,  2294,  2301,  2305,  2300,
    2325,  2344,  2342,  2363,  2369,  2376,  2375,  2389,  2387,  2402,
    2403,  2407,  2408,  2415,  2419,  2423,  2424,  2428,  2433,  2441,
    2451,  2452,  2453,  2454,  2455,  2456,  2457,  2458,  2462,  2463,
    2467,  2471,  2475,  2482,  2486,  2490,  2497,  2501,  2508,  2515,
    2522,  2529,  2536,  2543,  2550,  2551,  2555,  2556,  2557,  2562,
    2561,  2593,  2601,  2605,  2592,  2643,  2653,  2673,  2677,  2652,
    2690,  2694,  2689,  2726,  2727,  2731,  2737,  2748,  2751,  2752,
    2757,  2761,  2756,  2811,  2819,  2820,  2844,  2848,  2875,  2879,
    2843,  2898,  2903,  2901,  2907,  2908,  2916,  2940,  2977,  2981,
    2986,  2990,  2994,  2998,  3027,  3031,  2976,  3043,  3047,  3051,
    3055,  3056,  3124,  3127,  3128,  3133,  3132,  3171,  3187,  3201,
    3208,  3207,  3217,  3221,  3216,  3233,  3237,  3232,  3265,  3273,
    3274,  3275,  3276
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
  "\"end of file\"", "error", "\"invalid token\"", "IDL_IDENTIFIER",
  "IDL_ATTRIBUTE", "IDL_BOUND", "IDL_CASE", "IDL_CONST", "IDL_CONSTANTS",
  "IDL_CONSTRAINED", "IDL_DEFAULT", "IDL_ENUM", "IDL_EXCEPTION",
  "IDL_INTERFACE", "IDL_MAYBEAMBIGUOUS", "IDL_MAYBEDEFAULT",
  "IDL_MAYBEVOID", "IDL_MODULE", "IDL_NEEDS", "IDL_OBSERVES",
  "IDL_OPTIONAL", "IDL_PROPERTY", "IDL_RAISES", "IDL_READONLY",
  "IDL_REMOVEABLE", "IDL_SERVICE", "IDL_SEQUENCE", "IDL_SINGLETON",
  "IDL_STRUCT", "IDL_SWITCH", "IDL_TYPEDEF", "IDL_TRANSIENT", "IDL_UNION",
  "IDL_ANY", "IDL_CHAR", "IDL_BOOLEAN", "IDL_BYTE", "IDL_DOUBLE",
  "IDL_FLOAT", "IDL_HYPER", "IDL_LONG", "IDL_SHORT", "IDL_VOID",
  "IDL_STRING", "IDL_TYPE", "IDL_UNSIGNED", "IDL_TRUE", "IDL_FALSE",
  "IDL_IN", "IDL_OUT", "IDL_INOUT", "IDL_ONEWAY", "IDL_GET", "IDL_SET",
  "IDL_PUBLISHED", "IDL_ELLIPSIS", "IDL_LEFTSHIFT", "IDL_RIGHTSHIFT",
  "IDL_SCOPESEPARATOR", "IDL_INTEGER_LITERAL", "IDL_INTEGER_ULITERAL",
  "IDL_FLOATING_PT_LITERAL", "';'", "'{'", "'}'", "':'", "'['", "']'",
  "','", "'('", "')'", "'='", "'|'", "'^'", "'&'", "'+'", "'-'", "'*'",
  "'/'", "'%'", "'~'", "'<'", "'>'", "$accept", "start", "definitions",
  "definition", "$@1", "opt_published", "publishable_definition", "$@2",
  "$@3", "$@4", "$@5", "$@6", "$@7", "$@8", "module_dcl", "$@9", "$@10",
  "$@11", "$@12", "interface", "interface_decl", "$@13", "forward_dcl",
  "interface_dcl", "$@14", "$@15", "$@16", "interfaceheader",
  "inheritance_spec", "$@17", "exports", "export", "$@18", "$@19", "$@20",
  "attribute", "$@21", "$@22", "flag_header", "opt_attrflags",
  "opt_attrflag", "opt_attribute_block", "attribute_block_rest",
  "opt_attribute_raises", "opt_attribute_get_raises",
  "attribute_get_raises", "opt_attribute_set_raises",
  "attribute_set_raises", "$@23", "operation", "$@24", "$@25", "$@26",
  "$@27", "$@28", "operation_head", "$@29", "op_type_spec", "parameters",
  "$@30", "parameter", "$@31", "$@32", "direction", "opt_rest",
  "opt_raises", "raises", "$@33", "$@34", "exception_list",
  "exception_name", "interface_inheritance_decl", "$@35",
  "optional_inherited_interface", "constants_exports", "constants_export",
  "$@36", "const_dcl", "$@37", "$@38", "$@39", "$@40", "constants_dcl",
  "$@41", "$@42", "$@43", "$@44", "expression", "const_expr", "or_expr",
  "xor_expr", "and_expr", "shift_expr", "add_expr", "mult_expr",
  "unary_expr", "primary_expr", "literal", "positive_int_expr",
  "const_type", "exception_header", "$@45", "$@46", "exception_dcl",
  "$@47", "$@48", "$@49", "property", "$@50", "service_exports",
  "service_export", "service_interface_header", "service_service_header",
  "service_dcl", "$@51", "$@52", "service_dfn", "service_interface_dfn",
  "$@53", "opt_service_body", "service_body", "constructors",
  "constructor", "$@54", "$@55", "singleton_dcl", "$@56", "$@57",
  "singleton_dfn", "singleton_interface_dfn", "service_obsolete_dfn",
  "$@58", "$@59", "type_dcl", "$@60", "type_declarator", "$@61",
  "at_least_one_declarator", "declarators", "$@62", "declarator",
  "simple_declarator", "complex_declarator", "array_declarator", "$@63",
  "at_least_one_array_dim", "array_dims", "array_dim", "$@64", "$@65",
  "at_least_one_scoped_name", "scoped_names", "$@66", "scoped_name",
  "$@67", "$@68", "type_spec", "simple_type_spec", "fundamental_type",
  "opt_type_args", "type_args", "type_arg", "base_type_spec",
  "integer_type", "signed_int", "unsigned_int", "floating_pt_type",
  "char_type", "byte_type", "boolean_type", "any_type", "type_type",
  "string_type", "template_type_spec", "constructed_type_spec",
  "array_type", "$@69", "sequence_type_spec", "$@70", "$@71", "$@72",
  "struct_type", "$@73", "$@74", "$@75", "structure_header", "$@76",
  "$@77", "opt_type_params", "type_params", "at_least_one_member",
  "members", "member", "$@78", "$@79", "type_or_parameter", "enum_type",
  "$@80", "$@81", "$@82", "$@83", "at_least_one_enumerator", "enumerators",
  "$@84", "enumerator", "union_type", "$@85", "$@86", "$@87", "$@88",
  "$@89", "$@90", "$@91", "$@92", "switch_type_spec",
  "at_least_one_case_branch", "case_branches", "case_branch", "$@93",
  "at_least_one_case_label", "case_labels", "case_label", "$@94", "$@95",
  "$@96", "element_spec", "$@97", "$@98", "identifier", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-415)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-325)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     353,    -8,  -415,  -415,    19,  -415,    93,   748,  -415,  -415,
     147,  -415,  -415,     8,  -415,  -415,  -415,  -415,  -415,  -415,
    -415,  -415,  -415,  -415,  -415,  -415,   -34,  -415,  -415,  -415,
    -415,  -415,  -415,  -415,  -415,  -415,  -415,  -415,  -415,  -415,
    -415,    15,  -415,  -415,  -415,  -415,  -415,  -415,   677,   147,
     147,   147,   147,   147,   147,   147,   474,   147,    26,  -415,
    -415,    33,    37,    46,    53,    60,    64,    68,    76,    83,
    -415,    85,  -415,  -415,  -415,  -415,  -415,  -415,  -415,  -415,
     -13,  -415,  -415,    17,  -415,  -415,  -415,  -415,  -415,  -415,
    -415,  -415,  -415,  -415,  -415,  -415,  -415,  -415,  -415,    71,
    -415,  -415,  -415,  -415,  -415,   -35,  -415,    94,  -415,  -415,
    -415,  -415,  -415,  -415,  -415,  -415,  -415,  -415,  -415,  -415,
    -415,  -415,  -415,  -415,  -415,  -415,  -415,   133,  -415,  -415,
    -415,  -415,  -415,  -415,  -415,  -415,  -415,  -415,  -415,  -415,
    -415,   147,   147,  -415,   101,   103,   106,    48,    54,    97,
    -415,   100,   623,  -415,   147,   110,   155,    17,  -415,  -415,
     652,   441,  -415,  -415,   147,  -415,  -415,  -415,  -415,   133,
    -415,  -415,  -415,   133,  -415,  -415,  -415,   147,   106,  -415,
      94,   -47,  -415,  -415,  -415,  -415,  -415,  -415,   123,  -415,
    -415,  -415,  -415,     7,   294,   -32,   -35,  -415,   129,  -415,
    -415,  -415,  -415,   126,  -415,   196,   147,  -415,    17,    17,
      27,  -415,  -415,   623,   623,  -415,   136,   110,     9,   110,
     137,   723,   141,  -415,  -415,   623,  -415,   594,  -415,   194,
     144,  -415,  -415,   152,   158,   376,   147,   159,  -415,  -415,
     196,  -415,  -415,    12,   153,    73,   162,   147,  -415,   145,
    -415,  -415,  -415,  -415,  -415,  -415,  -415,  -415,     9,   195,
     195,   195,  -415,   160,   163,   157,    95,   -39,   -20,  -415,
    -415,  -415,  -415,    17,  -415,  -415,   168,  -415,  -415,  -415,
    -415,  -415,  -415,   171,  -415,  -415,  -415,  -415,  -415,    88,
    -415,  -415,   177,    94,   178,  -415,  -415,    94,   183,  -415,
    -415,  -415,  -415,  -415,     9,   182,  -415,   188,   193,   184,
     190,     9,   197,  -415,   133,   133,  -415,   778,   565,   198,
    -415,   133,   133,   201,  -415,  -415,  -415,  -415,   179,   147,
     192,  -415,  -415,  -415,     9,     9,     9,     9,     9,     9,
       9,     9,     9,     9,   199,   240,  -415,  -415,   200,  -415,
     778,  -415,   147,  -415,   147,  -415,   133,   206,  -415,  -415,
    -415,  -415,  -415,  -415,  -415,  -415,  -415,   209,    17,   210,
    -415,  -415,  -415,    94,  -415,   216,   220,  -415,   109,  -415,
    -415,  -415,   163,   157,    95,   -39,   -39,   -20,   -20,  -415,
    -415,  -415,  -415,    17,  -415,  -415,  -415,  -415,  -415,  -415,
    -415,  -415,  -415,  -415,    17,  -415,   147,  -415,   215,  -415,
     147,  -415,  -415,  -415,  -415,  -415,   214,   223,   218,  -415,
    -415,  -415,   219,  -415,    50,  -415,  -415,   133,    13,   226,
     227,   268,  -415,  -415,   232,   246,   252,    13,    17,   176,
      16,    59,  -415,  -415,  -415,  -415,   243,   268,  -415,  -415,
    -415,  -415,  -415,   238,  -415,    94,  -415,  -415,  -415,   241,
    -415,   268,    28,   244,  -415,   245,   239,  -415,    21,  -415,
    -415,  -415,  -415,  -415,  -415,  -415,  -415,  -415,  -415,  -415,
     623,   623,  -415,   248,     9,   247,   250,    28,   519,    28,
     133,   268,    94,  -415,  -415,  -415,  -415,  -415,   -30,  -415,
    -415,  -415,    75,  -415,    17,  -415,   260,   251,  -415,   147,
     133,  -415,  -415,   147,  -415,  -415,  -415,  -415,   255,  -415
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int16 yydefact[] =
{
       0,     0,    25,     9,     0,     2,     0,     0,     6,     8,
       0,     1,     3,     0,   125,   130,   316,   172,    32,   193,
     208,   300,   217,   328,     5,    17,    34,    31,    30,    35,
      13,    23,   175,    15,    19,    21,    11,   219,   296,   221,
     220,     0,   359,   360,   361,   362,    26,    39,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    41,
      40,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       7,     0,   278,   280,   279,   276,   277,   271,   270,   272,
       0,   245,   126,   171,   166,   268,   269,   170,   167,   168,
     169,   244,   131,   317,   173,    33,   194,   209,   301,     0,
     291,   281,   283,   282,   218,   256,   222,   249,   251,   253,
     260,   261,   262,   264,   263,   265,   266,   267,   254,   250,
     285,   284,   286,   288,   287,   329,    18,     0,    36,    14,
      24,   176,    16,    20,    22,    12,   297,    27,   274,   273,
     275,     0,     0,   247,     0,     0,    43,     0,     0,   304,
     295,     0,     0,   252,     0,     0,     0,    42,    45,   309,
       0,     0,   246,   127,     0,   132,   318,   174,   214,     0,
     195,   196,   197,     0,   210,   211,   212,     0,    43,   292,
     259,     0,   257,   223,   227,   228,   229,   231,   230,   237,
     290,   236,   330,    92,     0,     0,   256,   289,   314,   298,
     309,   310,    28,     0,   248,   122,     0,   183,   198,   213,
       0,   305,   302,     0,     0,   255,   224,     0,     0,   234,
       0,     0,     0,    44,    46,     0,    48,     0,    50,     0,
       0,   308,   313,   315,     0,     0,     0,     0,   128,   133,
     122,   123,   319,     0,   326,     0,   201,     0,   303,   289,
     258,   225,   233,   163,   164,   160,   161,   162,     0,     0,
       0,     0,   165,   136,   137,   139,   141,   143,   146,   149,
     153,   158,   238,   157,   235,   331,     0,    58,    63,    64,
      66,    67,    62,    61,    59,    60,    68,    65,    90,     0,
      57,    38,     0,    52,     0,    94,    84,    93,     0,   117,
     178,   299,   311,    29,     0,     0,   121,     0,     0,     0,
     321,     0,     0,   189,     0,     0,   191,     0,     0,     0,
     182,     0,     0,     0,   204,   199,   200,   306,     0,     0,
       0,   154,   155,   156,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    69,   119,     0,    55,
       0,    47,     0,    49,     0,    51,     0,     0,   129,   135,
     134,   124,   320,   325,   322,   327,   181,     0,   243,     0,
      61,   190,   192,   179,   188,     0,     0,   216,     0,   294,
     226,   159,   138,   140,   142,   144,   145,   147,   148,   150,
     151,   152,   239,   341,   337,   338,   339,   340,   332,    91,
      56,    53,   230,    85,   118,   312,     0,   187,   240,   186,
       0,   184,   185,   202,   203,   205,     0,    71,     0,   323,
     241,   180,     0,   333,     0,    54,    86,     0,     0,     0,
       0,     0,    82,    70,     0,    81,    78,     0,   242,     0,
       0,     0,    95,   334,    73,   111,     0,     0,    72,    74,
      80,    75,    77,    87,    99,   103,   104,   105,   106,     0,
      96,   110,     0,     0,    79,     0,     0,   100,     0,   206,
     109,   352,   350,   335,   344,   345,   349,   112,    83,    88,
       0,     0,    97,     0,     0,     0,     0,   342,     0,   347,
       0,   110,   101,   207,   353,   351,   336,   343,     0,   355,
     346,   348,     0,   114,   116,    89,   108,     0,   358,     0,
       0,   113,   107,     0,   354,   356,   115,   102,     0,   357
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -415,  -415,     4,  -415,  -415,  -415,  -415,  -415,  -415,  -415,
    -415,  -415,  -415,  -415,  -415,  -415,  -415,  -415,  -415,  -415,
    -415,  -415,  -415,  -415,  -415,  -415,  -415,  -415,  -130,  -415,
    -415,  -415,  -415,  -415,  -415,  -415,  -415,  -415,    77,  -415,
     -29,  -415,  -415,  -415,  -415,  -118,  -415,  -112,  -415,  -415,
    -415,  -415,  -415,  -415,  -415,  -415,  -415,  -415,  -113,  -415,
    -143,  -415,  -415,  -415,  -415,  -165,  -414,  -415,  -415,  -415,
    -174,  -415,  -415,  -415,   105,  -415,  -415,   334,  -415,  -415,
    -415,  -415,  -415,  -415,  -415,  -415,  -415,  -415,  -251,  -415,
      22,    14,    23,  -163,  -146,  -208,  -119,  -415,  -415,  -415,
    -415,  -415,  -415,  -415,  -415,  -415,  -415,  -415,  -415,  -415,
    -415,  -415,  -415,  -415,  -415,  -415,  -415,  -415,  -415,  -415,
    -415,  -415,  -415,  -415,  -415,  -415,  -415,  -415,  -415,  -415,
     202,  -415,  -415,  -415,  -415,  -415,  -415,  -227,  -415,  -415,
    -321,    -9,  -415,  -415,  -415,   134,  -415,   138,  -415,  -415,
    -297,  -415,  -415,   -45,  -415,  -415,  -133,   -55,  -145,   166,
    -415,   149,  -415,   -44,  -415,  -415,   319,   -43,   321,   -42,
    -415,  -415,  -415,  -415,  -415,  -415,  -415,  -415,  -415,  -415,
    -415,   364,  -415,  -415,  -415,  -415,  -415,  -415,  -415,  -415,
    -415,   172,   213,  -415,  -415,  -415,    -5,  -415,  -415,  -415,
    -415,  -415,  -415,  -415,   -31,   367,  -415,  -415,  -415,  -415,
    -415,  -415,  -415,  -415,  -415,  -415,  -415,  -111,  -415,  -415,
    -415,  -103,  -415,  -415,  -415,  -415,  -415,  -415,   -10
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,     4,     5,     6,    41,     7,    24,    68,    62,    65,
      58,    66,    67,    63,     8,    10,    71,   161,   237,    25,
      26,    52,    27,    28,    61,   158,   222,    29,    60,   127,
     193,   223,   292,   294,   298,   224,   352,   417,   225,   289,
     290,   425,   433,   434,   451,   435,   449,   436,   447,   226,
     354,   418,   437,   466,   491,   227,   348,   296,   441,   468,
     442,   480,   506,   459,   513,   469,   470,   463,   490,   502,
     503,   228,   356,   229,   239,   240,   307,   241,    48,   142,
     203,   304,    31,    49,   144,   205,   305,   358,   262,   263,
     264,   265,   266,   267,   268,   269,   270,   271,   272,    82,
      32,    51,   146,    33,    64,   159,   230,   319,   410,   245,
     320,   321,   322,    34,    53,   147,   170,   171,   246,   325,
     326,   378,   414,   422,   483,    35,    54,   148,   174,   175,
     172,   207,   323,    36,    56,   104,   154,   183,   216,   329,
     184,   185,   186,   187,   217,   190,   219,   191,   218,   344,
     367,   408,   427,   273,   141,   164,   106,   197,   108,   153,
     181,   182,   109,   110,    85,    86,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   155,   121,   151,   213,
     328,   122,    69,   160,   234,    38,    55,   149,   178,   210,
     199,   194,   231,   236,   357,   201,   123,    50,   145,   206,
     308,   242,   310,   406,   243,   124,    57,   156,   220,   345,
     416,   429,   462,   486,   398,   473,   487,   474,   488,   475,
     489,   476,   485,   484,   507,   500,   509,   518,    91
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      46,   107,    39,    83,    84,    88,    90,   330,   380,   302,
      12,   105,    42,   309,   439,   198,   167,   446,   369,    11,
    -120,   214,   481,   143,   375,   376,   138,   139,   140,   -43,
     232,    59,   508,   465,   471,   215,   339,   340,   472,    92,
      93,    94,    95,    96,    97,    98,   152,   125,   212,   198,
     150,   430,   150,   359,     9,   253,   254,   341,   342,   343,
     365,    43,    44,    45,   456,   457,   458,    81,   255,   256,
     257,   -37,    47,   221,   312,   143,  -324,    70,   258,   440,
    -324,   -98,   157,   -98,   259,   260,   313,   440,   126,   261,
     198,   314,   315,    -4,     1,   247,   128,   180,   316,   129,
     -10,   -10,   431,   432,   -10,   -10,   -10,   105,   130,   248,
       2,   168,    42,   169,   -76,   196,   131,   168,   -10,   173,
     -10,   -10,   132,   -10,   208,   -10,   133,   460,   209,   461,
     134,   162,   163,   389,   390,   391,    42,  -215,   135,   317,
     331,   332,   333,   510,   188,   511,   136,     3,   137,   196,
      42,   337,   338,   150,   204,   349,   350,    -4,   249,   180,
    -289,    43,    44,    45,   165,   202,   166,   211,   105,   105,
     293,    59,   297,   413,   385,   386,   189,    99,   177,    42,
     105,   179,   105,   421,   192,    43,    44,    45,   515,  -232,
     196,    81,   517,   387,   388,  -251,   244,   238,    42,    43,
      44,    45,   100,    14,   251,   291,   275,   299,   300,   101,
      72,    73,    74,    75,    76,    77,    78,    79,  -252,   102,
     103,    80,   301,   303,   311,   324,   188,  -293,    43,    44,
      45,   336,   334,   494,    81,   346,   335,   327,   347,   351,
     353,   253,   254,    42,   454,   355,   360,    43,    44,    45,
     361,    16,   363,    81,   255,   256,   257,   362,   364,   366,
     374,   379,   381,   373,   258,   377,   392,   399,   405,   368,
     368,   407,   409,   105,    72,    73,   368,   368,   411,    77,
      78,    79,   412,   420,   423,    80,   424,   426,   428,   443,
     445,   444,    43,    44,    45,   195,   448,    42,    81,   432,
     393,   394,   395,   396,   431,   464,   460,   478,   467,   479,
     493,   404,   495,   477,   496,   512,   514,   519,   452,   188,
     100,   400,   318,   450,   453,   482,   505,   101,    72,    73,
      74,    75,    76,    77,    78,    79,   516,   102,   103,    80,
     397,    30,   402,   401,   403,   306,    43,    44,    45,   383,
     176,   252,    81,    -4,     1,   499,   382,   274,  -177,   384,
     -10,   -10,   233,   250,   -10,   -10,   -10,    87,   415,    89,
       2,    37,   235,   200,    40,   419,   497,   195,   -10,    42,
     -10,   -10,   438,   -10,   455,   -10,   501,     0,     0,     0,
       0,     0,     0,     0,   105,     0,   244,     0,     0,     0,
     188,     0,   100,     0,     0,     0,     0,     3,     0,   101,
      72,    73,    74,    75,    76,    77,    78,    79,     0,   102,
     103,    80,     0,     0,     0,   492,   455,     0,    43,    44,
      45,     0,     0,   107,    81,   105,   105,     0,     0,     0,
    -307,     0,     1,   105,     0,   504,     0,     0,   -10,   -10,
       0,     0,   -10,   -10,   -10,     0,     0,     0,     2,     0,
       0,     0,     0,     0,     0,   504,   -10,     0,   -10,   -10,
       0,   -10,     0,   -10,     0,    99,     0,    42,     0,     0,
       0,     0,     0,     0,     0,    16,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     3,     0,     0,     0,   188,
     100,     0,    21,   188,     0,    -4,    23,   101,    72,    73,
      74,    75,    76,    77,    78,    79,     0,   102,   103,    80,
     498,     0,    42,     0,     0,     0,    43,    44,    45,     0,
      16,     0,    81,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   100,     0,    21,     0,     0,
       0,    23,   101,    72,    73,    74,    75,    76,    77,    78,
      79,     0,   102,   103,    80,     0,    99,     0,    42,     0,
       0,    43,    44,    45,     0,     0,     0,    81,   371,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     372,   100,     0,     0,     0,    99,     0,    42,   101,    72,
      73,    74,    75,    76,    77,    78,    79,     0,   102,   103,
      80,     0,     0,     0,     0,     0,     0,    43,    44,    45,
     100,     0,     0,    81,    99,     0,    42,   101,    72,    73,
      74,    75,    76,    77,    78,    79,   295,   102,   103,    80,
       0,     0,     0,     0,     0,     0,    43,    44,    45,   100,
       0,     0,    81,   195,     0,    42,   101,    72,    73,    74,
      75,    76,    77,    78,    79,     0,   102,   103,    80,     0,
       0,     0,     0,     0,     0,    43,    44,    45,   100,     0,
      42,    81,     0,     0,     0,   101,    72,    73,    74,    75,
      76,    77,    78,    79,     0,   102,   103,    80,     0,     0,
       0,     0,     0,     0,    43,    44,    45,     0,     0,     0,
      81,    72,    73,    74,    75,    76,    77,    78,    79,     0,
       0,     0,    80,     0,   276,     0,     0,   277,   278,    43,
      44,    45,   279,     0,     0,    81,     0,   280,   281,   282,
       0,     0,     0,   283,   284,     0,   285,   286,     0,    13,
       0,     0,     0,     0,   287,    14,    15,     0,     0,    16,
      17,    18,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    19,   288,    20,    21,     0,    22,   276,
      23,     0,   277,   278,     0,     0,     0,   279,     0,     0,
       0,     0,   280,   281,   282,     0,     0,     0,   370,   284,
       0,   285,   286,     0,     0,     0,     0,     0,     0,   287
};

static const yytype_int16 yycheck[] =
{
      10,    56,     7,    48,    48,    48,    48,   258,   329,   236,
       6,    56,     3,     1,     1,   160,   146,   431,   315,     0,
      13,    68,     1,    58,   321,   322,    39,    40,    41,    63,
      62,    65,    62,   447,     6,    82,    75,    76,    10,    49,
      50,    51,    52,    53,    54,    55,    81,    57,   178,   194,
      82,     1,    82,   304,    62,    46,    47,    77,    78,    79,
     311,    52,    53,    54,    48,    49,    50,    58,    59,    60,
      61,    64,    64,    66,     1,    58,    64,    62,    69,    66,
      68,    68,   127,    70,    75,    76,    13,    66,    62,    80,
     235,    18,    19,     0,     1,    68,    63,   152,    25,    62,
       7,     8,    52,    53,    11,    12,    13,   152,    62,    82,
      17,    63,     3,    65,    64,   160,    63,    63,    25,    65,
      27,    28,    62,    30,   169,    32,    62,    68,   173,    70,
      62,   141,   142,   341,   342,   343,     3,    64,    62,    66,
     259,   260,   261,    68,   154,    70,    63,    54,    63,   194,
       3,    56,    57,    82,   164,    67,    68,    64,   213,   214,
      66,    52,    53,    54,    63,   161,    63,   177,   213,   214,
     225,    65,   227,    64,   337,   338,    66,     1,    81,     3,
     225,    81,   227,   410,    29,    52,    53,    54,   509,    66,
     235,    58,   513,   339,   340,    66,   206,    71,     3,    52,
      53,    54,    26,     7,    68,    64,    69,    13,    64,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    66,    43,
      44,    45,    64,    64,    71,    63,   236,    82,    52,    53,
      54,    74,    72,   484,    58,    67,    73,   247,    67,    62,
      62,    46,    47,     3,    68,    62,    64,    52,    53,    54,
      62,    11,    68,    58,    59,    60,    61,    64,    68,    62,
      62,    82,    70,   318,    69,    64,    67,    67,    62,   314,
     315,    62,    62,   318,    34,    35,   321,   322,    62,    39,
      40,    41,    62,    68,    70,    45,    63,    69,    69,    63,
      22,    64,    52,    53,    54,     1,    64,     3,    58,    53,
     345,   345,   345,   345,    52,    62,    68,    62,    67,    70,
      62,   356,    65,    69,    64,    55,    65,    62,   436,   329,
      26,   350,   245,   435,   437,   468,   491,    33,    34,    35,
      36,    37,    38,    39,    40,    41,   510,    43,    44,    45,
     345,     7,   352,   352,   354,   240,    52,    53,    54,   335,
     148,   217,    58,     0,     1,   488,   334,   219,    64,   336,
       7,     8,   196,   214,    11,    12,    13,    48,   378,    48,
      17,     7,   200,   160,     7,   406,   487,     1,    25,     3,
      27,    28,   427,    30,   439,    32,   489,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   439,    -1,   406,    -1,    -1,    -1,
     410,    -1,    26,    -1,    -1,    -1,    -1,    54,    -1,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    -1,    43,
      44,    45,    -1,    -1,    -1,   480,   481,    -1,    52,    53,
      54,    -1,    -1,   488,    58,   480,   481,    -1,    -1,    -1,
      64,    -1,     1,   488,    -1,   490,    -1,    -1,     7,     8,
      -1,    -1,    11,    12,    13,    -1,    -1,    -1,    17,    -1,
      -1,    -1,    -1,    -1,    -1,   510,    25,    -1,    27,    28,
      -1,    30,    -1,    32,    -1,     1,    -1,     3,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    11,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    54,    -1,    -1,    -1,   509,
      26,    -1,    28,   513,    -1,    64,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    -1,    43,    44,    45,
       1,    -1,     3,    -1,    -1,    -1,    52,    53,    54,    -1,
      11,    -1,    58,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    26,    -1,    28,    -1,    -1,
      -1,    32,    33,    34,    35,    36,    37,    38,    39,    40,
      41,    -1,    43,    44,    45,    -1,     1,    -1,     3,    -1,
      -1,    52,    53,    54,    -1,    -1,    -1,    58,    13,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      25,    26,    -1,    -1,    -1,     1,    -1,     3,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    -1,    43,    44,
      45,    -1,    -1,    -1,    -1,    -1,    -1,    52,    53,    54,
      26,    -1,    -1,    58,     1,    -1,     3,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      -1,    -1,    -1,    -1,    -1,    -1,    52,    53,    54,    26,
      -1,    -1,    58,     1,    -1,     3,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    -1,    43,    44,    45,    -1,
      -1,    -1,    -1,    -1,    -1,    52,    53,    54,    26,    -1,
       3,    58,    -1,    -1,    -1,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    -1,    43,    44,    45,    -1,    -1,
      -1,    -1,    -1,    -1,    52,    53,    54,    -1,    -1,    -1,
      58,    34,    35,    36,    37,    38,    39,    40,    41,    -1,
      -1,    -1,    45,    -1,     1,    -1,    -1,     4,     5,    52,
      53,    54,     9,    -1,    -1,    58,    -1,    14,    15,    16,
      -1,    -1,    -1,    20,    21,    -1,    23,    24,    -1,     1,
      -1,    -1,    -1,    -1,    31,     7,     8,    -1,    -1,    11,
      12,    13,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    25,    51,    27,    28,    -1,    30,     1,
      32,    -1,     4,     5,    -1,    -1,    -1,     9,    -1,    -1,
      -1,    -1,    14,    15,    16,    -1,    -1,    -1,    20,    21,
      -1,    23,    24,    -1,    -1,    -1,    -1,    -1,    -1,    31
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int16 yystos[] =
{
       0,     1,    17,    54,    84,    85,    86,    88,    97,    62,
      98,     0,    85,     1,     7,     8,    11,    12,    13,    25,
      27,    28,    30,    32,    89,   102,   103,   105,   106,   110,
     160,   165,   183,   186,   196,   208,   216,   264,   268,   279,
     288,    87,     3,    52,    53,    54,   311,    64,   161,   166,
     280,   184,   104,   197,   209,   269,   217,   289,    93,    65,
     111,   107,    91,    96,   187,    92,    94,    95,    90,   265,
      62,    99,    34,    35,    36,    37,    38,    39,    40,    41,
      45,    58,   182,   236,   246,   247,   248,   249,   250,   251,
     252,   311,   311,   311,   311,   311,   311,   311,   311,     1,
      26,    33,    43,    44,   218,   236,   239,   240,   241,   245,
     246,   249,   250,   251,   252,   253,   254,   255,   256,   257,
     258,   260,   264,   279,   288,   311,    62,   112,    63,    62,
      62,    63,    62,    62,    62,    62,    63,    63,    39,    40,
      41,   237,   162,    58,   167,   281,   185,   198,   210,   270,
      82,   261,    81,   242,   219,   259,   290,   236,   108,   188,
     266,   100,   311,   311,   238,    63,    63,   111,    63,    65,
     199,   200,   213,    65,   211,   212,   213,    81,   271,    81,
     240,   243,   244,   220,   223,   224,   225,   226,   311,    66,
     228,   230,    29,   113,   274,     1,   236,   240,   241,   273,
     275,   278,    85,   163,   311,   168,   282,   214,   236,   236,
     272,   311,   111,   262,    68,    82,   221,   227,   231,   229,
     291,    66,   109,   114,   118,   121,   132,   138,   154,   156,
     189,   275,    62,   242,   267,   274,   276,   101,    71,   157,
     158,   160,   284,   287,   311,   192,   201,    68,    82,   240,
     244,    68,   228,    46,    47,    59,    60,    61,    69,    75,
      76,    80,   171,   172,   173,   174,   175,   176,   177,   178,
     179,   180,   181,   236,   230,    69,     1,     4,     5,     9,
      14,    15,    16,    20,    21,    23,    24,    31,    51,   122,
     123,    64,   115,   240,   116,    42,   140,   240,   117,    13,
      64,    64,   220,    64,   164,   169,   157,   159,   283,     1,
     285,    71,     1,    13,    18,    19,    25,    66,   121,   190,
     193,   194,   195,   215,    63,   202,   203,   311,   263,   222,
     171,   179,   179,   179,    72,    73,    74,    56,    57,    75,
      76,    77,    78,    79,   232,   292,    67,    67,   139,    67,
      68,    62,   119,    62,   133,    62,   155,   277,   170,   171,
      64,    62,    64,    68,    68,   171,    62,   233,   236,   233,
      20,    13,    25,   240,    62,   233,   233,    64,   204,    82,
     223,    70,   173,   174,   175,   176,   176,   177,   177,   178,
     178,   178,    67,   236,   246,   250,   252,   279,   297,    67,
     123,   224,   311,   311,   236,    62,   286,    62,   234,    62,
     191,    62,    62,    64,   205,   311,   293,   120,   134,   287,
      68,   220,   206,    70,    63,   124,    69,   235,    69,   294,
       1,    52,    53,   125,   126,   128,   130,   135,   236,     1,
      66,   141,   143,    63,    64,    22,   149,   131,    64,   129,
     130,   127,   128,   141,    68,   240,    48,    49,    50,   146,
      68,    70,   295,   150,    62,   149,   136,    67,   142,   148,
     149,     6,    10,   298,   300,   302,   304,    69,    62,    70,
     144,     1,   143,   207,   306,   305,   296,   299,   301,   303,
     151,   137,   240,    62,   171,    65,    64,   300,     1,   239,
     308,   304,   152,   153,   236,   148,   145,   307,    62,   309,
      68,    70,    55,   147,    65,   223,   153,   223,   310,    62
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int16 yyr1[] =
{
       0,    83,    84,    85,    85,    86,    87,    86,    86,    88,
      88,    90,    89,    91,    89,    92,    89,    93,    89,    94,
      89,    95,    89,    96,    89,    98,    99,   100,   101,    97,
     102,   102,   104,   103,   105,   107,   108,   109,   106,   106,
     110,   112,   111,   111,   113,   113,   115,   114,   116,   114,
     117,   114,   119,   120,   118,   121,   122,   122,   123,   123,
     123,   123,   123,   123,   123,   123,   123,   123,   123,   123,
     124,   124,   125,   125,   126,   126,   126,   127,   127,   128,
     129,   129,   131,   130,   133,   134,   135,   136,   137,   132,
     139,   138,   138,   140,   140,   141,   142,   141,   141,   141,
     144,   145,   143,   143,   146,   146,   146,   147,   147,   148,
     148,   150,   151,   149,   152,   152,   153,   155,   154,   156,
     156,   157,   157,   159,   158,   161,   162,   163,   164,   160,
     166,   167,   168,   169,   165,   170,   171,   172,   172,   173,
     173,   174,   174,   175,   175,   175,   176,   176,   176,   177,
     177,   177,   177,   178,   178,   178,   178,   179,   179,   179,
     180,   180,   180,   180,   180,   181,   182,   182,   182,   182,
     182,   182,   184,   185,   183,   187,   188,   189,   186,   191,
     190,   190,   192,   192,   193,   193,   193,   193,   193,   194,
     194,   195,   195,   197,   198,   196,   199,   199,   201,   200,
     202,   202,   203,   204,   204,   206,   207,   205,   209,   210,
     208,   211,   211,   212,   214,   215,   213,   217,   216,   216,
     216,   216,   219,   218,   220,   222,   221,   221,   223,   223,
     224,   225,   227,   226,   228,   229,   229,   231,   232,   230,
     233,   235,   234,   234,   236,   237,   236,   238,   236,   239,
     239,   240,   240,   241,   241,   242,   242,   243,   243,   244,
     245,   245,   245,   245,   245,   245,   245,   245,   246,   246,
     247,   247,   247,   248,   248,   248,   249,   249,   250,   251,
     252,   253,   254,   255,   256,   256,   257,   257,   257,   259,
     258,   261,   262,   263,   260,   260,   265,   266,   267,   264,
     269,   270,   268,   271,   271,   272,   272,   273,   274,   274,
     276,   277,   275,   275,   278,   278,   280,   281,   282,   283,
     279,   284,   286,   285,   285,   285,   287,   287,   289,   290,
     291,   292,   293,   294,   295,   296,   288,   297,   297,   297,
     297,   297,   298,   299,   299,   301,   300,   302,   303,   303,
     305,   304,   306,   307,   304,   309,   310,   308,   308,   311,
     311,   311,   311
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     2,     0,     2,     0,     3,     2,     1,
       0,     0,     3,     0,     3,     0,     3,     0,     3,     0,
       3,     0,     3,     0,     3,     0,     0,     0,     0,     9,
       1,     1,     0,     3,     1,     0,     0,     0,     7,     2,
       2,     0,     3,     0,     2,     0,     0,     3,     0,     3,
       0,     3,     0,     0,     6,     3,     3,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     2,
       2,     0,     2,     2,     2,     2,     0,     1,     0,     3,
       1,     0,     0,     4,     0,     0,     0,     0,     0,    12,
       0,     4,     0,     1,     1,     1,     0,     4,     0,     2,
       0,     0,     8,     2,     1,     1,     1,     1,     0,     1,
       0,     0,     0,     6,     1,     3,     1,     0,     4,     3,
       0,     2,     0,     0,     3,     0,     0,     0,     0,     9,
       0,     0,     0,     0,     9,     1,     1,     1,     3,     1,
       3,     1,     3,     1,     3,     3,     1,     3,     3,     1,
       3,     3,     3,     1,     2,     2,     2,     1,     1,     3,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     0,     0,     5,     0,     0,     0,     7,     0,
       4,     2,     2,     0,     3,     3,     3,     3,     2,     1,
       2,     1,     2,     0,     0,     5,     1,     1,     0,     4,
       1,     0,     3,     2,     0,     0,     0,     8,     0,     0,
       5,     1,     1,     2,     0,     0,     5,     0,     3,     1,
       1,     1,     0,     3,     2,     0,     4,     0,     1,     1,
       1,     1,     0,     3,     2,     2,     0,     0,     0,     5,
       2,     0,     4,     0,     1,     0,     3,     0,     4,     1,
       1,     1,     2,     1,     1,     3,     0,     1,     3,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     2,     2,     2,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     0,
       3,     0,     0,     0,     7,     2,     0,     0,     0,     7,
       0,     0,     6,     3,     0,     1,     3,     2,     2,     0,
       0,     0,     5,     2,     1,     2,     0,     0,     0,     0,
       9,     2,     0,     4,     0,     2,     1,     3,     0,     0,
       0,     0,     0,     0,     0,     0,    17,     1,     1,     1,
       1,     1,     2,     2,     0,     0,     3,     2,     2,     0,
       0,     3,     0,     0,     5,     0,     0,     5,     2,     1,
       1,     1,     1
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
  case 6: /* $@1: %empty  */
#line 425 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_ModuleDeclSeen);
	}
#line 2123 "main/idlc/source/parser.cxx"
    break;

  case 7: /* definition: module_dcl $@1 ';'  */
#line 429 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_NoState);
	}
#line 2131 "main/idlc/source/parser.cxx"
    break;

  case 8: /* definition: error ';'  */
#line 433 "main/idlc/source/parser.y"
        {
		yyerror("definitions");
		yyerrok;
	}
#line 2140 "main/idlc/source/parser.cxx"
    break;

  case 9: /* opt_published: IDL_PUBLISHED  */
#line 440 "main/idlc/source/parser.y"
                  { idlc()->setPublished(true); }
#line 2146 "main/idlc/source/parser.cxx"
    break;

  case 10: /* opt_published: %empty  */
#line 441 "main/idlc/source/parser.y"
                  { idlc()->setPublished(false); }
#line 2152 "main/idlc/source/parser.cxx"
    break;

  case 11: /* $@2: %empty  */
#line 446 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_TypeDeclSeen);
	}
#line 2160 "main/idlc/source/parser.cxx"
    break;

  case 12: /* publishable_definition: type_dcl $@2 ';'  */
#line 450 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_NoState);
	}
#line 2168 "main/idlc/source/parser.cxx"
    break;

  case 13: /* $@3: %empty  */
#line 454 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_ConstantDeclSeen);
	}
#line 2176 "main/idlc/source/parser.cxx"
    break;

  case 14: /* publishable_definition: const_dcl $@3 ';'  */
#line 458 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_NoState);
	}
#line 2184 "main/idlc/source/parser.cxx"
    break;

  case 15: /* $@4: %empty  */
#line 462 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_ExceptionDeclSeen);
	}
#line 2192 "main/idlc/source/parser.cxx"
    break;

  case 16: /* publishable_definition: exception_dcl $@4 ';'  */
#line 466 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_NoState);
	}
#line 2200 "main/idlc/source/parser.cxx"
    break;

  case 17: /* $@5: %empty  */
#line 470 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_InterfaceDeclSeen);
	}
#line 2208 "main/idlc/source/parser.cxx"
    break;

  case 18: /* publishable_definition: interface $@5 ';'  */
#line 474 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_NoState);
	}
#line 2216 "main/idlc/source/parser.cxx"
    break;

  case 19: /* $@6: %empty  */
#line 478 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_ServiceDeclSeen);
	}
#line 2224 "main/idlc/source/parser.cxx"
    break;

  case 20: /* publishable_definition: service_dcl $@6 ';'  */
#line 482 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_NoState);
	}
#line 2232 "main/idlc/source/parser.cxx"
    break;

  case 21: /* $@7: %empty  */
#line 486 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_SingletonDeclSeen);
	}
#line 2240 "main/idlc/source/parser.cxx"
    break;

  case 22: /* publishable_definition: singleton_dcl $@7 ';'  */
#line 490 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_NoState);
	}
#line 2248 "main/idlc/source/parser.cxx"
    break;

  case 23: /* $@8: %empty  */
#line 494 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_ConstantsDeclSeen);
	}
#line 2256 "main/idlc/source/parser.cxx"
    break;

  case 24: /* publishable_definition: constants_dcl $@8 ';'  */
#line 498 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_NoState);
	}
#line 2264 "main/idlc/source/parser.cxx"
    break;

  case 25: /* $@9: %empty  */
#line 505 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_ModuleSeen);
        idlc()->setPublished(false);
	}
#line 2273 "main/idlc/source/parser.cxx"
    break;

  case 26: /* $@10: %empty  */
#line 510 "main/idlc/source/parser.y"
        {
        idlc()->setParseState(PS_ModuleIDSeen);
        checkIdentifier((yyvsp[0].sval));

        AstScope* 		pScope = idlc()->scopes()->topNonNull();
        AstModule* 		pModule = NULL;
        AstDeclaration*	pExists = NULL;

        if ( pScope )
        {
        	pModule = new AstModule(*(yyvsp[0].sval), pScope);
			if( (pExists = pScope->lookupForAdd(pModule)) )
			{
				pExists->setInMainfile(idlc()->isInMainFile());
				pExists->setFileName(pModule->getFileName());
                if (pExists->isPredefined())
                {
                    pExists->setPredefined(false);
                    if (pExists->getDocumentation().getLength() == 0 &&
                        pModule->getDocumentation().getLength() > 0)
                    {
                        pExists->setDocumentation(pModule->getDocumentation());
                    }
                }
				delete(pModule);
				pModule = (AstModule*)pExists;
			} else
			{
				pScope->addDeclaration(pModule);
			}
			idlc()->scopes()->push(pModule);
        }
        delete (yyvsp[0].sval);
    }
#line 2312 "main/idlc/source/parser.cxx"
    break;

  case 27: /* $@11: %empty  */
#line 545 "main/idlc/source/parser.y"
    {
        idlc()->setParseState(PS_ModuleSqSeen);
    }
#line 2320 "main/idlc/source/parser.cxx"
    break;

  case 28: /* $@12: %empty  */
#line 549 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_ModuleBodySeen);
	}
#line 2328 "main/idlc/source/parser.cxx"
    break;

  case 29: /* module_dcl: IDL_MODULE $@9 identifier $@10 '{' $@11 definitions $@12 '}'  */
#line 553 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_ModuleQsSeen);
		/*
		 * Finished with this module - pop it from the scope stack
		 */
		idlc()->scopes()->pop();
	}
#line 2340 "main/idlc/source/parser.cxx"
    break;

  case 32: /* $@13: %empty  */
#line 569 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_InterfaceSeen);
	}
#line 2348 "main/idlc/source/parser.cxx"
    break;

  case 33: /* interface_decl: IDL_INTERFACE $@13 identifier  */
#line 573 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_InterfaceIDSeen);
       checkIdentifier((yyvsp[0].sval));
		(yyval.sval) = (yyvsp[0].sval);
	}
#line 2358 "main/idlc/source/parser.cxx"
    break;

  case 34: /* forward_dcl: interface_decl  */
#line 582 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_ForwardDeclSeen);

		AstScope* 		pScope = idlc()->scopes()->topNonNull();
		AstInterface*	pForward = NULL;
		AstDeclaration*	pDecl = NULL;

        /*
		 * Make a new forward interface node and add it to its enclosing scope
		 */
		if ( pScope && (yyvsp[0].sval) ) 
		{
			pForward = new AstInterface(*(yyvsp[0].sval), NULL, pScope);
			
			if ( pDecl = pScope->lookupByName(pForward->getScopedName()) ) 
			{
				if ( (pDecl != pForward) && 
					 (pDecl->getNodeType() == NT_interface) )
				{
					delete pForward;
				} else
				{
					idlc()->error()->error2(EIDL_REDEF_SCOPE, scopeAsDecl(pScope), pDecl);
				}
			} else
			{
				/*
				 * Add the interface to its definition scope
				 */
				pScope->addDeclaration(pForward);
			}
		}
		delete (yyvsp[0].sval);
	}
#line 2397 "main/idlc/source/parser.cxx"
    break;

  case 35: /* $@14: %empty  */
#line 620 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_InterfaceHeadSeen);

		AstScope* 		pScope = idlc()->scopes()->topNonNull();
		AstInterface*	pInterface = NULL;
		AstInterface*	pForward = NULL;
		AstDeclaration*	pDecl = NULL;

        /*
		 * Make a new interface node and add it to its enclosing scope
		 */
		if ( pScope && (yyvsp[0].ihval) ) 
		{
			pInterface = new AstInterface(
                *(yyvsp[0].ihval)->getName(),
                static_cast< AstInterface * >((yyvsp[0].ihval)->getInherits()), pScope);
			if ( pInterface &&
				(pDecl = pScope->lookupByName(pInterface->getScopedName())) ) 
			{
				/*
				 * See if we're defining a forward declared interface.
				 */
				if (pDecl->getNodeType() == NT_interface) 
				{
					pForward = (AstInterface*)pDecl;
					if ( !pForward->isDefined() ) 
					{
						/*
						 * Check if redefining in same scope
						 */
						if ( pForward->getScope() != pScope ) 
						{
							if ( pForward->getScopedName() != pInterface->getScopedName() )
							{
								idlc()->error()->error3(EIDL_SCOPE_CONFLICT,
										 pInterface, pForward, scopeAsDecl(pScope));
							}
						}
                        else if ( !pInterface->isPublished()
                                  && pForward->isPublished() )
                        {
                            idlc()->error()->error0(EIDL_PUBLISHED_FORWARD);
                        }
						/*
						 * All OK, set full definition
						 */
						else 
						{
                            pForward->forwardDefined(*pInterface);
							delete pInterface;
							pInterface = pForward;
						}
					} else {
                        // special handling for XInterface because it is predefined
                        if ( pForward->isPredefined() &&
                             pForward->getScopedName() == "com::sun::star::uno::XInterface")
                        {
                            /* replace the predefined XInterface */
                            *pForward = *pInterface;
                            delete pInterface;
                            pInterface = pForward;
                        }
                        
                    }
				}
			} else
			{
				/*
				 * Add the interface to its definition scope
				 */
				pScope->addDeclaration(pInterface);
			}
		}
		/*
		 * Push it on the scope stack
		 */
		idlc()->scopes()->push(pInterface);
		delete((yyvsp[0].ihval));
	}
#line 2481 "main/idlc/source/parser.cxx"
    break;

  case 36: /* $@15: %empty  */
#line 700 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_InterfaceSqSeen);
	}
#line 2489 "main/idlc/source/parser.cxx"
    break;

  case 37: /* $@16: %empty  */
#line 704 "main/idlc/source/parser.y"
        {
        AstInterface * ifc = static_cast< AstInterface * >(
            idlc()->scopes()->topNonNull());
        if (!ifc->hasMandatoryInheritedInterfaces()
            && ifc->getScopedName() != "com::sun::star::uno::XInterface")
        {
            addInheritedInterface(
                ifc, rtl::OString("::com::sun::star::uno::XInterface"), false,
                rtl::OUString());
        }
        ifc->setDefined();
		idlc()->setParseState(PS_InterfaceBodySeen);
	}
#line 2507 "main/idlc/source/parser.cxx"
    break;

  case 38: /* interface_dcl: interfaceheader $@14 '{' $@15 exports $@16 '}'  */
#line 718 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_InterfaceQsSeen);
		/*
		 * Done with this interface - pop it off the scopes stack
		 */
		idlc()->scopes()->pop();
	}
#line 2519 "main/idlc/source/parser.cxx"
    break;

  case 39: /* interface_dcl: error '}'  */
#line 726 "main/idlc/source/parser.y"
        {
		yyerror("interface definition");
		yyerrok;
	}
#line 2528 "main/idlc/source/parser.cxx"
    break;

  case 40: /* interfaceheader: interface_decl inheritance_spec  */
#line 734 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_InheritSpecSeen);

		(yyval.ihval) = new FeInheritanceHeader(NT_interface, (yyvsp[-1].sval), (yyvsp[0].sval), 0);
		delete (yyvsp[0].sval);
	}
#line 2539 "main/idlc/source/parser.cxx"
    break;

  case 41: /* $@17: %empty  */
#line 744 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_InheritColonSeen);
	}
#line 2547 "main/idlc/source/parser.cxx"
    break;

  case 42: /* inheritance_spec: ':' $@17 scoped_name  */
#line 748 "main/idlc/source/parser.y"
        {
        (yyval.sval) = (yyvsp[0].sval);
	}
#line 2555 "main/idlc/source/parser.cxx"
    break;

  case 43: /* inheritance_spec: %empty  */
#line 752 "main/idlc/source/parser.y"
        {
		(yyval.sval) = NULL;
	}
#line 2563 "main/idlc/source/parser.cxx"
    break;

  case 46: /* $@18: %empty  */
#line 764 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_AttributeDeclSeen);
	}
#line 2571 "main/idlc/source/parser.cxx"
    break;

  case 47: /* export: attribute $@18 ';'  */
#line 768 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_NoState);
	}
#line 2579 "main/idlc/source/parser.cxx"
    break;

  case 48: /* $@19: %empty  */
#line 772 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_OperationDeclSeen);
	}
#line 2587 "main/idlc/source/parser.cxx"
    break;

  case 49: /* export: operation $@19 ';'  */
#line 776 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_NoState);
	}
#line 2595 "main/idlc/source/parser.cxx"
    break;

  case 50: /* $@20: %empty  */
#line 780 "main/idlc/source/parser.y"
    {
		idlc()->setParseState(PS_InterfaceInheritanceDeclSeen);
    }
#line 2603 "main/idlc/source/parser.cxx"
    break;

  case 51: /* export: interface_inheritance_decl $@20 ';'  */
#line 784 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_NoState);
	}
#line 2611 "main/idlc/source/parser.cxx"
    break;

  case 52: /* $@21: %empty  */
#line 792 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_AttrTypeSeen);
	}
#line 2619 "main/idlc/source/parser.cxx"
    break;

  case 53: /* $@22: %empty  */
#line 796 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_AttrCompleted);
        if (((yyvsp[-3].ulval) & ~(AF_BOUND | AF_READONLY)) != AF_ATTRIBUTE) {
            idlc()->error()->flagError(EIDL_BAD_ATTRIBUTE_FLAGS, (yyvsp[-3].ulval));
        }
        AstInterface * scope = static_cast< AstInterface * >(
            idlc()->scopes()->top());
        AstAttribute * attr = new AstAttribute(
            (yyvsp[-3].ulval), (yyvsp[0].fdval)->compose((yyvsp[-2].cdclval)), (yyvsp[0].fdval)->getName(), scope);
        delete (yyvsp[0].fdval);
        AstInterface::DoubleMemberDeclarations doubleMembers(
            scope->checkMemberClashes(attr));
        if (doubleMembers.empty()) {
            scope->addMember(attr);
        } else {
            reportDoubleMemberDeclarations(doubleMembers);
        }
        idlc()->scopes()->push(attr);
    }
#line 2643 "main/idlc/source/parser.cxx"
    break;

  case 54: /* attribute: flag_header simple_type_spec $@21 simple_declarator $@22 opt_attribute_block  */
#line 816 "main/idlc/source/parser.y"
    {
        static_cast< AstAttribute * >(idlc()->scopes()->top())->setExceptions(
            (yyvsp[0].attexcval).get.documentation, (yyvsp[0].attexcval).get.exceptions, (yyvsp[0].attexcval).set.documentation,
            (yyvsp[0].attexcval).set.exceptions);
        delete (yyvsp[0].attexcval).get.documentation;
        delete (yyvsp[0].attexcval).get.exceptions;
        delete (yyvsp[0].attexcval).set.documentation;
        delete (yyvsp[0].attexcval).set.exceptions;
        idlc()->scopes()->pop();
    }
#line 2658 "main/idlc/source/parser.cxx"
    break;

  case 55: /* flag_header: '[' opt_attrflags ']'  */
#line 830 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_FlagHeaderSeen);
		(yyval.ulval) = (yyvsp[-1].ulval);
	}
#line 2667 "main/idlc/source/parser.cxx"
    break;

  case 56: /* opt_attrflags: opt_attrflags ',' opt_attrflag  */
#line 838 "main/idlc/source/parser.y"
        {
		if ( ((yyvsp[-2].ulval) & (yyvsp[0].ulval)) == (yyvsp[0].ulval) )
 			idlc()->error()->flagError(EIDL_DEFINED_ATTRIBUTEFLAG, (yyvsp[0].ulval));
 
 		(yyval.ulval) = (yyvsp[-2].ulval) | (yyvsp[0].ulval);
	}
#line 2678 "main/idlc/source/parser.cxx"
    break;

  case 57: /* opt_attrflags: opt_attrflag  */
#line 845 "main/idlc/source/parser.y"
        {
		(yyval.ulval) = (yyvsp[0].ulval);
	}
#line 2686 "main/idlc/source/parser.cxx"
    break;

  case 58: /* opt_attrflag: IDL_ATTRIBUTE  */
#line 852 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_AttrSeen);
		(yyval.ulval) = AF_ATTRIBUTE;
	}
#line 2695 "main/idlc/source/parser.cxx"
    break;

  case 59: /* opt_attrflag: IDL_PROPERTY  */
#line 857 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_PropertySeen);
		(yyval.ulval) = AF_PROPERTY;
	}
#line 2704 "main/idlc/source/parser.cxx"
    break;

  case 60: /* opt_attrflag: IDL_READONLY  */
#line 862 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_ReadOnlySeen);
		(yyval.ulval) = AF_READONLY;
	}
#line 2713 "main/idlc/source/parser.cxx"
    break;

  case 61: /* opt_attrflag: IDL_OPTIONAL  */
#line 867 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_OptionalSeen);
		(yyval.ulval) = AF_OPTIONAL;
	}
#line 2722 "main/idlc/source/parser.cxx"
    break;

  case 62: /* opt_attrflag: IDL_MAYBEVOID  */
#line 872 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_MayBeVoidSeen);
		(yyval.ulval) = AF_MAYBEVOID;
	}
#line 2731 "main/idlc/source/parser.cxx"
    break;

  case 63: /* opt_attrflag: IDL_BOUND  */
#line 877 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_BoundSeen);
		(yyval.ulval) = AF_BOUND;
	}
#line 2740 "main/idlc/source/parser.cxx"
    break;

  case 64: /* opt_attrflag: IDL_CONSTRAINED  */
#line 882 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_ConstrainedSeen);
		(yyval.ulval) = AF_CONSTRAINED;
	}
#line 2749 "main/idlc/source/parser.cxx"
    break;

  case 65: /* opt_attrflag: IDL_TRANSIENT  */
#line 887 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_TransientSeen);
		(yyval.ulval) = AF_TRANSIENT;
	}
#line 2758 "main/idlc/source/parser.cxx"
    break;

  case 66: /* opt_attrflag: IDL_MAYBEAMBIGUOUS  */
#line 892 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_MayBeAmbigiousSeen);
		(yyval.ulval) = AF_MAYBEAMBIGUOUS;
	}
#line 2767 "main/idlc/source/parser.cxx"
    break;

  case 67: /* opt_attrflag: IDL_MAYBEDEFAULT  */
#line 897 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_MayBeDefaultSeen);
		(yyval.ulval) = AF_MAYBEDEFAULT;
	}
#line 2776 "main/idlc/source/parser.cxx"
    break;

  case 68: /* opt_attrflag: IDL_REMOVEABLE  */
#line 902 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_RemoveableSeen);
		(yyval.ulval) = AF_REMOVEABLE;
	}
#line 2785 "main/idlc/source/parser.cxx"
    break;

  case 69: /* opt_attrflag: error ']'  */
#line 907 "main/idlc/source/parser.y"
        {
       yyerror("unknown property|attribute flag");
		yyerrok;
	}
#line 2794 "main/idlc/source/parser.cxx"
    break;

  case 70: /* opt_attribute_block: '{' attribute_block_rest  */
#line 914 "main/idlc/source/parser.y"
                             { (yyval.attexcval) = (yyvsp[0].attexcval); }
#line 2800 "main/idlc/source/parser.cxx"
    break;

  case 71: /* opt_attribute_block: %empty  */
#line 916 "main/idlc/source/parser.y"
    {
        (yyval.attexcval).get.documentation = 0;
        (yyval.attexcval).get.exceptions = 0;
        (yyval.attexcval).set.documentation = 0;
        (yyval.attexcval).set.exceptions = 0;
    }
#line 2811 "main/idlc/source/parser.cxx"
    break;

  case 73: /* attribute_block_rest: error '}'  */
#line 927 "main/idlc/source/parser.y"
    {
        yyerror("bad attribute raises block");
        yyerrok;
        (yyval.attexcval).get.documentation = 0;
        (yyval.attexcval).get.exceptions = 0;
        (yyval.attexcval).set.documentation = 0;
        (yyval.attexcval).set.exceptions = 0;
    }
#line 2824 "main/idlc/source/parser.cxx"
    break;

  case 74: /* opt_attribute_raises: attribute_get_raises opt_attribute_set_raises  */
#line 940 "main/idlc/source/parser.y"
    {
        (yyval.attexcval).get = (yyvsp[-1].attexcpval);
        (yyval.attexcval).set = (yyvsp[0].attexcpval);
    }
#line 2833 "main/idlc/source/parser.cxx"
    break;

  case 75: /* opt_attribute_raises: attribute_set_raises opt_attribute_get_raises  */
#line 946 "main/idlc/source/parser.y"
    {
        (yyval.attexcval).get = (yyvsp[0].attexcpval);
        (yyval.attexcval).set = (yyvsp[-1].attexcpval);
    }
#line 2842 "main/idlc/source/parser.cxx"
    break;

  case 76: /* opt_attribute_raises: %empty  */
#line 951 "main/idlc/source/parser.y"
    {
        (yyval.attexcval).get.documentation = 0;
        (yyval.attexcval).get.exceptions = 0;
        (yyval.attexcval).set.documentation = 0;
        (yyval.attexcval).set.exceptions = 0;
    }
#line 2853 "main/idlc/source/parser.cxx"
    break;

  case 78: /* opt_attribute_get_raises: %empty  */
#line 961 "main/idlc/source/parser.y"
                  { (yyval.attexcpval).documentation = 0; (yyval.attexcpval).exceptions = 0; }
#line 2859 "main/idlc/source/parser.cxx"
    break;

  case 79: /* attribute_get_raises: IDL_GET raises ';'  */
#line 966 "main/idlc/source/parser.y"
    {
        (yyval.attexcpval).documentation = new rtl::OUString(
            rtl::OStringToOUString(
                idlc()->getDocumentation(), RTL_TEXTENCODING_UTF8));
        (yyval.attexcpval).exceptions = (yyvsp[-1].dclsval);
    }
#line 2870 "main/idlc/source/parser.cxx"
    break;

  case 81: /* opt_attribute_set_raises: %empty  */
#line 976 "main/idlc/source/parser.y"
                  { (yyval.attexcpval).documentation = 0; (yyval.attexcpval).exceptions = 0; }
#line 2876 "main/idlc/source/parser.cxx"
    break;

  case 82: /* $@23: %empty  */
#line 981 "main/idlc/source/parser.y"
    {
        if (static_cast< AstAttribute * >(idlc()->scopes()->top())->
            isReadonly())
        {
            idlc()->error()->error0(EIDL_READONLY_ATTRIBUTE_SET_EXCEPTIONS);
        }
    }
#line 2888 "main/idlc/source/parser.cxx"
    break;

  case 83: /* attribute_set_raises: IDL_SET $@23 raises ';'  */
#line 989 "main/idlc/source/parser.y"
    {
        (yyval.attexcpval).documentation = new rtl::OUString(
            rtl::OStringToOUString(
                idlc()->getDocumentation(), RTL_TEXTENCODING_UTF8));
        (yyval.attexcpval).exceptions = (yyvsp[-1].dclsval);
    }
#line 2899 "main/idlc/source/parser.cxx"
    break;

  case 84: /* $@24: %empty  */
#line 1000 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_OpTypeSeen);
	}
#line 2907 "main/idlc/source/parser.cxx"
    break;

  case 85: /* $@25: %empty  */
#line 1004 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_OpIDSeen);
       checkIdentifier((yyvsp[0].sval));

		AstInterface * pScope = static_cast< AstInterface * >(
            idlc()->scopes()->top());
		AstOperation* 	pOp = NULL;

		/*
		 * Create a node representing an operation on an interface
		 * and add it to its enclosing scope
		 */
		if ( pScope && (yyvsp[-2].cdclval) )
		{
			AstType *pType = (AstType*)(yyvsp[-2].cdclval);
			if ( !pType || (pType->getNodeType() == NT_exception) )
			{
				// type ERROR 
			} else 
			{
				pOp = new AstOperation((yyvsp[-3].ulval), pType, *(yyvsp[0].sval), pScope);

                AstInterface::DoubleMemberDeclarations doubleMembers(
                    pScope->checkMemberClashes(pOp));
                if (doubleMembers.empty()) {
                    pScope->addMember(pOp);
                } else {
                    reportDoubleMemberDeclarations(doubleMembers);
                }
			}
		}
		delete (yyvsp[0].sval);
		/*
		 * Push the operation scope onto the scopes stack
		 */
		idlc()->scopes()->push(pOp);
	}
#line 2949 "main/idlc/source/parser.cxx"
    break;

  case 86: /* $@26: %empty  */
#line 1042 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_OpSqSeen);
	}
#line 2957 "main/idlc/source/parser.cxx"
    break;

  case 87: /* $@27: %empty  */
#line 1046 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_OpParsCompleted);
	}
#line 2965 "main/idlc/source/parser.cxx"
    break;

  case 88: /* $@28: %empty  */
#line 1050 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_OpQsSeen);
	}
#line 2973 "main/idlc/source/parser.cxx"
    break;

  case 89: /* operation: operation_head op_type_spec $@24 identifier $@25 '(' $@26 parameters $@27 ')' $@28 opt_raises  */
#line 1054 "main/idlc/source/parser.y"
        {
		AstScope*		pScope = idlc()->scopes()->topNonNull();
		AstOperation* 	pOp = NULL;
		/*
		 * Add exceptions and context to the operation
		 */
		if ( pScope && pScope->getScopeNodeType() == NT_operation)
		{
			pOp = (AstOperation*)pScope;

			if ( pOp )
				pOp->setExceptions((yyvsp[0].dclsval));
		}
        delete (yyvsp[0].dclsval);
		/*
		 * Done with this operation. Pop its scope from the scopes stack
		 */
		idlc()->scopes()->pop();
	}
#line 2997 "main/idlc/source/parser.cxx"
    break;

  case 90: /* $@29: %empty  */
#line 1078 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_OpOnewaySeen);
	}
#line 3005 "main/idlc/source/parser.cxx"
    break;

  case 91: /* operation_head: '[' IDL_ONEWAY $@29 ']'  */
#line 1082 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_OpHeadSeen);
		(yyval.ulval) = OP_ONEWAY;
	}
#line 3014 "main/idlc/source/parser.cxx"
    break;

  case 92: /* operation_head: %empty  */
#line 1087 "main/idlc/source/parser.y"
        {
		(yyval.ulval) = OP_NONE;
	}
#line 3022 "main/idlc/source/parser.cxx"
    break;

  case 94: /* op_type_spec: IDL_VOID  */
#line 1095 "main/idlc/source/parser.y"
        {
		(yyval.cdclval) = idlc()->scopes()->bottom()->lookupPrimitiveType(ET_void);
	}
#line 3030 "main/idlc/source/parser.cxx"
    break;

  case 96: /* $@30: %empty  */
#line 1104 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_OpParCommaSeen);
	}
#line 3038 "main/idlc/source/parser.cxx"
    break;

  case 99: /* parameters: error ','  */
#line 1110 "main/idlc/source/parser.y"
        {
		yyerror("parameter definition");
		yyerrok;
	}
#line 3047 "main/idlc/source/parser.cxx"
    break;

  case 100: /* $@31: %empty  */
#line 1120 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_OpParDirSeen);
	}
#line 3055 "main/idlc/source/parser.cxx"
    break;

  case 101: /* $@32: %empty  */
#line 1124 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_OpParTypeSeen);
	}
#line 3063 "main/idlc/source/parser.cxx"
    break;

  case 102: /* parameter: '[' direction ']' $@31 simple_type_spec $@32 opt_rest declarator  */
#line 1129 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_OpParDeclSeen);

        AstOperation * pScope = static_cast< AstOperation * >(
            idlc()->scopes()->top());
		AstParameter* 	pParam = NULL;

		/*
		 * Create a node representing an argument to an operation
		 * Add it to the enclosing scope (the operation scope)
		 */
		if ( pScope && (yyvsp[-3].cdclval) && (yyvsp[0].fdval) )
		{
            AstType const * pType = (yyvsp[0].fdval)->compose((yyvsp[-3].cdclval));
			if ( pType )
			{
                if (pScope->isConstructor() && (yyvsp[-6].ulval) != DIR_IN) {
                    idlc()->error()->error0(EIDL_CONSTRUCTOR_PARAMETER_NOT_IN);
                }
                if (pScope->isVariadic()) {
                    idlc()->error()->error0(EIDL_REST_PARAMETER_NOT_LAST);
                }
                if ((yyvsp[-1].bval)) {
                    AstDeclaration const * type = resolveTypedefs(pType);
                    if (type->getNodeType() != NT_predefined
                        || (static_cast< AstBaseType const * >(type)->
                            getExprType() != ET_any))
                    {
                        idlc()->error()->error0(EIDL_REST_PARAMETER_NOT_ANY);
                    }
                    if (pScope->isConstructor()) {
                        if (pScope->getIteratorBegin()
                            != pScope->getIteratorEnd())
                        {
                            idlc()->error()->error0(
                                EIDL_CONSTRUCTOR_REST_PARAMETER_NOT_FIRST);
                        }
                    } else {
                        idlc()->error()->error0(EIDL_METHOD_HAS_REST_PARAMETER);
                    }
                }

				pParam = new AstParameter(
                    static_cast< Direction >((yyvsp[-6].ulval)), (yyvsp[-1].bval), pType, (yyvsp[0].fdval)->getName(),
                    pScope);
								
				if ( !(yyvsp[0].fdval)->checkType((yyvsp[-3].cdclval)) )
				{
					// WARNING	
				}

				pScope->addDeclaration(pParam);
			}
		}
	}
#line 3123 "main/idlc/source/parser.cxx"
    break;

  case 103: /* parameter: error simple_type_spec  */
#line 1186 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_NoState);
		yyerrok;
	}
#line 3132 "main/idlc/source/parser.cxx"
    break;

  case 104: /* direction: IDL_IN  */
#line 1194 "main/idlc/source/parser.y"
        {
		(yyval.ulval) = DIR_IN;
	}
#line 3140 "main/idlc/source/parser.cxx"
    break;

  case 105: /* direction: IDL_OUT  */
#line 1198 "main/idlc/source/parser.y"
        {
		(yyval.ulval) = DIR_OUT;
	}
#line 3148 "main/idlc/source/parser.cxx"
    break;

  case 106: /* direction: IDL_INOUT  */
#line 1202 "main/idlc/source/parser.y"
        {
		(yyval.ulval) = DIR_INOUT;
	}
#line 3156 "main/idlc/source/parser.cxx"
    break;

  case 107: /* opt_rest: IDL_ELLIPSIS  */
#line 1209 "main/idlc/source/parser.y"
    {
        (yyval.bval) = true;
    }
#line 3164 "main/idlc/source/parser.cxx"
    break;

  case 108: /* opt_rest: %empty  */
#line 1213 "main/idlc/source/parser.y"
    {
        (yyval.bval) = false;
    }
#line 3172 "main/idlc/source/parser.cxx"
    break;

  case 110: /* opt_raises: %empty  */
#line 1221 "main/idlc/source/parser.y"
    {
        (yyval.dclsval) = 0;
    }
#line 3180 "main/idlc/source/parser.cxx"
    break;

  case 111: /* $@33: %empty  */
#line 1228 "main/idlc/source/parser.y"
    {
        idlc()->setParseState(PS_RaiseSeen);
    }
#line 3188 "main/idlc/source/parser.cxx"
    break;

  case 112: /* $@34: %empty  */
#line 1232 "main/idlc/source/parser.y"
    {
        idlc()->setParseState(PS_RaiseSqSeen);
    }
#line 3196 "main/idlc/source/parser.cxx"
    break;

  case 113: /* raises: IDL_RAISES $@33 '(' $@34 exception_list ')'  */
#line 1237 "main/idlc/source/parser.y"
    {
        idlc()->setParseState(PS_RaiseQsSeen);
        (yyval.dclsval) = (yyvsp[-1].dclsval);
    }
#line 3205 "main/idlc/source/parser.cxx"
    break;

  case 114: /* exception_list: exception_name  */
#line 1245 "main/idlc/source/parser.y"
    {
        (yyval.dclsval) = new DeclList;
        (yyval.dclsval)->push_back((yyvsp[0].dclval));
    }
#line 3214 "main/idlc/source/parser.cxx"
    break;

  case 115: /* exception_list: exception_list ',' exception_name  */
#line 1250 "main/idlc/source/parser.y"
    {
        (yyvsp[-2].dclsval)->push_back((yyvsp[0].dclval));
        (yyval.dclsval) = (yyvsp[-2].dclsval);
    }
#line 3223 "main/idlc/source/parser.cxx"
    break;

  case 116: /* exception_name: scoped_name  */
#line 1258 "main/idlc/source/parser.y"
    {
        // The topmost scope is either an AstOperation (for interface methods
        // and service constructors) or an AstAttribute (for interface
        // attributes), so look up exception names in the next-to-topmost scope:
        AstDeclaration * decl = idlc()->scopes()->nextToTop()->lookupByName(
            *(yyvsp[0].sval));
        if (decl == 0) {
            idlc()->error()->lookupError(*(yyvsp[0].sval));
        } else if (!idlc()->error()->checkPublished(decl)) {
            decl = 0;
        } else if (decl->getNodeType() != NT_exception) {
            idlc()->error()->error1(EIDL_ILLEGAL_RAISES, decl);
            decl = 0;
        }
        delete (yyvsp[0].sval);
        (yyval.dclval) = decl;
    }
#line 3245 "main/idlc/source/parser.cxx"
    break;

  case 117: /* $@35: %empty  */
#line 1280 "main/idlc/source/parser.y"
    {
        idlc()->setParseState(PS_ServiceIFHeadSeen);
    }
#line 3253 "main/idlc/source/parser.cxx"
    break;

  case 118: /* interface_inheritance_decl: optional_inherited_interface IDL_INTERFACE $@35 scoped_name  */
#line 1284 "main/idlc/source/parser.y"
    {
        AstInterface * ifc = static_cast< AstInterface * >(
            idlc()->scopes()->top());
        if (ifc->usesSingleInheritance()) {
            idlc()->error()->error0(EIDL_MIXED_INHERITANCE);
        } else {
            addInheritedInterface(
                ifc, *(yyvsp[0].sval), (yyvsp[-3].bval),
                rtl::OStringToOUString(
                    idlc()->getDocumentation(), RTL_TEXTENCODING_UTF8));
        }
        delete (yyvsp[0].sval);
    }
#line 3271 "main/idlc/source/parser.cxx"
    break;

  case 119: /* optional_inherited_interface: '[' IDL_OPTIONAL ']'  */
#line 1300 "main/idlc/source/parser.y"
                         { (yyval.bval) = true; }
#line 3277 "main/idlc/source/parser.cxx"
    break;

  case 120: /* optional_inherited_interface: %empty  */
#line 1301 "main/idlc/source/parser.y"
                  { (yyval.bval) = false; }
#line 3283 "main/idlc/source/parser.cxx"
    break;

  case 123: /* $@36: %empty  */
#line 1311 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_ConstantDeclSeen);
	}
#line 3291 "main/idlc/source/parser.cxx"
    break;

  case 124: /* constants_export: const_dcl $@36 ';'  */
#line 1314 "main/idlc/source/parser.y"
            {}
#line 3297 "main/idlc/source/parser.cxx"
    break;

  case 125: /* $@37: %empty  */
#line 1318 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_ConstSeen);
	}
#line 3305 "main/idlc/source/parser.cxx"
    break;

  case 126: /* $@38: %empty  */
#line 1322 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_ConstTypeSeen);
	}
#line 3313 "main/idlc/source/parser.cxx"
    break;

  case 127: /* $@39: %empty  */
#line 1326 "main/idlc/source/parser.y"
        {
        idlc()->setParseState(PS_ConstIDSeen);
        checkIdentifier((yyvsp[0].sval));
	}
#line 3322 "main/idlc/source/parser.cxx"
    break;

  case 128: /* $@40: %empty  */
#line 1331 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_ConstAssignSeen);
	}
#line 3330 "main/idlc/source/parser.cxx"
    break;

  case 129: /* const_dcl: IDL_CONST $@37 const_type $@38 identifier $@39 '=' $@40 expression  */
#line 1335 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_ConstExprSeen);

		AstScope* 		pScope = idlc()->scopes()->topNonNull();
		AstConstant*	pConstant = NULL;

		if ( (yyvsp[0].exval) && pScope )
		{
			if ( !(yyvsp[0].exval)->coerce((yyvsp[-6].etval)) )
			{
				idlc()->error()->coercionError((yyvsp[0].exval), (yyvsp[-6].etval));
			} else
			{
				pConstant = new AstConstant((yyvsp[-6].etval), (yyvsp[0].exval), *(yyvsp[-4].sval), pScope);
				pScope->addDeclaration(pConstant);
			}
		}
		delete (yyvsp[-4].sval);
	}
#line 3354 "main/idlc/source/parser.cxx"
    break;

  case 130: /* $@41: %empty  */
#line 1358 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_ConstantsSeen);
	}
#line 3362 "main/idlc/source/parser.cxx"
    break;

  case 131: /* $@42: %empty  */
#line 1362 "main/idlc/source/parser.y"
        {
        idlc()->setParseState(PS_ConstantsIDSeen);
        checkIdentifier((yyvsp[0].sval));
	}
#line 3371 "main/idlc/source/parser.cxx"
    break;

  case 132: /* $@43: %empty  */
#line 1367 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_ConstantsSqSeen);

		AstScope* 		pScope = idlc()->scopes()->topNonNull();
		AstConstants*	pConstants = NULL;
		AstDeclaration*	pExists = NULL;

		if ( pScope )
		{
			pConstants = new AstConstants(*(yyvsp[-2].sval), pScope);
			if( (pExists = pScope->lookupForAdd(pConstants)) )
			{
				pExists->setInMainfile(idlc()->isInMainFile());
				delete(pConstants);
				pConstants = (AstConstants*)pExists;
			} else
			{
				pScope->addDeclaration(pConstants);
			}
			idlc()->scopes()->push(pConstants);
		}
		delete (yyvsp[-2].sval);
	}
#line 3399 "main/idlc/source/parser.cxx"
    break;

  case 133: /* $@44: %empty  */
#line 1391 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_ConstantsBodySeen);
	}
#line 3407 "main/idlc/source/parser.cxx"
    break;

  case 134: /* constants_dcl: IDL_CONSTANTS $@41 identifier $@42 '{' $@43 constants_exports $@44 '}'  */
#line 1395 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_ConstantsQsSeen);
		/*
		 * Finished with this constants - pop it from the scope stack
		 */
		idlc()->scopes()->pop();
	}
#line 3419 "main/idlc/source/parser.cxx"
    break;

  case 138: /* or_expr: or_expr '|' xor_expr  */
#line 1411 "main/idlc/source/parser.y"
        {
		(yyval.exval) = new AstExpression(EC_or, (yyvsp[-2].exval), (yyvsp[0].exval));
	}
#line 3427 "main/idlc/source/parser.cxx"
    break;

  case 140: /* xor_expr: xor_expr '^' and_expr  */
#line 1419 "main/idlc/source/parser.y"
        {
		(yyval.exval) = new AstExpression(EC_xor, (yyvsp[-2].exval), (yyvsp[0].exval));
	}
#line 3435 "main/idlc/source/parser.cxx"
    break;

  case 142: /* and_expr: and_expr '&' shift_expr  */
#line 1427 "main/idlc/source/parser.y"
        {
		(yyval.exval) = new AstExpression(EC_and, (yyvsp[-2].exval), (yyvsp[0].exval));
	}
#line 3443 "main/idlc/source/parser.cxx"
    break;

  case 144: /* shift_expr: shift_expr IDL_LEFTSHIFT add_expr  */
#line 1435 "main/idlc/source/parser.y"
        {
		(yyval.exval) = new AstExpression(EC_left, (yyvsp[-2].exval), (yyvsp[0].exval));
	}
#line 3451 "main/idlc/source/parser.cxx"
    break;

  case 145: /* shift_expr: shift_expr IDL_RIGHTSHIFT add_expr  */
#line 1439 "main/idlc/source/parser.y"
        {
		(yyval.exval) = new AstExpression(EC_right, (yyvsp[-2].exval), (yyvsp[0].exval));
	}
#line 3459 "main/idlc/source/parser.cxx"
    break;

  case 147: /* add_expr: add_expr '+' mult_expr  */
#line 1447 "main/idlc/source/parser.y"
        {
		(yyval.exval) = new AstExpression(EC_add, (yyvsp[-2].exval), (yyvsp[0].exval));
	}
#line 3467 "main/idlc/source/parser.cxx"
    break;

  case 148: /* add_expr: add_expr '-' mult_expr  */
#line 1451 "main/idlc/source/parser.y"
        {
		(yyval.exval) = new AstExpression(EC_minus, (yyvsp[-2].exval), (yyvsp[0].exval));
	}
#line 3475 "main/idlc/source/parser.cxx"
    break;

  case 150: /* mult_expr: mult_expr '*' unary_expr  */
#line 1459 "main/idlc/source/parser.y"
        {
		(yyval.exval) = new AstExpression(EC_mul, (yyvsp[-2].exval), (yyvsp[0].exval));
	}
#line 3483 "main/idlc/source/parser.cxx"
    break;

  case 151: /* mult_expr: mult_expr '/' unary_expr  */
#line 1463 "main/idlc/source/parser.y"
        {
		(yyval.exval) = new AstExpression(EC_div, (yyvsp[-2].exval), (yyvsp[0].exval));
	}
#line 3491 "main/idlc/source/parser.cxx"
    break;

  case 152: /* mult_expr: mult_expr '%' unary_expr  */
#line 1467 "main/idlc/source/parser.y"
        {
		(yyval.exval) = new AstExpression(EC_mod, (yyvsp[-2].exval), (yyvsp[0].exval));
	}
#line 3499 "main/idlc/source/parser.cxx"
    break;

  case 154: /* unary_expr: '+' primary_expr  */
#line 1475 "main/idlc/source/parser.y"
        {
		(yyval.exval) = new AstExpression(EC_u_plus, (yyvsp[0].exval), NULL);
	}
#line 3507 "main/idlc/source/parser.cxx"
    break;

  case 155: /* unary_expr: '-' primary_expr  */
#line 1479 "main/idlc/source/parser.y"
        {
		(yyval.exval) = new AstExpression(EC_u_minus, (yyvsp[0].exval), NULL);
	}
#line 3515 "main/idlc/source/parser.cxx"
    break;

  case 156: /* unary_expr: '~' primary_expr  */
#line 1483 "main/idlc/source/parser.y"
        {
	}
#line 3522 "main/idlc/source/parser.cxx"
    break;

  case 157: /* primary_expr: scoped_name  */
#line 1489 "main/idlc/source/parser.y"
        {
		/*
		 * An expression which is a scoped name is not resolved now,
		 * but only when it is evaluated (such as when it is assigned
		 * as a constant value)
		 */
		(yyval.exval) = new AstExpression((yyvsp[0].sval));
	}
#line 3535 "main/idlc/source/parser.cxx"
    break;

  case 159: /* primary_expr: '(' const_expr ')'  */
#line 1499 "main/idlc/source/parser.y"
        {
		(yyval.exval) = (yyvsp[-1].exval);
	}
#line 3543 "main/idlc/source/parser.cxx"
    break;

  case 160: /* literal: IDL_INTEGER_LITERAL  */
#line 1506 "main/idlc/source/parser.y"
        {
		(yyval.exval) = new AstExpression((yyvsp[0].ival));
	}
#line 3551 "main/idlc/source/parser.cxx"
    break;

  case 161: /* literal: IDL_INTEGER_ULITERAL  */
#line 1510 "main/idlc/source/parser.y"
    {
        (yyval.exval) = new AstExpression((yyvsp[0].uval));
    }
#line 3559 "main/idlc/source/parser.cxx"
    break;

  case 162: /* literal: IDL_FLOATING_PT_LITERAL  */
#line 1514 "main/idlc/source/parser.y"
        {
		(yyval.exval) = new AstExpression((yyvsp[0].dval));
	}
#line 3567 "main/idlc/source/parser.cxx"
    break;

  case 163: /* literal: IDL_TRUE  */
#line 1518 "main/idlc/source/parser.y"
        {
		(yyval.exval) = new AstExpression((sal_Int32)1, ET_boolean);
	}
#line 3575 "main/idlc/source/parser.cxx"
    break;

  case 164: /* literal: IDL_FALSE  */
#line 1522 "main/idlc/source/parser.y"
        {
		(yyval.exval) = new AstExpression((sal_Int32)0, ET_boolean);
	}
#line 3583 "main/idlc/source/parser.cxx"
    break;

  case 165: /* positive_int_expr: const_expr  */
#line 1529 "main/idlc/source/parser.y"
        {
		(yyvsp[0].exval)->evaluate(EK_const);
		if ( !(yyvsp[0].exval)->coerce(ET_ulong) )
		{
			idlc()->error()->coercionError((yyvsp[0].exval), ET_ulong);
			delete (yyvsp[0].exval);
			(yyval.exval) = NULL;
		}
	}
#line 3597 "main/idlc/source/parser.cxx"
    break;

  case 171: /* const_type: scoped_name  */
#line 1547 "main/idlc/source/parser.y"
        {
		AstScope* 		pScope = idlc()->scopes()->topNonNull();
        AstDeclaration const * type = 0;
		
		/*
		 * If the constant's type is a scoped name, it must resolve
		 * to a scalar constant type
		 */
		if ( pScope && (type = pScope->lookupByName(*(yyvsp[0].sval))) ) {
            if (!idlc()->error()->checkPublished(type))
            {
                type = 0;
            }
            else
            {
                type = resolveTypedefs(type);
                if (type->getNodeType() == NT_predefined) 
                {
                    (yyval.etval) = static_cast< AstBaseType const * >(type)->
                        getExprType();
                } else
                    (yyval.etval) = ET_any;
            }
		} else
			(yyval.etval) = ET_any;
	}
#line 3628 "main/idlc/source/parser.cxx"
    break;

  case 172: /* $@45: %empty  */
#line 1577 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_ExceptSeen);
	}
#line 3636 "main/idlc/source/parser.cxx"
    break;

  case 173: /* $@46: %empty  */
#line 1581 "main/idlc/source/parser.y"
        {
        idlc()->setParseState(PS_ExceptIDSeen);
        checkIdentifier((yyvsp[0].sval));
	}
#line 3645 "main/idlc/source/parser.cxx"
    break;

  case 174: /* exception_header: IDL_EXCEPTION $@45 identifier $@46 inheritance_spec  */
#line 1586 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_InheritSpecSeen);

		(yyval.ihval) = new FeInheritanceHeader(NT_exception, (yyvsp[-2].sval), (yyvsp[0].sval), 0);
		delete (yyvsp[0].sval);
	}
#line 3656 "main/idlc/source/parser.cxx"
    break;

  case 175: /* $@47: %empty  */
#line 1596 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_ExceptHeaderSeen);

		AstScope* 		pScope = idlc()->scopes()->topNonNull();
		AstException*	pExcept = NULL;

		if ( pScope )
		{
			AstException* pBase = static_cast< AstException* >(
                (yyvsp[0].ihval)->getInherits());
			pExcept = new AstException(*(yyvsp[0].ihval)->getName(), pBase, pScope);	
			pScope->addDeclaration(pExcept);
		}
		/*
		 * Push the scope of the exception on the scopes stack
		 */
		idlc()->scopes()->push(pExcept);
		delete (yyvsp[0].ihval);
	}
#line 3680 "main/idlc/source/parser.cxx"
    break;

  case 176: /* $@48: %empty  */
#line 1616 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_ExceptSqSeen);
	}
#line 3688 "main/idlc/source/parser.cxx"
    break;

  case 177: /* $@49: %empty  */
#line 1620 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_ExceptBodySeen);
	}
#line 3696 "main/idlc/source/parser.cxx"
    break;

  case 178: /* exception_dcl: exception_header $@47 '{' $@48 members $@49 '}'  */
#line 1624 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_ExceptQsSeen);
		/* this exception is finished, pop its scope from the stack */ 
		idlc()->scopes()->pop();
	}
#line 3706 "main/idlc/source/parser.cxx"
    break;

  case 179: /* $@50: %empty  */
#line 1634 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_PropertyTypeSeen);
	}
#line 3714 "main/idlc/source/parser.cxx"
    break;

  case 180: /* property: flag_header simple_type_spec $@50 at_least_one_declarator  */
#line 1638 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_PropertyCompleted);

		AstScope* 		pScope = idlc()->scopes()->topNonNull();
		AstAttribute* 	pAttr = NULL;
		FeDeclList*		pList = (yyvsp[0].dlval);
		FeDeclarator*	pDecl = NULL;
        AstType const * pType = NULL;

		if ( pScope->getScopeNodeType() == NT_singleton )
		{
			idlc()->error()->error0(EIDL_ILLEGAL_ADD);			
		} else
		{
			if ( ((yyvsp[-3].ulval) & AF_ATTRIBUTE) == AF_ATTRIBUTE )
				idlc()->error()->flagError(EIDL_WRONGATTRIBUTEKEYWORD, AF_ATTRIBUTE);

			if ( ((yyvsp[-3].ulval) & AF_PROPERTY) != AF_PROPERTY )
				idlc()->error()->flagError(EIDL_MISSINGATTRIBUTEKEYWORD, AF_PROPERTY);

			/*
			 * Create nodes representing attributes and add them to the
			 * enclosing scope
			 */
			if ( pScope && (yyvsp[-2].cdclval) && pList )
			{
				FeDeclList::iterator iter = pList->begin();
				FeDeclList::iterator end = pList->end();

				while (iter != end)
				{
					pDecl = (*iter);
					if ( !pDecl )
					{
						iter++;
						continue;
					}

					pType = pDecl->compose((yyvsp[-2].cdclval));				

					if ( !pType )
					{
						iter++;
						continue;
					}

					pAttr = new AstAttribute(NT_property, (yyvsp[-3].ulval), pType, pDecl->getName(), pScope);

					pScope->addDeclaration(pAttr);
					iter++;
					delete pDecl;
				}
			}
		}			

		if ( pList )
			delete pList;
	}
#line 3777 "main/idlc/source/parser.cxx"
    break;

  case 181: /* property: error ';'  */
#line 1697 "main/idlc/source/parser.y"
        {
		yyerror("property");
		yyerrok;
	}
#line 3786 "main/idlc/source/parser.cxx"
    break;

  case 184: /* service_export: service_interface_header at_least_one_scoped_name ';'  */
#line 1712 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_ServiceMemberSeen);
		
		AstScope* 			pScope = idlc()->scopes()->topNonNull();
		AstDeclaration* 	pDecl = NULL;
		AstInterfaceMember* pIMember = NULL;

		if ( pScope->getScopeNodeType() == NT_singleton )
		{
			idlc()->error()->error0(EIDL_ILLEGAL_ADD);			
		} else
		{
			/*
			 * Create a node representing a class member.
			 * Store it in the enclosing scope
	         */
			if ( pScope && (yyvsp[-1].slval) )
			{
				StringList::iterator iter = (yyvsp[-1].slval)->begin();
				StringList::iterator end = (yyvsp[-1].slval)->end();						

				while ( iter != end )
				{
					pDecl = pScope->lookupByName(*iter);
					if ( pDecl && (pDecl->getNodeType() == NT_interface) )
					{
                        /* we relax the strict published check and allow to add new
                         * interfaces if they are optional
                         */
                        bool bOptional = (((yyvsp[-2].ulval) & AF_OPTIONAL) == AF_OPTIONAL);
                        if ( idlc()->error()->checkPublished(pDecl, bOptional) )
                        {
                            pIMember = new AstInterfaceMember(
                                (yyvsp[-2].ulval), (AstInterface*)pDecl, *iter, pScope);
                            pScope->addDeclaration(pIMember);
                        }
					} else
					{
						idlc()->error()->
							lookupError(EIDL_INTERFACEMEMBER_LOOKUP, *iter, scopeAsDecl(pScope));
					}
					iter++;
				}
			}				
		}
		delete (yyvsp[-1].slval);
	}
#line 3838 "main/idlc/source/parser.cxx"
    break;

  case 185: /* service_export: service_service_header at_least_one_scoped_name ';'  */
#line 1762 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_ServiceMemberSeen);

		AstScope* 		  pScope = idlc()->scopes()->topNonNull();
		AstDeclaration*   pDecl = NULL;
		AstServiceMember* pSMember = NULL;

		/*
		 * Create a node representing a class member.
		 * Store it in the enclosing scope
         */
		if ( pScope && (yyvsp[-1].slval) )
		{
			StringList::iterator iter = (yyvsp[-1].slval)->begin();
			StringList::iterator end = (yyvsp[-1].slval)->end();						

			while ( iter != end )
			{
				pDecl = pScope->lookupByName(*iter);
				if ( pDecl && (pDecl->getNodeType() == NT_service) )
				{
					if ( pScope->getScopeNodeType() == NT_singleton && pScope->nMembers() > 0 )
						idlc()->error()->error0(EIDL_ILLEGAL_ADD);										
                    else if ( idlc()->error()->checkPublished(pDecl) )
                    {
                        pSMember = new AstServiceMember(
                            (yyvsp[-2].ulval), (AstService*)pDecl, *iter, pScope);
                        pScope->addDeclaration(pSMember);
                    }
				} else
				{
					idlc()->error()->
						lookupError(EIDL_SERVICEMEMBER_LOOKUP, *iter, scopeAsDecl(pScope));
				}
				iter++;
			}
		}				
		delete (yyvsp[-1].slval);
	}
#line 3882 "main/idlc/source/parser.cxx"
    break;

  case 186: /* service_export: IDL_OBSERVES at_least_one_scoped_name ';'  */
#line 1804 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_ServiceMemberSeen);

		AstScope* 		pScope = idlc()->scopes()->topNonNull();
		AstDeclaration* pDecl = NULL;
		AstObserves* 	pObserves = NULL;

		if ( pScope->getScopeNodeType() == NT_singleton )
		{
			idlc()->error()->error0(EIDL_ILLEGAL_ADD);			
		} else
		{
			/*
			 * Create a node representing a class member.
			 * Store it in the enclosing scope
	         */
			if ( pScope && (yyvsp[-1].slval) )
			{
				StringList::iterator iter = (yyvsp[-1].slval)->begin();
				StringList::iterator end = (yyvsp[-1].slval)->end();						

				while ( iter != end )
				{
					pDecl = pScope->lookupByName(*iter);
					if ( pDecl && (pDecl->getNodeType() == NT_interface) )
					{
						pObserves = new AstObserves((AstInterface*)pDecl, *iter, pScope);
						pScope->addDeclaration(pObserves);
					} else
					{
						idlc()->error()->
							lookupError(EIDL_INTERFACEMEMBER_LOOKUP, *iter, scopeAsDecl(pScope));
					}
					iter++;
				}
			}				
		}
		delete (yyvsp[-1].slval);
	}
#line 3926 "main/idlc/source/parser.cxx"
    break;

  case 187: /* service_export: IDL_NEEDS at_least_one_scoped_name ';'  */
#line 1846 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_ServiceMemberSeen);

		AstScope* 		pScope = idlc()->scopes()->topNonNull();
		AstDeclaration*	pDecl = NULL;
		AstNeeds*	   	pNeeds = NULL;

		if ( pScope->getScopeNodeType() == NT_singleton )
		{
			idlc()->error()->error0(EIDL_ILLEGAL_ADD);			
		} else
		{
			/*
			 * Create a node representing a class member.
			 * Store it in the enclosing scope
	         */
			if ( pScope && (yyvsp[-1].slval) )
			{
				StringList::iterator iter = (yyvsp[-1].slval)->begin();
				StringList::iterator end = (yyvsp[-1].slval)->end();						

				while ( iter != end )
				{
					pDecl = pScope->lookupByName(*iter);
					if ( pDecl && (pDecl->getNodeType() == NT_service) )
					{
						pNeeds = new AstNeeds((AstService*)pDecl, *iter, pScope);
						pScope->addDeclaration(pNeeds);
					} else
					{
						idlc()->error()->
							lookupError(EIDL_SERVICEMEMBER_LOOKUP, *iter, scopeAsDecl(pScope));
					}
					iter++;
				}
			}				
		}
		delete (yyvsp[-1].slval);
	}
#line 3970 "main/idlc/source/parser.cxx"
    break;

  case 188: /* service_export: property ';'  */
#line 1887 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_PropertyDeclSeen);
	}
#line 3978 "main/idlc/source/parser.cxx"
    break;

  case 189: /* service_interface_header: IDL_INTERFACE  */
#line 1894 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_ServiceIFHeadSeen);
		(yyval.ulval) = AF_INVALID;
	}
#line 3987 "main/idlc/source/parser.cxx"
    break;

  case 190: /* service_interface_header: flag_header IDL_INTERFACE  */
#line 1900 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_ServiceIFHeadSeen);
		if ( (AF_OPTIONAL != (yyvsp[-1].ulval)) && ( AF_INVALID != (yyvsp[-1].ulval)) )
			idlc()->error()->flagError(EIDL_OPTIONALEXPECTED, (yyvsp[-1].ulval));
		(yyval.ulval) = (yyvsp[-1].ulval);
	}
#line 3998 "main/idlc/source/parser.cxx"
    break;

  case 191: /* service_service_header: IDL_SERVICE  */
#line 1910 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_ServiceSHeadSeen);
		(yyval.ulval) = AF_INVALID;
	}
#line 4007 "main/idlc/source/parser.cxx"
    break;

  case 192: /* service_service_header: flag_header IDL_SERVICE  */
#line 1916 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_ServiceSHeadSeen);
		if ( (AF_OPTIONAL != (yyvsp[-1].ulval)) && ( AF_INVALID != (yyvsp[-1].ulval)) )
			idlc()->error()->flagError(EIDL_OPTIONALEXPECTED, (yyvsp[-1].ulval));
		(yyval.ulval) = (yyvsp[-1].ulval);
	}
#line 4018 "main/idlc/source/parser.cxx"
    break;

  case 193: /* $@51: %empty  */
#line 1926 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_ServiceSeen);
	}
#line 4026 "main/idlc/source/parser.cxx"
    break;

  case 194: /* $@52: %empty  */
#line 1930 "main/idlc/source/parser.y"
        {
        idlc()->setParseState(PS_ServiceIDSeen);
        checkIdentifier((yyvsp[0].sval));

        AstScope* 	pScope = idlc()->scopes()->topNonNull();
        AstService*	pService = NULL;

        /*
         * Make a new service and add it to the enclosing scope
         */
        if (pScope != NULL)
        {
            pService = new AstService(*(yyvsp[0].sval), pScope);
            pScope->addDeclaration(pService);
        }
        delete (yyvsp[0].sval);
        /*
         * Push it on the stack
         */
        idlc()->scopes()->push(pService);
	}
#line 4052 "main/idlc/source/parser.cxx"
    break;

  case 195: /* service_dcl: IDL_SERVICE $@51 identifier $@52 service_dfn  */
#line 1952 "main/idlc/source/parser.y"
        {
		/* this service is finished, pop its scope from the stack */ 
		idlc()->scopes()->pop();
	}
#line 4061 "main/idlc/source/parser.cxx"
    break;

  case 198: /* $@53: %empty  */
#line 1965 "main/idlc/source/parser.y"
    {
        AstScope * scope = idlc()->scopes()->nextToTop();
            // skip the scope pushed by service_dcl
        AstDeclaration * decl = scope->lookupByName(*(yyvsp[0].sval));
        if (decl != 0 && resolveTypedefs(decl)->getNodeType() == NT_interface) {
            if (idlc()->error()->checkPublished(decl)) {
                idlc()->scopes()->top()->addDeclaration(decl);
            }
        } else {
            idlc()->error()->lookupError(
                EIDL_INTERFACEMEMBER_LOOKUP, *(yyvsp[0].sval), scopeAsDecl(scope));
        }
        delete (yyvsp[0].sval);
    }
#line 4080 "main/idlc/source/parser.cxx"
    break;

  case 199: /* service_interface_dfn: ':' scoped_name $@53 opt_service_body  */
#line 1980 "main/idlc/source/parser.y"
    {
        AstService * s = static_cast< AstService * >(idlc()->scopes()->top());
        if (s != 0) {
            s->setDefaultConstructor(!(yyvsp[0].bval));
        }
    }
#line 4091 "main/idlc/source/parser.cxx"
    break;

  case 200: /* opt_service_body: service_body  */
#line 1989 "main/idlc/source/parser.y"
                 { (yyval.bval) = true; }
#line 4097 "main/idlc/source/parser.cxx"
    break;

  case 201: /* opt_service_body: %empty  */
#line 1990 "main/idlc/source/parser.y"
                  { (yyval.bval) = false; }
#line 4103 "main/idlc/source/parser.cxx"
    break;

  case 205: /* $@54: %empty  */
#line 2006 "main/idlc/source/parser.y"
    {
        checkIdentifier((yyvsp[0].sval));
        AstScope * scope = idlc()->scopes()->top();
        AstOperation * ctor = new AstOperation(OP_NONE, 0, *(yyvsp[0].sval), scope);
        delete (yyvsp[0].sval);
        scope->addDeclaration(ctor);
		idlc()->scopes()->push(ctor);
    }
#line 4116 "main/idlc/source/parser.cxx"
    break;

  case 206: /* $@55: %empty  */
#line 2018 "main/idlc/source/parser.y"
    {
        static_cast< AstOperation * >(idlc()->scopes()->top())->setExceptions(
            (yyvsp[0].dclsval));
        delete (yyvsp[0].dclsval);
        idlc()->scopes()->pop();
        if (static_cast< AstService * >(idlc()->scopes()->top())->
            checkLastConstructor())
        {
            idlc()->error()->error0(EIDL_SIMILAR_CONSTRUCTORS);
        }
    }
#line 4132 "main/idlc/source/parser.cxx"
    break;

  case 208: /* $@56: %empty  */
#line 2034 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_SingletonSeen);
	}
#line 4140 "main/idlc/source/parser.cxx"
    break;

  case 209: /* $@57: %empty  */
#line 2038 "main/idlc/source/parser.y"
        {
        idlc()->setParseState(PS_SingletonIDSeen);
        checkIdentifier((yyvsp[0].sval));

		AstScope* 	pScope = idlc()->scopes()->topNonNull();
		AstService*	pService = NULL;
		
		/*
		 * Make a new service and add it to the enclosing scope
		 */
		if (pScope != NULL) 
		{
			pService = new AstService(NT_singleton, *(yyvsp[0].sval), pScope);
			pScope->addDeclaration(pService);
		}
		delete (yyvsp[0].sval);
		/*
		 * Push it on the stack
		 */
		idlc()->scopes()->push(pService);
	}
#line 4166 "main/idlc/source/parser.cxx"
    break;

  case 210: /* singleton_dcl: IDL_SINGLETON $@56 identifier $@57 singleton_dfn  */
#line 2060 "main/idlc/source/parser.y"
        {
		/* this singleton is finished, pop its scope from the stack */
		idlc()->scopes()->pop();
	}
#line 4175 "main/idlc/source/parser.cxx"
    break;

  case 213: /* singleton_interface_dfn: ':' scoped_name  */
#line 2073 "main/idlc/source/parser.y"
    {
        AstScope * scope = idlc()->scopes()->nextToTop();
            // skip the scope (needlessly) pushed by singleton_dcl
        AstDeclaration * decl = scope->lookupByName(*(yyvsp[0].sval));
        if (decl != 0 && resolveTypedefs(decl)->getNodeType() == NT_interface) {
            if (idlc()->error()->checkPublished(decl)) {
                idlc()->scopes()->top()->addDeclaration(decl);
            }
        } else {
            idlc()->error()->lookupError(
                EIDL_INTERFACEMEMBER_LOOKUP, *(yyvsp[0].sval), scopeAsDecl(scope));
        }
        delete (yyvsp[0].sval);
    }
#line 4194 "main/idlc/source/parser.cxx"
    break;

  case 214: /* $@58: %empty  */
#line 2091 "main/idlc/source/parser.y"
    {
        idlc()->setParseState(
            idlc()->scopes()->top()->getScopeNodeType() == NT_service
            ? PS_ServiceSqSeen : PS_SingletonSqSeen);
    }
#line 4204 "main/idlc/source/parser.cxx"
    break;

  case 215: /* $@59: %empty  */
#line 2097 "main/idlc/source/parser.y"
    {
        idlc()->setParseState(
            idlc()->scopes()->top()->getScopeNodeType() == NT_service
            ? PS_ServiceBodySeen : PS_SingletonBodySeen);
    }
#line 4214 "main/idlc/source/parser.cxx"
    break;

  case 216: /* service_obsolete_dfn: '{' $@58 service_exports $@59 '}'  */
#line 2103 "main/idlc/source/parser.y"
    {
        idlc()->setParseState(
            idlc()->scopes()->top()->getScopeNodeType() == NT_service
            ? PS_ServiceQsSeen : PS_SingletonQsSeen);
    }
#line 4224 "main/idlc/source/parser.cxx"
    break;

  case 217: /* $@60: %empty  */
#line 2112 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_TypedefSeen);
	}
#line 4232 "main/idlc/source/parser.cxx"
    break;

  case 218: /* type_dcl: IDL_TYPEDEF $@60 type_declarator  */
#line 2115 "main/idlc/source/parser.y"
                        {}
#line 4238 "main/idlc/source/parser.cxx"
    break;

  case 219: /* type_dcl: struct_type  */
#line 2116 "main/idlc/source/parser.y"
                      {}
#line 4244 "main/idlc/source/parser.cxx"
    break;

  case 220: /* type_dcl: union_type  */
#line 2117 "main/idlc/source/parser.y"
                     {}
#line 4250 "main/idlc/source/parser.cxx"
    break;

  case 221: /* type_dcl: enum_type  */
#line 2118 "main/idlc/source/parser.y"
                    {}
#line 4256 "main/idlc/source/parser.cxx"
    break;

  case 222: /* $@61: %empty  */
#line 2123 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_TypeSpecSeen);
        if ((yyvsp[0].cdclval) != 0 && (yyvsp[0].cdclval)->getNodeType() == NT_instantiated_struct) {
            idlc()->error()->error0(EIDL_INSTANTIATED_STRUCT_TYPE_TYPEDEF);
        }
	}
#line 4267 "main/idlc/source/parser.cxx"
    break;

  case 223: /* type_declarator: type_spec $@61 at_least_one_declarator  */
#line 2130 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_DeclaratorsSeen);

		AstScope* 		pScope = idlc()->scopes()->topNonNull();
		AstTypeDef* 	pTypeDef = NULL;
		FeDeclList*		pList = (yyvsp[0].dlval);
		FeDeclarator*	pDecl = NULL;
        AstType const * pType = NULL;

		/*
		 * Create nodes representing typedefs and add them to the
		 * enclosing scope
		 */
		if ( pScope && (yyvsp[-2].cdclval) && pList )
		{
			FeDeclList::iterator iter = pList->begin();
			FeDeclList::iterator end = pList->end();

			while (iter != end)
			{
				pDecl = (*iter);
				if ( !pDecl )
				{
					iter++;
					continue;
				}

				pType = pDecl->compose((yyvsp[-2].cdclval));

				if ( !pType )
				{
					iter++;
					continue;
				}

				pTypeDef = new AstTypeDef(pType, pDecl->getName(), pScope);

				pScope->addDeclaration(pTypeDef);
				iter++;
				delete pDecl;
			}
			delete pList;
		}
	}
#line 4316 "main/idlc/source/parser.cxx"
    break;

  case 224: /* at_least_one_declarator: declarator declarators  */
#line 2178 "main/idlc/source/parser.y"
        {
		if ( (yyvsp[0].dlval) )
		{
			(yyvsp[0].dlval)->push_back((yyvsp[-1].fdval));
			(yyval.dlval) = (yyvsp[0].dlval);
		} else
		{
			FeDeclList* pList = new FeDeclList();
			pList->push_back((yyvsp[-1].fdval));
			(yyval.dlval) = pList;
		}
	}
#line 4333 "main/idlc/source/parser.cxx"
    break;

  case 225: /* $@62: %empty  */
#line 2195 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_DeclsCommaSeen);
	}
#line 4341 "main/idlc/source/parser.cxx"
    break;

  case 226: /* declarators: declarators ',' $@62 declarator  */
#line 2199 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_DeclsDeclSeen);
		if ( (yyvsp[-3].dlval) )
		{
			(yyvsp[-3].dlval)->push_back((yyvsp[0].fdval));
			(yyval.dlval) = (yyvsp[-3].dlval);
		} else
		{
			FeDeclList* pList = new FeDeclList();
			pList->push_back((yyvsp[0].fdval));
			(yyval.dlval) = pList;
		}
	}
#line 4359 "main/idlc/source/parser.cxx"
    break;

  case 227: /* declarators: %empty  */
#line 2213 "main/idlc/source/parser.y"
        {
		(yyval.dlval) = NULL;
	}
#line 4367 "main/idlc/source/parser.cxx"
    break;

  case 230: /* simple_declarator: identifier  */
#line 2225 "main/idlc/source/parser.y"
        {
        // For historic reasons, the struct com.sun.star.uno.Uik contains
        // members with illegal names (of the form "m_DataN"); avoid useless
        // warnings about them:
        AstScope * scope = idlc()->scopes()->top();
        if (scope == 0 || scope->getScopeNodeType() != NT_struct
            || (scopeAsDecl(scope)->getScopedName()
                != "com::sun::star::uno::Uik"))
        {
            checkIdentifier((yyvsp[0].sval));
        }

        (yyval.fdval) = new FeDeclarator(*(yyvsp[0].sval), FeDeclarator::FD_simple, NULL);
        delete (yyvsp[0].sval);
	}
#line 4387 "main/idlc/source/parser.cxx"
    break;

  case 231: /* complex_declarator: array_declarator  */
#line 2244 "main/idlc/source/parser.y"
        {
		(yyval.fdval) = new FeDeclarator((yyvsp[0].dclval)->getLocalName(), FeDeclarator::FD_complex, (yyvsp[0].dclval));
	}
#line 4395 "main/idlc/source/parser.cxx"
    break;

  case 232: /* $@63: %empty  */
#line 2251 "main/idlc/source/parser.y"
        {
        idlc()->setParseState(PS_ArrayIDSeen);
        checkIdentifier((yyvsp[0].sval));
	}
#line 4404 "main/idlc/source/parser.cxx"
    break;

  case 233: /* array_declarator: identifier $@63 at_least_one_array_dim  */
#line 2256 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_ArrayCompleted);
		(yyval.dclval) = new AstArray(*(yyvsp[-2].sval), NULL, *(yyvsp[0].exlval), idlc()->scopes()->bottom());
		delete (yyvsp[-2].sval);
	}
#line 4414 "main/idlc/source/parser.cxx"
    break;

  case 234: /* at_least_one_array_dim: array_dim array_dims  */
#line 2265 "main/idlc/source/parser.y"
        {
		if( (yyvsp[0].exlval) )
		{
			(yyvsp[0].exlval)->push_front((yyvsp[-1].exval));
			(yyval.exlval) = (yyvsp[0].exlval);
		} else
		{
			ExprList* pList = new ExprList();
			pList->push_back((yyvsp[-1].exval));
			(yyval.exlval) = pList;
		}
	}
#line 4431 "main/idlc/source/parser.cxx"
    break;

  case 235: /* array_dims: array_dims array_dim  */
#line 2281 "main/idlc/source/parser.y"
        {
		if( (yyvsp[-1].exlval) )
		{
			(yyvsp[-1].exlval)->push_back((yyvsp[0].exval));
			(yyval.exlval) = (yyvsp[-1].exlval);
		} else
		{
			ExprList* pList = new ExprList();
			pList->push_back((yyvsp[0].exval));
			(yyval.exlval) = pList;
		}
	}
#line 4448 "main/idlc/source/parser.cxx"
    break;

  case 236: /* array_dims: %empty  */
#line 2294 "main/idlc/source/parser.y"
        {
		(yyval.exlval) = NULL;
	}
#line 4456 "main/idlc/source/parser.cxx"
    break;

  case 237: /* $@64: %empty  */
#line 2301 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_DimSqSeen);
	}
#line 4464 "main/idlc/source/parser.cxx"
    break;

  case 238: /* $@65: %empty  */
#line 2305 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_DimExprSeen);
	}
#line 4472 "main/idlc/source/parser.cxx"
    break;

  case 239: /* array_dim: '[' $@64 positive_int_expr $@65 ']'  */
#line 2309 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_DimQsSeen);
		/*
		 * Array dimensions are expressions which must be coerced to
		 * positive integers
		 */
		if ( !(yyvsp[-2].exval) || !(yyvsp[-2].exval)->coerce(ET_uhyper) ) 
		{
			idlc()->error()->coercionError((yyvsp[-2].exval), ET_uhyper);
			(yyval.exval) = NULL;
		} else
			(yyval.exval) = (yyvsp[-2].exval);
	}
#line 4490 "main/idlc/source/parser.cxx"
    break;

  case 240: /* at_least_one_scoped_name: scoped_name scoped_names  */
#line 2326 "main/idlc/source/parser.y"
        {
		if ((yyvsp[0].slval))
		{
			(yyvsp[0].slval)->push_front(*(yyvsp[-1].sval));
		 	(yyval.slval) = (yyvsp[0].slval);
		} else
		{
			StringList* pNames = new StringList();
			pNames->push_back(*(yyvsp[-1].sval));
			(yyval.slval) = pNames;
		}
		delete((yyvsp[-1].sval));
	}
#line 4508 "main/idlc/source/parser.cxx"
    break;

  case 241: /* $@66: %empty  */
#line 2344 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_SNListCommaSeen);
	}
#line 4516 "main/idlc/source/parser.cxx"
    break;

  case 242: /* scoped_names: scoped_names ',' $@66 scoped_name  */
#line 2348 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_ScopedNameSeen);
		if ((yyvsp[-3].slval))
		{
			(yyvsp[-3].slval)->push_back(*(yyvsp[0].sval));
		 	(yyval.slval) = (yyvsp[-3].slval);
		} else
		{
			StringList* pNames = new StringList();
			pNames->push_back(*(yyvsp[0].sval));
			(yyval.slval) = pNames;
		}
		delete((yyvsp[0].sval));
	}
#line 4535 "main/idlc/source/parser.cxx"
    break;

  case 243: /* scoped_names: %empty  */
#line 2363 "main/idlc/source/parser.y"
        {
		(yyval.slval) = NULL;
	}
#line 4543 "main/idlc/source/parser.cxx"
    break;

  case 244: /* scoped_name: identifier  */
#line 2370 "main/idlc/source/parser.y"
        {
        idlc()->setParseState(PS_SN_IDSeen);
        checkIdentifier((yyvsp[0].sval));
        (yyval.sval) = (yyvsp[0].sval);
	}
#line 4553 "main/idlc/source/parser.cxx"
    break;

  case 245: /* $@67: %empty  */
#line 2376 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_ScopeDelimSeen);
	}
#line 4561 "main/idlc/source/parser.cxx"
    break;

  case 246: /* scoped_name: IDL_SCOPESEPARATOR $@67 identifier  */
#line 2380 "main/idlc/source/parser.y"
        {
        checkIdentifier((yyvsp[0].sval));
        OString* pName = new OString("::");
        *pName += *(yyvsp[0].sval);
        delete (yyvsp[0].sval);
        (yyval.sval) = pName;
	}
#line 4573 "main/idlc/source/parser.cxx"
    break;

  case 247: /* $@68: %empty  */
#line 2389 "main/idlc/source/parser.y"
        {
	}
#line 4580 "main/idlc/source/parser.cxx"
    break;

  case 248: /* scoped_name: scoped_name IDL_SCOPESEPARATOR $@68 identifier  */
#line 2392 "main/idlc/source/parser.y"
    {
        checkIdentifier((yyvsp[0].sval));
        *(yyvsp[-3].sval) += ::rtl::OString("::");
        *(yyvsp[-3].sval) += *(yyvsp[0].sval);
        delete (yyvsp[0].sval);
        (yyval.sval) = (yyvsp[-3].sval);
	}
#line 4592 "main/idlc/source/parser.cxx"
    break;

  case 252: /* simple_type_spec: scoped_name opt_type_args  */
#line 2409 "main/idlc/source/parser.y"
        {
        (yyval.cdclval) = createNamedType((yyvsp[-1].sval), (yyvsp[0].dclsval));
	}
#line 4600 "main/idlc/source/parser.cxx"
    break;

  case 253: /* fundamental_type: base_type_spec  */
#line 2416 "main/idlc/source/parser.y"
        {
		(yyval.cdclval) = idlc()->scopes()->bottom()->lookupPrimitiveType((yyvsp[0].etval));
	}
#line 4608 "main/idlc/source/parser.cxx"
    break;

  case 255: /* opt_type_args: '<' type_args '>'  */
#line 2423 "main/idlc/source/parser.y"
                      { (yyval.dclsval) = (yyvsp[-1].dclsval); }
#line 4614 "main/idlc/source/parser.cxx"
    break;

  case 256: /* opt_type_args: %empty  */
#line 2424 "main/idlc/source/parser.y"
                  { (yyval.dclsval) = 0; }
#line 4620 "main/idlc/source/parser.cxx"
    break;

  case 257: /* type_args: type_arg  */
#line 2429 "main/idlc/source/parser.y"
    {
        (yyval.dclsval) = new DeclList;
        (yyval.dclsval)->push_back(const_cast< AstDeclaration * >((yyvsp[0].cdclval))); //TODO: const_cast
    }
#line 4629 "main/idlc/source/parser.cxx"
    break;

  case 258: /* type_args: type_args ',' type_arg  */
#line 2434 "main/idlc/source/parser.y"
    {
        (yyvsp[-2].dclsval)->push_back(const_cast< AstDeclaration * >((yyvsp[0].cdclval))); //TODO: const_cast
        (yyval.dclsval) = (yyvsp[-2].dclsval);
    }
#line 4638 "main/idlc/source/parser.cxx"
    break;

  case 259: /* type_arg: simple_type_spec  */
#line 2442 "main/idlc/source/parser.y"
    {
        if ((yyvsp[0].cdclval) != 0 && static_cast< AstType const * >((yyvsp[0].cdclval))->isUnsigned()) {
            idlc()->error()->error0(EIDL_UNSIGNED_TYPE_ARGUMENT);
        }
        (yyval.cdclval) = (yyvsp[0].cdclval);
    }
#line 4649 "main/idlc/source/parser.cxx"
    break;

  case 270: /* signed_int: IDL_LONG  */
#line 2468 "main/idlc/source/parser.y"
        {
		(yyval.etval) = ET_long;
	}
#line 4657 "main/idlc/source/parser.cxx"
    break;

  case 271: /* signed_int: IDL_HYPER  */
#line 2472 "main/idlc/source/parser.y"
        {
		(yyval.etval) = ET_hyper;
	}
#line 4665 "main/idlc/source/parser.cxx"
    break;

  case 272: /* signed_int: IDL_SHORT  */
#line 2476 "main/idlc/source/parser.y"
        {
		(yyval.etval) = ET_short;
	}
#line 4673 "main/idlc/source/parser.cxx"
    break;

  case 273: /* unsigned_int: IDL_UNSIGNED IDL_LONG  */
#line 2483 "main/idlc/source/parser.y"
        {
		(yyval.etval) = ET_ulong;
	}
#line 4681 "main/idlc/source/parser.cxx"
    break;

  case 274: /* unsigned_int: IDL_UNSIGNED IDL_HYPER  */
#line 2487 "main/idlc/source/parser.y"
        {
		(yyval.etval) = ET_uhyper;
	}
#line 4689 "main/idlc/source/parser.cxx"
    break;

  case 275: /* unsigned_int: IDL_UNSIGNED IDL_SHORT  */
#line 2491 "main/idlc/source/parser.y"
        {
		(yyval.etval) = ET_ushort;
	}
#line 4697 "main/idlc/source/parser.cxx"
    break;

  case 276: /* floating_pt_type: IDL_DOUBLE  */
#line 2498 "main/idlc/source/parser.y"
        {
		(yyval.etval) = ET_double;
	}
#line 4705 "main/idlc/source/parser.cxx"
    break;

  case 277: /* floating_pt_type: IDL_FLOAT  */
#line 2502 "main/idlc/source/parser.y"
        {
		(yyval.etval) = ET_float;
	}
#line 4713 "main/idlc/source/parser.cxx"
    break;

  case 278: /* char_type: IDL_CHAR  */
#line 2509 "main/idlc/source/parser.y"
        {
		(yyval.etval) = ET_char;
	}
#line 4721 "main/idlc/source/parser.cxx"
    break;

  case 279: /* byte_type: IDL_BYTE  */
#line 2516 "main/idlc/source/parser.y"
        {
		(yyval.etval) = ET_byte;
	}
#line 4729 "main/idlc/source/parser.cxx"
    break;

  case 280: /* boolean_type: IDL_BOOLEAN  */
#line 2523 "main/idlc/source/parser.y"
        {
		(yyval.etval) = ET_boolean;
	}
#line 4737 "main/idlc/source/parser.cxx"
    break;

  case 281: /* any_type: IDL_ANY  */
#line 2530 "main/idlc/source/parser.y"
        {
		(yyval.etval) = ET_any;
	}
#line 4745 "main/idlc/source/parser.cxx"
    break;

  case 282: /* type_type: IDL_TYPE  */
#line 2537 "main/idlc/source/parser.y"
        {
		(yyval.etval) = ET_type;
	}
#line 4753 "main/idlc/source/parser.cxx"
    break;

  case 283: /* string_type: IDL_STRING  */
#line 2544 "main/idlc/source/parser.y"
        {
		(yyval.etval) = ET_string;
	}
#line 4761 "main/idlc/source/parser.cxx"
    break;

  case 289: /* $@69: %empty  */
#line 2562 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_ArrayTypeSeen);
	}
#line 4769 "main/idlc/source/parser.cxx"
    break;

  case 290: /* array_type: simple_type_spec $@69 at_least_one_array_dim  */
#line 2566 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_ArrayCompleted);

		AstScope* pScope = idlc()->scopes()->bottom();
		AstDeclaration* pDecl = NULL;
		AstDeclaration* pArray = NULL;

		if ( (yyvsp[-2].cdclval) )
		{
			pArray = new AstArray((AstType*)(yyvsp[-2].cdclval), *(yyvsp[0].exlval), idlc()->scopes()->bottom());
			if ( pScope )
			{
				pDecl = pScope->addDeclaration(pArray);				
				if ( pArray != pDecl )
				{
					// if array type already defined then use it
					delete pArray;
					pArray = pDecl;					
				}
			}
		}
		(yyval.cdclval) = pArray; 
	}
#line 4797 "main/idlc/source/parser.cxx"
    break;

  case 291: /* $@70: %empty  */
#line 2593 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_SequenceSeen);
		/*
		 * Push a sequence marker on scopes stack
		 */
		idlc()->scopes()->push(NULL);
	}
#line 4809 "main/idlc/source/parser.cxx"
    break;

  case 292: /* $@71: %empty  */
#line 2601 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_SequenceSqSeen);
	}
#line 4817 "main/idlc/source/parser.cxx"
    break;

  case 293: /* $@72: %empty  */
#line 2605 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_SequenceTypeSeen);
	}
#line 4825 "main/idlc/source/parser.cxx"
    break;

  case 294: /* sequence_type_spec: IDL_SEQUENCE $@70 '<' $@71 simple_type_spec $@72 '>'  */
#line 2609 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_SequenceQsSeen);
		/*
		 * Remove sequence marker from scopes stack
		 */
		if (idlc()->scopes()->top() == NULL)
			idlc()->scopes()->pop();
		/*
		 * Create a node representing a sequence
		 */
		AstScope* pScope = idlc()->scopes()->bottom();
		AstDeclaration* pDecl = NULL;
		AstDeclaration* pSeq = NULL;
		
		if ( (yyvsp[-2].cdclval) )
		{
			AstType *pType = (AstType*)(yyvsp[-2].cdclval);
			if ( pType )
			{
				pSeq = new AstSequence(pType, pScope);
				/*
				 * Add this AstSequence to the types defined in the global scope
				 */
				pDecl = pScope->addDeclaration(pSeq);
				if ( pSeq != pDecl )
				{
					// if sequence type already defined then use it
					delete pSeq;
					pSeq = pDecl;
				}
			}
        }
		(yyval.cdclval) = pSeq;
	}
#line 4864 "main/idlc/source/parser.cxx"
    break;

  case 295: /* sequence_type_spec: error '>'  */
#line 2644 "main/idlc/source/parser.y"
        {
		yyerror("sequence declaration");
		yyerrok;
        (yyval.cdclval) = 0;
	}
#line 4874 "main/idlc/source/parser.cxx"
    break;

  case 296: /* $@73: %empty  */
#line 2653 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_StructHeaderSeen);

		AstScope* 	pScope = idlc()->scopes()->topNonNull();
		AstStruct*	pStruct = NULL;

		if ( pScope )
		{
			AstStruct* pBase= static_cast< AstStruct* >((yyvsp[0].ihval)->getInherits());
            pStruct = new AstStruct(
                *(yyvsp[0].ihval)->getName(), (yyvsp[0].ihval)->getTypeParameters(), pBase, pScope);
			pScope->addDeclaration(pStruct);
		}
		/*
		 * Push the scope of the struct on the scopes stack
		 */
		idlc()->scopes()->push(pStruct);
		delete (yyvsp[0].ihval);
	}
#line 4898 "main/idlc/source/parser.cxx"
    break;

  case 297: /* $@74: %empty  */
#line 2673 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_StructSqSeen);
	}
#line 4906 "main/idlc/source/parser.cxx"
    break;

  case 298: /* $@75: %empty  */
#line 2677 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_StructBodySeen);
	}
#line 4914 "main/idlc/source/parser.cxx"
    break;

  case 299: /* struct_type: structure_header $@73 '{' $@74 at_least_one_member $@75 '}'  */
#line 2681 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_StructQsSeen);
		/* this exception is finished, pop its scope from the stack */ 
		idlc()->scopes()->pop();
	}
#line 4924 "main/idlc/source/parser.cxx"
    break;

  case 300: /* $@76: %empty  */
#line 2690 "main/idlc/source/parser.y"
        {
        idlc()->setParseState(PS_StructSeen);
	}
#line 4932 "main/idlc/source/parser.cxx"
    break;

  case 301: /* $@77: %empty  */
#line 2694 "main/idlc/source/parser.y"
        {
        idlc()->setParseState(PS_StructIDSeen);
        checkIdentifier((yyvsp[0].sval));
	}
#line 4941 "main/idlc/source/parser.cxx"
    break;

  case 302: /* structure_header: IDL_STRUCT $@76 identifier $@77 opt_type_params inheritance_spec  */
#line 2700 "main/idlc/source/parser.y"
        {
        idlc()->setParseState(PS_InheritSpecSeen);

        // Polymorphic struct type templates with base types would cause various
        // problems in language bindings, so forbid them here.  For example,
        // GCC prior to version 3.4 fails with code like
        //
        //  struct Base { ... };
        //  template< typename typeparam_T > struct Derived: public Base {
        //      int member1 CPPU_GCC3_ALIGN(Base);
        //      ... };
        //
        // (Note that plain struct types with instantiated polymorphic struct
        // type bases, which might also cause problems in language bindings, are
        // already rejected on a syntactic level.)
        if ((yyvsp[-1].svals) != 0 && (yyvsp[0].sval) != 0) {
            idlc()->error()->error0(EIDL_STRUCT_TYPE_TEMPLATE_WITH_BASE);
        }

        (yyval.ihval) = new FeInheritanceHeader(NT_struct, (yyvsp[-3].sval), (yyvsp[0].sval), (yyvsp[-1].svals));
        delete (yyvsp[-1].svals);
        delete (yyvsp[0].sval);
	}
#line 4969 "main/idlc/source/parser.cxx"
    break;

  case 303: /* opt_type_params: '<' type_params '>'  */
#line 2726 "main/idlc/source/parser.y"
                        { (yyval.svals) = (yyvsp[-1].svals); }
#line 4975 "main/idlc/source/parser.cxx"
    break;

  case 304: /* opt_type_params: %empty  */
#line 2727 "main/idlc/source/parser.y"
                  { (yyval.svals) = 0; }
#line 4981 "main/idlc/source/parser.cxx"
    break;

  case 305: /* type_params: identifier  */
#line 2732 "main/idlc/source/parser.y"
    {
        (yyval.svals) = new std::vector< rtl::OString >;
        (yyval.svals)->push_back(*(yyvsp[0].sval));
        delete (yyvsp[0].sval);
    }
#line 4991 "main/idlc/source/parser.cxx"
    break;

  case 306: /* type_params: type_params ',' identifier  */
#line 2738 "main/idlc/source/parser.y"
    {
        if (std::find((yyvsp[-2].svals)->begin(), (yyvsp[-2].svals)->end(), *(yyvsp[0].sval)) != (yyvsp[-2].svals)->end()) {
            idlc()->error()->error0(EIDL_IDENTICAL_TYPE_PARAMETERS);
        }
        (yyvsp[-2].svals)->push_back(*(yyvsp[0].sval));
        delete (yyvsp[0].sval);
        (yyval.svals) = (yyvsp[-2].svals);
    }
#line 5004 "main/idlc/source/parser.cxx"
    break;

  case 310: /* $@78: %empty  */
#line 2757 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_MemberTypeSeen);
	}
#line 5012 "main/idlc/source/parser.cxx"
    break;

  case 311: /* $@79: %empty  */
#line 2761 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_MemberDeclsSeen);
	}
#line 5020 "main/idlc/source/parser.cxx"
    break;

  case 312: /* member: type_or_parameter $@78 at_least_one_declarator $@79 ';'  */
#line 2765 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_MemberDeclsCompleted);

		AstScope* 		pScope = idlc()->scopes()->topNonNull();
		AstMember*		pMember = NULL;
		FeDeclList*		pList = (yyvsp[-2].dlval);
		FeDeclarator*	pDecl = NULL;
        AstType const * pType = NULL;

		// !!! check recursive type

		if ( pScope && pList && (yyvsp[-4].cdclval) )
		{
			FeDeclList::iterator iter = pList->begin();
			FeDeclList::iterator end = pList->end();
			while (iter != end)
			{
				pDecl = (*iter);
				if ( !pDecl )
				{
					iter++;
					continue;
				}

				pType = pDecl->compose((yyvsp[-4].cdclval));				

				if ( !pType )
				{
					iter++;
					continue;
				}

				pMember = new AstMember(pType, pDecl->getName(), pScope);

				if ( !pDecl->checkType((yyvsp[-4].cdclval)) )
				{
					// WARNING	
				}

				pScope->addDeclaration(pMember);
				iter++;
				delete pDecl;
			}
			delete pList;
		}
	}
#line 5071 "main/idlc/source/parser.cxx"
    break;

  case 313: /* member: error ';'  */
#line 2812 "main/idlc/source/parser.y"
        {
		yyerror("member definition");
		yyerrok;
	}
#line 5080 "main/idlc/source/parser.cxx"
    break;

  case 315: /* type_or_parameter: scoped_name opt_type_args  */
#line 2821 "main/idlc/source/parser.y"
        {
        AstDeclaration const * decl = 0;
        AstStruct * scope = static_cast< AstStruct * >(idlc()->scopes()->top());
        if (scope != 0 && (yyvsp[0].dclsval) == 0) {
            decl = scope->findTypeParameter(*(yyvsp[-1].sval));
        }
        if (decl != 0) {
            delete (yyvsp[-1].sval);
            delete (yyvsp[0].dclsval);
        } else {
            decl = createNamedType((yyvsp[-1].sval), (yyvsp[0].dclsval));
            if (scope != 0 && includes(decl, scopeAsDecl(scope))) {
                idlc()->error()->error1(
                    EIDL_RECURSIVE_TYPE, scopeAsDecl(scope));
                decl = 0;
            }
        }
        (yyval.cdclval) = decl;
	}
#line 5104 "main/idlc/source/parser.cxx"
    break;

  case 316: /* $@80: %empty  */
#line 2844 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_EnumSeen);
	}
#line 5112 "main/idlc/source/parser.cxx"
    break;

  case 317: /* $@81: %empty  */
#line 2848 "main/idlc/source/parser.y"
        {
        idlc()->setParseState(PS_EnumIDSeen);
        checkIdentifier((yyvsp[0].sval));

		AstScope* 		pScope = idlc()->scopes()->topNonNull();
		AstEnum*		pEnum = NULL;

		/*
		 * Create a node representing an enum and add it to its
		 * enclosing scope
		 */
		if (pScope != NULL) 
		{
			pEnum = new AstEnum(*(yyvsp[0].sval), pScope);
			/*
			 * Add it to its defining scope
			 */
			pScope->addDeclaration(pEnum);
		}
		delete (yyvsp[0].sval);
		/*
		 * Push the enum scope on the scopes stack
		 */
		idlc()->scopes()->push(pEnum);
		
	}
#line 5143 "main/idlc/source/parser.cxx"
    break;

  case 318: /* $@82: %empty  */
#line 2875 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_EnumSqSeen);
	}
#line 5151 "main/idlc/source/parser.cxx"
    break;

  case 319: /* $@83: %empty  */
#line 2879 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_EnumBodySeen);
	}
#line 5159 "main/idlc/source/parser.cxx"
    break;

  case 320: /* enum_type: IDL_ENUM $@80 identifier $@81 '{' $@82 at_least_one_enumerator $@83 '}'  */
#line 2883 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_EnumQsSeen);
		/*
		 * Done with this enum. Pop its scope from the scopes stack
		 */
		if (idlc()->scopes()->top() == NULL)
			(yyval.cdclval) = NULL;
		else 
		{
			(yyval.cdclval) = (AstEnum*)idlc()->scopes()->topNonNull();
			idlc()->scopes()->pop();
		}
	}
#line 5177 "main/idlc/source/parser.cxx"
    break;

  case 322: /* $@84: %empty  */
#line 2903 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_EnumCommaSeen);
	}
#line 5185 "main/idlc/source/parser.cxx"
    break;

  case 325: /* enumerators: error ','  */
#line 2909 "main/idlc/source/parser.y"
        {
		yyerror("enumerator definition");
		yyerrok;
	}
#line 5194 "main/idlc/source/parser.cxx"
    break;

  case 326: /* enumerator: identifier  */
#line 2917 "main/idlc/source/parser.y"
        {
        checkIdentifier((yyvsp[0].sval));

		AstScope* 		pScope = idlc()->scopes()->topNonNull();
		AstEnum*		pEnum = NULL;
		AstConstant* 	pEnumVal = NULL;

		if ( pScope && pScope->getScopeNodeType() == NT_enum) 
		{
			pEnum = (AstEnum*)pScope;
			if (pEnum && (yyvsp[0].sval))
			{
				AstExpression* pExpr = new AstExpression(pEnum->getEnumValueCount());
				pEnumVal = new AstConstant(ET_long , NT_enum_val, 
										   pExpr, *(yyvsp[0].sval), pScope);
			}
			if ( pEnum->checkValue(pEnumVal->getConstValue()) )
				idlc()->error()->error1(EIDL_EVAL_ERROR, pEnum);				

			pScope->addDeclaration(pEnumVal);
		}
		delete (yyvsp[0].sval);
	}
#line 5222 "main/idlc/source/parser.cxx"
    break;

  case 327: /* enumerator: identifier '=' const_expr  */
#line 2943 "main/idlc/source/parser.y"
        {
        checkIdentifier((yyvsp[-2].sval));

		AstScope* 		pScope = idlc()->scopes()->topNonNull();
		AstEnum*		pEnum = NULL;
		AstConstant* 	pEnumVal = NULL;

		if ( (yyvsp[0].exval) && pScope && pScope->getScopeNodeType() == NT_enum) 
		{
			(yyvsp[0].exval)->evaluate(EK_const);
			if ( (yyvsp[0].exval)->coerce(ET_long) )
			{
				pEnum = (AstEnum*)pScope;
				if (pEnum)
				{
					pEnumVal = new AstConstant(ET_long , NT_enum_val, 
											   (yyvsp[0].exval), *(yyvsp[-2].sval), pScope);
				}
				if ( pEnum->checkValue(pEnumVal->getConstValue()) )
					idlc()->error()->error1(EIDL_EVAL_ERROR, pEnum);				

				pScope->addDeclaration(pEnumVal);
			} else
			{
				idlc()->error()->coercionError((yyvsp[0].exval), ET_long);
				delete (yyvsp[0].exval);
			}
		}
		delete (yyvsp[-2].sval);
	}
#line 5257 "main/idlc/source/parser.cxx"
    break;

  case 328: /* $@85: %empty  */
#line 2977 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_UnionSeen);
	}
#line 5265 "main/idlc/source/parser.cxx"
    break;

  case 329: /* $@86: %empty  */
#line 2981 "main/idlc/source/parser.y"
        {
        idlc()->setParseState(PS_UnionIDSeen);
        checkIdentifier((yyvsp[0].sval));
	}
#line 5274 "main/idlc/source/parser.cxx"
    break;

  case 330: /* $@87: %empty  */
#line 2986 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_SwitchSeen);
	}
#line 5282 "main/idlc/source/parser.cxx"
    break;

  case 331: /* $@88: %empty  */
#line 2990 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_SwitchOpenParSeen);
	}
#line 5290 "main/idlc/source/parser.cxx"
    break;

  case 332: /* $@89: %empty  */
#line 2994 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_SwitchTypeSeen);
	}
#line 5298 "main/idlc/source/parser.cxx"
    break;

  case 333: /* $@90: %empty  */
#line 2998 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_SwitchCloseParSeen);

		AstScope* 		pScope = idlc()->scopes()->topNonNull();
		AstUnion*		pUnion = NULL;

		/*
		 * Create a node representing a union. Add it to its enclosing
		 * scope
		 */
		if ( (yyvsp[-2].cdclval) && pScope ) 
		{
			AstType* pType = (AstType*)(yyvsp[-2].cdclval);
			if ( !pType) 
			{
				idlc()->error()->noTypeError((yyvsp[-2].cdclval));
			} else 
			{
				pUnion = new AstUnion(*(yyvsp[-8].sval), pType, pScope);
				pScope->addDeclaration(pUnion);
			}
		}
		delete (yyvsp[-8].sval);
		/*
		 * Push the scope of the union on the scopes stack
		 */
		idlc()->scopes()->push(pUnion);
	}
#line 5331 "main/idlc/source/parser.cxx"
    break;

  case 334: /* $@91: %empty  */
#line 3027 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_UnionSqSeen);
	}
#line 5339 "main/idlc/source/parser.cxx"
    break;

  case 335: /* $@92: %empty  */
#line 3031 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_UnionBodySeen);
	}
#line 5347 "main/idlc/source/parser.cxx"
    break;

  case 336: /* union_type: IDL_UNION $@85 identifier $@86 IDL_SWITCH $@87 '(' $@88 switch_type_spec $@89 ')' $@90 '{' $@91 at_least_one_case_branch $@92 '}'  */
#line 3035 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_UnionQsSeen);
		/* this union is finished, pop its scope from the stack */ 
		idlc()->scopes()->pop();
	}
#line 5357 "main/idlc/source/parser.cxx"
    break;

  case 337: /* switch_type_spec: integer_type  */
#line 3044 "main/idlc/source/parser.y"
        {	
		(yyval.cdclval) = idlc()->scopes()->bottom()->lookupPrimitiveType((yyvsp[0].etval));
	}
#line 5365 "main/idlc/source/parser.cxx"
    break;

  case 338: /* switch_type_spec: char_type  */
#line 3048 "main/idlc/source/parser.y"
        {
		(yyval.cdclval) = idlc()->scopes()->bottom()->lookupPrimitiveType((yyvsp[0].etval));
	}
#line 5373 "main/idlc/source/parser.cxx"
    break;

  case 339: /* switch_type_spec: boolean_type  */
#line 3052 "main/idlc/source/parser.y"
        {
		(yyval.cdclval) = idlc()->scopes()->bottom()->lookupPrimitiveType((yyvsp[0].etval));
	}
#line 5381 "main/idlc/source/parser.cxx"
    break;

  case 341: /* switch_type_spec: scoped_name  */
#line 3057 "main/idlc/source/parser.y"
        {
		AstScope* 		pScope = idlc()->scopes()->topNonNull();
		AstBaseType*	pBaseType = NULL;
        AstDeclaration const * pDecl = NULL;
		AstTypeDef*		pTypeDef = NULL;
		sal_Bool		bFound = sal_False;
		/*
		 * If the constant's type is a scoped name, it must resolve
		 * to a scalar constant type
		 */
		if ( pScope && (pDecl = pScope->lookupByName(*(yyvsp[0].sval))) ) 
		{
			/*
			 * Look through typedefs
			 */
			while ( !bFound ) 
			{
				switch (pDecl->getNodeType()) 
				{
					case NT_enum:
						(yyval.cdclval) = pDecl;
						bFound = sal_True;
						break;
					case NT_predefined:
						pBaseType = (AstBaseType*)pDecl;
						if ( pBaseType ) 
						{
							switch (pBaseType->getExprType()) 
							{
								case ET_short:
								case ET_ushort:
								case ET_long:
								case ET_ulong:
								case ET_hyper:
								case ET_uhyper:
								case ET_char:
								case ET_byte:
								case ET_boolean:
									(yyval.cdclval) = pBaseType;
									bFound = sal_True;
									break;
								default:
									(yyval.cdclval) = NULL;
									bFound = sal_True;
									break;
							}
						}
						break;
					case NT_typedef:
						pTypeDef = (AstTypeDef*)pDecl;
						if ( pTypeDef )
							pDecl = pTypeDef->getBaseType();
						break;
					default:
						(yyval.cdclval) = NULL;
						bFound = sal_True;
		               break;		
				}
			}
		} else
			(yyval.cdclval) = NULL;

		if ((yyval.cdclval) == NULL)
			idlc()->error()->lookupError(*(yyvsp[0].sval));
	}
#line 5451 "main/idlc/source/parser.cxx"
    break;

  case 345: /* $@93: %empty  */
#line 3133 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_UnionLabelSeen);
	}
#line 5459 "main/idlc/source/parser.cxx"
    break;

  case 346: /* case_branch: at_least_one_case_label $@93 element_spec  */
#line 3137 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_UnionElemSeen);

		AstScope* 		pScope = idlc()->scopes()->topNonNull();
		AstUnionLabel*	pLabel = NULL;
		AstUnionBranch* pBranch = NULL;
		AstMember*		pMember = (yyvsp[0].mval);

		/*
		 * Create several nodes representing branches of a union.
		 * Add them to the enclosing scope (the union scope)
		 */
		if ( pScope && (yyvsp[-2].llval) && (yyvsp[0].mval) ) 
		{
			LabelList::iterator iter = (yyvsp[-2].llval)->begin();
			LabelList::iterator end = (yyvsp[-2].llval)->end();
			for (;iter != end; iter++) 
			{
				pLabel = *iter;
				if ( !pLabel )
				{
					iter++;
					continue;
				}
				pBranch = new AstUnionBranch(pLabel, pMember->getType(),
                                        	 pMember->getLocalName(), pScope);
				pScope->addDeclaration(pBranch);
			}
		}
		if ( (yyvsp[-2].llval) ) delete((yyvsp[-2].llval));
	}
#line 5495 "main/idlc/source/parser.cxx"
    break;

  case 347: /* at_least_one_case_label: case_label case_labels  */
#line 3172 "main/idlc/source/parser.y"
        {
		if ( (yyvsp[0].llval) )
		{
			(yyvsp[0].llval)->push_front((yyvsp[-1].lbval));
		 	(yyval.llval) = (yyvsp[0].llval);
		} else
		{
			LabelList* pLabels = new LabelList();
			pLabels->push_back((yyvsp[-1].lbval));
			(yyval.llval) = pLabels;
		}
	}
#line 5512 "main/idlc/source/parser.cxx"
    break;

  case 348: /* case_labels: case_labels case_label  */
#line 3188 "main/idlc/source/parser.y"
        {
		if ( (yyvsp[-1].llval) )
		{
			(yyvsp[-1].llval)->push_back((yyvsp[0].lbval));
		 	(yyval.llval) = (yyvsp[-1].llval);
		} else
		{
			LabelList* pLabels = new LabelList();
			pLabels->push_back((yyvsp[0].lbval));
			(yyval.llval) = pLabels;
		}
	}
#line 5529 "main/idlc/source/parser.cxx"
    break;

  case 349: /* case_labels: %empty  */
#line 3201 "main/idlc/source/parser.y"
        {
		(yyval.llval) = NULL;
	}
#line 5537 "main/idlc/source/parser.cxx"
    break;

  case 350: /* $@94: %empty  */
#line 3208 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_DefaultSeen);
	}
#line 5545 "main/idlc/source/parser.cxx"
    break;

  case 351: /* case_label: IDL_DEFAULT $@94 ':'  */
#line 3212 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_LabelColonSeen);
		(yyval.lbval) = new AstUnionLabel(UL_default, NULL);
	}
#line 5554 "main/idlc/source/parser.cxx"
    break;

  case 352: /* $@95: %empty  */
#line 3217 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_CaseSeen);
	}
#line 5562 "main/idlc/source/parser.cxx"
    break;

  case 353: /* $@96: %empty  */
#line 3221 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_LabelExprSeen);
	}
#line 5570 "main/idlc/source/parser.cxx"
    break;

  case 354: /* case_label: IDL_CASE $@95 const_expr $@96 ':'  */
#line 3225 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_LabelColonSeen);
		(yyval.lbval) = new AstUnionLabel(UL_label, (yyvsp[-2].exval));
	}
#line 5579 "main/idlc/source/parser.cxx"
    break;

  case 355: /* $@97: %empty  */
#line 3233 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_UnionElemTypeSeen);
	}
#line 5587 "main/idlc/source/parser.cxx"
    break;

  case 356: /* $@98: %empty  */
#line 3237 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_UnionElemDeclSeen);
	}
#line 5595 "main/idlc/source/parser.cxx"
    break;

  case 357: /* element_spec: type_spec $@97 declarator $@98 ';'  */
#line 3241 "main/idlc/source/parser.y"
        {
		idlc()->setParseState(PS_UnionElemCompleted);

		AstScope* pScope = idlc()->scopes()->topNonNull();
		/*
		 * Check for illegal recursive use of type
		 */
//		if ( $1 && AST_illegal_recursive_type($1))
//			idlc()->error()->error1(EIDL_RECURSIVE_TYPE, $1);
		/*
		 * Create a field in a union branch
		 */
		if ( (yyvsp[-4].cdclval) && (yyvsp[-2].fdval) )
		{
            AstType const * pType = (yyvsp[-2].fdval)->compose((yyvsp[-4].cdclval));
			if ( !pType )
				(yyval.mval) = NULL;
			else
				(yyval.mval) = new AstMember(pType, (yyvsp[-2].fdval)->getName(), pScope);
		} else
			(yyval.mval) = NULL;
		
		if ( (yyvsp[-2].fdval) ) delete (yyvsp[-2].fdval);
	}
#line 5624 "main/idlc/source/parser.cxx"
    break;

  case 358: /* element_spec: error ';'  */
#line 3267 "main/idlc/source/parser.y"
        {
		(yyval.mval) = NULL;
	}
#line 5632 "main/idlc/source/parser.cxx"
    break;

  case 360: /* identifier: IDL_GET  */
#line 3274 "main/idlc/source/parser.y"
              { (yyval.sval) = new OString("get"); }
#line 5638 "main/idlc/source/parser.cxx"
    break;

  case 361: /* identifier: IDL_SET  */
#line 3275 "main/idlc/source/parser.y"
              { (yyval.sval) = new OString("set"); }
#line 5644 "main/idlc/source/parser.cxx"
    break;

  case 362: /* identifier: IDL_PUBLISHED  */
#line 3276 "main/idlc/source/parser.y"
                    { (yyval.sval) = new OString("published"); }
#line 5650 "main/idlc/source/parser.cxx"
    break;


#line 5654 "main/idlc/source/parser.cxx"

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

#line 3279 "main/idlc/source/parser.y"


/*
 * Report an error situation discovered in a production
 */
void yyerror(char const *errmsg)
{
	idlc()->error()->syntaxError(idlc()->getParseState(), idlc()->getLineNumber(), errmsg);
	idlc()->setParseState(PS_NoState);
}
