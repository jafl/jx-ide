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

#ifndef YY_YY_BACKEND_GDB_GDBVARTREEPARSERY_HPP_INCLUDED
# define YY_YY_BACKEND_GDB_GDBVARTREEPARSERY_HPP_INCLUDED
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
    P_NAME = 258,                  /* P_NAME  */
    P_NAME_EQ = 259,               /* P_NAME_EQ  */
    P_EMPTY_BRACKET = 260,         /* P_EMPTY_BRACKET  */
    P_EMPTY_BRACKET_EQ = 261,      /* P_EMPTY_BRACKET_EQ  */
    P_NO_DATA_FIELDS = 262,        /* P_NO_DATA_FIELDS  */
    P_INTEGER = 263,               /* P_INTEGER  */
    P_HEX = 264,                   /* P_HEX  */
    P_FLOAT = 265,                 /* P_FLOAT  */
    P_CHAR = 266,                  /* P_CHAR  */
    P_STRING = 267,                /* P_STRING  */
    P_EMPTY_SUMMARY = 268,         /* P_EMPTY_SUMMARY  */
    P_STATIC = 269,                /* P_STATIC  */
    P_BRACKET = 270,               /* P_BRACKET  */
    P_BRACKET_EQ = 271,            /* P_BRACKET_EQ  */
    P_GROUP_OPEN = 272,            /* P_GROUP_OPEN  */
    P_GROUP_CLOSE = 273,           /* P_GROUP_CLOSE  */
    P_PAREN_EXPR = 274,            /* P_PAREN_EXPR  */
    P_SUMMARY = 275,               /* P_SUMMARY  */
    P_EOF = 276                    /* P_EOF  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 25 "backend/gdb/GDBVarTreeParserY.y"

	JString*				pString;
	::VarNode*				pNode;
	JPtrArray<::VarNode>*	pList;
	GDBVarGroupInfo*		pGroup;

#line 92 "backend/gdb/GDBVarTreeParserY.hpp"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif




int yyparse (void);


#endif /* !YY_YY_BACKEND_GDB_GDBVARTREEPARSERY_HPP_INCLUDED  */
