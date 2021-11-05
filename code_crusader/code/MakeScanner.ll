%top {
/*
Copyright © 2001 by John Lindal.

This scanner reads a Makefile and returns TextScanner::Make::Scanner::Tokens.
*/

#include "StylingScannerBase.h"
#include <jx-af/jcore/jAssert.h>
}

%option namespace="TextScanner::Make" lexer="Scanner"
%option lex="NextToken" token-type="StylingScannerBase::Token"
%option unicode nodefault full freespace

%class {

public:

	// these types are ordered to correspond to the type table in MakeStyler

	enum TokenType
	{
		kEmptyVariable = kEOF+1,
		kUnterminatedString,
		kIllegalChar,
		kNonPrintChar,

		kWhitespace,	// must be the one before the first item in type table

		kID,
		kMakeTarget,
		kMakeSpecialTarget,
		kMakeVariable,
		kMakeOperator,

		kShellVariable,
		kShellReservedWord,
		kShellBuiltInCommand,

		kShellControlOperator,
		kShellRedirectionOperator,
		kShellHistoryOperator,

		kSingleQuoteString,
		kDoubleQuoteString,
		kExecString,

		kComment,
		kError,			// place holder for MakeStyler

		kOtherOperator
	};
}

%x ID_STATE SHELL_VAR_STATE
%x DOUBLE_STRING_STATE EXEC_STRING_STATE

WSCHAR               ([[:space:]]|\\\n)
WS                   ({WSCHAR}+)

IDCAR                (_|\p{L})
IDCDR                (_|\p{L}|\d)

MAKE_SPECIAL_VARNAME ([@%<?^+|*])
MAKE_VARNAME         ({IDCAR}{IDCDR}*|[1-9][0-9]*|[@%<?^+*][DF]|{MAKE_SPECIAL_VARNAME}|MAKEFILE_LIST|\.DEFAULT_GOAL|MAKE_RESTARTS|MAKE_TERMOUT|MAKE_TERMERR|\.RECIPEPREFIX|\.VARIABLES|\.FEATURES|\.INCLUDE_DIRS|\.EXTRA_PREREQS)
SHELL_VARNAME        ([[:alpha:]_][[:alnum:]_]*|[0-9]+)

MAKE_TARGET          ({IDCAR}{IDCDR}*)
MAKE_SPECIAL_TARGET  (\.(PHONY|SUFFIXES|DEFAULT|PRECIOUS|INTERMEDIATE|SECONDARY|DELETE_ON_ERROR|IGNORE|SILENT|EXPORT_ALL_VARIABLES|NOTPARALLEL))

SHELL_RESERVEDWORD   (case|do|done|elif|else|esac|fi|for|function|if|in|select|then|until|while)
SHELL_BUILTINCMD     (true|false|source|alias|bg|bind|break|builtin|cd|command|continue|declare|typeset|dirs|echo|enable|eval|exec|exit|export|fc|fg|getopts|hash|help|history|jobs|kill|let|local|logout|popd|pushd|pwd|read|readonly|return|set|shift|suspend|test|times|trap|type|ulimit|umask|unalias|unset|wait)

%%

"#".*\n? {
	StartToken();
	return ThisToken(kComment);
	}

\'[^']*\'? {
	StartToken();
	return ThisToken(size() > 1 && text()[size()-1] == '\'' ?
					 kSingleQuoteString : kUnterminatedString);
	}

\" {
	StartToken();
	start(DOUBLE_STRING_STATE);
	}

\` {
	StartToken();
	start(EXEC_STRING_STATE);
	}

^\s*{MAKE_TARGET}\s*::?[^=] {
	matcher().less(size()-1);
	StartToken();
	return ThisToken(kMakeTarget);
	}

^\s*{MAKE_SPECIAL_TARGET}\s*: {
	StartToken();
	return ThisToken(kMakeSpecialTarget);
	}

"$("{MAKE_VARNAME}")" |
"${"{MAKE_VARNAME}"}" |
"$"{MAKE_SPECIAL_VARNAME} {
	StartToken();
	return ThisToken(kMakeVariable);
	}

"$()" |
"${}" {
	StartToken();
	return ThisToken(kEmptyVariable);
	}

"${" |
"$(" |
":=" {
	StartToken();
	return ThisToken(kMakeOperator);
	}

"$$" {
	StartToken();
	start(SHELL_VAR_STATE);
	}

{SHELL_RESERVEDWORD} {
	StartToken();
	return ThisToken(kShellReservedWord);
	}

{SHELL_BUILTINCMD} {
	StartToken();
	return ThisToken(kShellBuiltInCommand);
	}

[^|&;()<>!#'"`=$\\ \v\t\f\r\n]+ |
\\.?                            {
	StartToken();
	start(ID_STATE);
	}

"||" |
"&"  |
"&&" |
";"  |
";;" |
"("  |
")"  |
"|"  {
	StartToken();
	return ThisToken(kShellControlOperator);
	}

"<"   |
"<<"  |
"<<-" |
"<&"  |
">"   |
">|"  |
">>"  |
"&>"  |
">&"  |
"<>"  {
	StartToken();
	return ThisToken(kShellRedirectionOperator);
	}

!    |
!!   |
!"?" |
!#   {
	StartToken();
	return ThisToken(kShellHistoryOperator);
	}

!(?={WSCHAR}|[=(]) {
	StartToken();
	return ThisToken(kShellControlOperator);
	}

"=" {
	StartToken();
	return ThisToken(kOtherOperator);
	}

{WS} {
	StartToken();
	return ThisToken(kWhitespace);
	}



	/* Match word */



<ID_STATE>{

[^|&;()<>!#'"`=$\\ \v\t\f\r\n]+ |
\\.?                            {
	ContinueToken();
	}

.|{WS} {
	matcher().less(0);
	start(INITIAL);
	return ThisToken(kID);
	}

<<EOF>> {
	start(INITIAL);
	return ThisToken(kID);
	}

}



	/* Match shell variable */



<SHELL_VAR_STATE>{

{SHELL_VARNAME}                                                                  |
"{"[#!]?{SHELL_VARNAME}("["[^]\n]+"]")?("}"|:[-=?+]?|"#"{1,2}|"%"{1,2}|"/"{1,2}) |
[-0-9*@#?$!_]                                                                    {
	ContinueToken();
	start(INITIAL);
	return ThisToken(kShellVariable);
	}

"{"  |
"{}" {
	ContinueToken();
	start(INITIAL);
	return ThisToken(kEmptyVariable);
	}

.|\n {
	matcher().less(0);
	start(INITIAL);
	return ThisToken(kID);
	}

<<EOF>> {
	start(INITIAL);
	return ThisToken(kID);
	}

}



	/* Match strings */



<DOUBLE_STRING_STATE>{

\" {
	ContinueToken();
	start(INITIAL);
	return ThisToken(kDoubleQuoteString);
	}

\\(.|\n)? |
[^\\\"]+  {
	ContinueToken();
	}

<<EOF>> {
	start(INITIAL);
	return ThisToken(kUnterminatedString);
	}

}

<EXEC_STRING_STATE>{

\` {
	ContinueToken();
	start(INITIAL);
	return ThisToken(kExecString);
	}

\\(.|\n)? |
[^\\\`]+   {
	ContinueToken();
	}

<<EOF>> {
	start(INITIAL);
	return ThisToken(kUnterminatedString);
	}

}



	/* Clean up */



. {
	StartToken();
	JUtf8Character c(text());
	if (c.IsPrint())
		{
		return ThisToken(kIllegalChar);
		}
	else
		{
		return ThisToken(kNonPrintChar);
		}
	}



	/* Must be last, or it applies to following states as well! */



<<EOF>> {
	return ThisToken(kEOF);
	}

%%
