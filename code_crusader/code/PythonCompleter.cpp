/******************************************************************************
 PythonCompleter.cpp

	BASE CLASS = StringCompleter

	Copyright © 2000 by John Lindal.

 ******************************************************************************/

#include "PythonCompleter.h"
#include <jx-af/jcore/jAssert.h>

PythonCompleter* PythonCompleter::itsSelf = nullptr;

static const JUtf8Byte* kKeywordList[] =
{
	"and", "access", "break", "class", "continue", "def", "del", "elif",
	"else", "except", "exec", "finally", "for", "from", "global", "if",
	"import", "in", "is", "lambda", "not", "or", "pass", "print", "raise",
	"return", "try", "while", "yield"
};

const JSize kKeywordCount = sizeof(kKeywordList)/sizeof(JUtf8Byte*);

/******************************************************************************
 Instance (static)

 ******************************************************************************/

static bool recursiveInstance = false;

StringCompleter*
PythonCompleter::Instance()
{
	if (itsSelf == nullptr && !recursiveInstance)
	{
		recursiveInstance = true;

		itsSelf = jnew PythonCompleter;

		recursiveInstance = false;
	}

	return itsSelf;
}

/******************************************************************************
 Shutdown (static)

 ******************************************************************************/

void
PythonCompleter::Shutdown()
{
	jdelete itsSelf;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

PythonCompleter::PythonCompleter()
	:
	StringCompleter(kPythonLang, kKeywordCount, kKeywordList, JString::kCompareCase)
{
	UpdateWordList();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

PythonCompleter::~PythonCompleter()
{
	itsSelf = nullptr;
}

/******************************************************************************
 IsWordCharacter (virtual protected)

	We include the namespace operator (.) to allow completion of fully
	qualified names.

 ******************************************************************************/

bool
PythonCompleter::IsWordCharacter
	(
	const JUtf8Character&	c,
	const bool			includeNS
	)
	const
{
	return c.IsAlnum() || c == '_' || (includeNS && c == '.');
}
