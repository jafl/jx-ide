/******************************************************************************
 SearchTE.cpp

	Class to search files and report back to main Code Crusader process.

	BASE CLASS = JTextEditor

	Copyright © 1998 by John Lindal.

 ******************************************************************************/

#include "SearchTE.h"
#include "SearchDocument.h"
#include "SearchFontManager.h"
#include "SearchTextDialog.h"
#include "sharedUtil.h"
#include "globals.h"
#include <jx-af/jcore/JStringIterator.h>
#include <jx-af/jcore/jFileUtil.h>
#include <stdio.h>
#include <jx-af/jcore/jAssert.h>

const JSize kMaxQuoteCharCount = 500;

// JBroadcaster message types

const JUtf8Byte* SearchTE::kIncrementProgress = "IncrementProgress::SearchTE";
const JUtf8Byte* SearchTE::kSearchResult      = "SearchResult::SearchTE";
const JUtf8Byte* SearchTE::kAdditionalMatch   = "AdditionalMatch::SearchTE";
const JUtf8Byte* SearchTE::kFileName          = "FileName::SearchTE";
const JUtf8Byte* SearchTE::kError             = "Error::SearchTE";

/******************************************************************************
 Constructor

 ******************************************************************************/

SearchTE::SearchTE()
	:
	JTextEditor(kFullEditor, jnew JStyledText(false, false), true,
				jnew SearchFontManager, true,
				1,1,1,1, 1000000),
	itsCancelledFlag(false)
{
	assert( TEGetFontManager() != nullptr );

	RecalcAll(true);
	GetText()->SetCharacterInWordFunction(::IsCharacterInWord);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SearchTE::~SearchTE()
{
}

/******************************************************************************
 SearchFiles

 ******************************************************************************/

void
SearchTE::SearchFiles
	(
	const JPtrArray<JString>&	fileList,
	const JPtrArray<JString>&	nameList,
	const bool					onlyListFiles,
	const bool					listFilesWithoutMatch,
	SearchDocument*				doc
	)
{
	doc->SetSearchTE(this);

	JRegex* searchRegex;
	JString replaceStr;
	JInterpolate* interpolator;
	bool entireWord, wrapSearch, preserveCase;
	const bool ok =
		GetSearchTextDialog()->GetSearchParameters(
			&searchRegex, &entireWord, &wrapSearch,
			&replaceStr, &interpolator, &preserveCase);
	assert( ok );

	const JSize count = fileList.GetElementCount();
	assert( count == nameList.GetElementCount() );

	for (JIndex i=1; i<=count; i++)
	{
		const JString* file = fileList.GetElement(i);
		const JString* name = nameList.GetElement(i);

		if (!itsCancelledFlag)
		{
			SearchFile(*file, *name, onlyListFiles, listFilesWithoutMatch,
					   *searchRegex, entireWord, doc);

			auto* msg = jnew IncrementProgress();
			assert( msg != nullptr );

			doc->QueueMessage(msg);
		}

		// toss temp files once we are done

		if (*file != *name)
		{
			JRemoveFile(*file);
		}
	}

	doc->SearchFinished();
}

/******************************************************************************
 SearchFile (private)

 ******************************************************************************/

void
SearchTE::SearchFile
	(
	const JString&	fileName,
	const JString&	printName,		// so we display it correctly to the user
	const bool		onlyListFiles,
	const bool		listFilesWithoutMatch,
	const JRegex&	searchRegex,
	const bool		entireWord,
	SearchDocument*	doc
	)
{
	const JUtf8Byte* map[] =
	{
		"n", fileName.GetBytes()
	};

	if (!JFileExists(fileName))
	{
		QueueErrorMessage(doc, "NotFound::SearchTE", map, sizeof(map));
		return;
	}
	else if (!JFileReadable(fileName))
	{
		QueueErrorMessage(doc, "NotReadable::SearchTE", map, sizeof(map));
		return;
	}

	const bool ignore = JUtf8Character::IgnoreBadUtf8();
	JUtf8Character::SetIgnoreBadUtf8(true);

	JStyledText::PlainTextFormat format;
	if (IsKnownBinaryFile(printName) ||
		!GetText()->ReadPlainText(fileName, &format, false))
	{
		// don't search binary files (cleaning is too slow and pointless)

		QueueErrorMessage(doc, "NotSearchedBinary::SearchTE", map, sizeof(map));
		JUtf8Character::SetIgnoreBadUtf8(ignore);
		return;
	}

	JUtf8Character::SetIgnoreBadUtf8(ignore);

	bool foundMatch = false;
	JStyledText::TextRange prevQuoteRange;
	bool prevQuoteTruncated = false;

	while (true)
	{
		bool wrapped;
		const JStringMatch m = SearchForward(searchRegex, entireWord, false, &wrapped);
		if (m.IsEmpty())
		{
			break;
		}

		foundMatch = true;
		if (onlyListFiles || listFilesWithoutMatch)
		{
			if (!listFilesWithoutMatch)
			{
				QueueFileNameMessage(doc, printName);
			}
			break;
		}

		JStringIterator iter(GetText()->GetText());

		const JStyledText::TextRange origMatchRange(
			m.GetCharacterRange(), m.GetUtf8ByteRange());

		auto matchRange = origMatchRange;
		if (matchRange.charRange.GetCount() > kMaxQuoteCharCount)
		{
			matchRange.charRange.last = matchRange.charRange.first + kMaxQuoteCharCount - 1;

			iter.UnsafeMoveTo(JStringIterator::kStartBeforeChar, matchRange.charRange.first, matchRange.byteRange.first);
			iter.MoveTo(JStringIterator::kStartAfterChar, matchRange.charRange.last);
			matchRange.byteRange.last = iter.GetPrevByteIndex();
		}

		const JStyledText::TextRange origQuoteRange(
			GetText()->GetParagraphStart(matchRange.GetFirst()),
			GetText()->AdjustTextIndex(GetText()->GetParagraphEnd(matchRange.GetLast(*GetText())), +1));

		JStyledText::TextRange quoteRange = origQuoteRange;
		if (prevQuoteRange.charRange.Contains(matchRange.charRange))
		{
			quoteRange = prevQuoteRange;
		}
		else if (quoteRange.charRange.GetCount() > kMaxQuoteCharCount)
		{
			const JSize extraCount =
				(kMaxQuoteCharCount - matchRange.charRange.GetCount())/2;

			const JIndex first = matchRange.charRange.first > extraCount ? 
									matchRange.charRange.first - extraCount : 1;

			if (quoteRange.charRange.first < first)
			{
				iter.UnsafeMoveTo(JStringIterator::kStartBeforeChar, quoteRange.charRange.first, quoteRange.byteRange.first);
				iter.MoveTo(JStringIterator::kStartBeforeChar, first);
				quoteRange.charRange.first = first;
				quoteRange.byteRange.first = iter.GetNextByteIndex();
			}

			const JIndex last = matchRange.charRange.last + extraCount;
			if (last < quoteRange.charRange.last)
			{
				iter.UnsafeMoveTo(JStringIterator::kStartAfterChar, quoteRange.charRange.last, quoteRange.byteRange.last);
				iter.MoveTo(JStringIterator::kStartAfterChar, last);
				quoteRange.charRange.last = last;
				quoteRange.byteRange.last = iter.GetPrevByteIndex();
			}
		}

		matchRange.charRange -= quoteRange.charRange.first-1;
		matchRange.byteRange -= quoteRange.byteRange.first-1;

		if (quoteRange.charRange == prevQuoteRange.charRange)
		{
			if (prevQuoteTruncated)
			{
				matchRange.charRange += 3;
				matchRange.byteRange += 3;
			}

			auto* msg = jnew AdditionalMatch(matchRange);
			assert( msg != nullptr );

			doc->QueueMessage(msg);
		}
		else
		{
			prevQuoteTruncated = false;

			iter.UnsafeMoveTo(JStringIterator::kStartBeforeChar, quoteRange.charRange.first, quoteRange.byteRange.first);
			iter.BeginMatch();
			iter.UnsafeMoveTo(JStringIterator::kStartAfterChar, quoteRange.charRange.last, quoteRange.byteRange.last);

			auto* quoteText = jnew JString(iter.FinishMatch().GetString());
			assert( quoteText != nullptr );

			if (quoteRange.charRange.first != origQuoteRange.charRange.first)
			{
				quoteText->Prepend("...");
				matchRange.charRange += 3;
				matchRange.byteRange += 3;
				prevQuoteTruncated = true;
			}
			if (quoteRange.charRange.last != origQuoteRange.charRange.last)
			{
				quoteText->Append("...");
			}
			if (matchRange.charRange.GetCount() != origMatchRange.charRange.GetCount())
			{
				matchRange.charRange.last += 3;	// underline ellipsis
				matchRange.byteRange.last += 3;
			}

			auto* n = jnew JString(printName);
			assert( n != nullptr );

			auto* msg = jnew SearchResult(n, GetLineForChar(quoteRange.charRange.first),
										  quoteText, matchRange);
			assert( msg != nullptr );

			doc->QueueMessage(msg);
		}

		prevQuoteRange = quoteRange;
	}

	if (listFilesWithoutMatch && !foundMatch)
	{
		QueueFileNameMessage(doc, printName);
	}
}

/******************************************************************************
 QueueErrorMessage (private)

 ******************************************************************************/

void
SearchTE::QueueErrorMessage
	(
	SearchDocument*		doc,
	const JUtf8Byte*	key,
	const JUtf8Byte*	map[],
	const JSize			size
	)
{
	auto* e = jnew JString(JGetString(key, map, size));
	assert( e != nullptr );

	auto* msg = jnew Error(e);
	assert( msg != nullptr );

	doc->QueueMessage(msg);
}

/******************************************************************************
 QueueFileNameMessage (private)

 ******************************************************************************/

void
SearchTE::QueueFileNameMessage
	(
	SearchDocument*	doc,
	const JString&	name
	)
{
	auto* n = jnew JString(name);
	assert( n != nullptr );

	auto* msg = jnew FileName(n);
	assert( msg != nullptr );

	doc->QueueMessage(msg);
}

/******************************************************************************
 IsKnownBinaryFile (private)

 ******************************************************************************/

static const JUtf8Byte* kSuffix[] =
{
	".o", ".a", ".so", ".dylib", ".jar", ".class",
	".zip", ".tar", ".gz", ".tgz", ".bz2", ".rpm"
};

bool
SearchTE::IsKnownBinaryFile
	(
	const JString& fileName
	)
	const
{
	if (GetPrefsManager()->GetFileType(fileName) == kBinaryFT)
	{
		return true;
	}

	for (auto suffix : kSuffix)
	{
		if (fileName.EndsWith(suffix))
		{
			return true;
		}
	}

	return false;
}

/******************************************************************************
 ReplaceAllForward

	Replace every occurrence of the search string with the replace string,
	starting from the current location.  Returns true if it replaced anything.

 ******************************************************************************/

bool
SearchTE::ReplaceAllForward()
{
	JRegex* searchRegex;
	JString replaceStr;
	JInterpolate* interpolator;
	bool entireWord, wrapSearch, preserveCase;
	if (JXGetSearchTextDialog()->GetSearchParameters(
			&searchRegex, &entireWord, &wrapSearch,
			&replaceStr, &interpolator, &preserveCase))
	{
		return JTextEditor::ReplaceAll(
					*searchRegex, entireWord,
					replaceStr, interpolator, preserveCase, false);
	}
	else
	{
		return false;
	}
}

/******************************************************************************
 TERefresh (virtual protected)

 ******************************************************************************/

void
SearchTE::TERefresh()
{
}

/******************************************************************************
 TERefreshRect (virtual protected)

 ******************************************************************************/

void
SearchTE::TERefreshRect
	(
	const JRect& rect
	)
{
}

/******************************************************************************
 TERedraw (virtual protected)

 ******************************************************************************/

void
SearchTE::TERedraw()
{
}

/******************************************************************************
 TESetGUIBounds (virtual protected)

 ******************************************************************************/

void
SearchTE::TESetGUIBounds
	(
	const JCoordinate w,
	const JCoordinate h,
	const JCoordinate changeY
	)
{
}

/******************************************************************************
 TEWidthIsBeyondDisplayCapacity (virtual protected)

 ******************************************************************************/

bool
SearchTE::TEWidthIsBeyondDisplayCapacity
	(
	const JSize width
	)
	const
{
	return false;
}

/******************************************************************************
 TEScrollToRect (virtual protected)

 ******************************************************************************/

bool
SearchTE::TEScrollToRect
	(
	const JRect&	rect,
	const bool		centerInDisplay
	)
{
	return true;
}

/******************************************************************************
 TEScrollForDrag (virtual protected)

 ******************************************************************************/

bool
SearchTE::TEScrollForDrag
	(
	const JPoint& pt
	)
{
	return true;
}

/******************************************************************************
 TEScrollForDND (virtual protected)

 ******************************************************************************/

bool
SearchTE::TEScrollForDND
	(
	const JPoint& pt
	)
{
	return true;
}

/******************************************************************************
 TESetVertScrollStep (virtual protected)

 ******************************************************************************/

void
SearchTE::TESetVertScrollStep
	(
	const JCoordinate vStep
	)
{
}

/******************************************************************************
 TEUpdateClipboard (virtual protected)

 ******************************************************************************/

void
SearchTE::TEUpdateClipboard
	(
	const JString&			text,
	const JRunArray<JFont>&	style
	)
	const
{
}

/******************************************************************************
 TEGetClipboard (virtual protected)

 ******************************************************************************/

bool
SearchTE::TEGetClipboard
	(
	JString*			text,
	JRunArray<JFont>*	style
	)
	const
{
	return false;
}

/******************************************************************************
 TEBeginDND (virtual protected)

 ******************************************************************************/

bool
SearchTE::TEBeginDND()
{
	return false;
}

/******************************************************************************
 TEPasteDropData (virtual protected)

 ******************************************************************************/

void
SearchTE::TEPasteDropData()
{
}

/******************************************************************************
 TECaretShouldBlink (virtual protected)

 ******************************************************************************/

void
SearchTE::TECaretShouldBlink
	(
	const bool blink
	)
{
}

/******************************************************************************
 TEHasSearchText (virtual)

 ******************************************************************************/

bool
SearchTE::TEHasSearchText()
	const
{
	return false;
}
