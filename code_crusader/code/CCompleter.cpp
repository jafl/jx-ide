/******************************************************************************
 CCompleter.cpp

	BASE CLASS = StringCompleter

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#include "CCompleter.h"
#include <jx-af/jcore/jAssert.h>

CCompleter* CCompleter::itsSelf = nullptr;

static const JUtf8Byte* kKeywordList[] =
{
	// C

	"auto", "break", "case", "char", "const", "continue", "default", "do",
	"double", "else", "enum", "extern", "float", "for", "goto", "if",
	"inline", "int", "long", "register", "restrict", "return", "short",
	"signed", "sizeof", "static", "struct", "switch", "typedef", "union",
	"unsigned", "void", "volatile", "while",

	// C++

	"alignas", "alignof", "and", "and_eq", "asm", "atomic_cancel",
	"atomic_commit", "atomic_noexcept", "bitand", "bitor", "bool", "catch",
	"char16_t", "char32_t", "class", "compl", "concept", "constexpr",
	"const_cast", "decltype", "delete", "dynamic_cast", "explicit",
	"export", "false", "final", "friend", "module", "mutable", "namespace",
	"new", "noexcept", "not", "not_eq", "nullptr", "operator", "or",
	"or_eq", "overload", "override", "private", "protected", "public",
	"reinterpret_cast", "requires", "static_assert", "static_cast",
	"template", "this", "thread_local", "throw", "true", "try", "typeid",
	"typename", "using", "virtual", "wchar_t", "xor", "xor_eq",

	// preprocessor

	"include", "include_next", "define", "undef", "if", "ifdef", "ifndef",
	"else", "elif", "endif", "line", "pragma", "error"
};

const JSize kKeywordCount = sizeof(kKeywordList)/sizeof(JUtf8Byte*);

/******************************************************************************
 Instance (static)

 ******************************************************************************/

static bool recursiveInstance = false;

StringCompleter*
CCompleter::Instance()
{
	if (itsSelf == nullptr && !recursiveInstance)
	{
		recursiveInstance = true;

		itsSelf = jnew CCompleter;

		recursiveInstance = false;
	}

	return itsSelf;
}

/******************************************************************************
 Shutdown (static)

 ******************************************************************************/

void
CCompleter::Shutdown()
{
	jdelete itsSelf;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

CCompleter::CCompleter()
	:
	StringCompleter(kCLang, kKeywordCount, kKeywordList, JString::kCompareCase)
{
	UpdateWordList();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

CCompleter::~CCompleter()
{
	itsSelf = nullptr;
}

/******************************************************************************
 IsWordCharacter (virtual protected)

	We include the namespace operator (::) to allow completion of fully
	qualified names.

 ******************************************************************************/

bool
CCompleter::IsWordCharacter
	(
	const JUtf8Character&	c,
	const bool			includeNS
	)
	const
{
	return c.IsAlnum() || c == '_' || (includeNS && c == ':');
}

/******************************************************************************
 GetDefaultWordList (static)

	Must be static because may be called before object is created.

 ******************************************************************************/

JSize
CCompleter::GetDefaultWordList
	(
	const JUtf8Byte*** list
	)
{
	*list = kKeywordList;
	return kKeywordCount;
}
