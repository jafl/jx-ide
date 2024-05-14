/******************************************************************************
 GoCompleter.cpp

	BASE CLASS = StringCompleter

	Copyright © 2021 by John Lindal.

 ******************************************************************************/

#include "GoCompleter.h"
#include <jx-af/jcore/jAssert.h>

GoCompleter* GoCompleter::itsSelf = nullptr;

static const JUtf8Byte* kKeywordList[] =
{
	// remember to update JSPCompleter

	// keywords

	"break", "case", "chan", "const", "continue", "default", "defer",
	"else", "fallthrough", "for", "func", "go", "goto", "if", "import",
	"interface", "map", "package", "range", "return", "select", "struct",
	"switch", "type", "var",

	// types

	"bool", "byte", "complex64", "complex128", "error", "float32", "float64",
	"int", "int8", "int16", "int32", "int64", "rune", "string",
	"uint", "uint8", "uint16", "uint32", "uint64", "uintptr",

	// values

	"true", "false", "iota", "nil",

	// functions

	"append", "cap", "close", "complex", "copy", "delete", "imag", "len",
	"make", "new", "panic", "print", "println", "real", "recover"
};

const JSize kKeywordCount = sizeof(kKeywordList)/sizeof(JUtf8Byte*);

/******************************************************************************
 Instance (static)

 ******************************************************************************/

static bool recursiveInstance = false;

StringCompleter*
GoCompleter::Instance()
{
	if (itsSelf == nullptr && !recursiveInstance)
	{
		recursiveInstance = true;

		itsSelf = jnew GoCompleter;

		recursiveInstance = false;
	}

	return itsSelf;
}

/******************************************************************************
 Shutdown (static)

 ******************************************************************************/

void
GoCompleter::Shutdown()
{
	jdelete itsSelf;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

GoCompleter::GoCompleter()
	:
	StringCompleter(kGoLang, kKeywordCount, kKeywordList, JString::kCompareCase)
{
	UpdateWordList();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

GoCompleter::~GoCompleter()
{
	itsSelf = nullptr;
}

/******************************************************************************
 IsWordCharacter (virtual protected)

	We include the namespace operator (.) to allow completion of fully
	qualified names.

 ******************************************************************************/

bool
GoCompleter::IsWordCharacter
	(
	const JUtf8Character&	c,
	const bool				includeNS
	)
	const
{
	return c.IsAlnum() || c == '_' || (includeNS && c == '.');
}
