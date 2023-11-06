/******************************************************************************
 CSharpCompleter.cpp

	BASE CLASS = StringCompleter

	Copyright © 2004 by John Lindal.

 ******************************************************************************/

#include "CSharpCompleter.h"
#include <jx-af/jcore/jAssert.h>

CSharpCompleter* CSharpCompleter::itsSelf = nullptr;

static const JUtf8Byte* kKeywordList[] =
{
	"abstract", "as", "base", "bool", "break", "byte",
	"case", "catch", "char", "checked", "class", "const", "continue",
	"decimal", "default", "delegate", "do", "double",
	"else", "enum", "event", "explicit", "extern",
	"false", "finally", "fixed", "float", "for", "foreach", "goto",
	"if", "implicit", "in", "int", "interface", "internal", "is",
	"lock", "long", "namespace", "new", "null",
	"object", "operator", "out", "override",
	"params", "private", "protected", "public",
	"readonly", "ref", "return", "sbyte", "sealed", "short",
	"sizeof", "stackalloc", "static", "string", "struct", "switch",
	"this", "throw", "true", "try", "typeof",
	"uint", "ulong", "unchecked", "unsafe", "ushort", "using",
	"virtual", "volatile", "void", "while",

	// preprocessor

	"if", "else", "elif", "endif", "define", "undef",
	"warning", "error", "line", "region", "endregion",
};

const JSize kKeywordCount = sizeof(kKeywordList)/sizeof(JUtf8Byte*);

/******************************************************************************
 Instance (static)

 ******************************************************************************/

static bool recursiveInstance = false;

StringCompleter*
CSharpCompleter::Instance()
{
	if (itsSelf == nullptr && !recursiveInstance)
	{
		recursiveInstance = true;

		itsSelf = jnew CSharpCompleter;

		recursiveInstance = false;
	}

	return itsSelf;
}

/******************************************************************************
 Shutdown (static)

 ******************************************************************************/

void
CSharpCompleter::Shutdown()
{
	jdelete itsSelf;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

CSharpCompleter::CSharpCompleter()
	:
	StringCompleter(kCSharpLang, kKeywordCount, kKeywordList, JString::kCompareCase)
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CSharpCompleter::~CSharpCompleter()
{
	itsSelf = nullptr;
}

/******************************************************************************
 IsWordCharacter (virtual protected)

	We include the namespace operator (.) to allow completion of fully
	qualified names.

 ******************************************************************************/

bool
CSharpCompleter::IsWordCharacter
	(
	const JUtf8Character&	c,
	const bool			includeNS
	)
	const
{
	return c.IsAlnum() || c == '_' || (includeNS && c == '.');
}
