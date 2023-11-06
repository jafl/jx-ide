/******************************************************************************
 PascalCompleter.cpp

	BASE CLASS = StringCompleter

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#include "PascalCompleter.h"
#include <jx-af/jcore/jAssert.h>

PascalCompleter* PascalCompleter::itsSelf = nullptr;

static const JUtf8Byte* kKeywordList[] =
{
	// keywords

	"and", "array", "begin", "case", "const", "div", "do", "downto",
	"else", "end", "file", "for", "forward", "function", "goto", "if",
	"in", "label", "mod", "nil", "not", "of", "or", "packed",
	"procedure", "program", "record", "repeat", "set", "then", "to",
	"type", "until", "var", "while", "with",

	// types

	"real", "integer", "char", "boolean", "text",

	// functions

	"abs", "sqr", "sqrt", "sin", "cos", "arctan", "ln", "exp",
	"trunc", "round",
	"ord", "chr", "succ", "pred",
	"odd", "eoln", "eof",

	// procedures

	"read", "readln", "write", "writeln",
	"rewrite", "reset", "put", "get", "page",
	"new", "dispose",
	"pack", "unpack"
};

const JSize kKeywordCount = sizeof(kKeywordList)/sizeof(JUtf8Byte*);

/******************************************************************************
 Instance (static)

 ******************************************************************************/

static bool recursiveInstance = false;

StringCompleter*
PascalCompleter::Instance()
{
	if (itsSelf == nullptr && !recursiveInstance)
	{
		recursiveInstance = true;

		itsSelf = jnew PascalCompleter;

		recursiveInstance = false;
	}

	return itsSelf;
}

/******************************************************************************
 Shutdown (static)

 ******************************************************************************/

void
PascalCompleter::Shutdown()
{
	jdelete itsSelf;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

PascalCompleter::PascalCompleter()
	:
	StringCompleter(kPascalLang, kKeywordCount, kKeywordList, JString::kCompareCase)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

PascalCompleter::~PascalCompleter()
{
	itsSelf = nullptr;
}

/******************************************************************************
 IsWordCharacter (virtual protected)

 ******************************************************************************/

bool
PascalCompleter::IsWordCharacter
	(
	const JUtf8Character&	c,
	const bool			includeNS
	)
	const
{
	return c.IsAlnum() || c == '_';
}
