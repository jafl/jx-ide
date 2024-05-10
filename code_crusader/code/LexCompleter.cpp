/******************************************************************************
 LexCompleter.cpp

	BASE CLASS = StringCompleter

	Copyright © 2001 by John Lindal.

 ******************************************************************************/

#include "LexCompleter.h"
#include "CCompleter.h"
#include "CStyler.h"
#include <jx-af/jcore/jAssert.h>

LexCompleter* LexCompleter::itsSelf = nullptr;

static const JUtf8Byte* kKeywordList[] =
{
};

const JSize kKeywordCount = sizeof(kKeywordList)/sizeof(JUtf8Byte*);

/******************************************************************************
 Instance (static)

 ******************************************************************************/

static bool recursiveInstance = false;

StringCompleter*
LexCompleter::Instance()
{
	if (itsSelf == nullptr && !recursiveInstance)
	{
		recursiveInstance = true;

		itsSelf = jnew LexCompleter;

		recursiveInstance = false;
	}

	return itsSelf;
}

/******************************************************************************
 Shutdown (static)

 ******************************************************************************/

void
LexCompleter::Shutdown()
{
	jdelete itsSelf;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

LexCompleter::LexCompleter()
	:
	StringCompleter(kLexLang, kKeywordCount, kKeywordList, JString::kCompareCase)
{
	UpdateWordListExtra();	// include C
	ListenTo(CStyler::Instance());
}

/******************************************************************************
 Destructor

 ******************************************************************************/

LexCompleter::~LexCompleter()
{
	itsSelf = nullptr;
}

/******************************************************************************
 IsWordCharacter (virtual protected)

	We include the namespace operator (::) to allow completion of fully
	qualified names.

 ******************************************************************************/

bool
LexCompleter::IsWordCharacter
	(
	const JUtf8Character&	c,
	const bool			includeNS
	)
	const
{
	return c.IsAlnum() || c == '_' || (includeNS && c == ':');
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
LexCompleter::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == CStyler::Instance() && message.Is(StylerBase::kWordListChanged))
	{
		UpdateWordList();
	}
	else
	{
		StringCompleter::Receive(sender, message);
	}
}

/******************************************************************************
 UpdateWordListExtra (virtual protected)

 ******************************************************************************/

void
LexCompleter::UpdateWordListExtra()
{
	// include C words

	const JUtf8Byte** cWordList;
	const JSize count = CCompleter::GetDefaultWordList(&cWordList);
	for (JUnsignedOffset i=0; i<count; i++)
	{
		Add(JString(cWordList[i], JString::kNoCopy));
	}

	CopySymbolsForLanguage(kCLang);
	CopyWordsFromStyler(CStyler::Instance());
}
