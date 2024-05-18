/******************************************************************************
 StringCompleter.cpp

	BASE CLASS = virtual JBroadcaster

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#include "StringCompleter.h"
#include "StylerBase.h"
#include "ProjectDocument.h"
#include "SymbolDirector.h"
#include "SymbolList.h"
#include "globals.h"
#include <jx-af/jx/JXStringCompletionMenu.h>
#include <jx-af/jx/JXFunctionTask.h>
#include <jx-af/jcore/JTextEditor.h>
#include <jx-af/jcore/JStringIterator.h>
#include <boost/fiber/operations.hpp>
#include <jx-af/jcore/jAssert.h>

const JSize kCheckInterval = 100;	// ms

/******************************************************************************
 Constructor

	*** Derived class must call UpdateWordList().

 ******************************************************************************/

StringCompleter::StringCompleter
	(
	const Language		lang,
	const JSize			keywordCount,
	const JUtf8Byte**	keywordList,
	const JString::Case	caseSensitive
	)
	:
	itsLanguage(lang),
	itsPredefKeywordCount(keywordCount),
	itsPredefKeywordList(keywordList),
	itsCaseSensitiveFlag(caseSensitive),
	itsNeedsUpdateTask(nullptr),
	itsUpdatingFlag(false)
{
	itsStringList = jnew JPtrArray<JString>(JPtrArrayT::kForgetAll, 4096);
	itsStringList->SetSortOrder(JListT::kSortAscending);

	itsStringList->SetCompareFunction(
		itsCaseSensitiveFlag ? JCompareStringsCaseSensitive : JCompareStringsCaseInsensitive);

	itsOwnedList = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);

	if (itsLanguage != kOtherLang)
	{
		ListenTo(GetDocumentManager());
	}

	GetStyler(lang, &itsStyler);
	if (itsStyler != nullptr)
	{
		ListenTo(itsStyler);
	}

	// We are constructed at a random point in time, so we have to
	// ListenTo() all pre-existing project documents.

	JPtrArray<ProjectDocument>* docList =
		GetDocumentManager()->GetProjectDocList();

	for (auto* doc : *docList)
	{
		ListenTo(doc->GetSymbolDirector()->GetSymbolList());
	}
}

/******************************************************************************
 Destructor

 ******************************************************************************/

StringCompleter::~StringCompleter()
{
	jdelete itsNeedsUpdateTask;
	itsNeedsUpdateTask = nullptr;	// ensure waiting only for thread

	WaitForUpdateThreadFinished();

	jdelete itsStringList;
	jdelete itsOwnedList;
}

/******************************************************************************
 Reset

 ******************************************************************************/

void
StringCompleter::Reset()
{
	RemoveAll();

	for (JUnsignedOffset i=0; i<itsPredefKeywordCount; i++)
	{
		Add(itsPredefKeywordList[i]);
	}
}

/******************************************************************************
 Add

 ******************************************************************************/

void
StringCompleter::Add
	(
	const JString& str
	)
{
	auto* s = jnew JString(str);
	if (itsStringList->InsertSorted(s, false))
	{
		itsOwnedList->Append(s);
	}
	else
	{
		jdelete s;
	}
}

// private

void
StringCompleter::Add
	(
	JString* s
	)
{
	itsStringList->InsertSorted(s, false);
}

/******************************************************************************
 RemoveAll

 ******************************************************************************/

void
StringCompleter::RemoveAll()
{
	itsStringList->CleanOut();
	itsOwnedList->CleanOut();
}

/******************************************************************************
 Complete

	Returns true if the text behind the caret was completed.

 ******************************************************************************/

bool
StringCompleter::Complete
	(
	JTextEditor*			te,
	JXStringCompletionMenu*	menu
	)
{
	WaitForUpdateThreadFinished();
	return Complete(te, true, menu);
}

// private

bool
StringCompleter::Complete
	(
	JTextEditor*			te,
	const bool				includeNS,
	JXStringCompletionMenu*	menu
	)
{
	assert( menu != nullptr );

	JIndex caretIndex;
	if (!te->GetCaretLocation(&caretIndex))
	{
		return false;
	}

	const JString& text = te->GetText()->GetText();

	JStringIterator iter(text, JStringIterator::kStartBeforeChar, caretIndex);
	iter.BeginMatch();
	JUtf8Character c;
	while (iter.Prev(&c, JStringIterator::kStay) && IsWordCharacter(c, includeNS))
	{
		iter.SkipPrev();
	}

	if ((iter.AtEnd() && caretIndex > text.GetCharacterCount()) ||
		iter.GetNextCharacterIndex() == caretIndex)
	{
		return false;
	}

	const JStringMatch m    = iter.FinishMatch();	// must exist after invalidating iterator
	const JSize matchLength = m.GetCharacterRange().GetCount();

	iter.Invalidate();

	JString s;
	const JSize matchCount = Complete(m.GetString(), &s, menu);
	if (matchCount > 0 &&
		s.GetCharacterCount() > matchLength)
	{
		te->Paste(JString(s.GetBytes() + m.GetUtf8ByteRange().GetCount(), JString::kNoCopy));
		menu->ClearRequestCount();
		return true;
	}
	else if (matchCount > 1)
	{
		menu->CompletionRequested(matchLength);
		return true;
	}
	else if (matchCount == 0 && includeNS)
	{
		// try once more without namespace
		return Complete(te, false, menu);
	}

	return false;
}

/******************************************************************************
 Complete (private)

	Checks if prefix is the beginning of one of our strings.

	If not, returns 0.
	If there is a single match, returns 1 and *maxPrefix contains the string.
	If there are multiple matches, returns how many and *maxPrefix contains
		the most characters that are common to all the matches.

	If there is an exact match, we still check for additional matches so
	the menu can give the user a choice.

 ******************************************************************************/

JSize
StringCompleter::Complete
	(
	const JString&			prefix,
	JString*				maxPrefix,
	JXStringCompletionMenu*	menu
	)
	const
{
	menu->RemoveAllItems();

	bool found;
	const JIndex startIndex =
		itsStringList->SearchSortedOTI(const_cast<JString*>(&prefix),
									 JListT::kAnyMatch, &found);

	const JSize stringCount = itsStringList->GetItemCount();
	if (startIndex > stringCount)
	{
		maxPrefix->Clear();
		return 0;
	}

	JPtrArray<JString> matchList(JPtrArrayT::kForgetAll, 100);
	*maxPrefix = *(itsStringList->GetItem(startIndex));

	JSize matchCount   = 0;
	bool addString = true;
	for (JIndex i=startIndex; i<=stringCount; i++)
	{
		const JString* s = itsStringList->GetItem(i);
		if (!s->StartsWith(prefix, itsCaseSensitiveFlag))
		{
			break;
		}

		if (matchCount > 0)
		{
			const JSize matchLength  = JString::CalcCharacterMatchLength(*maxPrefix, *s, itsCaseSensitiveFlag);
			const JSize prefixLength = maxPrefix->GetCharacterCount();
			if (matchLength < prefixLength)
			{
				JStringIterator iter(maxPrefix, JStringIterator::kStartAtEnd);
				iter.RemovePrev(prefixLength - matchLength);
			}
		}

		matchCount++;
		if (itsCaseSensitiveFlag)
		{
			matchList.Append(const_cast<JString*>(s));
		}
		else if (addString)
		{
			JString s1 = *s;
			MatchCase(prefix, &s1);
			addString = menu->AddString(s1);	// must process all to get maxPrefix
		}
	}

	if (itsCaseSensitiveFlag && matchCount > 0)
	{
		matchList.SetSortOrder(JListT::kSortAscending);
		matchList.SetCompareFunction(JCompareStringsCaseInsensitive);
		matchList.Sort();

		assert( matchCount == matchList.GetItemCount() );
		for (JIndex i=1; i<=matchCount; i++)
		{
			if (!menu->AddString(*(matchList.GetItem(i))))
			{
				matchCount = i-1;
				break;
			}
		}
	}
	else if (!itsCaseSensitiveFlag)
	{
		MatchCase(prefix, maxPrefix);
	}

	return matchCount;
}

/******************************************************************************
 MatchCase (virtual protected)

	Adjust target to match the case of source.  target is guaranteed to
	be at least as long as source.

	This is not virtual because only case-insensitive languages need to
	override it.

 ******************************************************************************/

void
StringCompleter::MatchCase
	(
	const JString&	source,
	JString*		target
	)
	const
{
	assert_msg( 0, "Programmer forgot to override StringCompleter::MatchCase()" );
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
StringCompleter::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == GetDocumentManager() &&
		message.Is(DocumentManager::kProjectDocumentCreated))
	{
		auto& info = dynamic_cast<const DocumentManager::ProjectDocumentCreated&>(message);
		ListenTo(info.GetProjectDocument()->GetSymbolDirector()->GetSymbolList());
		// SymbolList guaranteed to broadcast after first scan
	}
	else if (sender == GetDocumentManager() &&
			 (message.Is(DocumentManager::kProjectDocumentDeleted) ||
			  message.Is(DocumentManager::kProjectDocumentsDeleted)))
	{
		UpdateWordList();
	}

	else if (message.Is(SymbolList::kChanged))
	{
		UpdateWordList();
	}

	else if (sender == itsStyler && message.Is(StylerBase::kWordListChanged))
	{
		UpdateWordList();
	}

	else
	{
		JBroadcaster::Receive(sender, message);
	}
}

/******************************************************************************
 UpdateWordList (protected)

 ******************************************************************************/

void
StringCompleter::UpdateWordList()
{
	if (SymbolUpdateRunning() || itsUpdatingFlag)
	{
		if (itsNeedsUpdateTask == nullptr)
		{
			itsNeedsUpdateTask = jnew JXFunctionTask(kCheckInterval, [this]()
			{
				if (!SymbolUpdateRunning() && !itsUpdatingFlag)
				{
					jdelete itsNeedsUpdateTask;
					itsNeedsUpdateTask = nullptr;

					UpdateWordList();
				}
			},
			"StringCompleter::UpdateWordList::Wait");
			itsNeedsUpdateTask->Start();
		}

		RemoveAll();	// itsStringList may contain invalid pointers
		return;
	}

	CompleterUpdateStarted();
	itsUpdatingFlag = true;

	std::thread t([this]()
	{
		// start with predefined keywords

		Reset();

		// add words from styler's override list

		if (itsStyler != nullptr)
		{
			CopyWordsFromStyler(itsStyler);
		}

		// add symbols from source code

		if (itsLanguage != kOtherLang)
		{
			CopySymbolsForLanguage(itsLanguage);
		}

		UpdateWordListExtra();

		itsUpdatingFlag = false;
		CompleterUpdateFinished();
	});
	t.detach();
}

/******************************************************************************
 UpdateWordListExtra (virtual protected)

 ******************************************************************************/

void
StringCompleter::UpdateWordListExtra()
{
}

/******************************************************************************
 CopyWordsFromStyler (protected)

 ******************************************************************************/

void
StringCompleter::CopyWordsFromStyler
	(
	StylerBase* styler
	)
{
	JStringMapCursor<JFontStyle> cursor(&styler->GetWordList());
	while (cursor.Next())
	{
		Add(cursor.GetKey());
	}
}

/******************************************************************************
 CopySymbolsForLanguage (protected)

 ******************************************************************************/

void
StringCompleter::CopySymbolsForLanguage
	(
	const Language lang
	)
{
	JPtrArray<ProjectDocument>* docList =
		GetDocumentManager()->GetProjectDocList();

	for (auto* doc : *docList)
	{
		const SymbolList* symbolList = doc->GetSymbolDirector()->GetSymbolList();

		const JSize symbolCount = symbolList->GetItemCount();
		for (JIndex j=1; j<=symbolCount; j++)
		{
			Language l;
			SymbolList::Type type;
			bool fullyQualifiedFileScope;
			const JString& symbolName =
				symbolList->GetSymbol(j, &l, &type, &fullyQualifiedFileScope);
			if (l == lang && !fullyQualifiedFileScope)
			{
				Add(const_cast<JString*>(&symbolName));
			}
		}
	}
}

/******************************************************************************
 WaitForUpdateThreadFinished (private)

 ******************************************************************************/

void
StringCompleter::WaitForUpdateThreadFinished()
{
	while (itsUpdatingFlag || itsNeedsUpdateTask != nullptr)
	{
		boost::this_fiber::sleep_for(
			std::chrono::milliseconds(kCheckInterval));
	}
}
