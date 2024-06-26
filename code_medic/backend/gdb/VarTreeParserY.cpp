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
#define YYPURE 1

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* "%code top" blocks.  */
#line 3 "backend/gdb/VarTreeParserY.y"

#include "gdb/VarTreeParser.h"
#include "globals.h"
#include <jx-af/jcore/JStringIterator.h>

#define yyparse gdb::VarTreeParser::yyparse

// also uncomment yydebug=1; below
//#define YYERROR_VERBOSE
//#define YYDEBUG 1

inline bool
isOpenablePointer
	(
	const JString& s
	)
{
	return s != "0x0";
}


#line 90 "backend/gdb/VarTreeParserY.cpp"




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

#include "VarTreeParserY.hpp"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_P_NAME = 3,                     /* P_NAME  */
  YYSYMBOL_P_NAME_EQ = 4,                  /* P_NAME_EQ  */
  YYSYMBOL_P_EMPTY_BRACKET = 5,            /* P_EMPTY_BRACKET  */
  YYSYMBOL_P_EMPTY_BRACKET_EQ = 6,         /* P_EMPTY_BRACKET_EQ  */
  YYSYMBOL_P_NO_DATA_FIELDS = 7,           /* P_NO_DATA_FIELDS  */
  YYSYMBOL_P_INTEGER = 8,                  /* P_INTEGER  */
  YYSYMBOL_P_HEX = 9,                      /* P_HEX  */
  YYSYMBOL_P_FLOAT = 10,                   /* P_FLOAT  */
  YYSYMBOL_P_CHAR = 11,                    /* P_CHAR  */
  YYSYMBOL_P_STRING = 12,                  /* P_STRING  */
  YYSYMBOL_P_EMPTY_SUMMARY = 13,           /* P_EMPTY_SUMMARY  */
  YYSYMBOL_P_STATIC = 14,                  /* P_STATIC  */
  YYSYMBOL_P_BRACKET = 15,                 /* P_BRACKET  */
  YYSYMBOL_P_BRACKET_EQ = 16,              /* P_BRACKET_EQ  */
  YYSYMBOL_P_GROUP_OPEN = 17,              /* P_GROUP_OPEN  */
  YYSYMBOL_P_GROUP_CLOSE = 18,             /* P_GROUP_CLOSE  */
  YYSYMBOL_P_PAREN_EXPR = 19,              /* P_PAREN_EXPR  */
  YYSYMBOL_P_SUMMARY = 20,                 /* P_SUMMARY  */
  YYSYMBOL_P_EOF = 21,                     /* P_EOF  */
  YYSYMBOL_22_ = 22,                       /* ','  */
  YYSYMBOL_23_ = 23,                       /* '.'  */
  YYSYMBOL_24_ = 24,                       /* '@'  */
  YYSYMBOL_25_ = 25,                       /* ':'  */
  YYSYMBOL_YYACCEPT = 26,                  /* $accept  */
  YYSYMBOL_top_group = 27,                 /* top_group  */
  YYSYMBOL_group = 28,                     /* group  */
  YYSYMBOL_node_list = 29,                 /* node_list  */
  YYSYMBOL_node = 30,                      /* node  */
  YYSYMBOL_name_eq = 31,                   /* name_eq  */
  YYSYMBOL_value_list = 32,                /* value_list  */
  YYSYMBOL_value_node = 33,                /* value_node  */
  YYSYMBOL_value = 34,                     /* value  */
  YYSYMBOL_name = 35,                      /* name  */
  YYSYMBOL_reference_value = 36            /* reference_value  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;



/* Unqualified %code blocks.  */
#line 50 "backend/gdb/VarTreeParserY.y"


// debugging output

#define YYPRINT(file, type, value)	yyprint(file, type, value)
#include <stdio.h>

inline void
yyprint
	(
	FILE*	file,
	int		type,
	YYSTYPE	value
	)
{
	if (value.pString != nullptr)
	{
		fprintf(file, "string:  %s", value.pString->GetBytes());
	}
}

// must be last
#include <jx-af/jcore/jAssert.h>

#line 189 "backend/gdb/VarTreeParserY.cpp"

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
typedef yytype_int8 yy_state_t;

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
#define YYFINAL  51
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   174

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  26
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  11
/* YYNRULES -- Number of rules.  */
#define YYNRULES  64
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  107

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   276


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
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,    22,     2,    23,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    25,     2,
       2,     2,     2,     2,    24,     2,     2,     2,     2,     2,
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
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,    87,    87,   101,   118,   129,   145,   150,   155,   164,
     169,   174,   183,   188,   193,   205,   211,   217,   234,   244,
     263,   273,   292,   311,   319,   336,   338,   347,   349,   358,
     366,   377,   382,   388,   394,   406,   418,   420,   422,   424,
     426,   432,   439,   449,   451,   453,   462,   474,   484,   493,
     502,   511,   520,   522,   533,   542,   551,   556,   561,   563,
     572,   574,   583,   588,   598
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
  "\"end of file\"", "error", "\"invalid token\"", "P_NAME", "P_NAME_EQ",
  "P_EMPTY_BRACKET", "P_EMPTY_BRACKET_EQ", "P_NO_DATA_FIELDS", "P_INTEGER",
  "P_HEX", "P_FLOAT", "P_CHAR", "P_STRING", "P_EMPTY_SUMMARY", "P_STATIC",
  "P_BRACKET", "P_BRACKET_EQ", "P_GROUP_OPEN", "P_GROUP_CLOSE",
  "P_PAREN_EXPR", "P_SUMMARY", "P_EOF", "','", "'.'", "'@'", "':'",
  "$accept", "top_group", "group", "node_list", "node", "name_eq",
  "value_list", "value_node", "value", "name", "reference_value", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-34)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-61)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
      83,    47,   -34,    72,    36,    -9,    -2,   -34,   -34,   -34,
      16,    28,    21,    45,    46,    69,    78,   101,   -34,   -34,
     -34,   -34,   -34,   -34,   -34,    41,    86,    89,   -34,   -34,
      -8,   -34,   -34,   -34,    -1,   -34,   -34,    -5,   108,    79,
     -13,   -34,   101,    35,   -34,   -34,   111,    65,   140,    65,
     122,   -34,   -34,   -34,   105,   129,   -34,   -34,   -34,   -34,
     137,   -34,   -34,   -34,   -34,    44,   -34,   138,   133,   -34,
     -34,   -34,   119,   139,   -34,    97,    37,   117,   135,   115,
     141,   143,   -34,   -34,   -34,   -34,    65,   150,   -34,   152,
     -34,   -34,   -34,   146,   153,   161,   -34,   149,   -34,   -34,
     -34,   -34,   -34,   -34,   -34,   -34,   -34
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,    36,    37,    38,    40,    39,     0,    44,    43,    52,
       0,    57,     0,     0,     0,     0,     0,     0,    48,    54,
      55,    49,    58,    62,    45,     0,    47,     0,    50,    51,
      36,    25,    31,    23,     0,    27,    56,    57,     0,     0,
       0,    15,    24,     0,    32,    35,    42,     0,     0,     0,
      41,     1,     2,     4,     0,     0,    59,    61,    53,    29,
       0,    26,    28,    41,     8,     0,     6,     0,     0,    22,
      21,     7,     0,     0,    46,     0,     0,     0,     0,     0,
       0,     0,    64,     3,     5,    30,     0,     0,    18,     0,
      17,    19,    16,     0,     0,    57,    33,     0,    14,    12,
      13,    63,    11,     9,    10,    20,    34
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -34,   -34,     0,   -33,   106,   -34,    99,   102,     1,   -34,
     -34
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
       0,    14,    39,    40,    41,    42,    43,    44,    45,    27,
      17
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int8 yytable[] =
{
      15,    16,    60,    61,    46,    66,    28,    18,    59,    67,
      68,    19,    47,    29,    76,    62,    80,    54,    55,    30,
      31,     2,    32,    33,     3,     4,     5,     6,     7,     8,
      34,     9,    35,    10,    36,    37,    12,    46,    49,    22,
      38,    23,    69,    70,    56,    47,    51,    75,    24,    79,
      25,    26,    48,    71,    50,    99,    57,    72,    73,    67,
      68,    86,    18,    87,    12,    88,    19,    91,    30,    31,
       2,    32,    33,     3,     4,     5,     6,     7,     8,    34,
       9,    35,    10,    20,    37,    12,     1,    21,     2,    38,
      52,     3,     4,     5,     6,     7,     8,    64,     9,    53,
      10,    65,    11,    12,     1,   -60,     2,    13,    58,     3,
       4,     5,     6,     7,     8,    98,     9,    63,    10,    65,
      37,    12,     1,    74,     2,    38,    83,     3,     4,     5,
       6,     7,     8,   102,     9,   100,    94,    65,    95,    72,
      73,    89,    31,    38,    32,    33,    77,    82,    81,    78,
      84,    90,    34,    85,    35,    86,    93,    87,    12,   103,
     101,   104,    97,    67,    68,    72,    73,    47,    59,   105,
      46,    36,   106,    92,    96
};

static const yytype_int8 yycheck[] =
{
       0,     0,     3,     4,     9,    18,    15,    15,    16,    22,
      23,    19,    17,    15,    47,    16,    49,    17,    17,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    18,    19,    20,     9,    17,     3,
      24,     5,    42,    42,     3,    17,     0,    47,    12,    49,
      14,    15,    24,    18,     9,    18,    15,    22,    23,    22,
      23,    17,    15,    19,    20,    65,    19,    67,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    11,    19,    20,     3,    15,     5,    24,
      21,     8,     9,    10,    11,    12,    13,    18,    15,    21,
      17,    22,    19,    20,     3,    19,     5,    24,    19,     8,
       9,    10,    11,    12,    13,    18,    15,     9,    17,    22,
      19,    20,     3,    12,     5,    24,    21,     8,     9,    10,
      11,    12,    13,    18,    15,    18,    17,    22,    19,    22,
      23,     3,     4,    24,     6,     7,    47,    25,    49,     9,
      21,    13,    14,    16,    16,    17,    23,    19,    20,    18,
      25,    18,    23,    22,    23,    22,    23,    17,    16,    23,
       9,    18,    23,    67,    72
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     3,     5,     8,     9,    10,    11,    12,    13,    15,
      17,    19,    20,    24,    27,    28,    34,    36,    15,    19,
      11,    15,     3,     5,    12,    14,    15,    35,    15,    15,
       3,     4,     6,     7,    14,    16,    18,    19,    24,    28,
      29,    30,    31,    32,    33,    34,     9,    17,    24,    17,
       9,     0,    21,    21,    28,    34,     3,    15,    19,    16,
       3,     4,    16,     9,    18,    22,    18,    22,    23,    28,
      34,    18,    22,    23,    12,    28,    29,    32,     9,    28,
      29,    32,    25,    21,    21,    16,    17,    19,    28,     3,
      13,    28,    30,    23,    17,    19,    33,    23,    18,    18,
      18,    25,    18,    18,    18,    23,    23
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    26,    27,    27,    27,    27,    28,    28,    28,    28,
      28,    28,    28,    28,    28,    29,    29,    29,    29,    29,
      29,    30,    30,    30,    30,    31,    31,    31,    31,    31,
      31,    31,    32,    32,    32,    33,    34,    34,    34,    34,
      34,    34,    34,    34,    34,    34,    34,    34,    34,    34,
      34,    34,    34,    34,    34,    34,    34,    34,    35,    35,
      35,    35,    35,    36,    36
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     2,     3,     2,     3,     3,     3,     3,     4,
       4,     4,     4,     4,     4,     1,     3,     3,     3,     3,
       4,     2,     2,     1,     1,     1,     2,     1,     2,     2,
       3,     1,     1,     3,     4,     1,     1,     1,     1,     1,
       1,     2,     2,     1,     1,     2,     3,     2,     2,     2,
       2,     2,     1,     3,     2,     2,     2,     1,     1,     2,
       1,     2,     1,     4,     3
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






/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
/* Lookahead token kind.  */
int yychar;


/* The semantic value of the lookahead symbol.  */
/* Default value used for initialization, for pacifying older GCCs
   or non-GCC compilers.  */
YY_INITIAL_VALUE (static YYSTYPE yyval_default;)
YYSTYPE yylval YY_INITIAL_VALUE (= yyval_default);

    /* Number of syntax errors so far.  */
    int yynerrs = 0;

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


/* User initialization code.  */
#line 76 "backend/gdb/VarTreeParserY.y"
{
	itsGroupDepth   = 0;
	itsGDBErrorFlag = false;
//	yydebug         = 1;

if (yynerrs) ;	// keep C++20 happy
}

#line 1036 "backend/gdb/VarTreeParserY.cpp"

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
      yychar = yylex (&yylval);
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
  case 2: /* top_group: group P_EOF  */
#line 88 "backend/gdb/VarTreeParserY.y"
        {
		itsCurrentNode = (yyval.pNode) = GetLink()->CreateVarNode(nullptr, JString::empty, JString::empty, (yyvsp[-1].pGroup)->GetName());
		for (JIndex i=1; i<=(yyvsp[-1].pGroup)->list->GetItemCount(); i++)
		{
			itsCurrentNode->Append((yyvsp[-1].pGroup)->list->GetItem(i));
		}
		itsIsPointerFlag = false;

		jdelete (yyvsp[-1].pGroup);

		YYACCEPT;
	}
#line 1250 "backend/gdb/VarTreeParserY.cpp"
    break;

  case 3: /* top_group: reference_value group P_EOF  */
#line 102 "backend/gdb/VarTreeParserY.y"
        {
		itsCurrentNode = (yyval.pNode) = GetLink()->CreateVarNode(nullptr, JString::empty, JString::empty, *(yyvsp[-2].pString));
		for (JIndex i=1; i<=(yyvsp[-1].pGroup)->list->GetItemCount(); i++)
		{
			itsCurrentNode->Append((yyvsp[-1].pGroup)->list->GetItem(i));
		}
		itsIsPointerFlag = false;

		jdelete (yyvsp[-2].pString);
		jdelete (yyvsp[-1].pGroup);

		YYACCEPT;
	}
#line 1268 "backend/gdb/VarTreeParserY.cpp"
    break;

  case 4: /* top_group: value P_EOF  */
#line 119 "backend/gdb/VarTreeParserY.y"
        {
		itsCurrentNode = (yyval.pNode) = GetLink()->CreateVarNode(nullptr, JString::empty, JString::empty, *(yyvsp[-1].pString));
		itsCurrentNode->MakePointer(itsIsPointerFlag);
		itsIsPointerFlag = false;

		jdelete (yyvsp[-1].pString);

		YYACCEPT;
	}
#line 1282 "backend/gdb/VarTreeParserY.cpp"
    break;

  case 5: /* top_group: reference_value value P_EOF  */
#line 130 "backend/gdb/VarTreeParserY.y"
        {
		itsCurrentNode = (yyval.pNode) = GetLink()->CreateVarNode(nullptr, JString::empty, JString::empty, *(yyvsp[-2].pString));
		::VarNode* child = GetLink()->CreateVarNode(itsCurrentNode, JString::empty, JString::empty, *(yyvsp[-1].pString));
		child->MakePointer(itsIsPointerFlag);
		itsIsPointerFlag = false;

		jdelete (yyvsp[-2].pString);
		jdelete (yyvsp[-1].pString);

		YYACCEPT;
	}
#line 1298 "backend/gdb/VarTreeParserY.cpp"
    break;

  case 6: /* group: P_GROUP_OPEN node_list P_GROUP_CLOSE  */
#line 146 "backend/gdb/VarTreeParserY.y"
        {
		(yyval.pGroup) = jnew GDBVarGroupInfo(nullptr, (yyvsp[-1].pList));
	}
#line 1306 "backend/gdb/VarTreeParserY.cpp"
    break;

  case 7: /* group: P_GROUP_OPEN value_list P_GROUP_CLOSE  */
#line 151 "backend/gdb/VarTreeParserY.y"
        {
		(yyval.pGroup) = jnew GDBVarGroupInfo(nullptr, (yyvsp[-1].pList));
	}
#line 1314 "backend/gdb/VarTreeParserY.cpp"
    break;

  case 8: /* group: P_GROUP_OPEN group P_GROUP_CLOSE  */
#line 156 "backend/gdb/VarTreeParserY.y"
        {
		auto* list = jnew JPtrArray< ::VarNode>(JPtrArrayT::kForgetAll);
		AppendAsArrayElement(JString::empty, *((yyvsp[-1].pGroup)->list), list);
		(yyval.pGroup) = jnew GDBVarGroupInfo(nullptr, list);

		jdelete (yyvsp[-1].pGroup);
	}
#line 1326 "backend/gdb/VarTreeParserY.cpp"
    break;

  case 9: /* group: P_SUMMARY P_GROUP_OPEN node_list P_GROUP_CLOSE  */
#line 165 "backend/gdb/VarTreeParserY.y"
        {
		(yyval.pGroup) = jnew GDBVarGroupInfo((yyvsp[-3].pString), (yyvsp[-1].pList));
	}
#line 1334 "backend/gdb/VarTreeParserY.cpp"
    break;

  case 10: /* group: P_SUMMARY P_GROUP_OPEN value_list P_GROUP_CLOSE  */
#line 170 "backend/gdb/VarTreeParserY.y"
        {
		(yyval.pGroup) = jnew GDBVarGroupInfo((yyvsp[-3].pString), (yyvsp[-1].pList));
	}
#line 1342 "backend/gdb/VarTreeParserY.cpp"
    break;

  case 11: /* group: P_SUMMARY P_GROUP_OPEN group P_GROUP_CLOSE  */
#line 175 "backend/gdb/VarTreeParserY.y"
        {
		auto* list = jnew JPtrArray< ::VarNode>(JPtrArrayT::kForgetAll);
		AppendAsArrayElement((yyvsp[-1].pGroup)->GetName(), *((yyvsp[-1].pGroup)->list), list);
		(yyval.pGroup) = jnew GDBVarGroupInfo((yyvsp[-3].pString), list);

		jdelete (yyvsp[-1].pGroup);
	}
#line 1354 "backend/gdb/VarTreeParserY.cpp"
    break;

  case 12: /* group: P_PAREN_EXPR P_GROUP_OPEN node_list P_GROUP_CLOSE  */
#line 184 "backend/gdb/VarTreeParserY.y"
        {
		(yyval.pGroup) = jnew GDBVarGroupInfo((yyvsp[-3].pString), (yyvsp[-1].pList));
	}
#line 1362 "backend/gdb/VarTreeParserY.cpp"
    break;

  case 13: /* group: P_PAREN_EXPR P_GROUP_OPEN value_list P_GROUP_CLOSE  */
#line 189 "backend/gdb/VarTreeParserY.y"
        {
		(yyval.pGroup) = jnew GDBVarGroupInfo((yyvsp[-3].pString), (yyvsp[-1].pList));
	}
#line 1370 "backend/gdb/VarTreeParserY.cpp"
    break;

  case 14: /* group: P_PAREN_EXPR P_GROUP_OPEN group P_GROUP_CLOSE  */
#line 194 "backend/gdb/VarTreeParserY.y"
        {
		auto* list = jnew JPtrArray< ::VarNode>(JPtrArrayT::kForgetAll);
		AppendAsArrayElement((yyvsp[-1].pGroup)->GetName(), *((yyvsp[-1].pGroup)->list), list);
		(yyval.pGroup) = jnew GDBVarGroupInfo((yyvsp[-3].pString), list);

		jdelete (yyvsp[-1].pGroup);
	}
#line 1382 "backend/gdb/VarTreeParserY.cpp"
    break;

  case 15: /* node_list: node  */
#line 206 "backend/gdb/VarTreeParserY.y"
        {
		auto* list = (yyval.pList) = jnew JPtrArray< ::VarNode>(JPtrArrayT::kForgetAll);
		list->Append((yyvsp[0].pNode));
	}
#line 1391 "backend/gdb/VarTreeParserY.cpp"
    break;

  case 16: /* node_list: node_list ',' node  */
#line 212 "backend/gdb/VarTreeParserY.y"
        {
		(yyval.pList) = (yyvsp[-2].pList);
		(yyval.pList)->Append((yyvsp[0].pNode));
	}
#line 1400 "backend/gdb/VarTreeParserY.cpp"
    break;

  case 17: /* node_list: node_list ',' P_EMPTY_SUMMARY  */
#line 218 "backend/gdb/VarTreeParserY.y"
        {
		(yyval.pList) = (yyvsp[-2].pList);

		::VarNode* node = GetLink()->CreateVarNode(nullptr, JString::empty, JString::empty, *(yyvsp[0].pString));
		if ((yyval.pList)->GetFirstItem()->GetName().StartsWith("["))
		{
			AppendAsArrayElement(node, (yyval.pList));
		}
		else
		{
			(yyval.pList)->Append(node);
		}

		jdelete (yyvsp[0].pString);
	}
#line 1420 "backend/gdb/VarTreeParserY.cpp"
    break;

  case 18: /* node_list: group ',' group  */
#line 235 "backend/gdb/VarTreeParserY.y"
        {
		auto* list = (yyval.pList) = jnew JPtrArray< ::VarNode>(JPtrArrayT::kForgetAll);
		AppendAsArrayElement((yyvsp[-2].pGroup)->GetName(), *(yyvsp[-2].pGroup)->list, list);
		AppendAsArrayElement((yyvsp[0].pGroup)->GetName(), *(yyvsp[0].pGroup)->list, list);

		jdelete (yyvsp[-2].pGroup);
		jdelete (yyvsp[0].pGroup);
	}
#line 1433 "backend/gdb/VarTreeParserY.cpp"
    break;

  case 19: /* node_list: node_list ',' group  */
#line 245 "backend/gdb/VarTreeParserY.y"
        {
		if ((yyvsp[-2].pList)->GetFirstItem()->GetName().StartsWith("["))
		{
			(yyval.pList) = (yyvsp[-2].pList);
		}
		else
		{
			auto* list = (yyval.pList) = jnew JPtrArray< ::VarNode>(JPtrArrayT::kForgetAll);
			AppendAsArrayElement(JString::empty, *(yyvsp[-2].pList), list);

			jdelete (yyvsp[-2].pList);
		}

		AppendAsArrayElement((yyvsp[0].pGroup)->GetName(), *((yyvsp[0].pGroup)->list), (yyval.pList));

		jdelete (yyvsp[0].pGroup);
	}
#line 1455 "backend/gdb/VarTreeParserY.cpp"
    break;

  case 20: /* node_list: node_list '.' '.' '.'  */
#line 264 "backend/gdb/VarTreeParserY.y"
        {
		(yyval.pList) = (yyvsp[-3].pList);
		::VarNode* child = GetLink()->CreateVarNode(nullptr, "...", JString::empty, JString::empty);
		(yyval.pList)->Append(child);
	}
#line 1465 "backend/gdb/VarTreeParserY.cpp"
    break;

  case 21: /* node: name_eq value  */
#line 274 "backend/gdb/VarTreeParserY.y"
        {
		if ((yyvsp[-1].pString)->EndsWith("="))
		{
			JStringIterator iter((yyvsp[-1].pString), JStringIterator::kStartAtEnd);
			iter.RemovePrev();
			(yyvsp[-1].pString)->TrimWhitespace();
		}
		itsCurrentNode = (yyval.pNode) = GetLink()->CreateVarNode(nullptr, *(yyvsp[-1].pString), JString::empty, *(yyvsp[0].pString));
		if (!(yyvsp[-1].pString)->StartsWith("_vptr.") && !(yyvsp[-1].pString)->StartsWith("_vb."))
		{
			itsCurrentNode->MakePointer(itsIsPointerFlag);
		}
		itsIsPointerFlag = false;

		jdelete (yyvsp[-1].pString);
		jdelete (yyvsp[0].pString);
	}
#line 1487 "backend/gdb/VarTreeParserY.cpp"
    break;

  case 22: /* node: name_eq group  */
#line 293 "backend/gdb/VarTreeParserY.y"
        {
		if ((yyvsp[-1].pString)->EndsWith("="))
		{
			JStringIterator iter((yyvsp[-1].pString), JStringIterator::kStartAtEnd);
			iter.RemovePrev();
			(yyvsp[-1].pString)->TrimWhitespace();
		}
		itsCurrentNode = (yyval.pNode) = GetLink()->CreateVarNode(nullptr, *(yyvsp[-1].pString), JString::empty, (yyvsp[0].pGroup)->GetName());
		itsIsPointerFlag = false;
		for (JIndex i=1; i<=(yyvsp[0].pGroup)->list->GetItemCount(); i++)
		{
			itsCurrentNode->Append((yyvsp[0].pGroup)->list->GetItem(i));
		}

		jdelete (yyvsp[-1].pString);
		jdelete (yyvsp[0].pGroup);
	}
#line 1509 "backend/gdb/VarTreeParserY.cpp"
    break;

  case 23: /* node: P_NO_DATA_FIELDS  */
#line 312 "backend/gdb/VarTreeParserY.y"
        {
		itsCurrentNode = (yyval.pNode) = GetLink()->CreateVarNode(nullptr, *(yyvsp[0].pString), JString::empty, JString::empty);
		itsIsPointerFlag = false;

		jdelete (yyvsp[0].pString);
	}
#line 1520 "backend/gdb/VarTreeParserY.cpp"
    break;

  case 24: /* node: name_eq  */
#line 320 "backend/gdb/VarTreeParserY.y"
        {
		if ((yyvsp[0].pString)->EndsWith("="))
		{
			JStringIterator iter((yyvsp[0].pString), JStringIterator::kStartAtEnd);
			iter.RemovePrev();
			(yyvsp[0].pString)->TrimWhitespace();
		}
		itsCurrentNode = (yyval.pNode) = GetLink()->CreateVarNode(nullptr, *(yyvsp[0].pString), JString::empty, "<nothing>");
		itsIsPointerFlag = false;

		jdelete (yyvsp[0].pString);
	}
#line 1537 "backend/gdb/VarTreeParserY.cpp"
    break;

  case 26: /* name_eq: P_STATIC P_NAME_EQ  */
#line 339 "backend/gdb/VarTreeParserY.y"
        {
		(yyval.pString) = (yyvsp[-1].pString);
		(yyval.pString)->Append(" ");
		(yyval.pString)->Append(*(yyvsp[0].pString));

		jdelete (yyvsp[0].pString);
	}
#line 1549 "backend/gdb/VarTreeParserY.cpp"
    break;

  case 28: /* name_eq: P_STATIC P_BRACKET_EQ  */
#line 350 "backend/gdb/VarTreeParserY.y"
        {
		(yyval.pString) = (yyvsp[-1].pString);
		(yyval.pString)->Append(" ");
		(yyval.pString)->Append(*(yyvsp[0].pString));

		jdelete (yyvsp[0].pString);
	}
#line 1561 "backend/gdb/VarTreeParserY.cpp"
    break;

  case 29: /* name_eq: P_NAME P_BRACKET_EQ  */
#line 359 "backend/gdb/VarTreeParserY.y"
        {
		(yyval.pString) = (yyvsp[-1].pString);
		(yyval.pString)->Append(*(yyvsp[0].pString));

		jdelete (yyvsp[0].pString);
	}
#line 1572 "backend/gdb/VarTreeParserY.cpp"
    break;

  case 30: /* name_eq: P_STATIC P_NAME P_BRACKET_EQ  */
#line 367 "backend/gdb/VarTreeParserY.y"
        {
		(yyval.pString) = (yyvsp[-2].pString);
		(yyval.pString)->Append(" ");
		(yyval.pString)->Append(*(yyvsp[-1].pString));
		(yyval.pString)->Append(*(yyvsp[0].pString));

		jdelete (yyvsp[-1].pString);
		jdelete (yyvsp[0].pString);
	}
#line 1586 "backend/gdb/VarTreeParserY.cpp"
    break;

  case 32: /* value_list: value_node  */
#line 383 "backend/gdb/VarTreeParserY.y"
        {
		auto* list = (yyval.pList) = jnew JPtrArray< ::VarNode>(JPtrArrayT::kForgetAll);
		AppendAsArrayElement((yyvsp[0].pNode), (yyval.pList));
	}
#line 1595 "backend/gdb/VarTreeParserY.cpp"
    break;

  case 33: /* value_list: value_list ',' value_node  */
#line 389 "backend/gdb/VarTreeParserY.y"
        {
		(yyval.pList) = (yyvsp[-2].pList);
		AppendAsArrayElement((yyvsp[0].pNode), (yyval.pList));
	}
#line 1604 "backend/gdb/VarTreeParserY.cpp"
    break;

  case 34: /* value_list: value_list '.' '.' '.'  */
#line 395 "backend/gdb/VarTreeParserY.y"
        {
		(yyval.pList) = (yyvsp[-3].pList);
		::VarNode* child = GetLink()->CreateVarNode(nullptr, "...", JString::empty, JString::empty);
		(yyval.pList)->Append(child);
	}
#line 1614 "backend/gdb/VarTreeParserY.cpp"
    break;

  case 35: /* value_node: value  */
#line 407 "backend/gdb/VarTreeParserY.y"
        {
		itsCurrentNode = (yyval.pNode) = GetLink()->CreateVarNode(nullptr, JString::empty, JString::empty, *(yyvsp[0].pString));
		itsCurrentNode->MakePointer(itsIsPointerFlag);
		itsIsPointerFlag = false;

		jdelete (yyvsp[0].pString);
	}
#line 1626 "backend/gdb/VarTreeParserY.cpp"
    break;

  case 40: /* value: P_HEX  */
#line 427 "backend/gdb/VarTreeParserY.y"
        {
		itsIsPointerFlag = isOpenablePointer(*(yyvsp[0].pString));
		(yyval.pString) = (yyvsp[0].pString);
	}
#line 1635 "backend/gdb/VarTreeParserY.cpp"
    break;

  case 41: /* value: '@' P_HEX  */
#line 433 "backend/gdb/VarTreeParserY.y"
        {
		itsIsPointerFlag = isOpenablePointer(*(yyvsp[0].pString));
		(yyval.pString) = (yyvsp[0].pString);
		(yyval.pString)->Prepend("@");
	}
#line 1645 "backend/gdb/VarTreeParserY.cpp"
    break;

  case 42: /* value: P_PAREN_EXPR P_HEX  */
#line 440 "backend/gdb/VarTreeParserY.y"
        {
		itsIsPointerFlag = isOpenablePointer(*(yyvsp[0].pString));
		(yyval.pString) = (yyvsp[-1].pString);
		(yyval.pString)->Append(" ");
		(yyval.pString)->Append(*(yyvsp[0].pString));

		jdelete (yyvsp[0].pString);
	}
#line 1658 "backend/gdb/VarTreeParserY.cpp"
    break;

  case 45: /* value: P_HEX P_STRING  */
#line 454 "backend/gdb/VarTreeParserY.y"
        {
		(yyval.pString) = (yyvsp[-1].pString);
		(yyval.pString)->Append(" ");
		(yyval.pString)->Append(*(yyvsp[0].pString));

		jdelete (yyvsp[0].pString);
	}
#line 1670 "backend/gdb/VarTreeParserY.cpp"
    break;

  case 46: /* value: P_PAREN_EXPR P_HEX P_STRING  */
#line 463 "backend/gdb/VarTreeParserY.y"
        {
		(yyval.pString) = (yyvsp[-2].pString);
		(yyval.pString)->Append(" ");
		(yyval.pString)->Append(*(yyvsp[-1].pString));
		(yyval.pString)->Append(" ");
		(yyval.pString)->Append(*(yyvsp[0].pString));

		jdelete (yyvsp[-1].pString);
		jdelete (yyvsp[0].pString);
	}
#line 1685 "backend/gdb/VarTreeParserY.cpp"
    break;

  case 47: /* value: P_HEX P_BRACKET  */
#line 475 "backend/gdb/VarTreeParserY.y"
        {
		itsIsPointerFlag = isOpenablePointer(*(yyvsp[-1].pString));
		(yyval.pString) = (yyvsp[-1].pString);
		(yyval.pString)->Append(" ");
		(yyval.pString)->Append(*(yyvsp[0].pString));

		jdelete (yyvsp[0].pString);
	}
#line 1698 "backend/gdb/VarTreeParserY.cpp"
    break;

  case 48: /* value: P_NAME P_BRACKET  */
#line 485 "backend/gdb/VarTreeParserY.y"
        {
		(yyval.pString) = (yyvsp[-1].pString);
		(yyval.pString)->Append(" ");
		(yyval.pString)->Append(*(yyvsp[0].pString));

		jdelete (yyvsp[0].pString);
	}
#line 1710 "backend/gdb/VarTreeParserY.cpp"
    break;

  case 49: /* value: P_INTEGER P_BRACKET  */
#line 494 "backend/gdb/VarTreeParserY.y"
        {
		(yyval.pString) = (yyvsp[-1].pString);
		(yyval.pString)->Append(" ");
		(yyval.pString)->Append(*(yyvsp[0].pString));

		jdelete (yyvsp[0].pString);
	}
#line 1722 "backend/gdb/VarTreeParserY.cpp"
    break;

  case 50: /* value: P_FLOAT P_BRACKET  */
#line 503 "backend/gdb/VarTreeParserY.y"
        {
		(yyval.pString) = (yyvsp[-1].pString);
		(yyval.pString)->Append(" ");
		(yyval.pString)->Append(*(yyvsp[0].pString));

		jdelete (yyvsp[0].pString);
	}
#line 1734 "backend/gdb/VarTreeParserY.cpp"
    break;

  case 51: /* value: P_CHAR P_BRACKET  */
#line 512 "backend/gdb/VarTreeParserY.y"
        {
		(yyval.pString) = (yyvsp[-1].pString);
		(yyval.pString)->Append(" ");
		(yyval.pString)->Append(*(yyvsp[0].pString));

		jdelete (yyvsp[0].pString);
	}
#line 1746 "backend/gdb/VarTreeParserY.cpp"
    break;

  case 53: /* value: P_HEX name P_PAREN_EXPR  */
#line 523 "backend/gdb/VarTreeParserY.y"
        {
		(yyval.pString) = (yyvsp[-2].pString);
		(yyval.pString)->Append(" ");
		(yyval.pString)->Append(*(yyvsp[-1].pString));
		(yyval.pString)->Append(*(yyvsp[0].pString));

		jdelete (yyvsp[-1].pString);
		jdelete (yyvsp[0].pString);
	}
#line 1760 "backend/gdb/VarTreeParserY.cpp"
    break;

  case 54: /* value: P_NAME P_PAREN_EXPR  */
#line 534 "backend/gdb/VarTreeParserY.y"
        {
		(yyval.pString) = (yyvsp[-1].pString);
		(yyval.pString)->Append(" ");
		(yyval.pString)->Append(*(yyvsp[0].pString));

		jdelete (yyvsp[0].pString);
	}
#line 1772 "backend/gdb/VarTreeParserY.cpp"
    break;

  case 55: /* value: P_INTEGER P_CHAR  */
#line 543 "backend/gdb/VarTreeParserY.y"
        {
		(yyval.pString) = (yyvsp[-1].pString);
		(yyval.pString)->Append(" ");
		(yyval.pString)->Append(*(yyvsp[0].pString));

		jdelete (yyvsp[0].pString);
	}
#line 1784 "backend/gdb/VarTreeParserY.cpp"
    break;

  case 56: /* value: P_GROUP_OPEN P_GROUP_CLOSE  */
#line 552 "backend/gdb/VarTreeParserY.y"
        {
		(yyval.pString) = jnew JString("{}");
	}
#line 1792 "backend/gdb/VarTreeParserY.cpp"
    break;

  case 59: /* name: P_STATIC P_NAME  */
#line 564 "backend/gdb/VarTreeParserY.y"
        {
		(yyval.pString) = (yyvsp[-1].pString);
		(yyval.pString)->Append(" ");
		(yyval.pString)->Append(*(yyvsp[0].pString));

		jdelete (yyvsp[0].pString);
	}
#line 1804 "backend/gdb/VarTreeParserY.cpp"
    break;

  case 61: /* name: P_STATIC P_BRACKET  */
#line 575 "backend/gdb/VarTreeParserY.y"
        {
		(yyval.pString) = (yyvsp[-1].pString);
		(yyval.pString)->Append(" ");
		(yyval.pString)->Append(*(yyvsp[0].pString));

		jdelete (yyvsp[0].pString);
	}
#line 1816 "backend/gdb/VarTreeParserY.cpp"
    break;

  case 63: /* reference_value: P_PAREN_EXPR '@' P_HEX ':'  */
#line 589 "backend/gdb/VarTreeParserY.y"
        {
		(yyval.pString) = (yyvsp[-3].pString);
		(yyval.pString)->Append(" ");
		(yyval.pString)->Append("@");
		(yyval.pString)->Append(*(yyvsp[-1].pString));

		jdelete (yyvsp[-1].pString);
	}
#line 1829 "backend/gdb/VarTreeParserY.cpp"
    break;

  case 64: /* reference_value: '@' P_HEX ':'  */
#line 599 "backend/gdb/VarTreeParserY.y"
        {
		(yyval.pString) = (yyvsp[-1].pString);
		(yyval.pString)->Prepend("@");
	}
#line 1838 "backend/gdb/VarTreeParserY.cpp"
    break;


#line 1842 "backend/gdb/VarTreeParserY.cpp"

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

#line 605 "backend/gdb/VarTreeParserY.y"

