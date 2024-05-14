/******************************************************************************
 MakeCompleter.cpp

	BASE CLASS = StringCompleter

	Copyright © 2001 by John Lindal.

 ******************************************************************************/

#include "MakeCompleter.h"
#include <jx-af/jcore/jAssert.h>

MakeCompleter* MakeCompleter::itsSelf = nullptr;

static const JUtf8Byte* kKeywordList[] =
{
	"addprefix", "addsuffix",
	"basename",
	"call",
	"define", "dir",
	"else", "endef", "endif", "error", "export",
	"filter", "filter-out", "findstring", "firstword", "foreach",
	"if", "ifdef", "ifeq", "ifndef", "ifneq", "include",
	"join",
	"notdir",
	"origin", "override",
	"patsubst",
	"shell", "sort", "strip", "subst", "suffix",
	"unexport",
	"warning", "wildcard", "word", "wordlist", "words"
};

const JSize kKeywordCount = sizeof(kKeywordList)/sizeof(JUtf8Byte*);

/******************************************************************************
 Instance (static)

 ******************************************************************************/

static bool recursiveInstance = false;

StringCompleter*
MakeCompleter::Instance()
{
	if (itsSelf == nullptr && !recursiveInstance)
	{
		recursiveInstance = true;

		itsSelf = jnew MakeCompleter;

		recursiveInstance = false;
	}

	return itsSelf;
}

/******************************************************************************
 Shutdown (static)

 ******************************************************************************/

void
MakeCompleter::Shutdown()
{
	jdelete itsSelf;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

MakeCompleter::MakeCompleter()
	:
	StringCompleter(kMakeLang, kKeywordCount, kKeywordList, JString::kCompareCase)
{
	UpdateWordList();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

MakeCompleter::~MakeCompleter()
{
	itsSelf = nullptr;
}

/******************************************************************************
 IsWordCharacter (virtual protected)

 ******************************************************************************/

bool
MakeCompleter::IsWordCharacter
	(
	const JUtf8Character&	c,
	const bool			includeNS
	)
	const
{
	return c.IsAlnum() || c == '_';
}
