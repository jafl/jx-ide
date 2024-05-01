/******************************************************************************
 LineAddressTable.cpp

	BASE CLASS = LineIndexTable

	Copyright (C) 2016 by John Lindal.

 ******************************************************************************/

#include "LineAddressTable.h"
#include "SourceDirector.h"
#include "BreakpointManager.h"
#include "Link.h"
#include <jx-af/jx/JXColorManager.h>
#include <jx-af/jcore/JStringIterator.h>
#include <jx-af/jcore/JListUtil.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

LineAddressTable::LineAddressTable
	(
	SourceDirector*	dir,
	SourceText*		text,
	JXScrollbarSet*		scrollbarSet,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	LineIndexTable(CompareBreakpointAddresses, dir, text, scrollbarSet, enclosure, hSizing, vSizing, x, y, w, h)
{
	itsVisualBPIndexList = jnew JArray<JIndex>;

	itsLineTextList = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	itsLineTextList->SetCompareFunction(JCompareStringsCaseInsensitive);
	itsLineTextList->SetSortOrder(JListT::kSortAscending);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

LineAddressTable::~LineAddressTable()
{
	jdelete itsVisualBPIndexList;
	jdelete itsLineTextList;
}

/******************************************************************************
 SetLineNumbers

	We take ownership of the items in the list.

 ******************************************************************************/

void
LineAddressTable::SetLineNumbers
	(
	JPtrArray<JString>*	list
	)
{
	itsLineTextList->CopyPointers(*list, JPtrArrayT::kDeleteAll, false);
	list->SetCleanUpAction(JPtrArrayT::kForgetAll);

	if (itsLineTextList->IsEmpty())
	{
		return;
	}

	for (auto* s : *itsLineTextList)
	{
		JStringIterator iter(s);
		if (iter.Next(" "))
		{
			iter.SkipPrev();
			iter.RemoveAllNext();
		}
	}

	JSize charCount = 0;
	const JString* s1 = itsLineTextList->GetFirstItem();
	const JString* sN = itsLineTextList->GetLastItem();
	if (s1->StartsWith("0x") && s1->GetCharacterCount() > 2 &&
		sN->StartsWith("0x") && sN->GetCharacterCount() > 2)
	{
		charCount = 2;

		if (s1->GetCharacterCount() == sN->GetCharacterCount())
		{
			JStringIterator iter(*sN, JStringIterator::kStartAfterChar, charCount);
			JUtf8Character c;
			while (iter.Next(&c) && c == '0')
			{
				charCount++;
			}
		}
	}

	if (charCount > 0)
	{
		for (auto* s : *itsLineTextList)
		{
			JStringIterator iter(s);
			iter.RemoveNext(charCount);
		}
	}
}

/******************************************************************************
 ClearLineNumbers

 ******************************************************************************/

void
LineAddressTable::ClearLineNumbers()
{
	itsLineTextList->CleanOut();
}

/******************************************************************************
 FindAddressLineNumber

 ******************************************************************************/

bool
LineAddressTable::FindAddressLineNumber
	(
	const JString&	origAddress,
	JIndex*			index
	)
	const
{
	JString addr = GetLineTextFromAddress(origAddress);

	bool found;
	*index = itsLineTextList->SearchSortedOTI(&addr, JListT::kAnyMatch, &found);
	return true;
}

/******************************************************************************
 GetCurrentLineMarkerColor (virtual protected)

 ******************************************************************************/

JColorID
LineAddressTable::GetCurrentLineMarkerColor()
	const
{
	return JColorManager::GetMagentaColor();
}

/******************************************************************************
 GetLineText (virtual protected)

 ******************************************************************************/

JString
LineAddressTable::GetLineText
	(
	const JIndex lineIndex
	)
	const
{
	return itsLineTextList->IndexValid(lineIndex) ?
		*(itsLineTextList->GetItem(lineIndex)) : JString();
}

/******************************************************************************
 GetLongestLineText (virtual protected)

 ******************************************************************************/

JString
LineAddressTable::GetLongestLineText
	(
	const JIndex lineCount
	)
	const
{
	return !itsLineTextList->IsEmpty() ?
		*itsLineTextList->GetLastItem() : JString(lineCount);
}

/******************************************************************************
 GetBreakpointLineIndex (virtual protected)

 ******************************************************************************/

JIndex
LineAddressTable::GetBreakpointLineIndex
	(
	const JIndex		bpIndex,
	const Breakpoint*	bp
	)
	const
{
	return itsVisualBPIndexList->GetItem(bpIndex);
}

/******************************************************************************
 GetFirstBreakpointOnLine (virtual protected)

 ******************************************************************************/

bool
LineAddressTable::GetFirstBreakpointOnLine
	(
	const JIndex	lineIndex,
	JIndex*			bpIndex
	)
	const
{
	const JString addr = BuildAddress(*itsLineTextList->GetItem(lineIndex));
	Breakpoint bp(addr);
	return GetBreakpointList()->SearchSorted(&bp, JListT::kFirstMatch, bpIndex);
}

/******************************************************************************
 BreakpointsOnSameLine (virtual protected)

 ******************************************************************************/

bool
LineAddressTable::BreakpointsOnSameLine
	(
	const Breakpoint* bp1,
	const Breakpoint* bp2
	)
	const
{
	return bp1->GetAddress() == bp2->GetAddress();
}

/******************************************************************************
 GetBreakpoints (virtual protected)

 ******************************************************************************/

void
LineAddressTable::GetBreakpoints
	(
	JPtrArray<Breakpoint>* list
	)
{
	itsVisualBPIndexList->RemoveAll();

	const JString* fnName;
	if (!GetDirector()->GetFunctionName(&fnName))
	{
		list->CleanOut();
		return;
	}

	Location loc;
	loc.SetFunctionName(*fnName);
	if (!GetLink()->GetBreakpointManager()->GetBreakpoints(loc, list))
	{
		return;
	}
	list->Sort();

	const JSize count = list->GetItemCount();
	JString target;
	for (JIndex i=1; i<=count; i++)
	{
		const Breakpoint* bp = list->GetItem(i);

		target = GetLineTextFromAddress(bp->GetAddress());

		bool found;
		const JIndex j = itsLineTextList->SearchSortedOTI(&target, JListT::kAnyMatch, &found);
		itsVisualBPIndexList->AppendItem(j);
	}
}

/******************************************************************************
 SetBreakpoint (virtual protected)

 ******************************************************************************/

void
LineAddressTable::SetBreakpoint
	(
	const JIndex	lineIndex,
	const bool	temporary
	)
{
	const JString addr = BuildAddress(*itsLineTextList->GetItem(lineIndex));
	GetLink()->SetBreakpoint(addr, temporary);
}

/******************************************************************************
 RemoveAllBreakpointsOnLine (virtual protected)

 ******************************************************************************/

void
LineAddressTable::RemoveAllBreakpointsOnLine
	(
	const JIndex lineIndex
	)
{
	const JString addr = BuildAddress(*itsLineTextList->GetItem(lineIndex));
	GetLink()->RemoveAllBreakpointsAtAddress(addr);
}

/******************************************************************************
 RunUntil (virtual protected)

 ******************************************************************************/

void
LineAddressTable::RunUntil
	(
	const JIndex lineIndex
	)
{
	const JString addr = BuildAddress(*itsLineTextList->GetItem(lineIndex));
	GetLink()->RunUntil(addr);
}

/******************************************************************************
 SetExecutionPoint (virtual protected)

 ******************************************************************************/

void
LineAddressTable::SetExecutionPoint
	(
	const JIndex lineIndex
	)
{
	const JString addr = BuildAddress(*itsLineTextList->GetItem(lineIndex));
	GetLink()->SetExecutionPoint(addr);
}

/******************************************************************************
 BuildAddress (static private)

 ******************************************************************************/

JString
LineAddressTable::BuildAddress
	(
	const JString& addr
	)
{
	JString s = addr;
	JStringIterator iter(&s);
	if (iter.Next(" "))
	{
		iter.SkipPrev();
		iter.RemoveAllNext();
	}

	s.Prepend("0x");
	return s;
}

/******************************************************************************
 GetLineTextFromAddress (private)

 ******************************************************************************/

JString
LineAddressTable::GetLineTextFromAddress
	(
	const JString& addr
	)
	const
{
	JString s = addr;
	JStringIterator iter(&s);
	if (s.StartsWith("0x"))
	{
		iter.RemoveNext(2);
	}

	if (itsLineTextList->IsEmpty())
	{
		return s;
	}

	while (s.GetFirstCharacter() == '0' &&
		   s.GetCharacterCount() > itsLineTextList->GetLastItem()->GetCharacterCount())
	{
		iter.RemoveNext();
	}

	return s;
}

/******************************************************************************
 CompareBreakpointAddresses (static private)

 ******************************************************************************/

std::weak_ordering
LineAddressTable::CompareBreakpointAddresses
	(
	Breakpoint* const & bp1,
	Breakpoint* const & bp2
	)
{
	return JCompareStringsCaseInsensitive(
		const_cast<JString*>(&bp1->GetAddress()),
		const_cast<JString*>(&bp2->GetAddress()));
}
