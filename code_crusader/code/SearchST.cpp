/******************************************************************************
 SearchST.cpp

	Class to search files and report back to main Code Crusader process.

	BASE CLASS = JStyledText

	Copyright © 1998-24 by John Lindal.

 ******************************************************************************/

#include "SearchST.h"
#include "SearchDocument.h"
#include "SearchTextDialog.h"
#include "sharedUtil.h"
#include "globals.h"
#include <jx-af/jcore/JStringIterator.h>
#include <jx-af/jcore/jFileUtil.h>
#include <stdio.h>
#include <jx-af/jcore/jAssert.h>

const JSize kMaxQuoteCharCount = 500;

// JBroadcaster message types

const JUtf8Byte* SearchST::kIncrementProgress = "IncrementProgress::SearchST";
const JUtf8Byte* SearchST::kSearchResult      = "SearchResult::SearchST";
const JUtf8Byte* SearchST::kAdditionalMatch   = "AdditionalMatch::SearchST";
const JUtf8Byte* SearchST::kFileName          = "FileName::SearchST";
const JUtf8Byte* SearchST::kError             = "Error::SearchST";

/******************************************************************************
 Constructor

 ******************************************************************************/

SearchST::SearchST()
	:
	JStyledText(false, false),
	itsCancelledFlag(false)
{
	SetCharacterInWordFunction(::IsCharacterInWord);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

SearchST::~SearchST()
{
}

/******************************************************************************
 SearchFiles

 ******************************************************************************/

void
SearchST::SearchFiles
	(
	const JPtrArray<JString>&	fileList,
	const JPtrArray<JString>&	nameList,
	const bool					onlyListFiles,
	const bool					listFilesWithoutMatch,
	SearchDocument*				doc
	)
{
	doc->SetSearchST(this);

	JRegex* searchRegex;
	JString replaceStr;
	JInterpolate* interpolator;
	bool entireWord, wrapSearch, preserveCase;
	const bool ok =
		GetSearchTextDialog()->GetSearchParameters(
			&searchRegex, &entireWord, &wrapSearch,
			&replaceStr, &interpolator, &preserveCase);
	assert( ok );

	const JSize count = fileList.GetItemCount();
	assert( count == nameList.GetItemCount() );

	for (JIndex i=1; i<=count; i++)
	{
		const JString* file = fileList.GetItem(i);
		const JString* name = nameList.GetItem(i);

		if (!itsCancelledFlag)
		{
			SearchFile(*file, *name, onlyListFiles, listFilesWithoutMatch,
					   *searchRegex, entireWord, doc);

			doc->QueueMessage(jnew IncrementProgress());
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

inline JSize
countLines
	(
	JStringIterator*	iter,
	const JIndex		beyondEnd
	)
{
	JSize count = 0;

	JUtf8Character c;
	while (iter->Next(&c) && !iter->AtEnd() &&
		   iter->GetNextCharacterIndex() <= beyondEnd)
	{
		if (c == '\n')
		{
			count++;
		}
	}

	if (iter->GetPrevCharacterIndex() == beyondEnd)
	{
		iter->SkipPrev();
	}

	return count;
}

void
SearchST::SearchFile
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
		QueueErrorMessage(doc, "NotFound::SearchST", map, sizeof(map));
		return;
	}
	else if (!JFileReadable(fileName))
	{
		QueueErrorMessage(doc, "NotReadable::SearchST", map, sizeof(map));
		return;
	}

	const bool ignore = JUtf8Character::IgnoreBadUtf8();
	JUtf8Character::SetIgnoreBadUtf8(true);

	PlainTextFormat format;
	if (IsKnownBinaryFile(printName) ||
		!ReadPlainText(fileName, &format, false))
	{
		// don't search binary files (cleaning is too slow and pointless)

		QueueErrorMessage(doc, "NotSearchedBinary::SearchST", map, sizeof(map));
		JUtf8Character::SetIgnoreBadUtf8(ignore);
		return;
	}

	JUtf8Character::SetIgnoreBadUtf8(ignore);

	bool foundMatch = false;
	TextRange prevQuoteRange;
	bool prevQuoteTruncated = false;

	TextIndex startIndex(1,1);
	JIndex lineIndex = 1;
	while (true)
	{
		bool wrapped;
		const JStringMatch m = SearchForward(startIndex, searchRegex, entireWord, false, &wrapped);
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

		JStringIterator iter(GetText());
		iter.UnsafeMoveTo(JStringIterator::kStartBeforeChar, startIndex.charIndex, startIndex.byteIndex);
		lineIndex += countLines(&iter, m.GetCharacterRange().first);

		startIndex.charIndex = m.GetCharacterRange().first + m.GetCharacterRange().GetCount();
		startIndex.byteIndex = m.GetUtf8ByteRange().first + m.GetUtf8ByteRange().GetCount();

		const JSize extraLineCount = countLines(&iter, startIndex.charIndex);

		const TextRange origMatchRange(
			m.GetCharacterRange(), m.GetUtf8ByteRange());

		auto matchRange = origMatchRange;
		if (matchRange.charRange.GetCount() > kMaxQuoteCharCount)
		{
			matchRange.charRange.last = matchRange.charRange.first + kMaxQuoteCharCount - 1;

			iter.UnsafeMoveTo(JStringIterator::kStartBeforeChar, matchRange.charRange.first, matchRange.byteRange.first);
			iter.MoveTo(JStringIterator::kStartAfterChar, matchRange.charRange.last);
			matchRange.byteRange.last = iter.GetPrevByteIndex();
		}

		const TextRange origQuoteRange(
			GetParagraphStart(matchRange.GetFirst()),
			AdjustTextIndex(GetParagraphEnd(matchRange.GetLast(*this)), +1));

		TextRange quoteRange = origQuoteRange;
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

			doc->QueueMessage(msg);
		}
		else
		{
			prevQuoteTruncated = false;

			iter.UnsafeMoveTo(JStringIterator::kStartBeforeChar, quoteRange.charRange.first, quoteRange.byteRange.first);
			iter.BeginMatch();
			iter.UnsafeMoveTo(JStringIterator::kStartAfterChar, quoteRange.charRange.last, quoteRange.byteRange.last);

			auto* quoteText = jnew JString(iter.FinishMatch().GetString());
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

			doc->QueueMessage(
				jnew SearchResult(jnew JString(printName),
								  lineIndex, quoteText, matchRange));
		}

		prevQuoteRange = quoteRange;
		lineIndex     += extraLineCount;
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
SearchST::QueueErrorMessage
	(
	SearchDocument*		doc,
	const JUtf8Byte*	key,
	const JUtf8Byte*	map[],
	const JSize			size
	)
{
	doc->QueueMessage(
		jnew Error(jnew JString(JGetString(key, map, size))));
}

/******************************************************************************
 QueueFileNameMessage (private)

 ******************************************************************************/

void
SearchST::QueueFileNameMessage
	(
	SearchDocument*	doc,
	const JString&	name
	)
{
	doc->QueueMessage(
		jnew FileName(jnew JString(name)));
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
SearchST::IsKnownBinaryFile
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
SearchST::ReplaceAllForward()
{
	JRegex* searchRegex;
	JString replaceStr;
	JInterpolate* interpolator;
	bool entireWord, wrapSearch, preserveCase;
	if (JXGetSearchTextDialog()->GetSearchParameters(
			&searchRegex, &entireWord, &wrapSearch,
			&replaceStr, &interpolator, &preserveCase))
	{
		auto r = ReplaceAllInRange(
					TextRange(TextIndex(1,1), GetBeyondEnd()),
					*searchRegex, entireWord,
					replaceStr, interpolator, preserveCase);
		return !r.IsEmpty();
	}
	else
	{
		return false;
	}
}
