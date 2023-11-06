/******************************************************************************
 BisonCompleter.cpp

	BASE CLASS = StringCompleter

	Copyright © 2001 by John Lindal.

 ******************************************************************************/

#include "BisonCompleter.h"
#include "CCompleter.h"
#include "CStyler.h"
#include <jx-af/jcore/jAssert.h>

BisonCompleter* BisonCompleter::itsSelf = nullptr;

static const JUtf8Byte* kKeywordList[] =
{
};

const JSize kKeywordCount = sizeof(kKeywordList)/sizeof(JUtf8Byte*);

/******************************************************************************
 Instance (static)

 ******************************************************************************/

static bool recursiveInstance = false;

StringCompleter*
BisonCompleter::Instance()
{
	if (itsSelf == nullptr && !recursiveInstance)
	{
		recursiveInstance = true;

		itsSelf = jnew BisonCompleter;

		recursiveInstance = false;
	}

	return itsSelf;
}

/******************************************************************************
 Shutdown (static)

 ******************************************************************************/

void
BisonCompleter::Shutdown()
{
	jdelete itsSelf;
}

/******************************************************************************
 Constructor (protected)

 ******************************************************************************/

BisonCompleter::BisonCompleter()
	:
	StringCompleter(kBisonLang, kKeywordCount, kKeywordList, JString::kCompareCase)
{
	UpdateWordList();	// include C
	ListenTo(CStyler::Instance());
}

/******************************************************************************
 Destructor

 ******************************************************************************/

BisonCompleter::~BisonCompleter()
{
	itsSelf = nullptr;
}

/******************************************************************************
 IsWordCharacter (virtual protected)

	We include the namespace operator (::) to allow completion of fully
	qualified names.

 ******************************************************************************/

bool
BisonCompleter::IsWordCharacter
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
BisonCompleter::Receive
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
 UpdateWordList (virtual protected)

 ******************************************************************************/

void
BisonCompleter::UpdateWordList()
{
	StringCompleter::UpdateWordList();

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
