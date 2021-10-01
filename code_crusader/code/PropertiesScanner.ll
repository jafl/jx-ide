%top {
/*
Copyright © 2015 by John Lindal.

This scanner reads a .properties file and returns TextScanner::Properties::Scanner::Tokens.
*/

#include "StylingScannerBase.h"
#include <jx-af/jcore/jAssert.h>
%}

%option namespace="TextScanner::Properties" lexer="Scanner"
%option lex="NextToken" token-type="StylingScannerBase::Token"
%option unicode nodefault full freespace

%class {

public:

	// these types are ordered to correspond to the type table in PropertiesStyler

	enum TokenType
	{
		kNonPrintChar = kEOF+1,

		kWhitespace,	// must be the one before the first item in type table

		kKeyOrValue,
		kAssignment,

		kComment,

		kError			// place holder for PropertiesStyler
	};
}

WSCHAR  ([ \v\t\f\r])
WS      ([ \v\t\f\r]*)

%%



	/* Match normal tokens */



[^#= \v\t\f\r]+ {
	StartToken();
	return ThisToken(kKeyOrValue);
	}

= {
	StartToken();
	return ThisToken(kAssignment);
	}



	/* Match comments */



^{WS}"#".*\n? {
	StartToken();
	return ThisToken(kComment);
	}
	/* Miscellaneous */



{WSCHAR}+ |
{WS}\n    { /* Don't eat past a line end */
	StartToken();
	return ThisToken(kWhitespace);
	}

. {
	StartToken();
	return ThisToken(kNonPrintChar);
	}



	/* Must be last, or it applies to following states as well! */



<<EOF>> {
	return ThisToken(kEOF);
	}

%%
